/********************************************************************\

  Name:         fevme.cxx
  Created by:   K.Olchanski

  Contents:     Frontend for the ALPHA VME DAQ

  $Id: fevme.cxx 1173 2008-06-28 03:47:35Z olchansk $

\********************************************************************/

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
#include "sis3820drv.h"
#include "sis3820.h"
}

#if 0
int gMcsBase[] = { 0x38000000, 0x39000000, 0 };


#ifdef HAVE_SIS3820

  for (int i=0; gMcsBase[i]!=0; i++)
    {
      sis3820_Reset(gVme,gMcsBase[i]);

      int sismode = odbReadUint32("/experiment/edit on start/SIS_mode",0,0);
      if (sismode == 0)
        {
          printf("SIS mode 0: external LNE\n");
          sis3820_RegisterWrite(gVme,gMcsBase[i],SIS3820_LNE_PRESCALE,0);
          sis3820_RegisterWrite(gVme,gMcsBase[i],SIS3820_OPERATION_MODE,
                                SIS3820_CLEARING_MODE|
                                SIS3820_MCS_DATA_FORMAT_32BIT|
                                SIS3820_LNE_SOURCE_CONTROL_SIGNAL|
                                SIS3820_FIFO_MODE|
                                SIS3820_OP_MODE_MULTI_CHANNEL_SCALER|
                                SIS3820_CONTROL_INPUT_MODE1|
                                SIS3820_CONTROL_OUTPUT_MODE1);
        }
      else if (sismode == 1)
        {
          int prescale = odbReadUint32("/experiment/edit on start/SIS_LNE_prescale",0,40);
          
          printf("SIS mode 1: internal LNE, prescale %d\n", prescale);
          
          // LNE prescale 40 is for max VME rate 30 Mbytes/sec
          sis3820_RegisterWrite(gVme,gMcsBase[i],SIS3820_LNE_PRESCALE,prescale);
          sis3820_RegisterWrite(gVme,gMcsBase[i],SIS3820_OPERATION_MODE,
                                SIS3820_CLEARING_MODE|
                                SIS3820_MCS_DATA_FORMAT_32BIT|
                                SIS3820_LNE_SOURCE_INTERNAL_10MHZ|
                                SIS3820_FIFO_MODE|
                                SIS3820_OP_MODE_MULTI_CHANNEL_SCALER|
                                SIS3820_CONTROL_INPUT_MODE1|
                                SIS3820_CONTROL_OUTPUT_MODE1);
        }
      sis3820_Status(gVme,gMcsBase[i]);
    }


static const int kNumSisModules    =  2;   // number of SIS modules
static const int kSisChanPerModule = 32;   // number of channels in each SIS module
static const int kNumSisChannels = kNumSisModules*kSisChanPerModule; // total number of SIS channels
static uint32_t  gSumMcsEvents[kNumSisModules]; // sum the number of events
static uint32_t  gMaxMcs[kNumSisChannels];  // max value of SIS channels
static uint64_t  gSumMcs[kNumSisChannels];  // sum SIS channels
static uint32_t  gSaveMcs[kNumSisChannels]; // sampled SIS data

static uint32_t wc = 0;

static double timeDiff(const struct timeval&t1,const struct timeval&t2)
{
  return (t1.tv_sec - t2.tv_sec) + 0.000001*(t1.tv_usec - t2.tv_usec);
}

static struct timeval gSisLastRead[kNumSisModules];

static int have_SIS_data(int i)
{
  struct timeval now;

  int haveData = sis3820_DataReady(gVme,gMcsBase[i]);
  if (haveData == (-1))
    haveData = 0;

  int minread  = 1000*32;

  gettimeofday(&now,NULL);

  double td = timeDiff(now, gSisLastRead[i]);

  //  printf("sis: %d, td: %f, haveData: %d\n",i,td,haveData);

  if (td < 1.0 && haveData < minread)
    return 0;

  //int toRead = minread;
  //int toRead = 128*1024/4;
  int toRead = max_event_size/4 - 5000;

  if (toRead > haveData)
    toRead = haveData;

  if (0)
    if (toRead & 0x1F)
      printf("odd available data: %d (%d)\n",toRead,toRead&0x1F);

  // make sure we always read in multiples of 32 words
  toRead = toRead & 0xFFFFFFE0;

  if (toRead == 0)
    return 0;

  return toRead;
}

static int read_SIS(char* pevent, int isis, int toRead32)
{
  assert(isis >= 0);
  assert(isis < kNumSisModules);

  //if (toRead32*4 < 20000)
  //  return 0;
  //
  //toRead32 = 32*1;

  int used = bk_size(pevent);
  int free = max_event_size - used;

  if (toRead32*4 > free)
    {
      //printf("read_SIS: buffer used %d, free %d, toread %d\n", used, free, toRead32*4);
      toRead32 = (free-10*1024)/4;
      toRead32 = toRead32 & 0xFFFFFFE0;

      if (toRead32 <= 0)
        return 0;
    }

  int maxDma = 256*1024;
  if (toRead32*4 > maxDma)
    toRead32 = maxDma/4;

  gettimeofday(&gSisLastRead[isis], NULL);

  /* create data bank */
  uint32_t *pdata32;

  char bankname[] = "MCS0";
  bankname[3] = '0' + isis; 
  bk_create(pevent, bankname, TID_DWORD, &pdata32);

  //printf("read %d words\n",toRead32);

  int rd = sis3820_FifoRead(gVme,gMcsBase[isis],pdata32,toRead32);

  wc += toRead32*4;

  if (0)
    {
      printf("sis3820 data: 0x%x 0x%x 0x%x 0x%x\n",pdata32[16],pdata32[17],pdata32[18],pdata32[19]);
    }

  if (0)
    {
      printf("sis3820 data: rd: %d,",rd);
      for (int i=0; i<toRead32; i++)
        printf(" 0x%x",pdata32[i]);
      printf("\n");
    }

  bk_close(pevent, pdata32 + rd);

  int numEvents = toRead32/kSisChanPerModule;
  uint32_t *mptr = pdata32;
  int offset = isis*kSisChanPerModule;
  for (int ievt=0; ievt<numEvents; ievt++)
    {
      gSumMcsEvents[isis]++;
      for (int i=0; i<kSisChanPerModule; i++)
        {
          uint32_t v = *mptr++;
          gSumMcs[offset+i] += v;
          gSaveMcs[offset+i] = v;
          if (v > gMaxMcs[offset+i])
            gMaxMcs[offset+i] = v;
        }
    }

  return 1;
}


INT read_mcs_event(char *pevent, INT off)
{
#ifdef HAVE_SIS3820
  int haveData0 = sis3820_DataReady(gVme,gMcsBase[0]);
  int haveData1 = sis3820_DataReady(gVme,gMcsBase[1]);

  const int kSisBufferSize = 16000000;
  if (haveData0 > kSisBufferSize || haveData1 > kSisBufferSize)
    if (!gSisBufferOverflow)
      {
        gSisBufferOverflow = 1;
        al_trigger_alarm("sis_overflow", "SIS buffer overflow: LNE rate is too high",
                         "Warning", "", AT_INTERNAL);
      }

  //printf("read mcs event, have data: %d\n", haveData);

  /* init bank structure */
  bk_init32(pevent);

  uint32_t *pdata32;
  bk_create(pevent, "MCMX", TID_DWORD, &pdata32);
  for (int i=0; i<kNumSisChannels; i++) // kNumSisChannels -> static var : max number of SIS channels
    pdata32[i] = gMaxMcs[i];    // Max value of SIS channel
  bk_close(pevent, pdata32+kNumSisChannels);

  bk_create(pevent, "MCSA", TID_DWORD, &pdata32);
  for (int i=0; i<kNumSisChannels; i++)
    pdata32[i] = gSaveMcs[i];  // Sampled SIS data
  bk_close(pevent, pdata32+kNumSisChannels);

  if (false)
    {
      printf("MCS fifo: %12d, wc: %12d, 0x%08x, %6d MiB, data: %d %d %d %d\n", haveData0, wc, wc, wc/(1024*1024), gSaveMcs[16], gSaveMcs[17], gSaveMcs[18], gSaveMcs[19]);
    }

  uint32_t numClocks = gSumMcs[gMcsClockChan];
  double dt = numClocks/gMcsClockFreq;
  double dt1 = 0;
  if (dt > 0)
    dt1 = 1.0/dt; 

#if 0
  static struct timeval gTime;

  struct timeval t;
  gettimeofday(&t,NULL);

  double t1 = gTime.tv_sec + 0.000001*gTime.tv_usec;
  double t2 = t.tv_sec + 0.000001*t.tv_usec;
  double dt1 = 1.0/(t2 - t1);

  if (t1 == 0)
    dt1 = 0;
#endif

  float *pfloat;
  bk_create(pevent, "MCRT", TID_FLOAT, &pfloat);
  for (int i=0; i<kNumSisChannels; i++)
    pfloat[i] = gSumMcs[i]*dt1;  // sample RaTe in Hz
  bk_close(pevent, pfloat+kNumSisChannels);

#if 0
  gTime = t;
#endif

  bk_create(pevent, "MCST", TID_FLOAT, &pfloat);
  for (int i=0; i<10; i++)
    pfloat[i] = 0;
  pfloat[0] = haveData0;         // amount of data still buffered in the SIS
  pfloat[1] = gSumMcsEvents[0];  // number of LNE events from the SIS
  pfloat[2] = gSumMcsEvents[0]*dt1; // LNE frequency
  pfloat[3] = haveData1;         // amount of data still buffered in the SIS
  pfloat[4] = gSumMcsEvents[1];  // number of LNE events from the SIS
  pfloat[5] = gSumMcsEvents[1]*dt1; // LNE frequency
  pfloat[6] = haveData0 - haveData1;
  bk_close(pevent, pfloat+10);
  
  if (gMcsAdChan >= 0 && !gIsPedestalsRun)
    if (gSumMcs[gMcsAdChan] > 0)
      {
        //printf("bingo %d %d\n",gMcsAdChan,(int)gSumMcs[gMcsAdChan]);
        cm_msg(MTALK,frontend_name,gMcsAdTalk);
      }

  if (gMcsMixChan >= 0 && !gIsPedestalsRun)
    if (gSumMcs[gMcsMixChan] > 0)
      {
        //printf("bingo %d %d\n",gMcsMixChan,(int)gSumMcs[gMcsMixChan]);
        cm_msg(MTALK,frontend_name,gMcsMixTalk);
      }

  for (int i=0; i<kNumSisModules; i++)
    gSumMcsEvents[i] = 0;
  for (int i=0; i<kNumSisChannels; i++)
    gSumMcs[i] = 0;

  return bk_size(pevent);
#else
  return 0;
#endif
}
#endif
#endif

double utime()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  //printf("gettimeofday: %d %d\n", tv.tv_sec, tv.tv_usec);
  return tv.tv_sec + 0.000001*tv.tv_usec;
}

MVME_INTERFACE* gVme;

int main(int argc, char* argv[])
{
   int status;

   status = mvme_open(&gVme,0);
   status = mvme_set_am(gVme, MVME_AM_A24);
   status = mvme_set_dmode(gVme, MVME_DMODE_D32);

   int base = 0x39000000;

   sis3820_Reset(gVme, base);
   sis3820_Status(gVme, base);

   printf("available data: %d\n", sis3820_DataReady(gVme, base));

   // LNE prescale 40 is for max VME rate 30 Mbytes/sec
   int prescale = 10000;
   sis3820_RegisterWrite(gVme, base, SIS3820_LNE_PRESCALE, prescale);
   sis3820_RegisterWrite(gVme, base, SIS3820_OPERATION_MODE,
                         SIS3820_CLEARING_MODE|
                         SIS3820_MCS_DATA_FORMAT_32BIT|
                         SIS3820_LNE_SOURCE_INTERNAL_10MHZ|
                         SIS3820_FIFO_MODE|
                         SIS3820_OP_MODE_MULTI_CHANNEL_SCALER|
                         SIS3820_CONTROL_INPUT_MODE1|
                         SIS3820_CONTROL_OUTPUT_MODE1);

   sis3820_Status(gVme, base);
   printf("available data: %d\n", sis3820_DataReady(gVme, base));

   sis3820_RegisterWrite(gVme, base, SIS3820_KEY_OPERATION_ENABLE, 0);

   sleep(1);

   sis3820_Status(gVme, base);
   printf("available data: %d\n", sis3820_DataReady(gVme, base));

   exit(1);

   while (1)
      {
         DWORD p32[1000000];

         sis3820_Status(gVme, base);
         printf("available data: %d\n", sis3820_DataReady(gVme, base));

         int toread = sis3820_DataReady(gVme, base);

         double t0 = utime();

         int count = sis3820_FifoRead(gVme, base, p32, toread);

         double t1 = utime();
         printf("sis3820 0x%08x: sis3820_FifoRead count %d words, time %f s, rate %.6f Mb/s\n", base, count, t1-t0, count*4/(t1-t0)/1000000.0);

         sleep(1);
      }

   return 0;
}

//end
