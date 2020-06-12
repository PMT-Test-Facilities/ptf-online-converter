
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>

#include "v792.h"
extern "C" {
#include "v513.h"
}

#ifdef HAVE_ROOT

#include "TApplication.h"
#include "TCanvas.h"
#include "TProfile.h"

TApplication app("test_v792", NULL, NULL);

TCanvas* gWindow = NULL;

TProfile* gHist = NULL;

#endif

double getTime()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec + 0.000001*tv.tv_usec;
}

void stats(int n, const double x[], double* meanp, double* rmsp)
{
  double s0 = 0;
  double s1 = 0;
  double s2 = 0;

  for (int i=0; i<n; i++)
    {
      s0 += 1;
      s1 += x[i];
      s2 += x[i]*x[i];
    }

  double mean = s1/s0;
  double var  = s2/s0 - mean*mean;
  double rms  = 0;
  if (var > 0)
    rms = sqrt(var);

  if (meanp)
    *meanp = mean;

  if (rmsp)
    *rmsp  = rms;
}

int main (int argc, char* argv[])
{
  setbuf(stdout, NULL);
  setbuf(stderr, NULL);

  int   v792_base  = 0x100000;
  int   v513_base  = 0x510000;
  
  MVME_INTERFACE *myvme;

  int status;
  DWORD cnt;
  DWORD dest[1000];

  if (argc>1) {
    v792_base = strtoul(argv[1], NULL, 0);
  }

  // Test under vmic   
  status = mvme_open(&myvme, 0);

  // Set am to A24 non-privileged Data
  mvme_set_am(myvme, MVME_AM_A24_ND);

  // Set dmode to D32
  mvme_set_dmode(myvme, MVME_DMODE_D32);

  v513_Reset(myvme, v513_base);
  v513_Status(myvme, v513_base);
  for (int i=0; i<16; i++)
    v513_SetChannelMode(myvme, v513_base, i, V513_CHANMODE_OUTPUT|V513_CHANMODE_POS|V513_CHANMODE_TRANSP);

  v792_SingleShotReset(myvme, v792_base);

  WORD threshold[32];
  for (int i=0;i<V792_MAX_CHANNELS;i++) {
    threshold[i] = 0;
  }
  v792_ThresholdWrite(myvme, v792_base, threshold);
  //v792_DataClear(myvme, v792_base);
  //v792_Setup(myvme, v792_base, 1);
  //v792_EmptyEnable(myvme, v792_base);

  v792_Write16(myvme, v792_base, 0x1032, 1<<3); // Over-range enabled
  v792_Write16(myvme, v792_base, 0x1032, 1<<4); // Low thr en
  v792_Write16(myvme, v792_base, 0x1060, 100);  // iped register

  v792_Status(myvme, v792_base);

  if (!v792_isPresent(myvme, v792_base))
    exit(1);

  if (!gWindow)
    {
      gWindow = new TCanvas("test_v792", "test_v792", 1000, 1000);
      gWindow->Clear();
      //gWindow->Divide(8,4);

      gHist = new TProfile("v792_profile", "v792_profile", 32, -0.5, 31.5, 0, 2000);
      gHist->SetMinimum(0);
      gHist->SetMaximum(2000);
      gHist->Draw();
    }

  int xval[100];
  int zadc[100][32];
  
  int ievt = 0;
  while (1)
    {
      int xiped = ievt*20;
      v792_Write16(myvme, v792_base, V792_IPED_RW, xiped);

      int iped = v792_Read16(myvme, v792_base, V792_IPED_RW);

      printf("Trigger, iped %d!\n", iped);
      //v792_Trigger(myvme, v792_base);

      double t1 = getTime();
      v513_Write(myvme, v513_base, 1);
      v513_Read(myvme,  v513_base);
      //v513_Read(myvme,  v513_base);
      v513_Write(myvme, v513_base, 0);
      v513_Read(myvme,  v513_base);
      double t2 = getTime();

      double t = t2 - t1;

      printf("Gate time %f us\n", t2-t1);

      int ready = 0;
      while (!ready)
        {
          ready = v792_DataReady(myvme, v792_base);
          printf("ready 0x%x\n", ready);
        }
      
      if (!ready)
        {
          printf("FAULT: software trigger timeout!\n");
          v792_Status(myvme, v792_base);
          //return(1);
        }

      printf("Event!\n");

      // Read Event Counter
      v792_EvtCntRead(myvme, v792_base, &cnt);
      //printf("Event counter: 0x%lx\n", cnt);

      //  cnt=32;
      //  v792_DataRead(myvme, v792_base, dest, &cnt);
      
      //  for (i=0;i<32;i++) {
      //    printf("Data[%i]=0x%x\n", i, dest[i]);
      //  }
  
      int wcount = 0;
      v792_EventRead(myvme, v792_base, dest, &wcount);
      //printf("Event word count: %d 0x%x\n", wcount, wcount);

      if (t > 0.000010)
        continue;

      if (0)
        {
          for (int i=0;i<status;i++)
            {
              printf("%02d)",i);
              v792_printEntry((v792_Data*)&dest[i]);
            }
        }

      int xadc[32];
      for (int i=0; i<32; i++)
        xadc[i] = 0;

      for (int i=0;i<wcount;i++)
        {
          bool verbose = false;
          uint32_t w = dest[i];
          switch (w & 0x07000000)
            {
            default:
              if (verbose)
                printf("type 0x%x\n", w & 0x07000000);
              break;
            case 0x00000000:
              int chan = (w & 0x001F0000) >> 16;
              int adc  = (w & 0x00000FFF);
              int ov   = (w & 0x00001000);
              int un   = (w & 0x00002000);

              if (verbose)
                printf("chan %2d, adc %6d, ov %d, un %d\n", chan, adc, ov, un);

              if (ov)
                adc = 9999;
              else if (un)
                adc = 0;

              xadc[chan] = adc;

              //gHist->Fill(chan, adc);
              break;
            }
        }

      xval[ievt] = iped;
      for (int i=0; i<32; i++)
        {
          zadc[ievt][i] = xadc[i];
          gHist->Fill(i, xadc[i]);
        }
      ievt++;

      if (1)
        {
          if (ievt > 5)
            {
              double slope[32];
              double xoffset[32];
              double yoffset[32];

              for (int i=0; i<32; i++)
                {
                  slope[i]  = (zadc[ievt-1][i] - zadc[ievt-4][i])/(double)(xval[ievt-1] - xval[ievt-4]);
                  yoffset[i] = zadc[ievt-1][i] - slope[i]*xval[ievt-1];
                  if (fabs(slope[i]) > 0.1)
                    xoffset[i] = xval[ievt-1] - zadc[ievt-1][i]/slope[i];
                  else
                    xoffset[i] = 0;
                }

              double meanslope, rmsslope;
              stats(ievt, slope, &meanslope, &rmsslope);

              double meanyo, rmsyo;
              stats(ievt, yoffset, &meanyo, &rmsyo);

              double meanxo, rmsxo;
              stats(ievt, xoffset, &meanxo, &rmsxo);

              printf("Summary:\n");
              printf("Xvalue:  ");
              for (int j=0; j<ievt; j++)
                printf(" %5d", xval[j]);
              printf(" yoffset xoffset  slope\n");
              for (int i=0; i<32; i++)
                {
                  bool flag = false;
                  printf("Chan %2d: ", i);
                  for (int j=0; j<ievt; j++)
                    printf(" %5d", zadc[j][i]);

                  double xr = (xoffset[i]-meanxo)/rmsxo;
                  double yr = (yoffset[i]-meanyo)/rmsyo;
                  double sr = (slope[i]-meanslope)/rmsslope;

                  printf(" (%6.1f) (%6.1f) %6.1f (%6.1f)", yr, xr, slope[i], sr);

                  flag = (fabs(xr) > 3) || (fabs(yr) > 3) || (fabs(sr) > 3);

                  if (flag)
                    printf(" <---- defective? ");
                  printf("\n");
                }

            }
        }

#if 0
      printf("Summary:\n");
      printf("Xvalue:  ");
      for (int j=0; j<ievt; j++)
        printf(" %5d", xval[j]);
      printf("\n");
      for (int i=0; i<32; i++)
        {
          printf("Chan %2d: ", i);
          for (int j=0; j<ievt; j++)
            printf(" %5d", zadc[j][i]);
          printf("\n");
        }
#endif

      gWindow->Modified();
      gWindow->Update();

#if 0
      if (ievt > 12)
        break;
#endif

      if (ievt > 12)
        ievt = 12;
      else
        gHist->Reset();


      //v792_Write16(myvme, v792_base, V792_IPED_RW, iped + 20);

      sleep(1);
    }

  status = mvme_close(myvme);

  app.Run(true);

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
