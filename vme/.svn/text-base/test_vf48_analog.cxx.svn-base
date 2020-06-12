
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/time.h>

#include "TApplication.h"
#include "TCanvas.h"
#include "TH1D.h"

extern "C"
{
#include "v513.h"
#include "vf48.h"
}

#include "UnpackVF48.h"

TApplication app("test_vf48_analog", NULL, NULL);

TCanvas* gWindow;

TH1D* gHist[MAX_CHANNELS];

int vf48samples = 1000;
int vf48divisor = 1;

bool plotAll = true;
bool plotOne = false;
int  plotChan = -1;

int hmin =    0;
int hmax = 1000;

int tmin =    0;
int tmax =  1000;

int findEdge(int nadc, uint16_t adc[], int first)
{
   int min = adc[first];
   int max = adc[first];

   for (int s=first; s<nadc; s++)
      {
         int v = adc[s];
         if (v < min)
            min = v;
         if (v > max)
            max = v;
      }

   //printf("min %d, max %d\n", min, max);

   if (fabs(min-max)<10)
      return -1;

   int thr = (min+max)/2;

   for (int s=first; s<nadc-1; s++)
      {
         if (adc[s]<thr && adc[s+1]>=thr)
            return s;

         if (adc[s]>thr && adc[s+1]<=thr)
            return s;
      }

   return 0;
}

void HandleVF48event(const VF48event *e)
{
  printf("VF48 event!\n");
  e->PrintSummary();

  if (plotAll)
     {
        for (int unit=0; unit<1; unit++)
           for (int j=0; j<MAX_CHANNELS; j++)
              {
                 gWindow->cd(1+j);
              
                 TH1D* hh = gHist[j];
              
                 if (!hh)
                    {
                       char name[256];
                       sprintf(name, "adc%d", j);
                       hh = gHist[j] = new TH1D(name, name, vf48samples, 0, vf48samples);
                    }
                 
                 for (int s=0; s<e->modules[unit]->channels[j].numSamples; s++)
                    {
                       int v =  e->modules[unit]->channels[j].samples[s];
                       hh->SetBinContent((s+1), v);
                    }
                 
                 for (int s=e->modules[unit]->channels[j].numSamples; s<vf48samples; s++)
                    hh->SetBinContent((s+1), 0);
        
                 int edge = findEdge(e->modules[unit]->channels[j].numSamples, e->modules[unit]->channels[j].samples, 0);\
                 if (edge>=0)
                    printf("vf48 %d chan %d, t %d, q %d, edge %d\n", unit, j, e->modules[unit]->channels[j].time, e->modules[unit]->channels[j].charge, edge);

                 if (hmin || hmax)
                    {
                       hh->SetMinimum(hmin);
                       hh->SetMaximum(hmax);
                    }
                 
                 gPad->Clear();
                 hh->Draw();
                 gPad->Modified();
              }
     }
  else if (plotOne)
     {
        int unit = 0;
        int j = plotChan;

        gWindow->cd();
        
        TH1D* hh = gHist[j];
        
        if (!hh)
           {
              char name[256];
              sprintf(name, "adc%d", j);
              hh = gHist[j] = new TH1D(name, name, vf48samples, 0, vf48samples);
           }
        
        for (int s=0; s<e->modules[unit]->channels[j].numSamples; s++)
           {
              int v =  e->modules[unit]->channels[j].samples[s];
              hh->SetBinContent((s+1), v);
           }

        static int minedge = 9999;
        static int maxedge = 0;

        int edge = findEdge(e->modules[unit]->channels[j].numSamples, e->modules[unit]->channels[j].samples, 0);
        if (edge>0)
           {
              if (edge<minedge)
                 minedge = edge;

              if (edge>maxedge)
                 maxedge = edge;
           }
        printf("vf48 t %d, q %d, edge %d, min %d, max %d\n", e->modules[unit]->channels[j].time, e->modules[unit]->channels[j].charge, edge, minedge, maxedge);

        for (int s=e->modules[unit]->channels[j].numSamples; s<vf48samples; s++)
           hh->SetBinContent((s+1), 0);
        
        if (hmin || hmax)
           {
              hh->SetMinimum(hmin);
              hh->SetMaximum(hmax);
           }
        
        if (tmin || tmax)
           {
              hh->GetXaxis()->SetRangeUser(tmin, tmax);
           }
        
        gPad->Clear();
        hh->Draw();
        gPad->Modified();
     }

  gWindow->Modified();
  gWindow->Update();
  gWindow->SaveAs("vf48.root");

  delete e;
}

double utime()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  //printf("gettimeofday: %d %d\n", tv.tv_sec, tv.tv_usec);
  return tv.tv_sec + 0.000001*tv.tv_usec;
}

int main (int argc, char* argv[])
{
  setbuf(stdout, NULL);
  setbuf(stderr, NULL);

  int v513base = 0x510000;
  int vf48base = 0xac0000;

  if (argc<3)
     {
        printf("Usage: test_vf48_analog addr samples [channel]\n");
        exit(1);
     }

  if (argc>1)
    vf48base = strtoul(argv[1],NULL,0);
  
  if (argc>2)
    vf48samples = strtoul(argv[2],NULL,0);
  
  if (argc>3)
     {
        plotChan = strtoul(argv[3],NULL,0);
        plotAll = false;
        plotOne = true;
     }
  
  MVME_INTERFACE *myvme;

  // Test under vmic   
  int status = mvme_open(&myvme, 0);

  // Set am to A24 non-privileged Data
  mvme_set_am(myvme, MVME_AM_A24_ND);

  // Set dmode to D32
  mvme_set_dmode(myvme, MVME_DMODE_D32);

  v513_Reset(myvme, v513base);
  v513_Status(myvme, v513base);

  printf("Setting v513 channels to OUTPUT mode\n");

  for (int i=0; i<16; i++)
    v513_SetChannelMode(myvme, v513base, i, V513_CHANMODE_OUTPUT|V513_CHANMODE_POS|V513_CHANMODE_TRANSP);

  printf("Setting up the VF48 at 0x%x!\n", vf48base);

  for (int i=0; i<MAX_MODULES; i++)
     gVF48freq[i] = 40.0e6;

  vf48_Reset(myvme, vf48base);
  sleep(1);

  vf48_AcqStop(myvme, vf48base);

  vf48_Status(myvme, vf48base);

  if (!gWindow)
    {
      gWindow = new TCanvas("test_vf48_analog", "test_vf48_analog", 1000, 1000);
      gWindow->Clear();
      gWindow->Divide(3, 16);
    }

  int countCycles   = 0;
  int countTimeouts = 0;
  int countBabbling = 0;

  while (1)
    {
    reset:

      printf("Cycles: %d, timeouts: %d (%.0f%%), babbling: %d (%.0f%%)\n", countCycles,
             countTimeouts, 100.0*(double)countTimeouts/(double)countCycles,
             countBabbling, 100.0*(double)countBabbling/(double)countCycles
             );

      countCycles++;

      vf48_Reset(myvme, vf48base);

      vf48_GrpEnable(myvme, vf48base, 0x3F);

      // Active Channel Mask
      for (int i=0;i<6;i++)
        vf48_ParameterWrite(myvme, vf48base, i, VF48_ACTIVE_CH_MASK, VF48_ALL_CHANNELS_ACTIVE);

      vf48_SegmentSizeSet(myvme, vf48base, vf48samples);

      //while (1)
      for (int j=0; j<6; j++)
        {
          int s = vf48_ParameterRead(myvme, vf48base, j, VF48_SEGMENT_SIZE);
          //printf("Number of samples, group %d: set: %d, read: %d\n", j, numSamples, s);
          assert(s == vf48samples);
        }

      //return -1;

      for (int j=0; j<6; j++)
        vf48_ParameterWrite(myvme, vf48base, j, VF48_LATENCY, 10);
      
      for (int j=0; j<6; j++)
        vf48_ParameterWrite(myvme, vf48base, j, VF48_PRE_TRIGGER, 50);

      for (int j=0; j<6; j++)
         vf48_TrgThresholdSet(myvme, vf48base, j, 100);

      for (int j=0; j<6; j++)
         vf48_HitThresholdSet(myvme, vf48base, j, 100);

      //for (int j=0; j<6; j++)
      //  vf48_ChSuppSet(myvme, vf48base, j, 1);
      
      if (vf48divisor > 0)
        vf48_DivisorWrite(myvme, vf48base, vf48divisor);
  
      vf48_ExtTrgClr(myvme, vf48base);
      //vf48_ExtTrgSet(myvme, vf48base);
      
      //vf48_GrpEnable(myvme, vf48base, 0x01);

      printf("VF48[%d] at VME A24 0x%06x: Firmware: 0x%x, CSR: 0x%x\n",
             0,
             vf48base,
             vf48_ParameterRead(myvme, vf48base, 0, VF48_FIRMWARE_ID),
             vf48_CsrRead(myvme, vf48base));

      vf48_Status(myvme, vf48base);

      vf48_AcqStart(myvme, vf48base);

      sleep(1);

      double tstart = utime();
      double bstart = 0;

      for (int iloop=0; iloop<5000; iloop++)
        {
          // pulse v513 NIM output 0

          printf("Pulse!\n");

          if (1)
            {
              v513_Write(myvme, v513base, 0x3);
              v513_Read(myvme, v513base);
              v513_Write(myvme, v513base, 0x0);
              v513_Read(myvme, v513base);
            }
          
          if (0)
            {
              //usleep(100000);
              //if (iloop==0)
              vf48_Trigger(myvme, vf48base);
            }
          
          //sleep(1);
          
          // wait until the VF48 has data to read
          
          double tt = utime();
          while (1)
            {
              int haveData = vf48_NFrameRead(myvme, vf48base);
              
              //printf("VF48 waiting for data: %6d\n", haveData);
              
              if (haveData > 50)
                break;
              
              //usleep(10);
              
              double t = utime() - tt;
              if (t < 5)
                continue;
              
              printf("Timeout waiting for data, nframe %d!\n", vf48_NFrameRead(myvme, vf48base));

              //exit(123);

              countTimeouts ++;
              goto reset;
            }
          
          // read the data
          
          const int kSize32 = 100000;
          uint32_t pdata32[kSize32];
          int iptr = 0;
          
          double t0 = utime();
          double xt = 0;
          
          //sleep(1);
          
          int lastHave = 0;
          while (1)
            {
              int haveData = vf48_NFrameRead(myvme, vf48base);
              
              //printf("VF48 has data: %6d, last: %6d, iptr: %6d, xt: %f\n", haveData, lastHave, iptr, xt);
              
              if (haveData < 4)
                break;
              
              if ((haveData < 2000) && (haveData != lastHave))
                {
                  lastHave = haveData;
                  //usleep(10);
                  continue;
                }
              
              lastHave = haveData;
              
              int count = haveData&~0x1;
              
              if (count == 0)
                continue;

              if (iptr + count > kSize32)
                {
                  printf("VF48 is babbling, nframe %d!\n", vf48_NFrameRead(myvme, vf48base));
                  vf48_AcqStop(myvme, vf48base);
                  vf48_Status(myvme, vf48base);
                  countBabbling++;
                  goto reset;
                }
              
              double xt0 = utime();
              int wc = vf48_DataRead(myvme, vf48base, pdata32+iptr, &count);
              double xt1 = utime();
              
              xt += xt1 - xt0;
              
              //printf("Data read: wc: %d, count: %d\n", wc, count);
              
              iptr += wc;
            }
          
          double t1 = utime();
          
          double t = t1 - t0;
          
          //printf("time %f %f %f\n", t0, t1, t);
          
          bstart += 4*iptr;
          double tnow = utime() - tstart;
          
          printf("Event read: %d words in %f sec, %f sec DMA, %6.3f Mbytes/sec per event, %6.3f Mbytes/sec per DMA, %6.3f Mbytes/sec sustained!\n", iptr, t, xt, 4*iptr/t/1000000.0, 4*iptr/xt/1000000.0, bstart/tnow/1000000.0);

          int unit = 0;

          UnpackVF48(unit, iptr, pdata32, false);
          
          //sleep(1);
        }

      if (0)
         {
            vf48divisor += 1;
            vf48samples = 1000/vf48divisor;
         }

      for (int i=0; i<48; i++)
        gHist[i] = NULL; // FIXME: memory leak

      if (vf48samples < 5)
        break;
    }

  status = mvme_close(myvme);
  return 0;
}

/* emacs
 * Local Variables:
 * mode:C++
 * tab-width: 8
 * c-basic-offset: 3
 * End:
 */
