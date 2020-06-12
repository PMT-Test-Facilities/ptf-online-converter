//
// ./test_vme.cxx
//

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/time.h>
#include <math.h>

#include "mvmestd.h"

double utime()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  //printf("gettimeofday: %d %d\n", tv.tv_sec, tv.tv_usec);
  return tv.tv_sec + 0.000001*tv.tv_usec;
}

double gettime()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec + 0.000001*tv.tv_usec;
}

uint32_t vme_read(MVME_INTERFACE* vme, uint32_t base, int ireg)
{
   return mvme_read_value(vme, base+ireg);
}

void vme_write(MVME_INTERFACE *vme, DWORD base, int ireg, int value)
{
   mvme_write_value(vme, base+ireg, value);
}

int main(int argc, char* argv[])
{
  setbuf(stdout, NULL);
  setbuf(stderr, NULL);

  MVME_INTERFACE *myvme;

  // Test under vmic   
  int status = mvme_open(&myvme, 0);

  // Set am to A24 non-privileged Data
  mvme_set_am(myvme, MVME_AM_A24);

  // Set dmode to D32
  mvme_set_dmode(myvme, MVME_DMODE_D32);

  uint32_t vf48base = 0;

  int iarg = 0;

  for (int i=0; i<argc; i++)
     {
        if (strcmp(argv[i], "--exit")==0)
           {
              exit(0);
           }
        else if (strcmp(argv[i], "--addr")==0)
           {
              i++;
              vf48base = strtoul(argv[i], NULL, 0);
           }
        else if (strcmp(argv[i], "--write")==0)
           {
              i++;
              int ireg = strtoul(argv[i], NULL, 0);
              i++;
              int ival = strtoul(argv[i], NULL, 0);

              printf("Write register 0x%x value 0x%08x\n", ireg, ival);

              vme_write(myvme, vf48base, ireg, ival);
           }
        else if (strcmp(argv[i], "--read")==0)
           {
              i++;
              int ireg = strtoul(argv[i], NULL, 0);

              int ival = vme_read(myvme, vf48base, ireg);

              printf("Read register 0x%x value 0x%08x\n", ireg, ival);
           }
        else if (strcmp(argv[i], "--sleep")==0)
           {
              i++;
              int t = strtoul(argv[i], NULL, 0);
              printf("sleep %d sec\n", t);
              sleep(t);
           }
        else if (strcmp(argv[i], "--am")==0)
           {
              i++;
              int am = strtoul(argv[i], NULL, 0);
              printf("Using VME AM 0x%x\n", am);

              mvme_set_am(myvme, am);
           }
        else if (strcmp(argv[i], "--dmamode")==0)
           {
              i++;
              int d = strtoul(argv[i], NULL, 0);
              printf("Setting DMA mode %d\n", d);
              mvme_set_blt(myvme, d);
           }
        else if (strcmp(argv[i], "--dma")==0)
           {
              i++;
              int count = strtoul(argv[i], NULL, 0);

              int wcount = count/4;
              uint32_t pdata32[wcount];
              for (int i=0; i<wcount; i++)
                 pdata32[i] = 0xbaadbaad;

              int status = mvme_read(myvme, pdata32, vf48base, count);
              
              printf("mvme_read() returned %d\n", status);
              
              printf("read of %d words\n", wcount);
              for (int i=0; i<wcount; i++)
                 printf("count %d, word 0x%08x\n", i, pdata32[i]);
           }
#if 0
        else if (strcmp(argv[i], "--dataread")==0)
           {
              i++;
              int count = strtoul(argv[i], NULL, 0);

              int wcount = 100*1024;
              uint32_t pdata32[wcount];

              int wc = vf48_DataRead(myvme, vf48base, pdata32, &count);
              
              printf("vf48_DataRead() returned %d\n", wc);
              
              printf("read of %d words\n", wc);
              for (int i=0; i<count; i++)
                 printf("count %d, word 0x%08x\n", i, pdata32[i]);
           }
        else if (strcmp(argv[i], "--timedataread")==0)
           {
              i++;
              int count = strtoul(argv[i], NULL, 0);

              int wcount = 100*1024;
              uint32_t pdata32[wcount];

              for (int i=0; i<20; i++)
                 {
                    double t0 = utime();
                    int xcount = count;
                    int wc = vf48_DataRead(myvme, vf48base, pdata32, &xcount);
                    double t1 = utime();
                    printf("vf48_DataRead() returned %d in %.6f sec, %.3f Mbytes/sec\n", wc, t1-t0, wc*4/(t1-t0)/1e6);
                 }
           }
#if 0
              else if (1) // read using PIO
                 {
                    uint32_t* wptr = (uint32_t*)(pdata32+iptr);
                    mvme_set_dmode(myvme, MVME_DMODE_D32);
                    for (int i=0; i<count; i++)
                       *wptr++ = mvme_read_value(myvme, vf48base + VF48_DATA_FIFO_R);
                    wc = count;
                 }
              else // read using PIO, dump data and frame counters
                 {
                    uint32_t* wptr = (uint32_t*)(pdata32+iptr);
                    mvme_set_dmode(myvme, MVME_DMODE_D32);
                    for (int i=0; i<count; i++)
                       {
                          int nf0 = vf48_NFrameRead(myvme, vf48base);
                          mvme_set_dmode(myvme, MVME_DMODE_D32);
                          uint32_t w = mvme_read_value(myvme, vf48base + VF48_DATA_FIFO_R);
                          *wptr++ = w;
                          int nf1 = vf48_NFrameRead(myvme, vf48base);
                          printf("count %d, word 0x%08x, nf %d %d\n", i, w, nf0, nf1);
                       }
                    wc = count;
                 }
#endif
#endif
     }

  status = mvme_close(myvme);
  return 0;
}

/* emacs
 * Local Variables:
 * tab-width: 8
 * c-basic-offset: 3
 * End:
 */
