
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>
#include <assert.h>

#include "midas.h"
#include "mvmestd.h"

extern "C" {
#include "vt48.h"
}

MVME_INTERFACE *gVme = 0;

// Before Jun 4th 2009 - config is below
int gVT48Base[]  = {0x200000,0};
int vt48_enable[] = {1,2,3,3,3,0}; /* make it same size as above */

INT init_vme_modules()
{
  int status;

  for (int i=0; gVT48Base[i] != 0; i++) {
    if (vt48_enable[i]) {
	printf("\nSetting up multihit TDC VT48 at address 0x%x\n\n",gVT48Base[i]);
	printf("VT48 at 0x%x CSR is 0x%x\n",gVT48Base[i],vt48_RegRead(gVme,gVT48Base[i],VT48_CSR0_REG));
	if( vt48_RegRead(gVme,gVT48Base[i],VT48_CSR0_REG) == 0xFFFFFFFF){
	  printf("TDC VT48 at address 0x%X is NOT responding; program will exit\n",gVT48Base[i]);
	  return FE_ERR_HW;
	}
	// Set mode defined by vt48_enable[i]:
	vt48_Setup(gVme, gVT48Base[i], vt48_enable[i]);

	//vt48_RegPrint(gVme,  gVT48Base[i]);
	//vt48_Status(gVme,  gVT48Base[i]);
	printf("VT48 at 0x%x AMT CSR is 0x%x\n",gVT48Base[i],vt48_RegRead(gVme,gVT48Base[i],VT48_CSR0_REG));
    }
  }
}

double utime()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  //printf("gettimeofday: %d %d\n", tv.tv_sec, tv.tv_usec);
  return tv.tv_sec + 0.000001*tv.tv_usec;
}

int xread_vt48(MVME_INTERFACE* myvme, int base, DWORD* pbank)
{
  int status;
  DWORD csr;
  int nread = 0;

  for (int loop=0; ; loop++)
    {
      csr = vt48_RegRead(myvme, base, VT48_CSR_RO);

      int count = csr & 0xFFF;

      //printf("vt48 0x%06x, CSR 0x%x, count %d\n", base, csr, count);

      if (count == 0)
	break;

      //if (count&1)
      //count --;

      status = vt48_DataRead(myvme, base, pbank + nread, count);

      //printf("read %d, status %d\n", count, status);

      //for (int i=0; i<count; i++)
      //printf("word %3d: 0x%08x\n", i, pbank[nread+i]);

      if (status != VT48_SUCCESS)
	return nread;

      nread += count;

      //if (loop>5)
      //exit(123);
    }

  csr = vt48_RegRead(myvme, base, VT48_CSR_RO);
  //printf("vt48 0x%06x, CSR 0x%x (after)\n", base, csr);

  return nread;
}

int read_vt48(int base,char* pevent,int imod)
{
   DWORD pdata[100000];

   double t0 = utime();

   int count = xread_vt48(gVme, base, pdata);

   double t1 = utime();
   printf("vt48 0x%06x: vt48_EventRead count %d words, time %f s, rate %f b/s\n", base, count, t1-t0, count*4/(t1-t0));

   if ((pdata[0] & 0xF0000000) != VT48_HEADER) {
      printf("Error: Vt48 0x%06x: Missing event header: 0x%08x\n", base, pdata[0]);
   }
   
   if ((pdata[count-1] & 0xF0000000) != VT48_TRAILER) {
      printf("Error: Vt48 0x%06x: Missing event trailer: 0x%08x\n", base, pdata[count-1]);
   }
}

#if 0

INT read_event(char *pevent, INT off)
{
  int latched_word;
  // The following is used to read the input trigger mask, not used at beginning
  disable_trigger();
  err_ev = false;
#ifdef UTIME
  double tStart = utime();
#endif
#ifdef HAVE_V513
    // On this VME1, we only want to read the 3 least significant bits of trig #
    int  strobe_word,index;
    latched_word = v513_Read(gVme, gVmeioBase);
    //CHECK THE BITS 4 - 8 of the latched word later !!!
    // Clear Strobe 
    v513_RegisterWrite(gVme,gVmeioBase,0x44,1);
    strobe_word = v513_RegisterRead(gVme,gVmeioBase,0x06);
    //printf("V513 after clear Strobe: strobe reg 0x%x\n",(strobe_word & 0x000ff));
#else
    latched_word = 0;
#endif

  if(debug) printf("read event %d!\n", SERIAL_NUMBER(pevent));

  if (gHaveRun) {

    /* init bank structure */
    bk_init32(pevent);
    CurrentEvNum = SERIAL_NUMBER(pevent);
    //CHECK THE BITS 4 - 8 of the latched word HERE !!!
    // printf("Ev 0x%x; latched word 0x%x\n",CurrentEvNum,latched_word); 
#ifdef HAVE_V513
    if ( (latched_word & 7) != (CurrentEvNum & 7) )
      {
	printf("Least significant 3 bits of latched word(0x%04x) .ne. fe EvNum(0x%04x)\n",
	       latched_word, CurrentEvNum);
	//      exit(1);
      }
#endif
#ifdef UTIME
    double tVT48start = utime();
#endif
    
#ifdef HAVE_VT48
    for (int i=0; gVT48Base[i] != 0; i++) 
      if (vt48_enable[i]) read_vt48(gVT48Base[i],pevent,i);
#endif
#ifdef UTIME
    double tVT48end = utime();
    double tVT48 = tVT48end - tVT48start;
#endif
    
#ifdef UTIME
    double tVF48start = utime();
    double tVF48M = 0;
    double tVF48N = 0;
#endif
    
#ifdef HAVE_VF48
    
#ifdef VF48_MULTI
#ifdef UTIME
    double tVF48Mstart = utime();
#endif
    while (1)
      {
	int rd = vf48_multi_read(gVme, gVF48_buf, 0); // nodebug
	//printf("read %d\n", rd);
	if (rd <= 0)
	  break;
      }
    // use max_event_size/2 for the VF48s since there is already data
    // from the VT48s
    vf48_multi_bank(gVF48_buf, pevent, max_event_size/2, 0, CurrentEvNum);
#ifdef UTIME
    double tVF48Mend = utime();
    tVF48M = tVF48Mend - tVF48Mstart;
#endif
#endif //  #ifdef VF48_MULTI
    
#ifdef UTIME
    double tVF48Nstart = utime();
#endif
    for (int i=0; gVF48Base[i] != 0; i++)
      if (!vf48_multi[i])
	if (vf48_enable[i]) read_vf48(gVF48Base[i],pevent,i);
#ifdef UTIME
    double tVF48Nend = utime();
    tVF48N = tVF48Nend - tVF48Nstart;
#endif
#endif // #ifdef HAVE_VF48
    
#ifdef UTIME
    double tVF48end = utime();
    double tVF48 = tVF48end - tVF48start;
#endif
    
    //printf("read event %d\n", SERIAL_NUMBER(pevent));

/*-- LOCAL FUNCTIONS -----------------------------------------------*/
/*-- local_vt48_load  -----------------------------------------------*/

#endif

int usage()
{
   printf("Usage: test_vt48.exe baseaddr\n");
   exit(1);
}
 
int main(int argc, char* argv[])
{
   int status;

   status = mvme_open(&gVme,0);
   status = mvme_set_am(gVme, MVME_AM_A24);
   status = mvme_set_dmode(gVme, MVME_DMODE_D32);

   //status = init_vme_modules();

   int base = 0;
   int xcount = 8000;

   if (argc >= 2)
      base = strtoul(argv[1], NULL, 0);

   if (argc >= 3)
      xcount = strtoul(argv[2], NULL, 0);

   if (base == 0)
      usage(); // does not return

   if (1)
      {
         DWORD p32[100000];
         int count = xcount;

         if (count > 100000)
            count = 100000;

         for (int i=0; i<100; i++)
            {
               double t0 = utime();
               status = vt48_DataRead(gVme, base, p32, count);
               double t1 = utime();
               printf("vt48 0x%06x: loop %d, vt48_DataRead status %d, count %d words, time %f s, rate %f b/s\n", base, i, status, count, t1-t0, count*4/(t1-t0));
               if (status != SUCCESS)
                  exit(1);
            }

         return 0;
      }

   if (1)
      {
         for (int i=0; i<10000; i++)
            {
               int addr = VT48_DATA_FIFO + 4*i;
               DWORD w = vt48_RegRead(gVme, base, addr);
               printf("read %d, addr 0x%08x, data 0x%08x\n", i, addr, w);
            }

         return 0;
      }

   return 0;
}


// end
