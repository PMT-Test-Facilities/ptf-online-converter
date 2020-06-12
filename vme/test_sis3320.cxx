
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

extern "C" {
#include "sis3320drv.h"
}

int gSis3320base   = 0x30000000;
MVME_INTERFACE *gVme;

int main (int argc, char* argv[])
{
  setbuf(stdout, NULL);
  setbuf(stderr, NULL);

  int status;

  // Test under vmic   
  status = mvme_open(&gVme, 0);

  if (false)
    {
      // Print Current status
      sis3320_Reset(gVme, gSis3320base);
  
      // Print Current status
      sis3320_Status(gVme, gSis3320base);
    }

  if (false)
    {
      int testReg = 0x30;
      uint32_t tmp = sis3320_RegisterRead(gVme, gSis3320base, testReg);
      printf("reg0x%x: 0x%08x\n", testReg, tmp);
      
      sis3320_RegisterWrite(gVme, gSis3320base, testReg, 0x01234567);
      
      tmp = sis3320_RegisterRead(gVme, gSis3320base, testReg);
      printf("reg0x%x: 0x%08x\n", testReg, tmp);
    }

  if (false)
    {
      printf("Flash the U LED\n");
      while (1)
        {
          sis3320_RegisterWrite(gVme, gSis3320base, SIS3320_CONTROL_STATUS, 1);
          sleep(1);
          sis3320_RegisterWrite(gVme, gSis3320base, SIS3320_CONTROL_STATUS, 1<<16);
          sleep(1);
        }

    }

  if (true)
    {
      printf("ADC12 input mode: 0x%x\n", sis3320_RegisterRead(gVme, gSis3320base, SIS3320_ADC_INPUT_MODE_ADC12));
      printf("ADC34 input mode: 0x%x\n", sis3320_RegisterRead(gVme, gSis3320base, SIS3320_ADC_INPUT_MODE_ADC34));
      printf("ADC56 input mode: 0x%x\n", sis3320_RegisterRead(gVme, gSis3320base, SIS3320_ADC_INPUT_MODE_ADC56));
      printf("ADC78 input mode: 0x%x\n", sis3320_RegisterRead(gVme, gSis3320base, SIS3320_ADC_INPUT_MODE_ADC78));
    }

  if (true)
    {
      printf("ADC12 values: 0x%08x\n", sis3320_RegisterRead(gVme, gSis3320base, SIS3320_ACTUAL_SAMPLE_VALUE_ADC12));
      printf("ADC34 values: 0x%08x\n", sis3320_RegisterRead(gVme, gSis3320base, SIS3320_ACTUAL_SAMPLE_VALUE_ADC34));
      printf("ADC56 values: 0x%08x\n", sis3320_RegisterRead(gVme, gSis3320base, SIS3320_ACTUAL_SAMPLE_VALUE_ADC56));
      printf("ADC78 values: 0x%08x\n", sis3320_RegisterRead(gVme, gSis3320base, SIS3320_ACTUAL_SAMPLE_VALUE_ADC78));
    }

  if (true)
    {
      printf("ADC1 memory dump!\n");

      for (int i=0; i<10; i+=4)
        {
          printf("mem %d: 0x%08x\n", i, sis3320_RegisterRead(gVme, gSis3320base, SIS3320_ADC1_OFFSET + i));
        }
    }

  // Print Current status
  sis3320_Reset(gVme, gSis3320base);
  
  // Print Current status
  sis3320_Status(gVme, gSis3320base);

  status = mvme_close(gVme);
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
