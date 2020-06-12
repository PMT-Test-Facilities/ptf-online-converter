//
// Name: alphaTTC.cxx
// Author: K.Olchanski
// Date: 2 May 2007
// Description: driver for the ALPHA trigger and control TTC board
//
// $Id: alphaTTC.cxx 1176 2008-07-04 04:46:12Z olchansk $
//

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

#include "VMENIMIO32.h"
#include "mvmestd.h"

uint32_t VMENIMIO32::read32(int ireg)
{
  mvme_set_am((MVME_INTERFACE*)fVme, MVME_AM_A24);
  mvme_set_dmode((MVME_INTERFACE*)fVme, MVME_DMODE_D32);
  return mvme_read_value((MVME_INTERFACE*)fVme, fVmeBaseAddr + ireg);
}

void VMENIMIO32::write32(int ireg, uint32_t data)
{
  mvme_set_am((MVME_INTERFACE*)fVme, MVME_AM_A24);
  mvme_set_dmode((MVME_INTERFACE*)fVme, MVME_DMODE_D32);
  mvme_write_value((MVME_INTERFACE*)fVme, fVmeBaseAddr + ireg, data);
}

uint32_t VMENIMIO32::flip32(int ireg, uint32_t setBits, uint32_t clearBits, uint32_t invertBits)
{
  mvme_set_am((MVME_INTERFACE*)fVme, MVME_AM_A24);
  mvme_set_dmode((MVME_INTERFACE*)fVme, MVME_DMODE_D32);
  const uint32_t oldvalue = mvme_read_value((MVME_INTERFACE*)fVme, fVmeBaseAddr + ireg);
  uint32_t data = oldvalue;
  data |= setBits;
  data &= ~clearBits;
  data ^= invertBits;
  mvme_write_value((MVME_INTERFACE*)fVme, fVmeBaseAddr + ireg, data);
  return oldvalue;
}

VMENIMIO32::VMENIMIO32(void* vme, uint32_t vmeA32addr)
{
  fVme = vme;
  fVmeBaseAddr = vmeA32addr;
  fDebug = 0;
  fRevision = 0;

#if 0
  uint32_t reg00 = read32(0x00);
  uint32_t reg04 = read32(0x04);
  uint32_t reg08 = read32(0x08);

  printf("VMENIMIO32: VME A24 addr: 0x%08x, reg00: 0x%x, reg04: 0x%x, reg08: 0x%x\n",
         vmeA32addr,
         reg00,
         reg04,
         reg08);

  fRevision = reg00;
#endif
};

uint32_t VMENIMIO32::revision()
{
   if (!fRevision) {
      fRevision =  read32(0);
   }
   
   return fRevision;
}

int VMENIMIO32::init()
{
  status();
  return 0;
};

int VMENIMIO32::status()
{
   uint32_t reg;

   printf("VMENIMIO32 status:\n");

   reg = read32(0);
   printf("reg 0x00: 0x%08x, firmare revision\n", reg);

#if 0
  uint32_t reg48 = read32(0x48);
  SetDelayScales(reg48);

  int reg = read8(0);
  printf("reg0: 0x%02x, %s %s %s %s %s %s %s %s\n", reg, (reg&(1<<7))?"BOOT_LOAD":"",(reg&(1<<6))?"BOOT_READ":"",(reg&(1<<5))?"BOOT_REGIN":"",(reg&(1<<4))?"BOOL_CLKIN":"",(reg&(1<<3))?"bit3":"",(reg&(1<<2))?"DIS_VETO":"",(reg&(1<<1))?"TRIGIN_ENA":"",(reg&(1<<0))?"TRIGOUT_ENA":"");
  reg = read8(1);
  printf("reg1: 0x%02x, FRC_REGOUT %d, BUSY %d\n", reg, reg&(1<<2), reg&(1<<0));
  reg = read8(2);
  printf("reg2: 0x%02x, FRC address: 0x%x\n", reg, reg);
  reg = read8(4);
  printf("reg4: 0x%02x, ADC delay %d ns\n", reg, reg*adcDelayScale);
  reg = read8(5);
  printf("reg5: 0x%02x, HOLD delay %d ns\n", reg, reg*holdDelayScale);

  if (fRevision <= 2008050301)
    {
      reg = read8(6);
      printf("reg6: 0x%02x, CAL2/DACB set to %f mV on 50 Ohm termination\n", reg, reg*1.5);
      reg = read8(7);
      printf("reg7: 0x%02x, CAL1/DACA set to %f mV on 50 Ohm termination\n", reg, reg*1.5);
    }

  reg = read32(0x48);
  printf("reg 0x48: %d, revision code\n", reg);

  if (fRevision >= 2008021701)
    {
      if (0)
	{
          reg = read32(0x88);
          printf("reg 0x88: 0x%08x, trigger FPGA write\n", reg);
          reg = read32(0x8C);
          printf("reg 0x8C: 0x%08x, trigger FPGA read\n", reg);
	}
      for (int i=0; i<6; i++)
	{
	  int addr = 0x90 + i*4;
	  reg = read32(addr);
	  printf("reg 0x%02x: 0x%08x, counter for NIM output TRIG%d\n", addr, reg, i+1);
	}
      reg = read32(0xA8);
      printf("reg 0xA8: 0x%08x, counter for NIM output timestamp\n", reg);
      reg = read32(0xAC);
      printf("reg 0xAC: 0x%08x, VA1TA mux control:", reg);
      if (reg&TTC_AC_SEQ_DIS)
	printf(" SEQ_DIS");
      if (reg&TTC_AC_CKB_POL)
	printf(" CKB_POL");
      if (reg&TTC_AC_SHIN_POL)
	printf(" SHIN_POL");
      if (reg&TTC_AC_HOLD_ENA)
	printf(" HOLD_ENA");
      if (reg&TTC_AC_HOLD_POL)
	printf(" HOLD_POL");
      if (reg&TTC_AC_RESET_ENA)
	printf(" RESET_ENA");
      if (reg&TTC_AC_RESET_POL)
	printf(" RESET_POL");
      if (reg&TTC_AC_DAC_CSN)
	printf(" DAC_CSN");
      if (reg&TTC_AC_DAC_DIN)
	printf(" DAC_DIN");
      if (reg&TTC_AC_DAC_SCLK)
	printf(" DAC_SCLK");
      if (reg&TTC_AC_DAC_LEVEL)
	printf(" DAC_LEVEL");
      printf("\n");
      reg = read32(0xB0);
      printf("reg 0xB0: 0x%08x, self trigger control\n", reg);
    }

  for (int ifpga=0; ifpga<2; ifpga++)
    {
      printf("\n");
      printf("Trigger FPGA %d\n", ifpga);

      printf("  reg 0x00: 0x%08x (routing for M1/M4 and M2/M5)\n", readTriggerFPGA(ifpga, 0));
      printf("  reg 0x01: 0x%08x (routing for M3/M6 and misc control)\n", readTriggerFPGA(ifpga, 1));
      printf("  reg 0x02: 0x%08x (multiplicity latch)\n", readTriggerFPGA(ifpga, 2));
      printf("  reg 0x03: 0x%08x (latch counter)\n", readTriggerFPGA(ifpga, 3));

      printf("  reg 0x04: 0x%08x (TA layer map)\n", readTriggerFPGA(ifpga, 4));
      printf("  reg 0x05: 0x%08x (TA layer map)\n", readTriggerFPGA(ifpga, 5));
      printf("  reg 0x06: 0x%08x (TA layer map)\n", readTriggerFPGA(ifpga, 6));
      printf("  reg 0x07: 0x%08x (TA layer map)\n", readTriggerFPGA(ifpga, 7));

#if 1
      printf("  reg 0x%02x+8: ", 8);
      for (int i=0; i<8; i++)
	printf(" 0x%04x", 0xFFFF & readTriggerFPGA(ifpga, 8+i));
      printf(" (TA latch)\n");

      printf("  reg 0x%02x+16: ", 16);
      for (int i=0; i<16; i++)
	printf(" 0x%04x", 0xFFFF & readTriggerFPGA(ifpga, 16+i));
      printf(" (TA counters)\n");
#endif

      printf("  reg 32: 0x%08x (fifo status)\n", readTriggerFPGA(ifpga, 32));
      printf("  reg 33: xxxxxxxxxx (fifo data)\n");

      printf("  reg 34: 0x%08x (layer A/1 min/max multiplicity)\n", readTriggerFPGA(ifpga, 34));
      printf("  reg 35: 0x%08x (layer B/2 min/max multiplicity)\n", readTriggerFPGA(ifpga, 35));
      printf("  reg 36: 0x%08x (layer C/3 min/max multiplicity)\n", readTriggerFPGA(ifpga, 36));

      printf("  reg 37: 0x%08x (time stamp latch, low bits)\n", readTriggerFPGA(ifpga, 37));
      printf("  reg 38: 0x%08x (time stamp latch, high bits)\n", readTriggerFPGA(ifpga, 38));

      printf("  reg 39: 0x%08x (multa counter)\n", readTriggerFPGA(ifpga, 39));
      printf("  reg 40: 0x%08x (multb counter)\n", readTriggerFPGA(ifpga, 40));
      printf("  reg 41: 0x%08x (multc counter)\n", readTriggerFPGA(ifpga, 41));
      printf("  reg 42: 0x%08x (mult_and counter)\n", readTriggerFPGA(ifpga, 42));

      printf("  reg 43: 0x%08x (layer A/1 min/max multiplicity)\n", readTriggerFPGA(ifpga, 43));
      printf("  reg 44: 0x%08x (layer B/2 min/max multiplicity)\n", readTriggerFPGA(ifpga, 44));
      printf("  reg 45: 0x%08x (layer C/3 min/max multiplicity)\n", readTriggerFPGA(ifpga, 45));
      printf("  reg 46: 0x%08x (multiplicity latch)\n", readTriggerFPGA(ifpga, 46));
    }

  printf("\n");
  printf("Trigger configuration:\n");
  printf("\n");

  uint32_t regB0 = read32(0xB0);
  printf("Self trigger regB0: 0x%08x: ", regB0);
  if (regB0 & 0x10000)
     printf(" (TRIG123 or TRIG456)");
  if (regB0 & 0x20000)
     printf(" (TRIG123 and TRIG456)");
  printf("\n");

  printf("  TRIG123: ");
  if (regB0 & 0x1)
     printf(" TRIG1");
  if (regB0 & 0x2)
     printf(" TRIG2");
  if (regB0 & 0x4)
     printf(" TRIG3");
  if (regB0 & 0x10)
     printf(" TRIG1*TRIG2");
  if (regB0 & 0x20)
     printf(" TRIG1*TRIG3");
  if (regB0 & 0x40)
     printf(" TRIG2*TRIG3");
  if (regB0 & 0x80)
     printf(" TRIG1*TRIG2*TRIG3");
  printf("\n");

  printf("  TRIG456: ");
  if (regB0 & 0x100)
     printf(" TRIG4");
  if (regB0 & 0x200)
     printf(" TRIG5");
  if (regB0 & 0x400)
     printf(" TRIG6");
  if (regB0 & 0x1000)
     printf(" TRIG4*TRIG5");
  if (regB0 & 0x2000)
     printf(" TRIG4*TRIG6");
  if (regB0 & 0x4000)
     printf(" TRIG5*TRIG6");
  if (regB0 & 0x8000)
     printf(" TRIG4*TRIG5*TRIG6");
  printf("\n");

  printf("TRIG1..6 source:\n");

  for (int i=1; i<=6; i++)
     {
        int isource = getTrig(i);
        printf("  TRIG%d is %d (%s)\n", i, isource, trigSourceName(isource));
     }

  for (int ifpga=0; ifpga<2; ifpga++)
     {
        printf("FPGA%d trigger configuration:\n", ifpga);

#if 0
        printf("  Layer  map: 0x%04x %04x %04x %04x\n",
               0xFFFF & readTriggerFPGA(ifpga, 7),
               0xFFFF & readTriggerFPGA(ifpga, 6),
               0xFFFF & readTriggerFPGA(ifpga, 5),
               0xFFFF & readTriggerFPGA(ifpga, 4));
#endif

        printf("  Enable map: 0x");
        for (int i=0; i<8; i++)
           {
              uint16_t v = getTaEnableBitmap(ifpga, i);
              printf(" %01x %01x %01x %01x ",
                     (v>>0)&0xF,
                     (v>>4)&0xF,
                     (v>>8)&0xF,
                     (v>>12)&0xF
                     );
           }

        if (fTaEnableBitmapIsWritten)
           printf("\n");
        else
           printf(" --- NOT WRITTEN TO HARDWARE!\n");

        printf("  Layer  map:   ");
        for (int i=4; i<8; i++)
           {
              uint32_t v = 0xFFFF & readTriggerFPGA(ifpga, i);
              for (int j=0 ;j<8; j++)
                 {
                    int ll = (v>>(j*2)) & 3;
                    printf(" %d", ll);

                    if (j%4 == 3)
                       printf(" ");
                 }
           }
        printf("\n");

        uint32_t multa = readTriggerFPGA(ifpga, 34);
        uint32_t multb = readTriggerFPGA(ifpga, 35);
        uint32_t multc = readTriggerFPGA(ifpga, 36);

        uint32_t multaa = readTriggerFPGA(ifpga, 43);
        uint32_t multbb = readTriggerFPGA(ifpga, 44);
        uint32_t multcc = readTriggerFPGA(ifpga, 45);

        printf("  Layer A: %d<=multa<=%d, %d<=multaa<=%d\n", multa&0xFF, (multa>>8)&0xFF, multaa&0xFF, (multaa>>8)&0xFF);
        printf("  Layer B: %d<=multb<=%d, %d<=multbb<=%d\n", multb&0xFF, (multb>>8)&0xFF, multbb&0xFF, (multbb>>8)&0xFF);
        printf("  Layer C: %d<=multc<=%d, %d<=multcc<=%d\n", multc&0xFF, (multc>>8)&0xFF, multcc&0xFF, (multcc>>8)&0xFF);
     }
#endif

  return 0;
}

int VMENIMIO32::SetNimOutputFunction(int ioutput, int ifunc)
{
   assert(ioutput >= 0 && ioutput < 4);
   assert(ifunc >= 0 && ifunc <= 3);

   int mask = (0x3)<<(16+2*ioutput);
   int data = (ifunc)<<(16+2*ioutput);

   uint32_t w32 = read32(IO32_NIMOUT); // read NIM output control register
   w32 &= ~mask;
   w32 |=  data;
   write32(IO32_NIMOUT, w32); // write NIM output control register

   printf("VMENIMIO32:: Set NIM output %d func %d, wrote IO32_NIMOUT 0x%08x\n", ioutput, ifunc, w32);

   return 0;
}

void VMENIMIO32::SetTsc4Input(int itsc4chan, int input)
{
   assert(itsc4chan>=0 && itsc4chan<4);
   assert(input>=0 && input<32);

   uint32_t r = read32(IO32_TSC4_ROUTE);
   r &= ~(0x1F<<(itsc4chan*5));
   r |= ((input)<<(itsc4chan*5));
   write32(IO32_TSC4_ROUTE, r);
}

int VMENIMIO32::NimOutput(int setBits, int clearBits)
{
   setBits   &= 0xFFFF;
   clearBits &= 0xFFFF;

   uint32_t r = read32(IO32_NIMOUT);
   r |= setBits;
   r &= ~clearBits;
   write32(IO32_NIMOUT, r);
   
   return 0;
}

int VMENIMIO32::ReadScalers33(uint32_t* data)
{
   write32(IO32_COMMAND, IO32_CMD_LATCH_SCALERS);

   for (int i=0; i<32; i++)
      data[i] = read32(IO32_SCALER_0 + 4*i);

   data[32] = read32(IO32_SCALERS_TS);

   return 0;
}

int VMENIMIO32::ReadScaler(uint32_t* data, int maxwords)
{
   assert(revision() >= 0x01120524);
   
   int count = 0;
   uint32_t fifoStatus = 0;

   for (int i=0; i<1000; i++) {
      fifoStatus = read32(IO32_SCALER_STATUS);
      if (fifoStatus & 0x2000) // wait for "busy" flag to clear
         continue;
      break;
   }

   //printf("status 0x%08x\n", fifoStatus);

   if (fifoStatus & 0x4000) { // FIFO full
      return -1;

      //uint32_t w = read32(IO32_SCALER_FIFO);
      //uint32_t c = (w>>4) + (w&0xF);
      //data[count++] = c;
      //fifoStatus = read32(IO32_SCALER_STATUS);
      //printf("new status 0x%08x\n", fifoStatus);
   }

   int nw = fifoStatus & 0xfff;

   for (int i=0; i<nw; i++) {
      fifoStatus = read32(IO32_SCALER_STATUS);
      uint32_t w = read32(IO32_SCALER_FIFO);
      uint32_t c = (w>>4) + (w&0xF);
      //uint32_t c = (w&0xF);
      
      //printf("read status 0x%08x. data 0x%08x 0x%08x, count %d\n", fifoStatus, w, c, count);
      data[count++] = c;
      if (count >= maxwords)
         return count;
   }

   return count;
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
