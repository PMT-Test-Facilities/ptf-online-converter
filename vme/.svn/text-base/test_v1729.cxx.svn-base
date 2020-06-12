
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

extern "C" {
#include "v1729.h"
#include "v513.h"
}

#ifdef HAVE_ROOT

#include "TApplication.h"
#include "TCanvas.h"
#include "TH1D.h"

TApplication app("test_v1729", NULL, NULL);

TCanvas* gWindow;

TH1D* gHist[4];

#endif

int gV1729Base   = 0x120000;
int gIoregBase   = 0x510000;

MVME_INTERFACE *gVme;

void v513_init(MVME_INTERFACE *vme)
{
  v513_Reset(vme, gIoregBase);
  v513_Status(vme, gIoregBase);

  printf("Setting V513 channels to OUTPUT mode\n");

  for (int i=0; i<16; i++)
    {
      v513_SetChannelMode(vme, gIoregBase, i, V513_CHANMODE_OUTPUT|V513_CHANMODE_POS|V513_CHANMODE_TRANSP);
    }
}

void v513_pulse(MVME_INTERFACE *vme)
{
  v513_Write(vme,gIoregBase,0x07);
  v513_Write(vme,gIoregBase,0x00);
}

int main (int argc, char* argv[])
{
  setbuf(stdout, NULL);
  setbuf(stderr, NULL);

  int status;

  // Test under vmic   
  status = mvme_open(&gVme, 0);

  mvme_set_am(gVme, MVME_AM_A24);

  v513_init(gVme);

  // Print Current status
  v1729_Reset(gVme, gV1729Base);
  
  // Print Current status
  v1729_Status(gVme, gV1729Base);
  
  // Collect Pedestals
  printf("Calibrate pedestals!\n");
  v1729_PedestalRun(gVme, gV1729Base, 50, 1);
  
  // Do vernier calibration
  printf("Calibrate time!\n");
  v1729_TimeCalibrationRun(gVme, gV1729Base, 1);
  
  // Set mode 3 (128 full range)
  v1729_Setup(gVme, gV1729Base, 3);

  //v1729_NColsSet(gVme, gV1729Base, V1729_MAX_CHANNEL_SIZE-1);
  
  //v1729_PostTrigSet(gVme, gV1729Base, ts.post_trigger);
  v1729_PostTrigSet(gVme, gV1729Base, 64);
  
  // Print Current status
  v1729_Status(gVme, gV1729Base);
  
  // Start acquisition
  v1729_TriggerTypeSet(gVme, gV1729Base, V1729_SOFT_TRIGGER);

  // Start acquisition
  v1729_AcqStart(gVme, gV1729Base);

#ifdef HAVE_ROOT
  if (!gWindow)
    {
      gWindow = new TCanvas("test_v1729", "test_v1729", 1000, 1000);
      gWindow->Clear();
      gWindow->Divide(1,V1729_MAX_CHANNELS);
    }

  int nsamples = V1729_MAX_CHANNEL_SIZE; // v1729_NColsGet(gVme, gV1729Base);

  while (1)
    {
      v1729_SoftTrigger(gVme, gV1729Base);
      //v513_pulse(gVme);
      //sleep(1);

      printf("Waiting for trigger...\n");
      while (1)
        {
          int status = v1729_isTrigger(gVme, gV1729Base);
          printf("status %d\n", status);
          if (status == 255)
            break;
          sleep(1);
        }

      WORD data[V1729_MAX_CHANNELS*V1729_MAX_CHANNEL_SIZE];
      v1729_DataRead(gVme, gV1729Base, data, V1729_MAX_CHANNELS, V1729_MAX_CHANNEL_SIZE);

      for (int j=0; j<V1729_MAX_CHANNELS; j++)
        {
          gWindow->cd(1+j);
          
          TH1D* hh = gHist[j];
          
          //int nsamples = V1729_MAX_CHANNEL_SIZE;
          
          if (nsamples < 1)
            continue;
          
          if (!hh)
            {
              char name[256];
              sprintf(name, "adc%d", j);
              hh = gHist[j] = new TH1D(name, name, nsamples, 0, nsamples);
            }

          int samples[V1729_MAX_CHANNEL_SIZE];

          v1729_OrderData(gVme, gV1729Base, data, samples, V1729_MAX_CHANNELS, j, V1729_MAX_CHANNEL_SIZE);
          
          for (int s=0; s<nsamples; s++)
            {
              int v =  samples[s];
              hh->SetBinContent((s+1), v);
            }

          //hh->SetMinimum(hmin);
          //hh->SetMaximum(hmax);

          gPad->Clear();
          hh->Draw();
          gPad->Modified();
        }

      gWindow->Modified();
      gWindow->Update();
      gWindow->SaveAs("v1729.root");

      // Start acquisition
      v1729_AcqStart(gVme, gV1729Base);

      sleep(1);
    }

#endif

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
