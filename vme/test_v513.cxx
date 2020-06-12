
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

extern "C"
{
#include "v513.h"
}

int main (int argc, char* argv[])
{
  setbuf(stdout, NULL);
  setbuf(stderr, NULL);

  int base = 0x510000;
  
  MVME_INTERFACE *myvme;

  // Test under vmic   
  int status = mvme_open(&myvme, 0);

  // Set am to A24 non-privileged Data
  mvme_set_am(myvme, MVME_AM_A24_ND);

  // Set dmode to D32
  mvme_set_dmode(myvme, MVME_DMODE_D32);

  v513_Reset(myvme, base);

  v513_Status(myvme, base);

  printf("Setting channels to OUTPUT mode, one by one\n");

  for (int i=0; i<16; i++)
    {
      v513_SetChannelMode(myvme, base, i, V513_CHANMODE_OUTPUT|V513_CHANMODE_POS|V513_CHANMODE_TRANSP);
      sleep(1);
    }

  printf("Pulsing channel 1\n");

  while (1)
    {
      v513_SetChannelMode(myvme, base, 1, V513_CHANMODE_INPUT);
      v513_Write(myvme, base, 0x1);
      sleep(2);
      v513_SetChannelMode(myvme, base, 1, V513_CHANMODE_OUTPUT);
      v513_Write(myvme, base, 0);
      sleep(2);
    }

  status = mvme_close(myvme);
  return 0;
}

/* emacs
 * Local Variables:
 * mode:C
 * mode:font-lock
 * tab-width: 8
 * c-basic-offset: 2
 * End:
 */
