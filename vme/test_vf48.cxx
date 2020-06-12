//
// ./test_vf48_gef.exe --vf48addr 0xa80000 --samples 1000 --plot 1 --loopsleep 1 --exttrig 1
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

extern "C"
{
#include "vf48.h"

#ifdef HAVE_MULTI
#include "vf48_multi.h"
#endif
}

#ifdef HAVE_MULTI
#define BOOL int
#include "midas.h"
#endif

#include "UnpackVF48.h"

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

#ifdef HAVE_ROOT

#include "TApplication.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TProfile.h"
#include "TGraph.h"

TApplication *app = NULL;

void makeApp()
{
  if (!app)
    app = new TApplication("testTTC", NULL, NULL);
}

TCanvas*gWindow;
TH1D* hchan;
TH1D* htime;

#endif // HAVE_ROOT

int vf48samples = 256;
int countEvents   = 0;
int countBadData  = 0;
int countBadUnpack  = 0;
int gCountUnpacked = 0;
int gCountOffset = 0;
int doPermitNoSamples = 0;

double loctime0 = 0;
double loctime1 = 0;
double timestamp0 = 0;
double timestamp1 = 0;

bool zdebug = false;

int gPlotChan = -1;
int gTimeChan = -1;
int gTimeChan1 = -1;
int gTimeChan2 = -1;

int gTimeOffset = 0;
int gTimeRange = 1000;

void HandleVF48event(const VF48event *e)
{
  //printf("VF48 event!\n");
  //e->PrintSummary();

#ifdef HAVE_ROOT
   if (gPlotChan >= 0) {
      makeApp();

      if (!gWindow) {
         gWindow = new TCanvas("Window", "window", 1000, 1000);
         gWindow->Clear();
         //gWindow->Divide(2, 4);

         hchan = new TH1D("hchan", "chan", vf48samples, 0, vf48samples-1);
         hchan->Draw();
      }
      
      //TH1D* hphase1 = new TH1D("hphase1", "phase1", 120, -30, 30);
      //gWindow->cd(2);
      //hphase1->Draw();

      hchan->Reset();
      for (int i=0; i<vf48samples; i++)
         hchan->SetBinContent(i+1,  e->modules[0]->channels[gPlotChan].samples[i]);
      
      gWindow->Modified();
      gWindow->Update();
   }

   if (gTimeChan >= 0) {
      makeApp();

      if (!gWindow) {
         gWindow = new TCanvas("Window", "window", 1000, 1000);
         gWindow->Clear();
         //gWindow->Divide(2, 4);

         htime = new TH1D("htime", "time", gTimeRange+1, gTimeOffset, gTimeOffset+gTimeRange);
         htime->Draw();
      }
   }

   if (gTimeChan1 >= 0) {
      makeApp();

      if (!gWindow) {
         gWindow = new TCanvas("Window", "window", 1000, 1000);
         gWindow->Clear();
         //gWindow->Divide(2, 4);

         htime = new TH1D("htime", "time", gTimeRange+1, gTimeOffset, gTimeOffset+gTimeRange);
         htime->Draw();
      }
   }
#endif

   countEvents ++;

   if (e->error)
      countBadUnpack++;

  double meanmin = 99999999999.0;
  double meanmax = 0;
  double rmsmin = 999999999999.0;
  double rmsmax = 0;

  int trigNo = -1;

  int tt1 = 0;
  int tt2 = 0;
  
  for (int i=0; i<VF48_MAX_MODULES; i++)
     if (e->modules[i])
	for (int j=0; j<VF48_MAX_CHANNELS; j++)
	   if (e->modules[i]->channels[j].channel == j)
	      {
		 trigNo = e->modules[i]->trigger;

                 if (e->modules[i]->channels[j].numSamples==0 && doPermitNoSamples)
                    {
                       // empty line
                    }
                 else if (e->modules[i]->channels[j].numSamples != vf48samples)
		    {

		       countBadData++;
		       printf("Module %d channel %d has %d, should be %d samples\n", i, j, e->modules[i]->channels[j].numSamples, vf48samples);
		       continue;
		    }

                 int nsamples = e->modules[i]->channels[j].numSamples;
                 int vv = e->modules[i]->channels[j].samples[0];
                 int dd = 0;
                 int tt = 0;
		 for (int k=1; k<nsamples; k++)
		    {
		       int v = e->modules[i]->channels[j].samples[k];
                       int d = v-vv;
                       if (d > dd) {
                          dd = d;
                          tt = k;
                       }
                       //printf("sample %d, adc %d, diff %d, dd %d, tt %d\n", k, v, d, dd, tt);
                       vv = v;
                    }

                 //if (j==1)
                 //exit(1);

                 if (gTimeChan1 >= 0 && j==gTimeChan1)
                    tt1 = tt;

                 if (gTimeChan2 >= 0 && j==gTimeChan2)
                    tt2 = tt;

                 if (gTimeChan >= 0 && j==gTimeChan) {
                    printf("mod %d, chan %d, dd %d, tt %d\n", i, j, dd, tt);
      
#ifdef HAVE_ROOT
                    htime->Fill(tt);
                    gWindow->Modified();
                    gWindow->Update();
#endif
                 }

		 double sum0 = 0;
		 double sum1 = 0;
		 double sum2 = 0;
                 int smin = 0xFFFF;
                 int smax = 0;
		 
		 for (int k=0; k<vf48samples; k++)
		    {
		       int v = e->modules[i]->channels[j].samples[k];

                       if (v < smin)
                          smin = v;
                       if (v > smax)
                          smax = v;

		       sum0 += 1;
		       sum1 += v;
		       sum2 += v*v;

		       //printf("sample %d %f\n", k, v);
		    }

		 double mean = sum1/sum0;
		 double var = sum2/sum0 - mean*mean;
		 double rms = 0;
		 if (var>0)
		    rms = sqrt(var);

                 if (e->modules[i]->channels[j].numSamples>0)
                    if (rms>10 || mean==0)
                       {
                          if (0)
                             printf("event %d, error %d, trigger %d, suspect data for module %d, channel %d: min: %d, max: %d, range: %d, mean: %f, rms %f\n", e->eventNo, e->error, trigNo, i, j, smin, smax, smax-smin, mean, rms);
                          
                          if (false && mean != 0)
                             {
                                for (int k=0; k<vf48samples; k++)
                                   {
                                      double v = e->modules[i]->channels[j].samples[k];
                                      printf("sample %d %f\n", k, v);
                                   }
                                sleep(1);
                             }
                       }

                 if (zdebug)
                    printf("event %d, error %d, trigger %d, module %d, channel %d: supp mask 0x%02x, bit 0x%02x, min: %d, max: %d, range: %d, mean: %f, rms %f\n", e->eventNo, e->error, trigNo, i, j, e->modules[i]->suppressMask[j>>3], e->modules[i]->suppressMask[j>>3]&(1<<(j&0x7)), smin, smax, smax-smin, mean, rms);

		 //printf("mean %f, rms %f\n", mean, rms);

		 if (mean < meanmin)
		    meanmin = mean;

		 if (mean > meanmax)
		    meanmax = mean;

		 if (rms < rmsmin)
		    rmsmin = rms;

		 if (rms > rmsmax)
		    rmsmax = rms;
	      }

  if (gTimeChan1 >= 0) {
     int tt = tt2 - tt1;

     printf("time %d %d, tt %d\n", tt1, tt2, tt);

#ifdef HAVE_ROOT
     htime->Fill(tt);
     gWindow->Modified();
     gWindow->Update();
#endif
  }

  printf("Event unpack: %d, error %d, trigger %d, pedestals mean: %f - %f, rms %f - %f\n", e->eventNo, e->error, trigNo, meanmin, meanmax, rmsmin, rmsmax);

  if (timestamp0 == 0)
     {
        loctime0 = utime();
        timestamp0 = e->timestamp;
     }

  loctime1 = utime();
  timestamp1 = e->timestamp;

  if (rmsmax > 10)
     {
	printf("******* Bad data? Pedestal rms is %f\n", rmsmax);
     }

  if (trigNo > 0)
     {
        gCountUnpacked ++;
  
        if (trigNo != gCountUnpacked)
           {
              countBadData++;
              printf("Bad event number: got %d, should be %d\n", trigNo, gCountUnpacked);
           }
     }

  delete e;
}

int vf48_evb(uint32_t pdata32[], int nwords)
{
   int nout = 0;
   uint32_t buf[100*1024];

   int seenHdr = 0;
   uint32_t lastEoe = 0;

   int debug = 0;

   if (0) {
      int k = 0;
      uint32_t expected = 0;
      int wc = 0;

      for (int i=0; i<nwords; i++) {
         uint32_t w = pdata32[i];
         
         if (w == 0xdeadbeef)
            continue;

         if (w == 0xdeaddead)
            continue;

         if ((w & 0xFF000000) == 0xFF000000)
            continue;

         if (wc==0)
            expected = w;

         uint32_t x = expected;

         //printf("0x%08x at %d, wc %d, x 0x%08x\n", w, i, wc, x);

         expected ++;

         wc++;

         if (w != x) {
            printf("found 0x%08x at %d/%d, wc %d, expected 0x%08x, previous 0x%08x 0x%08x (ERROR)\n", w, i, nwords, wc, x, pdata32[i-1], pdata32[i-2]);
            exit(1);
         }
      }

      return 0;
   }

   if (0) {
      int k = 0;
      uint32_t expected = 0;
      int wc = 0;

      for (int i=0; i<nwords; i++) {
         uint32_t w = pdata32[i];
         
         if (w == 0xdeadbeef)
            continue;

         if (w == 0xdeaddead)
            continue;

         if (wc==0)
            expected = w;

         //printf("0x%08x at %d, wc %d\n", w, i, wc);

         uint32_t x = expected;

         expected ++;

         wc++;

         if (w != x) {
            printf("found 0x%08x at %d/%d, wc %d, expected 0x%08x, previous 0x%08x 0x%08x (ERROR)\n", w, i, nwords, wc, x, pdata32[i-1], pdata32[i-2]);
            exit(1);
         }
      }

      return 0;
   }

   if (0) {
      int k = 0;
      uint32_t expected = 0;
      int wc = 0;

      for (int i=0; i<nwords; i++) {
         uint32_t w = pdata32[i];
         
         if (w == 0xdeadbeef)
            continue;

         if (w == 0xdeaddead)
            continue;

         //printf("0x%08x at %d, wc %d\n", w, i, wc);

         if (wc==1)
            expected = w;

         uint32_t x = 0;

         if (wc&1) {
            x = expected;
            expected ++;
         }

         wc++;

         if (w != x) {
            printf("found 0x%08x at %d/%d, wc %d, expected 0x%08x, previous 0x%08x 0x%08x (ERROR)\n", w, i, nwords, wc, x, pdata32[i-1], pdata32[i-2]);
            exit(1);
         }
      }

      return 0;
   }

   if (0) {
      int k = 0;

      for (int i=0; i<nwords; i++) {
         uint32_t w = pdata32[i];
         
         //if (w == (0xFFFFFFF0 | grp)) // skip repeated markers for the group we ar elooking for
         //continue;
         
         uint32_t mark = w & 0xF0000000;
         
         if (mark == 0xD0000000) {
            printf("0x%08x at %d\n", w, i);
         }

         if (mark == 0xF0000000) {
            printf("0x%08x at %d\n", pdata32[i-1], i-1);
            printf("0x%08x at %d, %d words\n", w, i, i-k);
            printf("0x%08x at %d\n", pdata32[i+1], i+1);
            k = i;
         }
      }
   }

   for (int grp=0; grp<6; grp++)
      {
         int chan = -1;
         int nsamples = 0;
         int offset = 0;
         int wc = 0;
         int expected = 0;

         for (int i=0; i<nwords; i++)
            {
               uint32_t w = pdata32[i];

               if (grp == 0) {
                  if (w == 0xdeaddead) {
                     buf[nout++] = w;
                     continue;
                  }

                  if (w == 0xdeadbeef) {
                     buf[nout++] = w;
                     continue;
                  }
               }
                  
               if (w != (0xFFFFFFF0 | grp)) {
                  //if (grp==0)
                  //   printf("at %d: skip 0x%08x at %d\n", i, w, nout);
                  continue;
               }

               //printf("group %d, found 0x%x at %d\n", grp, w, i);

               for (; i<nwords; i++) {
                  w = pdata32[i];

                  if (w == (0xFFFFFFF0 | grp)) { // skip duplicate markers for the group we are looking for
                     if (debug)
                        printf("at %d: insert 0x%08x at %d\n", i, w, nout);
                     buf[nout++] = w;
                     continue;
                  }

                  uint32_t mark = w & 0xF0000000;

                  if (mark == 0xF0000000) // mark for a different group, get out
                     {
                        if (debug)
                           printf("0x%08x at %d\n", w, i);
                        buf[nout++] = w;
                        break;
                     }

                  if (0) {
                     if (wc == 0)
                        expected = w;

                     //printf("group %d, found 0x%08x at %d/%d, wc %d, expected 0x%08x\n", grp, w, i, nwords, wc, expected);

                     if (w == 0xdeadbeef)
                        continue;

                     if (w == 0xdeaddead) {
                        if (debug)
                           printf("next 0x%08x 0x%08x 0x%08x\n", pdata32[i+1], pdata32[i+2], pdata32[i+3]);
                        continue;
                     }

                     if (w != expected) {
                        printf("group %d, found 0x%08x at %d/%d, wc %d, expected 0x%08x, previous 0x%08x (ERROR)\n", grp, w, i, nwords, wc, expected, pdata32[i-1]);
                        return 0;
                        exit(1);
                     }

                     wc ++;
                     expected ++;

                     continue;
                  }

                  wc ++; // count only normal data words received from link fifo

                  if (mark == 0x80000000) { // begin of event marker
                     if (debug)
                        printf("at %d got 0x%08x at %d\n", i, w, nout);
                     if (seenHdr)
                        continue;
                     else
                        seenHdr = 1;
                  }

                  if (mark == 0xE0000000) // end of event marker
                     {
                        if (debug)
                           printf("at %d got 0x%08x at %d\n", i, w, nout);
                        lastEoe = w;
                        continue;
                     }

                  if (mark == 0xC0000000) { // channel marker - add group id
                     w |= (grp<<4);

                     if (debug)
                        printf("channel %d had %d samples\n", chan, nsamples);

                     chan = w & 0x7;
                     nsamples = 0;
                  }

                  if (mark == 0xD0000000) { // DMA marker
                     if (debug)
                        printf("at %d got 0x%08x at %d\n", i, w, nout);
                     wc --; // uncount dma markers - they did not come from link fifo
                  }

                  if (mark == 0xA0000000) { // timestamp marker
                     if (debug)
                        printf("at %d got 0x%08x at %d\n", i, w, nout);
                  }

                  if (mark == 0x00000000) { // adc samples

                     if (nsamples == 0)
                        offset = w & 0xFFF;

                     if (debug) {
                        uint32_t expected = ((offset + nsamples + 1)<<16) | (offset + nsamples + 0);
                        expected &= 0x03FF03FF;
                        if (w != expected)
                           printf("at %d: expected 0x%08x, got 0x%08x at %d, addr 0x%p, sample count %d\n", i, expected, w, nout, pdata32+i, nsamples);
                     }

                     nsamples += 2;
                  }

                  buf[nout++] = w;
               }
            }

         if (debug)
            printf("group %d wc %d, channel %d had %d samples\n", grp, wc, chan, nsamples);
      }

   if (lastEoe)
      buf[nout++] = lastEoe;

   if (nout == 0)
      return nwords;

   memcpy(pdata32, buf, 4*nout);

   return nout;
}

int main(int argc, char* argv[])
{
  setbuf(stdout, NULL);
  setbuf(stderr, NULL);

  int vf48base = 0xa00000;

  int extTrig = 0;

  MVME_INTERFACE *myvme;

  // Test under vmic   
  int status = mvme_open(&myvme, 0);

  // Set am to A24 non-privileged Data
  mvme_set_am(myvme, MVME_AM_A24);

  // Set dmode to D32
  mvme_set_dmode(myvme, MVME_DMODE_D32);

  int nloops = 200;

  bool xdebug = false;
  bool ydebug = false;

  int suppEnabled = 0;
  int suppThreshold = 10;
  int trigThreshold = 1000;

  int mbits1 = -1;
  int mbits2 = -1;

  int divisor = -1;

  int grpEnabled = 0x3F;
  //int grpEnabled = 0x1F;
  //int grpEnabled = (1<<5);
  //int grpEnabled = 0x3F & ~(1<<5);
  //int grpEnabled = (1<<3)|(1<<5); // 0x3F & ~(1<<1) & ~(1<<3) & ~(1<<4);

  int doReadPio = 0;
  int doReadSync = 0;
  int doReadMulti = 0;
  int doParamTest = 1;

  int chanEnabled = VF48_ALL_CHANNELS_ACTIVE;
  //int chanEnabled = 1<<1;


  bool noreset = false;
  bool once = false;

  int loopsleep = 0;

  int latency = 60;
  int pretrigger = 30;

  int iarg = 0;

  for (int i=0; i<argc; i++)
     {
        if (strcmp(argv[i], "--exit")==0)
           {
              exit(0);
           }
        else if (strcmp(argv[i], "--vf48addr")==0)
           {
              i++;
              vf48base = strtoul(argv[i], NULL, 0);
           }
        else if (strcmp(argv[i], "--addr")==0)
           {
              i++;
              vf48base = strtoul(argv[i], NULL, 0);
           }
        else if (strcmp(argv[i], "--samples")==0)
           {
              i++;
              vf48samples = strtoul(argv[i],NULL,0);

              for (int j=0; j<6; j++)
                 if (grpEnabled&(1<<j))
                    vf48_ParameterWrite(myvme, vf48base, j, VF48_SEGMENT_SIZE, vf48samples);
           }
        else if (strcmp(argv[i], "--groups")==0)
           {
              i++;
              grpEnabled = strtoul(argv[i],NULL,0);
              vf48_GrpEnable(myvme, vf48base, grpEnabled);
           }
        else if (strcmp(argv[i], "--channels")==0)
           {
              i++;
              chanEnabled = strtoul(argv[i],NULL,0);

              for (int j=0;j<6;j++)
                 if (grpEnabled&(1<<j))
                    vf48_ParameterWrite(myvme, vf48base, j, VF48_ACTIVE_CH_MASK, chanEnabled);
           }
        else if (strcmp(argv[i], "--exttrig")==0)
           {
              i++;
              extTrig = strtoul(argv[i], NULL, 0);
           }
        else if (strcmp(argv[i], "--nloops")==0)
           {
              i++;
              nloops = strtoul(argv[i], NULL, 0);
           }
        else if (strcmp(argv[i], "--loopsleep")==0)
           {
              i++;
              loopsleep = strtoul(argv[i], NULL, 0);
           }
        else if (strcmp(argv[i], "--mbits1")==0)
           {
              i++;
              mbits1 = strtoul(argv[i], NULL, 0);
           }
        else if (strcmp(argv[i], "--mbits2")==0)
           {
              i++;
              mbits2 = strtoul(argv[i], NULL, 0);
           }
        else if (strcmp(argv[i], "--divisor")==0)
           {
              i++;
              divisor = strtoul(argv[i], NULL, 0);
              vf48_DivisorWrite(myvme, vf48base, divisor);
           }
        else if (strcmp(argv[i], "--latency")==0)
           {
              i++;
              latency = strtoul(argv[i], NULL, 0);
           }
        else if (strcmp(argv[i], "--pretrigger")==0)
           {
              i++;
              pretrigger = strtoul(argv[i], NULL, 0);
           }
        else if (strcmp(argv[i], "--trigger")==0)
           {
              vf48_Trigger(myvme, vf48base);
           }
        else if (strcmp(argv[i], "--threshold")==0)
           {
              i++;
              suppThreshold = strtoul(argv[i], NULL, 0);
           }
        else if (strcmp(argv[i], "--trigThreshold")==0)
           {
              i++;
              trigThreshold = strtoul(argv[i], NULL, 0);
           }
        else if (strcmp(argv[i], "--suppress")==0)
           {
              suppEnabled = 1;
              i++;
              suppThreshold = strtoul(argv[i], NULL, 0);

              for (int j=0; j<6; j++)
                 if (grpEnabled&(1<<j))
                    vf48_ChSuppSet(myvme, vf48base, j, suppEnabled);

              for (int j=0; j<6; j++)
                 if (grpEnabled&(1<<j))
                    vf48_ParameterWrite(myvme, vf48base, j, VF48_HIT_THRESHOLD, suppThreshold);
           }
        else if (strcmp(argv[i], "--parameterRead")==0)
           {
              i++;
              int grp = strtoul(argv[i], NULL, 0);
              i++;
              int ipar = strtoul(argv[i], NULL, 0);

              int rd = vf48_ParameterRead(myvme, vf48base, grp, ipar);

              printf("Read group %d, parameter %d: 0x%x (%d)\n", grp, ipar, rd, rd);
           }
        else if (strcmp(argv[i], "--testParameterRead")==0)
           {
              i++;
              int grp = strtoul(argv[i], NULL, 0);
              i++;
              int ipar = strtoul(argv[i], NULL, 0);

              int value = 0;
              for (int count=0; ; count++) {
                 int rd = vf48_ParameterRead(myvme, vf48base, grp, ipar);
                 if (rd != value)
                    printf("Count %d, Read group %d, parameter %d: 0x%04x (%d)\n", count, grp, ipar, rd, rd);
                 value = rd;
              }
           }
        else if (strcmp(argv[i], "--parameterWrite")==0)
           {
              i++;
              int grp = strtoul(argv[i], NULL, 0);
              i++;
              int ipar = strtoul(argv[i], NULL, 0);
              i++;
              int data = strtoul(argv[i], NULL, 0);

              printf("Write group %d, parameter %d, data 0x%x (%d)\n", grp, ipar, data, data);

              vf48_ParameterWrite(myvme, vf48base, grp, ipar, data);
           }
        else if (strcmp(argv[i], "--regWrite")==0)
           {
              i++;
              int ireg = strtoul(argv[i], NULL, 0);
              i++;
              int ival = strtoul(argv[i], NULL, 0);

              printf("Write register 0x%x value 0x%08x\n", ireg, ival);

              vf48_RegisterWrite(myvme, vf48base, ireg, ival);
           }
        else if (strcmp(argv[i], "--regRead")==0)
           {
              i++;
              int ireg = strtoul(argv[i], NULL, 0);

              int ival = vf48_RegisterRead(myvme, vf48base, ireg);

              printf("Read register 0x%x value 0x%08x\n", ireg, ival);
           }
        else if (strcmp(argv[i], "--status")==0)
           {
              vf48_Status(myvme, vf48base);
           }
        else if (strcmp(argv[i], "--noparamtest")==0)
           {
              doParamTest = 0;
           }
        else if (strcmp(argv[i], "--xdebug")==0)
           {
              xdebug = true;
           }
        else if (strcmp(argv[i], "--ydebug")==0)
           {
              ydebug = true;
           }
        else if (strcmp(argv[i], "--zdebug")==0)
           {
              zdebug = true;
           }
        else if (strcmp(argv[i], "--sleep")==0)
           {
              i++;
              int t = strtoul(argv[i], NULL, 0);
              printf("sleep %d sec\n", t);
              sleep(t);
           }
        else if (strcmp(argv[i], "--plot")==0)
           {
              i++;
              gPlotChan = strtoul(argv[i], NULL, 0);
           }
        else if (strcmp(argv[i], "--timeoffset")==0)
           {
              i++;
              gTimeOffset = strtoul(argv[i], NULL, 0);
           }
        else if (strcmp(argv[i], "--timerange")==0)
           {
              i++;
              gTimeRange = strtoul(argv[i], NULL, 0);
           }
        else if (strcmp(argv[i], "--time")==0)
           {
              i++;
              gTimeChan = strtoul(argv[i], NULL, 0);
           }
        else if (strcmp(argv[i], "--time2")==0)
           {
              i++;
              gTimeChan1 = strtoul(argv[i], NULL, 0);
              i++;
              gTimeChan2 = strtoul(argv[i], NULL, 0);
           }
        else if (strcmp(argv[i], "--testReg")==0)
           {
              int count = 0;
              mvme_set_dmode(myvme, MVME_DMODE_D32);
              for (int w=0; w<0xFFFF; w++)
                 {
                    mvme_write_value(myvme, vf48base + VF48_TEST_REG_RW, w);
                    int r = mvme_read_value(myvme, vf48base + VF48_TEST_REG_RW);
                    if (r != w)
                       {
                          count++;
                          printf("Write 0x%08x, read 0x%08x, diff 0x%08x, count %d\n", w, r, r^w, count);
                       }
                 }
                    
              printf("Test register test %d errors\n", count);
              exit(1);
           }
        else if (strcmp(argv[i], "--testSlowReset")==0)
           {
              vf48_GrpEnable(myvme, vf48base, 0);
              sleep(1);
              vf48_GrpEnable(myvme, vf48base, grpEnabled);
              sleep(1);
              vf48_Status(myvme, vf48base);
              exit(1);
           }
        else if (strcmp(argv[i], "--once")==0)
           {
              once = true;
           }
        else if (strcmp(argv[i], "--noreset")==0)
           {
              noreset = true;
           }
        else if (strcmp(argv[i], "--reconfigure")==0)
           {
              vf48_ReconfigureFPGA(myvme, vf48base);
           }
        else if (strcmp(argv[i], "--reset")==0)
           {
              vf48_Reset(myvme, vf48base);
           }
        else if (strcmp(argv[i], "--resetFrontends")==0)
           {
              vf48_ResetFrontends(myvme, vf48base, 0x3F);
           }
        else if (strcmp(argv[i], "--resetCollector")==0)
           {
              vf48_ResetCollector(myvme, vf48base);
           }
        else if (strcmp(argv[i], "--start")==0)
           {
              vf48_AcqStart(myvme, vf48base);
           }
        else if (strcmp(argv[i], "--stop")==0)
           {
              vf48_AcqStop(myvme, vf48base);
           }
        else if (strcmp(argv[i], "--testBusy")==0)
           {
              printf("Testing VF48 busy!\n");
              int csr = vf48_CsrRead(myvme, vf48base);
              printf("CSR 0x%08x: before trigger\n", csr);
              vf48_Trigger(myvme, vf48base);
              while (1)
                 {
                    int xcsr = vf48_CsrRead(myvme, vf48base);
                    if (xcsr != csr)
                       printf("CSR 0x%08x: after trigger\n", xcsr);
                    csr = xcsr;
                 }
              exit(0);
           }
        else if (strcmp(argv[i], "--testFifo")==0)
           {
              printf("Testing VF48 fifo!\n");

              int csr = vf48_CsrRead(myvme, vf48base);
              int nframe = vf48_NFrameRead(myvme, vf48base);

              printf("CSR 0x%08x, NFrame 0x%08x: before trigger\n", csr, nframe);

              vf48_Trigger(myvme, vf48base);

              for (int i=0; ; i++)
                 {
                    int xcsr = vf48_CsrRead(myvme, vf48base);
                    int xnframe = vf48_NFrameRead(myvme, vf48base);
                    if (xcsr != csr || xnframe != nframe)
                       printf("CSR 0x%08x, NFrame 0x%08x: after trigger, loop %d\n", xcsr, xnframe, i);
                    csr = xcsr;
                    nframe = xnframe;
                 }
              exit(0);
           }
        else if (strcmp(argv[i], "--csr")==0)
           {
              int csr = vf48_CsrRead(myvme, vf48base);
              printf("CSR 0x%x\n", csr);
           }
        else if (strcmp(argv[i], "--eventreadpio")==0)
           {
              doReadPio = 1;
              printf("Will read VF48 using PIO from FE FIFOs\n");
           }
        else if (strcmp(argv[i], "--eventreadsync")==0)
           {
              doReadSync = 1;
              printf("Will read VF48 using vf48_EventReadSync()\n");
           }
        else if (strcmp(argv[i], "--eventreadmulti")==0)
           {
              doReadMulti = 1;
              printf("Will read VF48 using vf48_multi\n");
           }
        else if (strcmp(argv[i], "--pioread")==0)
           {
              int wcount = 100*1024;
              uint32_t pdata32[wcount];
              int wc = wcount;

              status = vf48_EventReadPio(myvme, vf48base, pdata32, &wc);

              printf("read of %d words returned status %d, wc %d\n", wcount, status, wc);
              for (int i=0; i<wc; i++)
                 printf("count %d, word 0x%08x\n", i, pdata32[i]);
           }
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
        else if (iarg==0)
           {
              iarg++;
           }
        else if (iarg==1)
           {
              iarg++;
              vf48base = strtoul(argv[i],NULL,0);
           }
        else if (iarg==2)
           {
              iarg++;
              vf48samples = strtoul(argv[i],NULL,0);
           }
        else if (iarg==3)
           {
              iarg++;
              extTrig = strtoul(argv[i],NULL,0);
           }
     }

#if 0
  if (argc>1)
     vf48base = strtoul(argv[1],NULL,0);
  
  if (argc>2)
     vf48samples = strtoul(argv[2],NULL,0);
  
  if (argc>3)
     extTrig = strtoul(argv[3],NULL,0);
#endif
  
  printf("Setting up the VF48 at 0x%x!\n", vf48base);

  if (!noreset)
     {
        status = vf48_Reset(myvme, vf48base);
        if (status != VF48_SUCCESS)
           {
              printf("Cannot reset VF48 at 0x%x\n", vf48base);
              exit(1);
           }
        sleep(1);
     }

  //status = vf48_Trigger(myvme, vf48base);
  //exit(123);

  vf48_AcqStop(myvme, vf48base);

  int param_status = 0;

  if (doParamTest)
     {
        int countReadFail = 0;
        int countWriteFail = 0;

        printf("Testing parameter read!\n");

        for (int grp=0; grp<6; grp++)
           {
              int count = 0;
              
              time_t t0 = time(NULL);
              
              for (int iloop=0; count<10; iloop++)
                 {
                    static int expectedValue = 0;
                    
                    int csr = mvme_read_value(myvme, vf48base+VF48_CSR_REG_RW);
                    if (csr & VF48_CSR_START_ACQ)
                       {
                          printf("vf48 is running!\n");
                          exit(1);
                       }
                    
                    int ipar = VF48_SEGMENT_SIZE;
                    int rd = vf48_ParameterRead(myvme, vf48base, grp, ipar);
                    
                    if (iloop == 0 && rd >= 0)
                       {
                          expectedValue = rd;
                          printf("Group %d, parameter %d value %d 0x%04x\n", grp, ipar, rd, rd);
                       }
                    
                    if (rd != expectedValue)
                       {
                          printf("Group %d, Data mismatch error: read %6d instead of %6d (0x%04x vs 0x%04x, diff 0x%04x)\n", grp, rd, expectedValue, rd, expectedValue, rd^expectedValue);
                          countReadFail++;
                          count++;
                       }
                    
                    //sleep(1);
                    
                    if ((iloop%1000)==0) {
                       time_t now = time(NULL);
                       //printf("%d %d\n", iloop, now-t0);
                       if (now - t0 > 1) // run the test for so many seconds
                          break;
                    }
                 }
           }

        printf("Testing parameter write!\n");
        
        for (int grp=0; grp<6; grp++)
           {
              int count = 0;
              
              for (int value=10; value<100 && count<10; value+=2)
                 {
                    int wr = vf48_ParameterWrite(myvme, vf48base, grp, VF48_SEGMENT_SIZE, value);
                    int rd1 = vf48_ParameterRead(myvme, vf48base, grp, VF48_SEGMENT_SIZE);
                    int rd2 = vf48_ParameterRead(myvme, vf48base, grp, VF48_SEGMENT_SIZE);
                    //printf("group %d, write %d, read %d\n", j, value, rd);
                    if (rd1 != rd2)
                       {
                          printf("Group %d, Data mismatch error: wrote %6d, read %6d and %6d (0x%04x and 0x%04x, diff 0x%04x)\n", grp, value, rd1, rd2, rd1, rd2, rd1^rd2);
                          countReadFail++;
                          count++;
                       }
                    else if (rd1 != value)
                       {
                          printf("Group %d, Data mismatch error: wrote %6d, read %6d (0x%04x vs 0x%04x, diff 0x%04x)\n", grp, value, rd1, value, rd1, value^rd1);
                          countWriteFail++;
                          count++;
                       }
                    //sleep(1);
                 }
           }
        
        if (countReadFail==0 && countWriteFail==0)
           {
              printf("Parameter read/write tests passed!\n");
              param_status = 0;
           }
        else
           {
              printf("Parameter read/write tests failed: %d read errors, %d write errors!\n", countReadFail, countWriteFail);
              sleep(1);
              //exit(1);
              param_status =  countReadFail + countWriteFail;
           }
     }

  vf48_Status(myvme, vf48base);

  sleep(1);

  int countCycles   = 0;
  int countGood     = 0;
  int countTimeouts = 0;
  int countReadErrors = 0;
  int countBabbling = 0;

#ifdef HAVE_MULTI
  static vf48_multi_buffer multi_buffer;

  if (doReadMulti) {
     vf48_multi_init(myvme, &multi_buffer, 0, vf48base, "AAAA");
  }
#endif

  while (1)
    {
    reset:
       
      if (!noreset)
         {
            vf48_Reset(myvme, vf48base);
            
            gCountUnpacked = 0;
            gCountOffset = 0;
         }
      else
         {
            gCountOffset = gCountUnpacked;
         }

      //sleep(1);

      vf48_AcqStop(myvme, vf48base);

      if (chanEnabled != VF48_ALL_CHANNELS_ACTIVE || suppEnabled)
         doPermitNoSamples = 1;

      vf48_GrpEnable(myvme, vf48base, grpEnabled);

      for (int j=0;j<6;j++)
        if (grpEnabled&(1<<j))
          vf48_ParameterWrite(myvme, vf48base, j, VF48_ACTIVE_CH_MASK, chanEnabled);

      for (int j=0; j<6; j++)
        if (grpEnabled&(1<<j))
          vf48_ParameterWrite(myvme, vf48base, j, VF48_SEGMENT_SIZE, vf48samples);

      for (int j=0; j<6; j++)
        if (grpEnabled&(1<<j))
          vf48_ParameterWrite(myvme, vf48base, j, VF48_PRE_TRIGGER, pretrigger);
      
      for (int j=0; j<6; j++)
        if (grpEnabled&(1<<j))
          vf48_ParameterWrite(myvme, vf48base, j, VF48_LATENCY, latency);
      
      for (int j=0; j<6; j++)
         if (grpEnabled&(1<<j))
            vf48_ChSuppSet(myvme, vf48base, j, suppEnabled);

      for (int j=0; j<6; j++)
         if (grpEnabled&(1<<j))
            vf48_ParameterWrite(myvme, vf48base, j, VF48_HIT_THRESHOLD, suppThreshold);

      if (mbits1 >= 0)
         for (int j=0; j<6; j++)
            if (grpEnabled&(1<<j))
               vf48_ParameterWrite(myvme, vf48base, j, VF48_MBIT1, mbits1);
      
      if (mbits2 >= 0)
         for (int j=0; j<6; j++)
            if (grpEnabled&(1<<j))
               vf48_ParameterWrite(myvme, vf48base, j, VF48_MBIT2, mbits2);

      if (trigThreshold >= 0)
         for (int j=0; j<6; j++)
            if (grpEnabled&(1<<j))
               vf48_ParameterWrite(myvme, vf48base, j, VF48_TRIG_THRESHOLD, trigThreshold);

      printf("suppThreshold %d, trigThreshold %d, mbits1: 0x%04x, mbits2: 0x%04x\n", suppThreshold, trigThreshold, mbits1, mbits2);

      if (divisor > 0)
        vf48_DivisorWrite(myvme, vf48base, divisor);
  
      vf48_ExtTrgSet(myvme, vf48base);

      if (doReadPio) {
         vf48_CsrWrite(myvme, vf48base, vf48_CsrRead(myvme, vf48base) | VF48_CSR_FIFO_DISABLE);
      }
      
      printf("VF48[%d] at VME A24 0x%06x: Firmware: 0x%x, CSR: 0x%x\n",
             0,
             vf48base,
             vf48_ParameterRead(myvme, vf48base, 0, VF48_FIRMWARE_ID),
             vf48_CsrRead(myvme, vf48base));

      vf48_Status(myvme, vf48base);

      vf48_AcqStart(myvme, vf48base);

      printf("Cycles: %d, good: %d (%.0f%%), timeouts: %d (%.0f%%), babbling: %d (%.0f%%), Events: %d, with read errors: %d (%.0f%%), with unpack errors: %d (%.0f%%), bad data: %d, parameter access errors: %d\n",
             countCycles,
             countGood,     100.0*(double)countGood/(double)countCycles,
             countTimeouts, 100.0*(double)countTimeouts/(double)countCycles,
             countBabbling, 100.0*(double)countBabbling/(double)countCycles,
             countEvents,
             countReadErrors, 100.0*(double)countReadErrors/(double)countEvents,
             countBadUnpack, 100.0*(double)countBadUnpack/(double)countEvents,
             countBadData,
             param_status
             );

      countCycles++;

      timestamp0 = 0;
      timestamp1 = 0;

      bool have_fifo_incomplete = false;
      DWORD fwrev = vf48_RegisterRead(myvme, vf48base, VF48_FIRMWARE_R);
      if ((fwrev&0xFF000000)==0x07000000) 
         if (fwrev >= 0x07110726)
            have_fifo_incomplete = true;

      VF48event::SetNumModules(1);
      VF48event::SetTrigWarning(1);
      VF48event::BeginRun(grpEnabled, vf48samples, 2);

      sleep(1);

      double tstart = utime();
      double bstart = 0;

      int unit = 0;
      int countTrigger = 0;

      for (int iloop=0; iloop<nloops; iloop++)
        {
        again:
          // pulse v513 NIM output 0

          //printf("Pulse!\n");

          if (extTrig)
            {
               //printf("Ext Pulse!\n");
            }
          else if (1)
             {
                //usleep(100000);
                vf48_Trigger(myvme, vf48base);
                countTrigger++;
             }
          else if (iloop==0)
             {
                // trigger once
                vf48_Trigger(myvme, vf48base);
                countTrigger++;
             }
          
          //sleep(1);
          
          // wait until the VF48 has data to read
          
          double tt = utime();
          while (1)
            {
              int csr = vf48_CsrRead(myvme, vf48base);
              int haveData = vf48_NFrameRead(myvme, vf48base);
              
              //printf("VF48 waiting for data: csr 0x%08x, nframe %6d\n", csr, haveData);

              int fifo_not_empty = ((csr & VF48_CSR_FIFO_EMPTY) == 0);

              if (fifo_not_empty && haveData > 10)
                break;
              
              if (doReadPio && haveData > 10)
                break;

              if (have_fifo_incomplete)
                 if ((csr & VF48_CSR_FIFO_INCOMPLETE) == 0)
                    if (fifo_not_empty)
                       break;

              //usleep(10);
              
              double t = utime() - tt;
              if (t < 5)
                continue;
              
              printf("Timeout waiting for data, csr 0x%x, nframe %d!\n", vf48_CsrRead(myvme, vf48base), vf48_NFrameRead(myvme, vf48base));

              vf48_Status(myvme, vf48base);

              //exit(123);
              //vf48_Reset(myvme, vf48base);

              countTimeouts ++;

              //if (countTimeouts>2)
              //goto again;

              goto reset;
            }
          
          // read the data
          
          const int kSize32 = 100000;
          uint32_t pdata32[kSize32];
          int iptr = 0;
          
          double t0 = utime();
          double xt = 0;
          
          //sleep(1);

          if (doReadPio) {
             iptr = kSize32;
             status = vf48_EventReadPio(myvme, vf48base, pdata32, &iptr);
             //printf("vf48_EventReadPio() returned status %d, wc %d\n", status, iptr);

             double t1 = utime();
          
             double t = t1 - t0;
          
             //printf("time %f %f %f\n", t0, t1, t);
          
             double tnow = utime() - tstart;
             bstart += 4*iptr;
          
             printf("Event read PIO: %d words in %f sec, %6.3f Mbytes/sec per event, %6.3f Mbytes/sec sustained!\n", iptr, t, 4*iptr/t/1000000.0, bstart/tnow/1000000.0);

          } else if (doReadSync) {

             iptr = kSize32;
             status = vf48_EventReadSync(myvme, vf48base, pdata32, &iptr);
             //printf("vf48_EventReadSync() returned status %d, wc %d\n", status, iptr);

             double t1 = utime();
          
             double t = t1 - t0;
          
             //printf("time %f %f %f\n", t0, t1, t);
          
             double tnow = utime() - tstart;
             bstart += 4*iptr;
          
             printf("Event read Sync: %d words in %f sec, %6.3f Mbytes/sec per event, %6.3f Mbytes/sec, %.0f evt/s sustained!\n", iptr, t, 4*iptr/t/1000000.0, bstart/tnow/1000000.0, (iloop+1)/tnow);

#ifdef HAVE_MULTI
          } else if (doReadMulti) {

             bk_init32((char*)pdata32);

             status = vf48_read_event(myvme, &multi_buffer, (char*)pdata32, 4*kSize32, iloop+1);
             //printf("vf48_read_event() returned status %d, wc %d\n", status, iptr);

             iptr = 5+status/4;

             // kill MIDAS event and bank headers

             pdata32[0] = 0xdeaddead;
             pdata32[1] = 0xdeaddead;
             pdata32[2] = 0xdeaddead;
             pdata32[3] = 0xdeaddead;
             pdata32[4] = 0xdeaddead;

             double t1 = utime();
          
             double t = t1 - t0;
          
             //printf("time %f %f %f\n", t0, t1, t);
          
             double tnow = utime() - tstart;
             bstart += 4*iptr;
          
             printf("Event read multi: %d words in %f sec, %6.3f Mbytes/sec per event, %6.3f Mbytes/sec, %.0f evt/s sustained!\n", iptr, t, 4*iptr/t/1000000.0, bstart/tnow/1000000.0, (iloop+1)/tnow);

#endif
          } else if (1) {
             int nread = 0;
             while (1)
                {
                   int csr = vf48_CsrRead(myvme, vf48base);
                   int haveData = vf48_NFrameRead(myvme, vf48base);

                   //printf("VF48 csr 0x%08x, has data: %6d, iptr: %6d, xt: %f\n", csr, haveData, iptr, xt);
                   //vf48_Status(myvme, vf48base);

                   if (haveData == 0)
                      break;

                   bool read_last_data = false;

                   if (have_fifo_incomplete)
                      if ((csr & VF48_CSR_FIFO_INCOMPLETE)==0)
                         read_last_data = true;
              
                   if (!read_last_data && haveData < 2000)
                      {
                         int xframe = haveData;
                         int nframe;
                         double tt0 = utime();
                         double tt00 = tt0;
                         // delay 10 usec
                         for (int i=0; i<100; i++)
                            {
                               for (int j=0; j<10; j++)
                                  csr = vf48_CsrRead(myvme, vf48base);

                               if (have_fifo_incomplete)
                                  if ((csr & VF48_CSR_FIFO_INCOMPLETE)==0)
                                     read_last_data = true;
              
                               nframe = vf48_NFrameRead(myvme, vf48base);

                               if (read_last_data || nframe > 2000)
                                  break;

                               double now = utime();

                               if (nframe != xframe)
                                  {
                                     //printf("VF48 has data: %6d, new: %6d, time %f\n", haveData, nframe, now-tt0);

                                     xframe = nframe;
                                     tt0 = now;
                                     continue;
                                  }

                               if (now - tt0 > 0.000200)
                                  break;
                            }
                         
                         haveData = nframe;

                         //printf("VF48 has data: %6d, wait time %f\n", haveData, utime()-tt00);
                      }

                   if (0)
                      {
                         vf48_Status(myvme, vf48base);
                         vf48_Reset(myvme, vf48base);
                         vf48_Status(myvme, vf48base);
                         exit(1);
                      }

#if 0
                   int count = haveData&~0x1;
                   //int count = haveData;
                   //if (count&1)
                   //   count += 1;

                   if (haveData == 2)
                      count = 4;
#endif
                   int count = haveData&~0xF;

                   //if (haveData > 2000)
                   //count = 10000;
            
                   if (read_last_data || haveData < 2000)
                      count += 16;
              
                   csr = vf48_CsrRead(myvme, vf48base);

                   //printf("VF48 csr 0x%08x, haveData %d, count %d\n", csr, haveData, count);

                   if (count == 0)
                      continue;

                   if (iptr + count + 10 > kSize32)
                      {
                         printf("VF48 is babbling, nframe %d!\n", vf48_NFrameRead(myvme, vf48base));
                         vf48_AcqStop(myvme, vf48base);
                         vf48_Status(myvme, vf48base);
                         countBabbling++;
                         goto reset;
                      }

                   while (0)
                      {
                         int nf = vf48_NFrameRead(myvme, vf48base);
                         mvme_set_dmode(myvme, MVME_DMODE_D32);
                         uint32_t dd = mvme_read_value(myvme, vf48base + VF48_DATA_FIFO_R);
                         printf("nframe %d, data 0x%08x\n", nf, dd);
                         sleep(1);
                      }
	      
              
                   double xt0 = utime();
              
                   //printf("VF48 has data: %6d, iptr: %6d, count: %d, xt: %f\n", haveData, iptr, count, xt);

                   nread++;

                   int wc = 0;

                   if (1) // read using DMA
                      {
                         //printf("vf48_DataRead() called with iptr %d, count %d...\n", iptr, count);

                         //pdata32[iptr++] = 0xdeadbeef;
                         //pdata32[iptr++] = 0xdeadbeef;

                         while (((uint64_t)(pdata32+iptr)&0xf) != 0)
                            pdata32[iptr++] = 0xdeadbeef;

                         //printf("DMA to %p, wc %d\n", pdata32+iptr, count);

                         wc = vf48_DataRead(myvme, vf48base, pdata32+iptr, &count);
                         if (wc != count)
                            {
                               printf("Error reading VF48 data!\n");
                               countReadErrors++;
                               //exit(1);
                            }

                         //printf("vf48_DataRead() returned %d\n", wc);

                         if (0)
                            {
                               printf("read of %d words\n", count);
                               for (int i=0; i<count; i++)
                                  printf("count %d, word 0x%08x\n", i, pdata32[iptr+i]);
                            }
                      }
                   else if (0) // read using PIO
                      {
                         uint32_t* wptr = (uint32_t*)(pdata32+iptr);
                         mvme_set_dmode(myvme, MVME_DMODE_D32);
                         for (int i=0; i<count; i++)
                            *wptr++ = mvme_read_value(myvme, vf48base + VF48_DATA_FIFO_R);
                         wc = count;
                      }
                   else if (1) // read using PIO access to FE FIFOs
                      {
                         wc = kSize32 - iptr - 100;
                         status = vf48_EventReadPio(myvme, vf48base, pdata32+iptr, &wc);
                         //printf("vf48_EventReadPio() returned status %d, wc %d\n", status, wc);
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

                   double xt1 = utime();
                   double dxt = xt1 - xt0;

                   //exit(123);
              
                   xt += dxt;
              
                   //printf("Data read: wc: %d, count: %d, %f us\n", wc, count, dxt);
              
                   iptr += wc;
                }

             double t1 = utime();
          
             double t = t1 - t0;
          
             //printf("time %f %f %f\n", t0, t1, t);
          
             bstart += 4*iptr;
             double tnow = utime() - tstart;
          
             printf("Event read: %d words in %f sec in %d reads, %f sec DMA, %6.3f Mbytes/sec per event, %6.3f Mbytes/sec per DMA, %6.3f Mbytes/sec, %.0f evt/s sustained!\n", iptr, t, nread, xt, 4*iptr/t/1000000.0, 4*iptr/xt/1000000.0, bstart/tnow/1000000.0, (iloop+1)/tnow);

             //vf48_Status(myvme, vf48base);

             //iptr = vf48_evb(pdata32, iptr);

             //exit(123);

             //if (iloop>3)
             //exit(123);
          }
          
          gVF48badDataCount = 0;

          UnpackVF48(unit, iptr, pdata32, xdebug, ydebug);

          if (xdebug && gVF48badDataCount > 0)
             exit(1);

          if (false && !xdebug && gVF48badDataCount > 0)
            {
              bool verbose = true;
              bool verboseSamples = false;
              
              if (verbose)
                printf("Event dump!\n");

              //for (int i=0; i<iptr; i++)
              //  printf("0x%08x: 0x%08x\n", i, pdata32[i]);
              
              int countSamples = 0;
              int channel = -1;
              int trigger = -1;
              bool expectSamples = false;
              bool hadFooter = false;
              bool hadHeader = false;
              for (int i=0; i<iptr; i++)
                {
                  uint32_t w = pdata32[i];
                  switch (w & 0xF0000000)
                    {
                    default:
                      printf("unit %d, word %5d: 0x%08x\n", unit, i, w);
                      countBadData++;
                      expectSamples = false;
                      break;
                    case 0x80000000:
                      {
                        expectSamples = false;
                        int trigNo = w & 0x00FFFFFF;

                        trigger = trigNo;

                        if (verbose)
                          printf("unit %d, word %5d: 0x%08x: Event header:  unit %d, trigger %d\n", unit, i, w, unit, trigNo);

                        hadHeader = true;
                        break;
                      }
                    case 0xA0000000:
                      {
                        expectSamples = true;
                        uint32_t ts = w&0x00FFFFFF;
                        if (verbose)
                          printf("unit %d, word %5d: 0x%08x: Timestamp (%d)\n", unit, i, w, ts);
                      }
                      break;
                    case 0xC0000000:
                      {
                        expectSamples = true;

                        if (channel >= 0)
                          {
                            if (verbose)
                              printf("unit %d, Previously %d samples\n", unit, countSamples);

                            if (countSamples != vf48samples)
                              {
                                printf("unit %d, channel %d, Previously %d samples, expected %d\n", unit, channel, countSamples, vf48samples);
                                countBadData++;
                              }
                          }

                        countSamples = 0;
                        
                        int group = (w&0x70)>>4;
                        int chan = (w&0x7) | (group<<3);
                        channel = chan;
                        if (verbose)
                          printf("unit %d, word %5d: 0x%08x: Group %d, Channel %2d\n", unit, i, w, group, chan);
                      }
                      break;
                    case 0x00000000:
                      {
                        int sample1 = w & 0x3FFF;
                        int sample2 = (w>>16) & 0x3FFF;
                        
                        if (verboseSamples)
                          printf("unit %d, word %5d: 0x%08x: 0x%04x 0x%04x (%d %d)\n", unit, i, w, sample1, sample2, sample1, sample2);
                        
                        if (!expectSamples)
                          {
                            printf("unit %d, word %5d: 0x%08x: unexpected samples, channel %d\n", unit, i, w, channel);
                            countBadData++;
                          }

                        countSamples += 2;
                      }
                      break;
                    case 0x50000000:
                      {
                        expectSamples = false;
                        int t = w & 0x00FFFFFF;
                        if (verbose)
                          printf("unit %d, word %5d: 0x%08x: Time %d\n", unit, i, w, t);
                      }
                      break;
                    case 0x40000000:
                      {
                        expectSamples = false;
                        int c = w & 0x00FFFFFF;
                        if (verbose)
                          printf("unit %d, word %5d: 0x%08x: Charge %d\n", unit, i, w, c);
                      }
                      break;
                    case 0xE0000000:
                      {
                        expectSamples = false;

                        if (verbose)
                          printf("unit %d, Previously %d samples\n", unit, countSamples);

                        if (countSamples != vf48samples)
                          {
                            printf("unit %d, channel %d, Previously %d samples, expected %d\n", unit, channel, countSamples, vf48samples);
                            countBadData++;
                          }

                        countSamples = 0;
                        
                        uint32_t trigNo2 = w & 0x00FFFFFF;

                        if (verbose)
                          printf("unit %d, word %5d: 0x%08x: Event trailer: unit %d, trigger %d\n", unit, i, w, unit, trigNo2);

                        if (trigNo2 != trigger)
                          {
                            printf("unit %d, word %5d: 0x%08x: Trigger number mismatch: header %d, footer %d\n", unit, i, w, trigger, trigNo2);
                            countBadData++;
                          }

                        hadFooter = true;
                      }
                      break;
                    }
                }

              if (!hadHeader)
                {
                  printf("unit %d: missing event header\n", unit);
                  countBadData++;
                }

              if (!hadFooter)
                {
                  printf("unit %d: missing event footer\n", unit);
                  countBadData++;
                }
            }
          
          if (gVF48badDataCount)
             {
                countBadData ++;
                //goto reset;
             }

          if (once)
             exit(0);

          if (loopsleep)
             sleep(loopsleep);
        }

      //printf("time %f -> %f vs %f -> %f\n", loctime0, loctime1, timestamp0, timestamp1);
      double locdt = loctime1 - loctime0;
      double vf48dt = (timestamp1 - timestamp0)*VF48event::GetTsFreq(unit);
      printf("VF48 timestamp frequency is %.3f MHz\n", vf48dt/locdt * 1e-6);

      VF48event::EndRun();

      if (gCountUnpacked-gCountOffset != countTrigger)
         {
            printf("Error: sent %d triggers but unpacked %d events\n", countTrigger, gCountUnpacked);            
            countBadData++;
            continue;
         }

      if (xdebug && gVF48badDataCount > 0)
         exit(1);

      countGood++;

      //sleep(1);
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
