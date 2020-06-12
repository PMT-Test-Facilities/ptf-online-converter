
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

#include <sys/time.h>

extern "C" {
#include "mvmestd.h"
}

int gLrs1190base   = 0x007F0000;
MVME_INTERFACE *gVme;

double utime()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec + 0.000001*tv.tv_usec;
}

int main (int argc, char* argv[])
{
  setbuf(stdout, NULL);
  setbuf(stderr, NULL);

  int status;

  // Test under vmic   
  status = mvme_open(&gVme, 0);

  mvme_set_am(gVme, MVME_AM_A24);
  mvme_set_dmode(gVme, MVME_DMODE_D32);

  if (true)
    {
      double t0 = utime();

      printf("writing LRS1190 memory...\n");

      for (int i=0; i<0x8000; i+=4)
        mvme_write_value(gVme, gLrs1190base + i, ((i+1)<<16) | (i+0));

      double t1 = utime();

      printf("done, %f sec elapsed\n", t1-t0);

      printf("reading and comparing LRS1190 memory...\n");

      for (int i=0; i<0x8000; i+=4)
        {
          uint32_t rd = mvme_read_value(gVme, gLrs1190base + i);
          uint32_t ex = ((i+1)<<16) | (i+0);
          if (rd != ex)
            {
              printf("memory mismatch at 0x%08x: read 0x%08x, should be 0x%08x\n", i, rd, ex);
            }
        }

      double t2 = utime();
      
      printf("memory test done, %f sec elapsed!\n", t2-t1);
    }

  if (false)
    {
      for (int i=0; i<0x8000; i+=4)
        printf("LRS1190 mem 0x%04x: 0x%08x\n", i, mvme_read_value(gVme, gLrs1190base + i));
    }

  if (false)
    {
      double t0 = utime();
      double t1, t;
  
      int count = 0;
      uint32_t sum = 0;
      
      while (1)
        {
          sum = 0;
          for (int i=0; i<4000; i+=4, count++)
            {
              uint32_t rd = mvme_read_value(gVme, gLrs1190base + i);
              sum += rd;
            }
          
          t1 = utime();
          
          t = t1-t0;
          
          if (t > 5)
            break;
        }
      
      printf("A24/D32 single word read: %d reads, %f elapsed, %f reads/sec, %f usec/read, sum 0x%08x\n", count, t, count/t, 1.0e6*t/count, sum);

      return 1;
    }

  if (true)
    {
      uint32_t buf[1000];
      int xsize[] = { 
        //1000,          2000,          4000,
        1000,          2000,          4000,
        0 };
      int bmode[] = {
        //MVME_BLT_NONE,  MVME_BLT_NONE,  MVME_BLT_NONE,
        MVME_BLT_BLT32, MVME_BLT_BLT32, MVME_BLT_BLT32,
        0 };
      int fmode[] = {
        //1, 0, 2,
        1, 0, 2,
        0 };

      double rr[2];
      double tt[2];

      for (int isize=0; xsize[isize] > 0; isize++)
        {
          int size = xsize[isize];

          mvme_set_blt(gVme, bmode[isize]);
          mvme_set_am(gVme, MVME_AM_A24);

          double t0 = utime();
          double t1, t;
          
          uint32_t sum = 0;
          int count = 0;
          int bytes = 0;
          
          while (1)
            {
              sum = 0;
              int rd = size;
              status = mvme_read(gVme, buf, gLrs1190base, rd);
              if (status != MVME_SUCCESS)
                {
                  printf("mvme_read() returned %d\n", status);
                  return -1;
                }

              int rd32 = rd/4;
              for (int i=0; i<rd32; i++)
                sum += buf[i];
              
              for (int i=0; i<rd32; i++)
                printf("buf[%02d] = 0x%08x\n", i, buf[i]);
              return -1;
              
              count += 1;
              bytes += rd;
              
              t1 = utime();
              
              t = t1-t0;
              
              if (t > 5)
                break;
            }

          printf("A24/D32 block read, %5d bytes: %5d reads, %.3f elapsed, %4.0f reads/sec, %10.3f usec/read, %7.3f usec/D32, %6.3f Mbytes/sec, sum 0x%08x\n",
                 size, count, t, count/t, 1.0e6*t/count, 1.0e6*t/(bytes/4), bytes/t/1.0e6, sum);

          if (fmode[isize] == 1)
            {
              rr[0] = size;
              tt[0] = 1.0e6*t/count;
            }
          else if (fmode[isize] == 2)
            {
              rr[1] = size;
              tt[1] = 1.0e6*t/count;

              double c0, c1;

              c1 = (tt[1]-tt[0])/(rr[1]-rr[0]);
              c0 = tt[0] - rr[0]*c1;

              printf("read %f -> %f, %f -> %f, fit: %.3f usec + %.3f usec/byte\n", rr[0], tt[0], rr[1], tt[1], c0, c1);
            }

        }
    }
      
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
