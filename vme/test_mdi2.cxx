// testTTC.cxx
// $Id: testTTC.cxx 1172 2008-06-28 03:18:18Z olchansk $
//
// ./test_VMENIMIO32_gef.exe --addr 0xa30000 --dma 3 0x1000 16
//

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <signal.h>
#include <sys/time.h>

#include "mvmestd.h"

double gettime()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec + 0.000001*tv.tv_usec;
}

#undef G
//#define G 1

#if G

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

#if 0

TH1D* hhh[48];

int xcount = 0;

int hold = -200;
int holdCount = 0;
int holdInter = 0;

int hmin = 0;
int hmax = 0;

int tmin = 0;
int tmax = 0;

bool doPlotAll = true;
bool doPlot1   = false;
bool doPlot4   = false;
bool doPlot6   = false;
bool doPlotAdc = false;
bool doProfile = false;

bool doHardcopy = true;

int base = 0;

void PlotSample(const VF48event *e, int i, int j)
{
  TH1D* hh = hhh[j];

  int nsamples = e->modules[i]->channels[j].numSamples;
  
  if (nsamples < 1)
    return;
  
  if (!hh)
    {
      char name[256];
      sprintf(name, "adc%d", j);
      if (doProfile)
	{
	  TProfile *hp = new TProfile(name, name, nsamples, 0, nsamples, 0, 1000);
	  hp->BuildOptions(0,1000,"s");
	  hh = hhh[j] = hp;
	}
      else
        hh = hhh[j] = new TH1D(name, name, nsamples, 0, nsamples);
    }
  
  for (int s=0; s<nsamples; s++)
    {
      uint16_t v =  e->modules[i]->channels[j].samples[s];
      if (doProfile)
        hh->Fill((s+1), v);
      else
        hh->SetBinContent((s+1), v);
    }
  
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

void PlotVF48event(const VF48event*e)
{
  makeApp();

  printf("*********************** plot event!\n");
  //e->PrintSummary();

  //return;

  // reinit the plots

  static int nrows = 0;

  if (!gVF48Window)
    {
      gVF48Window = new TCanvas("testTTC", "testTTC", 1000, 1000);
      gVF48Window->Clear();
      if (doPlotAll)
        gVF48Window->Divide(8,nrows=6);
      else if (doPlot1)
        gVF48Window->Divide(1,1);
      else if (doPlot4)
        gVF48Window->Divide(4,nrows=8);
      else if (doPlot6)
        gVF48Window->Divide(4,nrows=6);
      else
        gVF48Window->Divide(4,nrows=12);
    }

  //printf("xcount %d, nrows %d\n", xcount, nrows);

  if (xcount < 0)
    xcount = nrows-1;
  
  if (xcount >= nrows)
    {
      xcount = 0;
      //gVF48Window->SaveAs("testTTC.root");
      //gVF48Window->SaveAs("testTTC.ps");
    }

  int i=0;
  int bcount = 12;

  int interesting = 0;

  holdCount++;

  if (!doPlotAll && !doPlot1 && !doPlot4 && !doPlot6)
    {
#if 0
      for (int j=base; j<base+bcount; j++)
        {
          interesting += CalculatePulseHeight(e,i,j);
        }
#endif

      printf("hold %d, events %d, interesting %d\n", hold, holdCount, holdInter);
      
      //holdCount++;
      
      printf("Interesting %d\n", interesting);

      if (interesting==0)
        return;

      holdInter++;
    }

  if (doPlotAll)
    {
      for (int j=0; j<48; j++)
        {
#if 0
          if (doPlotAdc)
            interesting += CalculatePulseHeight(e,i,j);
#endif
          gVF48Window->cd(1+j);
          PlotSample(e, i, j);
        }
    }
  else if (doPlot1)
    {
      gVF48Window->cd(1);
      PlotSample(e, i, base);
    }
  else if (doPlot4)
    {
      for (int j=base; j<base+4; j++)
        {
          gVF48Window->cd(1+xcount*4+j-base);
          PlotSample(e, i, j);
        }
    }
  else if (doPlot6)
    {
      int c = 0;
      xcount = 0;

      base = 0*4+0*16;
      for (int j=base; j<base+4; j++)
        {
          gVF48Window->cd(1+xcount*4+c);
          PlotSample(e, i, j);
          c++;
        }

      base = 1*4+0*16;
      for (int j=base; j<base+4; j++)
        {
          gVF48Window->cd(1+xcount*4+c);
          PlotSample(e, i, j);
          c++;
        }

      base = 2*4+0*16;
      for (int j=base; j<base+4; j++)
        {
          gVF48Window->cd(1+xcount*4+c);
          PlotSample(e, i, j);
          c++;
        }

      base = 1*4+2*16;
      for (int j=base; j<base+4; j++)
        {
          gVF48Window->cd(1+xcount*4+c);
          PlotSample(e, i, j);
          c++;
        }

      base = 2*4+2*16;
      for (int j=base; j<base+4; j++)
        {
          gVF48Window->cd(1+xcount*4+c);
          PlotSample(e, i, j);
          c++;
        }

      base = 3*4+2*16;
      for (int j=base; j<base+4; j++)
        {
          gVF48Window->cd(1+xcount*4+c);
          PlotSample(e, i, j);
          c++;
        }
    }
  else
    {
      for (int j=base; j<base+bcount; j++)
        {
          gVF48Window->cd(1+xcount*bcount+j-base);
          PlotSample(e, i, j);
        }

      xcount++;
      if (xcount >= 4)
        {
          xcount = 0;
          //gVF48Window->SaveAs("testTTC.root");
          //gVF48Window->SaveAs("testTTC.ps");
        }
    }

  gVF48Window->Modified();
  gVF48Window->Update();
  if (doHardcopy)
    {
      gVF48Window->SaveAs("testTTC.root");
      gVF48Window->SaveAs("testTTC.pdf");
    }
  //gVF48Window->SaveAs("testTTC.ps");
  //sleep(1);
}
#endif
#endif

#include "mdi2.cxx"

int main(int argc, char*argv[])
{
   setbuf(stdout, NULL);
   setbuf(stderr, NULL);
   
   signal(SIGILL,  SIG_DFL);
   signal(SIGBUS,  SIG_DFL);
   signal(SIGSEGV, SIG_DFL);
   signal(SIGPIPE, SIG_DFL);
   
   MVME_INTERFACE *vme = 0;
   
   int status = mvme_open(&vme,0);
   assert(status == SUCCESS);

   int addr = 0x200000;

   if (1) {
      for (int i=1; i<argc; i++)
         if (strcmp(argv[i],"--addr")==0) {
            addr = strtoul(argv[i+1], NULL, 0);
         } else if (strcmp(argv[i],"--sleep")==0) {
            int isleep = strtoul(argv[i+1], NULL, 0);
            printf("sleep %d sec\n", isleep);
            sleep(isleep);
         } else if (strcmp(argv[i],"--exit")==0) {
            exit(0);
         } else if (strcmp(argv[i],"--status")==0) {
            mdi2_Status(vme, addr);
         } else if (strcmp(argv[i],"--reset")==0) {
            mdi2_Reset(vme, addr);
         } else if (strcmp(argv[i],"--init")==0) {
            mdi2_Init(vme, addr);
         } else if (strcmp(argv[i],"--start")==0) {
            mdi2_AcqStart(vme, addr);
         } else if (strcmp(argv[i],"--stop")==0) {
            mdi2_AcqStop(vme, addr);
         } else if (strcmp(argv[i],"--scan")==0) {
            //mdi2_SCX(vme, addr);
            for (int bus=0; bus<=1; bus++)
               for (int modnum=0; modnum<=15; modnum++) {
                  int r = mdi2_mtm16_read(vme, addr, bus, modnum, 6, 0);
                  printf("bus %d, modnum %d, read 0x%x\n", bus, modnum, r);
                  //exit(1);
               }
         } else if (strcmp(argv[i],"--test-mtm16")==0) {
            int r = mdi2_mtm16_read(vme, addr, 0, 0, 6, 0);
            printf("read 0x%x\n", r);
            //mdi2_mtm16_write(vme, addr, 0, 0, 16, 0, 7);
         } else if (strcmp(argv[i],"--init-mtm16")==0) {
            for (int bus=0; bus<=0; bus++)
               for (int modnum=0; modnum<=7; modnum++) {
                  int ctl[8] = { 7, 7, 7, 7, 7, 7, 7, 7 };
                  int thr[8] = { 300, 300, 300, 300, 300, 300, 300, 300 };

                  mdi2_mtm16_write(vme, addr, bus, modnum, 3, 0, 255); // enable remote control
                  mdi2_mtm16_write(vme, addr, bus, modnum, 16, 1, thr[modnum]); // write threshold
                  mdi2_mtm16_write(vme, addr, bus, modnum, 16, 0, ctl[modnum]); // write control: bit0=gain, bit1=pol, bit2=trig_dis
               }
         } else if (strcmp(argv[i],"--write-mtm16")==0) {
            int bus = strtoul(argv[i+1], NULL, 0);
            int mod = strtoul(argv[i+2], NULL, 0);
            int ctl = strtoul(argv[i+3], NULL, 0);
            int thr = strtoul(argv[i+4], NULL, 0);
            i+=3;
            mdi2_mtm16_write(vme, addr, bus, mod, 3, 0, 255); // enable remote control
            mdi2_mtm16_write(vme, addr, bus, mod, 16, 1, thr); // write threshold
            mdi2_mtm16_write(vme, addr, bus, mod, 16, 0, ctl); // write control: bit0=gain, bit1=pol, bit2=trig_dis
         } else if (strcmp(argv[i],"--run")==0) {
            while (1) {
               if (!mdi2_DataReady(vme, addr)) {
                  printf("data not ready!\n");
                  sleep(1);
                  continue;
               }

               sleep(1);
               uint32_t buf[100*1024];
               int wc = read_mdi2(vme, addr, buf);
               printf("wc %d\n", wc);
               for (int i=0; i<wc; i++) {
                  uint32_t w = buf[i];
                  printf("data[%03d]: 0x%08x", i, w);

                  if ((w&0xFF000000)==0x40000000) {
                     int xwc = w&0x7ff;
                     int ovf = w&0x800;
                     int modid = (w>>16)&0xff;
                     printf(", header: modid %d, ovf %d, wc %d", modid, ovf, xwc);
                  } else if ((w&0xFC000000)==0x04000000) {
                     int bussa = (w & 0x03ff0000) >> 16;
                     int busno = (w & 0x8000)!=0;
                     int ovf   = (w & 0x4000)!=0;
                     // w & 0x3000
                     int adc = w & 0xFFF;
                     if (ovf)
                        adc = 0xFFF;
                     printf(", adc sample: bussa %3d, busno %d, ovf %d, adc %5d", bussa, busno, ovf, adc);
                  } else if ((w&0xC0000000)==0xC0000000) {
                     int tc = w&0x3fffffff;
                     printf(", footer: tc %d", tc);
                  }

                  printf("\n");
               }
            }
         } else if (strcmp(argv[i],"--read")==0) {
            int ireg = strtoul(argv[i+1], NULL, 0);
            uint32_t r = mdi2_RegRead16(vme, addr, ireg);
            printf("mdi2 reg 0x%04x reads 0x%08x\n", ireg, r);
         } else if (strcmp(argv[i],"--write")==0) {
            int ireg = strtoul(argv[i+1], NULL, 0);
            int ival = strtoul(argv[i+2], NULL, 0);
            i++;
            mdi2_RegWrite16(vme, addr, ireg, ival);
            printf("mdi2 reg 0x%04x wrote 0x%08x\n", ireg, ival);
         }
   }

#if G
            makeApp();
            if (!gWindow)
               {
                  gWindow = new TCanvas("Window", "window", 1000, 1000);
                  gWindow->Clear();
                  gWindow->Divide(2, 4);
               }

            TH1D* hphase0 = new TH1D("hphase0", "phase0", 120, -30, 30);
            gWindow->cd(1);
            hphase0->Draw();

            TH1D* hphase1 = new TH1D("hphase1", "phase1", 120, -30, 30);
            gWindow->cd(2);
            hphase1->Draw();

            TH1D* hphase2 = new TH1D("hphase2", "phase2", 120, -30, 30);
            gWindow->cd(3);
            hphase2->Draw();

            TH1D* hphase3 = new TH1D("hphase3", "phase3", 120, -30, 30);
            gWindow->cd(4);
            hphase3->Draw();

            TH1D* htph0 = new TH1D("htph0", "tph0", 100, -30, 30);
            gWindow->cd(5);
            htph0->Draw();

            TH1D* htph1 = new TH1D("htph1", "tph1", 100, -30, 30);
            gWindow->cd(6);
            htph1->Draw();

            TH1D* htdc = new TH1D("htdc", "tdc", 100, 20, 50);
            gWindow->cd(8);
            htdc->Draw();

            gWindow->Modified();
            gWindow->Update();
#endif

#if G
                     if (ch==0)
                        hphase0->Fill(ph[0]);
                     if (ch==1)
                        hphase1->Fill(ph[1]);
                     if (ch==2)
                        hphase2->Fill(ph[2]);
                     if (ch==3)
                        hphase3->Fill(ph[3]);

                     if (0)
                        {
                           for (int i=1; i<=8; i++)
                              gWindow->cd(i)->Modified();

                           gWindow->Modified();
                           gWindow->Update();
                        }
#endif

#if G
               //if (ph[0] < 0)
               //   hphasen0->Fill(-ph[0]);
               //else
               //hphasep0->Fill(ph[0]);
               
               //if (ph[1] < 0)
               //   hphasen1->Fill(-ph[1]);
               //else
               //   hphasep1->Fill(ph[1]);
#endif


#if G
               htph0->Fill(tph0);
               htph1->Fill(tph1);
#endif

#if G
               htdc->Fill(dtt);

               for (int i=1; i<=8; i++)
                  gWindow->cd(i)->Modified();

               gWindow->Modified();
               gWindow->Update();
#endif


  mvme_close(vme);

  return 0;
}

/* emacs
 * Local Variables:
 * mode:c++
 * tab-width: 8
 * c-basic-offset: 3
 * indent-tabs-mode: nil
 * End:
 */

// end
#if 0
MDI2 at VME A24 0x200000:
vmic_mmap: Mapped VME AM 0x3d addr 0x00000000 size 0x00ffffff at address 0x25000000
  reg 0x6000: 0x0000 - address_source
  reg 0x6002: 0x0000 - address_reg
  reg 0x6004: 0x0001 - module_id
  reg 0x6008: 0xffff - soft_reset
  reg 0x6030: 0x0080 - buffer_data_length
  reg 0x6032: 0x0002 - data_len_format
  reg 0x6034: 0xffff - readout_reset
  reg 0x6038: 0x0000 - marking_type
  reg 0x603A: 0x0000 - start_acq
  reg 0x603C: 0xffff - fifo_reset
  reg 0x603E: 0x0000 - data_ready
  reg 0x6040: 0x0001 - seq_enable
  reg 0x6042: 0x0004 - trig_source0
  reg 0x6044: 0x0000 - trig_source1
  reg 0x6046: 0x0001 - com_trig_source
  reg 0x6048: 0xffff - gen_trigger
  reg 0x604A: 0xffff - gen_event
  reg 0x604C: 0x0001 - veto_gate
  reg 0x6050: 0x03e8 - hold_delay0
  reg 0x6052: 0x03e8 - hold_delay1
  reg 0x6054: 0x0022 - hold_width0
  reg 0x6056: 0x0022 - hold_width1
  reg 0x6060: 0x0001 - bus_watchdog
  reg 0x6062: 0xffff - frontend_reset
  reg 0x6064: 0x0002 - seq_clk_freq0
  reg 0x6066: 0x0002 - seq_clk_freq1
  reg 0x6068: 0x0000 - seq_busy
  reg 0x606A: 0x0004 - sample_delay_reg0
  reg 0x606C: 0x0004 - sample_delay_reg1
  reg 0x606E: 0x0001 - enable_busy
  reg 0x6074: 0x0080 - seq0_cct
  reg 0x6076: 0x0080 - seq1_cct
  reg 0x6078: 0x0000 - allow_sync_word
  reg 0x6090: 0x0000 - evctr_res
  reg 0x6092: 0x0000 - evctr_lo
  reg 0x6094: 0x0000 - evctr_hi
mvme_close:
#endif
