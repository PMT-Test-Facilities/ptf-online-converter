// testTTC.cxx
// $Id: testTTC.cxx 1172 2008-06-28 03:18:18Z olchansk $
//
// ./test_VMENIMIO32_gef.exe --addr 0xa30000 --dma 3 0x1000 16
//
// VF48 clock 80 MHz, 12.5 ns
//
// tsi148
//
// dma 5 2esst 8 clocks - 100 ns cycle - 10 MHz - 64 Mbytes/sec
// dma 4 2evme 6 clocks - 75 ns - 13.3 MHz - 106.66 Mbytes/sec
// dma 3 mblt64 12 clocks - 150 ns - 6.66 MHz - 53 Mbytes/sec
// dma 2 blt32  12 clocks - same - same - 26.66 Mbytes/sec
// dma 1 single 217 clocks - 2.7 us cycle - 0.3 MHz - 1.4 Mbytes/sec
// dma 0 pio    217 clocks - same - same - same
//
// universeII
//
// dma 3 mblt64 16 clocks -
// dma 2 blt32  15 clocks -
// dma 1 single 21 clocks -
// dma 0 pio    same
//
//
// SIS3820 tsi148
//
// dma 2 blt32  12 clocks
// dma 3 mblt64 12 clocks
// dma 4 2evme   6 clocks
// dma 5 2esst   5 clocks

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <signal.h>
#include <sys/time.h>

#include "mvmestd.h"

#include "VMENIMIO32.h"

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

void probe(VMENIMIO32* io32, int ireg)
{
   uint32_t v = io32->read32(ireg);
   printf("reg 0x%04x reads 0x%08x\n", ireg, v);
}

void print32(uint32_t v)
{
   for (int i=0; i<32; i++)
      {
         if (v & (1<<(31-i)))
            printf("1");
         else
            printf("0");

         if ((i%4)==3)
            printf("-");
      }
}

int fixHoles(uint32_t v)
{
   if ((v & 0xF) == 2)
      v |= 1;

   //if ((v & 0x3) == 1)
   //   v &= ~1;

   //if ((v & 0xF) == 0xE)
   //   v |= 1;

   for (int j=1; j<31; j++)
      if ((v & (1<<(j-1))) && ((v & (1<<(j+1)))))
         v |= (1<<j);

   for (int j=1; j<31; j++)
      if (((v & (1<<(j-1)))==0) && (((v & (1<<(j+1))))==0))
         v &= ~(1<<j);

   return v;
}

int findLast1(uint32_t v)
{
   int i=0;
   for (; i<32; i++)
      if ((v & (1<<i)) != 0)
         break;
   for (; i<32; i++)
      if ((v & (1<<i)) == 0)
         return i;

   return i;
}

int findBits(uint32_t v)
{
   if ((v & 1) == 0)
      {
         int i=0;
         for (; i<32; i++)
            if ((v & (1<<i)) != 0)
               break;

         int start = i;

         for (; i<32; i++)
            if ((v & (1<<i)) == 0)
               return (start+i)/2;
      }
   else
      {
         int i=0;
         for (; i<32; i++)
            if ((v & (1<<i)) == 0)
               break;

         int start = i;

         for (; i<32; i++)
            if ((v & (1<<i)) != 0)
               return -(start+i)/2;
      }

   return 0;
}

int findEdge(uint32_t v)
{
   if ((v & 1) == 0)
      {
         int i=0;
         for (; i<32; i++)
            if ((v & (1<<i)) != 0)
               return -i;
      }
   else
      {
         int i=0;
         for (; i<32; i++)
            if ((v & (1<<i)) == 0)
               return i;
      }

   return 0;
}

#define SMB_SCK     0x1
#define SMB_SDA     0x2
#define SMB_SDA_ENA 0x4
#define SMB_SDA_IN  0x8

void printSMB(uint32_t w)
{
   printf("0x%01x: ", w&0xF);
   if (w & SMB_SCK)
      printf("SCK ");
   if (w & SMB_SDA)
      printf("SDA ");
   if (w & SMB_SDA_ENA)
      printf("SDA_ENA ");
   if (w & SMB_SDA_IN)
      printf("SDA_IN ");
   printf(": ");
}

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

   VMENIMIO32* io32 = NULL;

   int addr = 0xFA0000;

   double tscfreq = 20e6; // 20 MHz
   
   if (1) {
      for (int i=1; i<argc; i++)
         if (strcmp(argv[i],"--addr")==0) {
            addr = strtoul(argv[i+1], NULL, 0);
            io32 = new VMENIMIO32(vme, addr);
         } else if (strcmp(argv[i],"--sleep")==0) {
            int isleep = strtoul(argv[i+1], NULL, 0);
            printf("sleep %d sec\n", isleep);
            sleep(isleep);
         } else if (strcmp(argv[i],"--command")==0) {
            int icmd = strtoul(argv[i+1], NULL, 0);
            printf("io32 command %d\n", icmd);
            io32->write32(IO32_COMMAND, icmd);
         } else if (strcmp(argv[i],"--read")==0) {
            int ireg = strtoul(argv[i+1], NULL, 0);
            uint32_t r = io32->read32(ireg*4);
            printf("io32 reg %d reads 0x%08x\n", ireg, r);
         } else if (strcmp(argv[i],"--write")==0) {
            int ireg = strtoul(argv[i+1], NULL, 0);
            int ival = strtoul(argv[i+2], NULL, 0);
            i++;
            io32->write32(ireg*4, ival);
            printf("io32 reg %d wrote 0x%08x\n", ireg, ival);
         } else if (strcmp(argv[i],"--init")==0) {
            io32->init();
         } else if (strcmp(argv[i],"--status")==0) {
            io32->status();
         } else if (strcmp(argv[i],"--reboot")==0) {
            int ireg = 0;
            uint32_t r = io32->read32(ireg*4);
            printf("io32 reg %d reads 0x%08x (old running firmware revision)\n", ireg, r);
            io32->write32(IO32_COMMAND, IO32_CMD_FPGA_RECONFIGURE);
            r = io32->read32(ireg*4);
            printf("io32 reg %d reads 0x%08x (should read 0xFFFFFFFF while FPGA is rebooting)\n", ireg, r);
            r = io32->read32(ireg*4);
            printf("io32 reg %d reads 0x%08x (should read 0xFFFFFFFF while FPGA is rebooting)\n", ireg, r);
            sleep(1);
            r = io32->read32(ireg*4);
            printf("io32 reg %d reads 0x%08x (new firmware revision)\n", ireg, r);
         } else if (strcmp(argv[i],"--exit")==0) {
            exit(0);
         } else if (strcmp(argv[i],"--pulsenim")==0) {
            while (1) {
               io32->flip32(2*4, 0xffff, 0x0000, 0x0000);
               printf("reg 2 (nim out): 0x%08x, reg 3 (nim in): 0x%08x, reg 7 (LVDS in): 0x%08x\n", io32->read32(IO32_NIMOUT), io32->read32(IO32_NIMIN), io32->read32(IO32_LVDSIN));
               sleep(1);
               io32->flip32(2*4, 0x0000, 0xffff, 0x0000);
               printf("reg 2 (nim out): 0x%08x, reg 3 (nim in): 0x%08x, reg 7 (LVDS in): 0x%08x\n", io32->read32(IO32_NIMOUT), io32->read32(IO32_NIMIN), io32->read32(IO32_LVDSIN));
               sleep(1);
            }
         } else if (strcmp(argv[i],"--nimout")==0) {
            int inim = strtoul(argv[i+1], NULL, 0);
            int iout = strtoul(argv[i+2], NULL, 0);
            assert(inim >= 0 && inim < 4);
            assert(iout >= 0 && iout <= 3);
            i++;
#if 0
            uint32_t reg = io32->read32(4*2);
            reg &= ~(3<<(16+inim*2));
            reg |= (iout<<(16+inim*2));
            io32->write32(4*2, reg);
            printf("set NIM output %d function %d, reg2 write 0x%08x\n", inim, iout, reg);
#endif
            io32->SetNimOutputFunction(inim, iout);
         } else if (strcmp(argv[i],"--readscalers33")==0) {
#if 0
            printf("clearing scalers...\n");
            io32->write32(0x18, 0);
            uint32_t tscx = 0;
            uint32_t cccx[32];
            for (int i=0; i<32; i++)
               cccx[i] = 0;
            double tttx = gettime();

            while (1) {
               double   ttt = gettime();
               uint32_t tsc = io32->read32(0x18);
               uint32_t ccc[32]; 
               for (int i=0; i<16; i++)
                  ccc[i] = io32->read32((16+i)*4);
               for (int i=0; i<16; i++)
                  ccc[16+i] = io32->read32((32+i)*4);

               double dtsc = (tsc-tscx)/tscfreq;

               printf("TSC:     0x%08x, %.0f %.0f\n", tsc, tscfreq, (tsc-tscx)/(ttt-tttx));
               for (int i=0; i<16; i++)
                  printf("NIM[%2d]: 0x%08x, %.0f %.0f\n", i, ccc[i], (ccc[i]-cccx[i])/dtsc, (ccc[i]-cccx[i])/(ttt-tttx));
               for (int i=16; i<32; i++)
                  printf("ECL[%2d]: 0x%08x, %.0f %.0f\n", i-16, ccc[i], (ccc[i]-cccx[i])/dtsc, (ccc[i]-cccx[i])/(ttt-tttx));

               tttx = ttt;
               tscx = tsc;
               for (int i=0; i<32; i++)
                  cccx[i] = ccc[i];

               sleep(1);
            }
#endif
            printf("clearing scalers...\n");
            io32->write32(IO32_COMMAND, IO32_CMD_RESET_SCALERS);

            uint32_t cccx[33];
            for (int i=0; i<33; i++)
               cccx[i] = 0;
            double tttx = gettime();

            while (1) {
               double   ttt = gettime();
               uint32_t ccc[33];
               io32->ReadScalers33(ccc);
               uint32_t tsc = ccc[32];
               uint32_t tscx = cccx[32];

               double dtsc = (tsc-tscx)/tscfreq;

               printf("TSC:     0x%08x, %.0f %.0f\n", tsc, tscfreq, (tsc-tscx)/(ttt-tttx));
               for (int i=0; i<16; i++)
                  printf("NIM[%2d]: 0x%08x, %.0f %.0f\n", i, ccc[i], (ccc[i]-cccx[i])/dtsc, (ccc[i]-cccx[i])/(ttt-tttx));
               for (int i=16; i<32; i++)
                  printf("ECL[%2d]: 0x%08x, %.0f %.0f\n", i-16, ccc[i], (ccc[i]-cccx[i])/dtsc, (ccc[i]-cccx[i])/(ttt-tttx));

               tttx = ttt;
               for (int i=0; i<33; i++)
                  cccx[i] = ccc[i];

               sleep(1);
            }
         } else if (strcmp(argv[i],"--readscalers")==0) {

            printf("clearing scalers...\n");
            io32->write32(IO32_COMMAND, IO32_CMD_RESET_SCALERS);

            double tttx = gettime();

            sleep(1);

            int num = 32;
            uint32_t sum[num];
            for (int i=0; i<num; i++)
               sum[i] = 0;

            while (1) {
               double   ttt = gettime();
               uint32_t ccc[num];

               io32->write32(IO32_COMMAND, IO32_CMD_LATCH_SCALERS);
               int count = io32->ReadScaler(ccc, num);
               assert(count == num);

               for (int i=0; i<count; i++)
                  sum[i] += ccc[i];

               //uint32_t tsc = ccc[31]; // Rev0
               //uint32_t tsc = ccc[15]; // Rev1
               uint32_t tsc = ccc[31]; // new firmware

               double dtsc = tsc/tscfreq;

               printf("Clock: %10d (0x%08x),                  rate: %10.0f %10.0f\n", tsc, tsc, tscfreq, tsc/(ttt-tttx));
               for (int i=0; i<count; i++)
                  printf(" [%2d]: %10d (0x%08x), sum: %10d, rate: %10.0f %10.0f\n", i, ccc[i], ccc[i], sum[i], ccc[i]/dtsc, ccc[i]/(ttt-tttx));
               printf("\n");

               tttx = ttt;

               sleep(1);
            }
         } else if (strcmp(argv[i],"--readnim")==0) {
            printf("clearing NIM input latch...\n");
            io32->write32(0x0C, 0xFFFF);
            while (1) {
               printf("reg 0x0C: 0x%08x (NIM input latch)\n", io32->read32(0x0C));
               sleep(1);
            }
         } else if (strcmp(argv[i],"--readlvds")==0) {
            printf("clearing LVDS input latch...\n");
            io32->write32(0x1C, 0xFFFF);
            while (1) {
               printf("reg 0x1C: 0x%08x (LVDS input latch)\n", io32->read32(0x1C));
               sleep(1);
            }
         } else if (strcmp(argv[i],"--readtsc")==0) {
            uint32_t prev = 0;
            while (1) {
               uint32_t rd = io32->read32(0x18);
               printf("reg 0x18: 0x%08x, diff 0x%x, %d\n", rd, rd-prev, rd-prev);
               if (rd == 0)
                  sleep(1);
               else
                  prev = rd;
            }
         } else if (strcmp(argv[i],"--readtsc4")==0) {
            uint32_t reg10 = io32->read32(4*10);
            io32->write32(4*10, reg10|(0<<0)|(1<<5)|(2<<10)|(3<<15)); // program TSC4 routing NIM[0]->TSC[0], 1->1, 2->2, 3->3.
            printf("reg10: 0x%08x\n", io32->read32(4*10));
            io32->write32(4*11, 0); // reset TSC4 data fifo
            int tscx = 0;
            while (1) {
               uint32_t ctrl = io32->read32(4*12);
               int count = ctrl & 0x7FFF;
               int full  = ctrl & 0x8000;

               if (count == 0) {
                  sleep(1);
                  continue;
               }

               for (int i=0; i<count; i++) {
                  uint32_t fifo = io32->read32(4*11);
                  int chan = (fifo>>30) & 3;
                  uint32_t t = fifo & 0x3FFFFFFF;
                  printf("tsc ctrl: 0x%08x, fifo %3d/%3d, data: 0x%08x, chan %d, time 0x%08x, %.9f sec\n", ctrl, i+1, count, fifo, chan, t, (t-tscx)/tscfreq);
                  tscx = t;
               }
            }
         } else if (strcmp(argv[i],"--testbits")==0) {
            int ireg = strtoul(argv[i+1], NULL, 0);
            i++;
            bool good = true;
            for (int j=0; j<32; j++) {
               uint32_t wr = (1<<j);
               io32->write32(4*ireg, wr);
               uint32_t rd = io32->read32(4*ireg);
               printf("reg %d: wr 0x%08x, rd 0x%08x", ireg, wr, rd);
               if (rd != wr) {
                  good = false;
                  printf(" --- MISMATCH --- diff is 0x%08x, bad bit is %d", wr^rd, j);
               }
               printf("\n");
            }
            if (good)
               printf("reg %d read-write test passed!\n", ireg);
            else
               printf("reg %d read-write test FAILED!\n", ireg);
         } else if (strcmp(argv[i],"--testscaler20")==0) {
            while (1) {
               io32->NimOutput(0, 0xFFFF); // clear nim outputs
               io32->SetNimOutputFunction(0, 0); // set NIMOUT[0] function 0
               io32->SetNimOutputFunction(1, 0); // set NIMOUT[0] function 0
               io32->write32(IO32_COMMAND, IO32_CMD_RESET_SCALERS); // reset scalers

               io32->SetNimOutputFunction(0, 1); // set NIMOUT[0] function 1 is 20 MHz clock
               io32->SetNimOutputFunction(1, 2); // set NIMOUT[1] function 2 is PLL1 clock
               //sleep(1);
               double t0 = gettime();
               while (1) {
                  double t = gettime();
                  if (t < t0)
                     break;
                  if (t > t0 + 1.0)
                     break;
                  io32->write32(IO32_COMMAND, IO32_CMD_LATCH_SCALERS); // latch scalers
                  uint32_t sc0 = io32->read32(IO32_SCALER_0); // read scaler 0
                  //printf("scaler 0 reads %d\n", sc0);
                  io32->read32(0);
               }

               io32->SetNimOutputFunction(0, 0); // set NIMOUT[0] function 0
               io32->SetNimOutputFunction(1, 0); // set NIMOUT[1] function 0
               io32->write32(IO32_COMMAND, IO32_CMD_LATCH_SCALERS); // latch scalers

               uint32_t sc0 = io32->read32(IO32_SCALER_0); // read scaler 0
               printf("scaler 0 reads %d\n", sc0);
               sleep(5);
               exit(0);
            }
         } else if (strcmp(argv[i],"--testscalerxxx")==0) {
            while (1) {
               //io32->NimOutput(0, 0xFFFF); // clear nim outputs

               // stop counting
               io32->NimOutput(0x8000, 0x0000); // set NIM output 15
               sleep(1);

               io32->write32(IO32_COMMAND, IO32_CMD_RESET_SCALERS); // reset scalers

               uint32_t reg59;

               reg59 = io32->read32(4*59);
               printf("reg59 0x%08x\n", reg59);

               io32->write32(4*59, 0);

               reg59 = io32->read32(4*59);
               printf("reg59 0x%08x\n", reg59);

               uint32_t ncnt = 0;
               int fcount = 0;
               int lcount = 0;

               uint32_t cnt0 = 0;

               // counting starts here
               sleep(1);
               io32->NimOutput(0x0000, 0x8000); // clear NIM output 15


               printf("counting...\n");
               double t0 = gettime();
               while (1) {
                  double t = gettime();
                  if (t < t0)
                     break;
                  if (t > t0 + 10.0)
                     break;

                  if (1) {
                     lcount++;
                     io32->write32(IO32_COMMAND, IO32_CMD_LATCH_SCALERS); // latch scalers
                     //uint32_t sc0 = io32->read32(IO32_SCALER_0); // read scaler 0
                     uint32_t scnn[32];
                     io32->ReadScaler(scnn,32);
                     uint32_t sc0 = scnn[0];
                     //printf("scaler 0 reads %d\n", sc0);
                     cnt0 += sc0;
                  }

                  if (0) {
                     io32->write32(4*59, 0);
                     fcount ++;

                     reg59 = io32->read32(4*59);
                     //printf("reg59 0x%08x\n", reg59);
                     ncnt += reg59;
                  }

                  io32->read32(0);
               }

               // stop counting
               io32->NimOutput(0x8000, 0x0000); // set NIM output 15
               // counting is stopped
               sleep(1);

               printf("done...\n");

               io32->write32(IO32_COMMAND, IO32_CMD_LATCH_SCALERS); // latch scalers

               //uint32_t sc0 = io32->read32(IO32_SCALER_0); // read scaler 0
               uint32_t scnn[32];
               io32->ReadScaler(scnn,32);
               uint32_t sc0 = scnn[0];
               printf("scaler 0 reads %d, latch count %d\n", sc0, lcount);

               cnt0 += sc0;

               printf("cnt0 %d, latch count %d\n", cnt0, lcount);

               int truecount = 0;
               if (lcount == 0)
                  truecount = sc0;

               reg59 = io32->read32(4*59);
               printf("reg59 0x%08x\n", reg59);

               io32->write32(4*59, 0);

               reg59 = io32->read32(4*59);
               printf("reg59 0x%08x (%d)\n", reg59, reg59);

               ncnt += reg59;

               if (0) {
                  int diff = ncnt-truecount;

                  printf("------>  total %d, flip count %d, diff %d, glitch rate %e per flip\n", ncnt, fcount, diff, (1.0*diff)/(1.0*fcount));
               }

               if (1) {
                  int diff = ncnt-cnt0;

                  printf("------>  total %d/%d, diff %d, latch count %d, flip count %d, glitch rate %e per read\n", ncnt, cnt0, diff, lcount, fcount, (1.0*diff)/(1.0*(fcount+lcount)));
               }

               io32->write32(4*59, 0);

               reg59 = io32->read32(4*59);
               printf("reg59 0x%08x (%d)\n", reg59, reg59);

               io32->write32(4*59, 0);

               reg59 = io32->read32(4*59);
               printf("reg59 0x%08x (%d)\n", reg59, reg59);

               //sleep(10);
               //io32->NimOutput(0x0000, 0x8000); // clear NIM output 15

               //sleep(5);
               exit(0);
            }
         } else if (strcmp(argv[i],"--testread")==0) {
            int ireg = strtoul(argv[i+1], NULL, 0);
            while (1) {
               uint32_t r = io32->read32(ireg*4);
               printf("io32 reg %d reads 0x%08x\n", ireg, r);
            }
         } else if (strcmp(argv[i],"--testtrig")==0) {
            int expected = 1;
            int mismatch = 0;
            uint32_t pts = 0;
            io32->write32(IO32_TRIG_COUNT, 0); // clear trigger counter
            io32->write32(IO32_TSC4_FIFO, 0); // clear timestamp
            io32->write32(IO32_NIMIN, 0xFFFF); // clear nim input latch
            io32->write32(IO32_NIMOUT, 0); // clear nim output register
            io32->SetNimOutputFunction(1, 1); // use NIM output 1 as BUSY

            while (0) {
               io32->NimOutput(2, 0);
               sleep(1);
               io32->NimOutput(0, 2);
               sleep(1);
            }

            while (1) {
               uint32_t r = io32->read32(IO32_NIMIN);
               static uint32_t pr = -1;
               if (r != pr) {
                  printf("io32 NIM_IN reads 0x%08x\n", r);
                  pr = r;
               }
               if ((r & 0x00020000) == 0)
                  continue;

               uint32_t tc = io32->read32(IO32_TRIG_COUNT);
               uint32_t ts = io32->read32(IO32_TRIG_TS);
               if (tc != expected) {
                  mismatch++;
                  expected = tc;
               }
               printf("trigger %d, count %d, ts 0x%08x, diff 0x%08x %10d, count mismatched: %d!\n", expected, tc, ts, ts-pts, ts-pts, mismatch);
               expected ++;
               pts = ts;
               //sleep(1);
               io32->write32(IO32_NIMIN, 0x0002); // clear busy
            }
         } else if (strcmp(argv[i],"--testsmb")==0) {
            uint32_t w;
            w = io32->read32(4*13);
            printSMB(w);
            printf("SMB reg 0x%08x\n", w);

            io32->write32(4*13, SMB_SDA|SMB_SDA_ENA);
            w = io32->read32(4*13);
            printSMB(w);
            printf("SMB reg 0x%08x\n", w);

            // SDA high, SCK low

            io32->write32(4*13, SMB_SDA|SMB_SDA_ENA);
            w = io32->read32(4*13);
            printSMB(w);
            printf("SMB reg 0x%08x\n", w);

            // SDA high, SCK low->high

            io32->write32(4*13, SMB_SCK|SMB_SDA|SMB_SDA_ENA);
            w = io32->read32(4*13);
            printSMB(w);
            printf("SMB reg 0x%08x\n", w);

            // START state

            printf("going to START state!\n");

            // SDA high->low, SCK high

            io32->write32(4*13, SMB_SCK|SMB_SDA_ENA);
            w = io32->read32(4*13);
            printSMB(w);
            printf("SMB reg 0x%08x\n", w);

            // read transaction

            //int addr = 0x00;
            //int addr = 0x22;
            int addr = 0xB0;
            //int addr = 0xAE;

            printf("driving address!\n");

            for (int i=7; i>0; i--) {
               int bit = 0;
               if (addr & (1<<i))
                  bit = SMB_SDA;

               // clock low
               io32->write32(4*13, SMB_SDA_ENA);

               // clock low, drive bit
               io32->write32(4*13, SMB_SDA_ENA|bit);

               // clock high, drive bit
               io32->write32(4*13, SMB_SCK|SMB_SDA_ENA|bit);

               // clock low, drive bit
               io32->write32(4*13, SMB_SDA_ENA|bit);
            }

            // clock low, drive 0 for WRITE
            io32->write32(4*13, SMB_SDA_ENA|SMB_SDA);

            // clock high, drive 0 for WRITE
            io32->write32(4*13, SMB_SCK|SMB_SDA_ENA|SMB_SDA);

            // clock low, drive 0 for WRITE
            io32->write32(4*13, SMB_SDA_ENA|SMB_SDA);

            // listen for the reply

            w = io32->read32(4*13); printSMB(w); printf("\n");

            // clock low, no drive
            io32->write32(4*13, 0);

            w = io32->read32(4*13); printSMB(w); printf("\n");

            // clock high, no drive
            io32->write32(4*13, SMB_SCK);

            w = io32->read32(4*13); printSMB(w); printf("\n");

            // clock low, no drive
            io32->write32(4*13, SMB_SCK);

            w = io32->read32(4*13); printSMB(w); printf("\n");

            // clock low, drive 0
            io32->write32(4*13, SMB_SDA_ENA);

            w = io32->read32(4*13); printSMB(w); printf("\n");

            // STOP state

            printf("going to STOP state!\n");

            // SDA low, SCK high

            io32->write32(4*13, SMB_SCK|SMB_SDA_ENA);
            w = io32->read32(4*13);
            printSMB(w);
            printf("SMB reg 0x%08x\n", w);

            // SDA low->high, SCK high

            io32->write32(4*13, SMB_SCK|SMB_SDA_ENA|SMB_SDA);
            w = io32->read32(4*13);
            printSMB(w);
            printf("SMB reg 0x%08x\n", w);

            // SDA off, SCK high

            io32->write32(4*13, SMB_SCK|SMB_SDA);
            w = io32->read32(4*13);
            printSMB(w);
            printf("SMB reg 0x%08x\n", w);

            printf("done!\n");

         } else if (strcmp(argv[i],"--dma")==0) {
            int imode  = strtoul(argv[i+1], NULL, 0);
            int iaddr  = strtoul(argv[i+2], NULL, 0);
            int count = strtoul(argv[i+3], NULL, 0);
            i+=2;

            char buf[count];
            memset(buf, 0, count);

            mvme_set_am(vme, MVME_AM_A24);
            //mvme_set_am(vme, MVME_AM_A32);
            mvme_set_dmode(vme, MVME_DMODE_D32);
            mvme_set_blt(vme, imode);
            status = mvme_read(vme, buf, io32->fVmeBaseAddr + iaddr, count);

            printf("DMA status %d\n", status);

            for (int i=0; i<count; i+=4) {
               printf("data[%3d] 0x%08x\n", i, *((uint32_t*)(buf+i)));
            }

            printf("DMA status %d\n", status);
            exit(0);

         } else if (strcmp(argv[i],"--testdma")==0) {
            int imode  = strtoul(argv[i+1], NULL, 0);
            int iaddr  = strtoul(argv[i+2], NULL, 0);
            int count = strtoul(argv[i+3], NULL, 0);
            i+=2;

            int once = 1;
            uint64_t expected = 0;

            int errors = 0;

            for (int c=0; c<count; c++) {
               int count = 1024;
               char buf[count];
               memset(buf, 0, count);

               mvme_set_am(vme, MVME_AM_A24);
               //mvme_set_am(vme, MVME_AM_A32);
               mvme_set_dmode(vme, MVME_DMODE_D32);
               mvme_set_blt(vme, imode);
               status = mvme_read(vme, buf, io32->fVmeBaseAddr + iaddr, count);

               printf("DMA status %d\n", status);
               
               //for (int i=0; i<count; i+=4) {
               //printf("data[%3d] 0x%08x\n", i, *((uint32_t*)(buf+i)));
               //}
               
               for (int i=0; i<count; i+=8) {
                  uint64_t rd = *((uint64_t*)(buf+i));

                  if (once)
                     expected = rd;
                  once = 0;

                  int err = 0;
                  if (rd != expected)
                     err = 1;

                  if (err)
                     errors ++;

                  if (err)
                     printf("data64[%3d] 0x%016llx, expected 0x%016llx, diff 0x%016llx, errors %d so far\n", i, rd, expected, rd^expected, errors);

                  if (expected == 0)
                     expected = 1;
                  else
                     expected <<= 1;
               }

               printf("DMA status %d, errors %d\n", status, errors);
            }

            exit(0);

         } else if (strcmp(argv[i],"--testtdc")==0) {
            while (1) {
#if 0
               uint32_t t1 = io32->read32(0x24);
               uint32_t t2 = io32->read32(0x28);
               printf("t1: 0x%08x, t2: 0x%08x\n", t1, t2);
               print32(t1);
               printf(" last %d\n", findLast1(t1));
#endif

               static int ph_min[2][4] = { 9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999 };
               static int ph_max[2][4] = { -9999, -9999, -9999, -9999, -9999, -9999, -9999, -9999 };

               //printf("!\n");

               uint32_t ts[2] = { 0, 0 };

               int ph[2] = { 0, 0 };

               double clk = 10.0;

               double ns_per_bit = 0.5; // clk/(ph_max-ph_min);

               int bit_per_clk = clk/ns_per_bit;
               int xphase = 2*bit_per_clk;

               //printf("clk %f ns, ns_per_bit %f, bit_per_clk %d, xphase %d\n", clk, ns_per_bit, bit_per_clk, xphase);

               for (int ch = 0; ch<2; ch++)
                  for (int i=0; i<2; i++)
                     {
                        uint32_t tdc0_ph = io32->read32((60+2*ch)*4);
                        uint32_t tdc0_phh = io32->read32((58+ch)*4);
                        uint32_t tdc0_ts = io32->read32((61+2*ch)*4);

                        if (tdc0_ph == 0)
                           continue;

                        if (tdc0_ph == 0xFFFFFFFF)
                           continue;

                        ts[ch] = tdc0_ts;

                        uint32_t hh = fixHoles(tdc0_ph);

                        //ph0 = findBits(hh);
                        ph[ch] = findEdge(hh);

                        int tslsb = ts[ch]&1;

                        int ind = tslsb<<1;
                        if (ph[ch] > 0)
                           ind |= 1;

                        int xph0 = abs(ph[ch]);
                        
                        if (xph0 < ph_min[ch][ind])
                           ph_min[ch][ind] = xph0;
                        
                        if (xph0 > ph_max[ch][ind])
                           ph_max[ch][ind] = xph0;

                        //tph0 = (ph0-ph_min)*clk/(ph_max-ph_min);
                        
                        //printf("TDC[%d] count %d, ts 0x%08x, phase 0x%08x-0x%08x 0x%08x %3d lsb %d, %d:%d, %d:%d, %d:%d, %d:%d\n", ch, i, ts[ch], tdc0_phh, tdc0_ph, hh, ph[ch], tslsb, ph_min[ch][0], ph_max[ch][0], ph_min[ch][1], ph_max[ch][1], ph_min[ch][2], ph_max[ch][2], ph_min[ch][3], ph_max[ch][3]);
                     }

#if 0
               for (int i=0; i<2; i++)
                  {
                     uint32_t tdc1_ph = io32->read32(62*4);
                     uint32_t tdc1_phh = io32->read32(59*4);
                     uint32_t tdc1_ts = io32->read32(63*4);

                     if (tdc1_ph == 0)
                        continue;

                     if (tdc1_ph == 0xFFFFFFFF)
                        continue;

                     ts1 = tdc1_ts;

                     uint32_t hh = fixHoles(tdc1_ph);

                     //ph1 = findBits(hh);
                     ph1 = findEdge(hh);

#if 0
                     if (ph1 < ph_min)
                        ph_min = ph1;

                     if (ph1 > ph_max)
                        ph_max = ph1;
#endif

                     int xph1 = ph1;

                     if (ph1 < 0)
                        ph1 = -ph1 + xphase/2;

                     printf("TDC1 count %d, ts 0x%08x, phase 0x%08x-0x%08x 0x%08x %3d %3d lsb %d\n", i, ts1, tdc1_phh, tdc1_ph, hh, ph1, xph1, ts1&1);

                     //if (ts1&1 && ph1<0)
                     //   ph1 += xphase;
                  }
#endif

               if (ts[0] == 0 || ts[1] == 0)
                  {
                     sleep(1);
                     continue;
                  }

               double t0 = ts[0]*clk;
               double t1 = ts[1]*clk;

               //printf("0x%08x 0x%08x, %f %f - %f\n", ts0, ts1, t0, t1, t1-t0);

               double dtt = clk*((ts[1]-ts[0])&~1);

               int dph = 0;

               int xph[2];
               xph[0] = ph[0];
               xph[1] = ph[1];

               for (int i=0; i<2; i++)
                  {
                     if (xph[i] < 0)
                        xph[i] = -ph[i] + xphase/2;

                     if (xph[i] > xphase/2)
                        xph[i] -= xphase;

                     if (xph[i] < -xphase/2)
                        xph[i] += xphase;
                  }

               dph = xph[1] - xph[0];

               double tph0 = ph[0] * ns_per_bit; // this is wrong, should use min and max
               double tph1 = ph[1] * ns_per_bit; // same wrong

               if (tph0 < 0)
                  tph0 = -tph0 + clk;

               if (tph1 < 0)
                  tph1 = -tph1 + clk;

               if ((ts[0] & 1) == 0)
                  if (tph0 > clk)
                     tph0 -= 2*clk;

               if ((ts[1] & 1) == 0)
                  if (tph1 > clk)
                     tph1 -= 2*clk;

               dtt = clk*(ts[1]&(~1)) + tph1 - (clk*(ts[0]&(~1)) + tph0);

               double tdph = tph1 - tph0;

               printf("ts 0x%08x 0x%08x, diff %5.0f ns, lsb %d %d, phase %3d %3d, %3d %3d, tph  %5.0f %5.0f ns, tdph %5.0f ns, dt ---> %5.0f ns\n", ts[0], ts[1], clk*(ts[1]-ts[0]), ts[0]&1, ts[1]&1, ph[0], ph[1], xph[0], xph[1], tph0, tph1, tdph, dtt);

               //if (ts1 != ts0)
               //printf("ts0 0x%08x, ts1 0x%08x, ph0 %3d, ph1 %3d\n", ts0, ts1, ph0, ph1);


               sleep(1);
            }
         } else if (strcmp(argv[i],"--testtdc4")==0) {

#ifdef HAVE_ROOT
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

            int tdc_reg = 9*4;

            double clk =  10.0; // TDC clock in ns.
            
            double ns_per_bit = 0.33; //clk/18; //0.25; // clk/(ph_max-ph_min);
            
            int bit_per_clk = clk/ns_per_bit;
            int xphase = 2*bit_per_clk;

            //printf("clk %f ns, ns_per_bit %f, bit_per_clk %d, xphase %d\n", clk, ns_per_bit, bit_per_clk, xphase);

            // reset the TDC
            //io32->write32(4*3, 0);
            io32->write32(tdc_reg, 0xFFFFFFFF);

            static int ph_min[4][4] = { 9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999 };
            static int ph_max[4][4] = { -9999, -9999, -9999, -9999, -9999, -9999, -9999, -9999 };

            uint32_t ts[4] = { 0, 0, 0, 0 };

            int ph[4] = { 0, 0, 0, 0 };


            while (1) {


               uint32_t xt1 = io32->read32(tdc_reg);
               //printf("xt1: 0x%08x\n", xt1);

               if (xt1 == 0xFFFFFFFF)
                  exit(1);

               if (xt1 == 0x00000000)
                  {
                     sleep(1);
                     continue;
                  }

               if ((xt1 & 0xFFF00000) == 0xFEE00000)
                  {
                     int ch = (xt1 & 0x000F0000) >> 16;
                     uint32_t tdc0_ph  = io32->read32(tdc_reg);
                     uint32_t tdc0_phh = io32->read32(tdc_reg);
                     uint32_t tdc0_ts  = io32->read32(tdc_reg);

                     //assert(ch>=0 && ch<=1);

                     ts[ch] = tdc0_ts;

                     uint32_t hh = fixHoles(tdc0_ph);
                     
                     //ph0 = findBits(hh);
                     ph[ch] = findEdge(hh);
                     
                     int tslsb = ts[ch]&1;
                     
                     int ind = tslsb<<1;
                     if (ph[ch] > 0)
                        ind |= 1;
                     
                     int xph0 = abs(ph[ch]);
                     
                     if (xph0 < ph_min[ch][ind])
                        ph_min[ch][ind] = xph0;
                     
                     if (xph0 > ph_max[ch][ind])
                        ph_max[ch][ind] = xph0;
                     
                     //tph0 = (ph0-ph_min)*clk/(ph_max-ph_min);
                        
                     printf("TDC[%d] count %d, ts 0x%08x, phase 0x%08x-0x%08x 0x%08x %3d\n", ch, i, ts[ch], tdc0_phh, tdc0_ph, hh, ph[ch]);

#ifdef HAVE_ROOT
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

                     if (ch != 1)
                        continue;
                  }

               if (ts[0] == 0 || ts[1] == 0)
                  {
                     sleep(1);
                     continue;
                  }

#ifdef HAVE_ROOT
               //if (ph[0] < 0)
               //   hphasen0->Fill(-ph[0]);
               //else
               //hphasep0->Fill(ph[0]);
               
               //if (ph[1] < 0)
               //   hphasen1->Fill(-ph[1]);
               //else
               //   hphasep1->Fill(ph[1]);
#endif

               double t0 = ts[0]*clk;
               double t1 = ts[1]*clk;

               //printf("0x%08x 0x%08x, %f %f - %f\n", ts0, ts1, t0, t1, t1-t0);

               double dtt = clk*((ts[1]-ts[0])&~1);

               int dph = 0;

               int xph[2];
               xph[0] = ph[0];
               xph[1] = ph[1];

               for (int i=0; i<2; i++)
                  {
                     if (xph[i] < 0)
                        xph[i] = -ph[i] + xphase/2;

                     if (xph[i] > xphase/2)
                        xph[i] -= xphase;

                     if (xph[i] < -xphase/2)
                        xph[i] += xphase;
                  }

               dph = xph[1] - xph[0];

               double tph0 = ph[0] * ns_per_bit; // this is wrong, should use min and max
               double tph1 = ph[1] * ns_per_bit; // same wrong

               if (tph0 < 0)
                  tph0 = -tph0 + clk;

               if (tph1 < 0)
                  tph1 = -tph1 + clk;

#ifdef HAVE_ROOT
               htph0->Fill(tph0);
               htph1->Fill(tph1);
#endif

               if ((ts[0] & 1) == 0)
                  if (tph0 > clk)
                     tph0 -= 2*clk;

               if ((ts[1] & 1) == 0)
                  if (tph1 > clk)
                     tph1 -= 2*clk;

               dtt = clk*(ts[1]&(~1)) + tph1 - (clk*(ts[0]&(~1)) + tph0);

               double tdph = tph1 - tph0;

               printf("ts 0x%08x 0x%08x, diff %5.0f ns, lsb %d %d, phase %3d %3d, %3d %3d, tph  %5.0f %5.0f ns, tdph %5.0f ns, dt ---> %5.0f ns\n", ts[0], ts[1], clk*(ts[1]-ts[0]), ts[0]&1, ts[1]&1, ph[0], ph[1], xph[0], xph[1], tph0, tph1, tdph, dtt);

               //if (ts1 != ts0)
               //printf("ts0 0x%08x, ts1 0x%08x, ph0 %3d, ph1 %3d\n", ts0, ts1, ph0, ph1);


               //sleep(1);
#ifdef HAVE_ROOT
               htdc->Fill(dtt);

               for (int i=1; i<=8; i++)
                  gWindow->cd(i)->Modified();

               gWindow->Modified();
               gWindow->Update();
#endif

            }
         }
   }

   if (!io32)
      io32 = new VMENIMIO32(vme, addr);
   io32->fDebug = true;

   exit(0);

   if (1) {
      probe(io32, 0x0);
      probe(io32, 0x4);
      probe(io32, 0x8);
      probe(io32, 0xC);
      probe(io32, 0x10);
      probe(io32, 0xFF0);
      probe(io32, 0xFFF0);
      //probe(io32, 0x10000);

      io32->write32(0x8, 0xdeadbeef);
      probe(io32, 0x8);
      io32->write32(0x8, 0xffffffff);
      probe(io32, 0x8);
      io32->write32(0x8, 0x00000000);
      probe(io32, 0x8);

      while (1) {
         io32->write32(0x8, 0xaaaaffff);
         io32->read32(0x8);
         //probe(io32, 0x8);
         sleep(1);
         io32->write32(0x8, 0x00000000);
         probe(io32, 0x8);
         sleep(1);
      }
   }

   if (0) {
      for (uint32_t i=0; ; i++) {
         printf("write 0x%x\n", i);
         io32->write32(0x10, i);
         sleep(1);
      }
   }

   if (1) {
      for (uint32_t i=0; ; i++) {
         printf("write 0x%x\n", i);
         io32->write32(0x10, i);
         sleep(1);
         probe(io32, 0x10);
         sleep(1);
      }
   }

   if (0) {
      for (uint32_t i=0; ; i++) {
         io32->write32(0x10, i);
      }
   }

   if (0) {
      for (uint32_t i=0; ; i++) {
         io32->read32(0x10);
      }
   }

   if (1) {
      io32->write32(0x8, 0xaaaaffff);
      probe(io32, 0x8);

      while (1) {
         uint32_t w = io32->read32(0xC);
         printf("reg 0xC: 0x%08x, inv 0x%08x\n", w, ~w);
         sleep(1);
      }
   }

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
