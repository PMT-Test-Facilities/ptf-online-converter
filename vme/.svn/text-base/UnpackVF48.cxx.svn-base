// UnpackVF48.cxx

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>

#include "UnpackVF48.h"

static int   gNumModules = 0;
static uint32_t gGroupEnabled[VF48_MAX_MODULES];
static bool  gChanEnabled[VF48_MAX_MODULES][VF48_MAX_CHANNELS];
static int   gNumSamples[VF48_MAX_MODULES];
static int   gTrigWarning = 0;

static double gVF48freq[VF48_MAX_MODULES] = { 60.0e6, 60.0e6, 60.0e6, 60.0e6, 60.0e6, 60.0e6, 60.0e6 };

int    gVF48badDataCount = 0;

static int gDelay = 2;

static int tgroup = 0;

class Context
{
public:
   int module;
   int group;

   uint32_t headerTrigNo;
   uint32_t trailerTrigNo;
   int      timestampCount;
   uint32_t timestamp1;
   uint32_t timestamp2;
   int      timestampFlag;
   int      currentGroup;
   int      currentChannel;
   //VF48module *m;
   bool accept;

   void Init()
   {
      headerTrigNo = 0;
      trailerTrigNo = 0;
      timestampCount = 0;
      timestamp1 = 0;
      timestamp2 = 0;
      currentGroup = 0;
      currentChannel = 0;
      //m = 0;
      accept = false;
   }
};

Context gContexts[VF48_MAX_MODULES][VF48_MAX_GROUPS];

VF48module::VF48module(int i) // ctor  
{
  unit = i;
  trigger = 0;
  error = 0;
  for (int i=0; i<VF48_MAX_GROUPS; i++) {
    timestamp64[i] = 0;
    timestamps[i] = 0;
  }
  for (int i=0; i<VF48_MAX_GROUPS; i++)
    suppressMask[i] = 0;
  complete = false;
  completeGroupMask = 0;
  timestampGroupMask = 0;
  groupMask = 0;
  numSamples = 0;
  numChannels = 0;
  for (int i=0; i<VF48_MAX_CHANNELS; i++)
    {
      channels[i].channel    = -1;
      channels[i].numSamples =  0;
    }
}

void VF48module::Print() const
{
  printf("Unit %d, trigger %d, timestamp %.3fms, samples %d, channels %d, complete %d, error %d\n", 
         unit,
         trigger,
         timestamps[tgroup]*1000.0, // convert sec to msec
         numSamples,
         numChannels,
         complete,
	 error);
}

void VF48module::Complete()
{
  if (groupMask == 0)
    {
      // empty event
      complete = true;
      return;
    }

  for (int i=0; i<VF48_MAX_CHANNELS; i++)
    {
      if (!gChanEnabled[unit][i])
        continue;

      // no data for this channel
      if (channels[i].channel==(-1) && channels[i].numSamples==0)
        continue;
      
      // channel is suppressed
      if (channels[i].numSamples==0)
        continue;
      
      if (channels[i].numSamples != gNumSamples[unit])
	{
	  printf("*** Unit %d, trigger %d: Channel %d %d has %4d samples, should be %3d\n", unit, trigger, i, channels[i].channel, channels[i].numSamples, gNumSamples[unit]), gVF48badDataCount++;
	  error = 1;
	}

      if (channels[i].numSamples > VF48_MAX_SAMPLES)
        channels[i].numSamples = VF48_MAX_SAMPLES;
    }
  
  double t = timestamps[tgroup];
  for (int i=0; i<VF48_MAX_GROUPS; i++)
    if (gGroupEnabled[unit] & (1<<i))
       {
          if (!(groupMask & (1<<i)))
             {
                printf("*** Unit %d, trigger %d: No data for group %d\n", unit, trigger, i), gVF48badDataCount++;
                error = 1;
             }
          else if (!(timestampGroupMask & (1<<i)))
             {
                printf("*** Unit %d, trigger %d: No timestamp for group %d\n", unit, trigger, i), gVF48badDataCount++;
                error = 1;
             }
          else if (fabs(timestamps[i] - t) > 0.010)
             {
                printf("*** Unit %d, trigger %d: Internal timestamp mismatch: group %d timestamp %.6fs should be %.6fs\n", unit, trigger, i, timestamps[i], t), gVF48badDataCount++;
                printf("groups 0x%x\n", groupMask);
                Print();
                error = 1;
             }
       }
          
  complete = true;
  
  //printf("Complete: "); Print();
}

static int gEventNo = 0;

VF48event::VF48event() // ctor
{
  next = NULL;
  eventNo = ++gEventNo;
  timestamp = 0;
  error = 0;
  for (int i=0; i<VF48_MAX_MODULES; i++)
    modules[i] = NULL;
}

VF48event::~VF48event() // dtor
{
  next = NULL;
  eventNo = 0;
  timestamp = 0;
  for (int i=0; i<VF48_MAX_MODULES; i++)
    if (modules[i])
      {
        delete modules[i];
        modules[i] = NULL;
      }
}

double xtime = 0;

static double GetTimeSec()
{
  struct timeval tv;
  gettimeofday(&tv,NULL);
  return tv.tv_sec + 0.000001*tv.tv_usec;
}

void PrintEvent(const VF48event* e)
{
  printf("VF48 event: %d\n", e->eventNo);
  printf("Timestamps: \n");
  for (int i=0; i<VF48_MAX_MODULES; i++)
    {
      printf("Unit %d: ", i);
      for (int j=0; j<VF48_MAX_GROUPS; j++)
        printf(" %.3fms", e->modules[i]->timestamps[j]*1000.0);
      printf(" freq %fMHz\n", gVF48freq[i]/1e6);
    }
}

void VF48event::PrintSummary() const
{
  int max = 0;

  for (int i=0; i<VF48_MAX_MODULES; i++)
    if (modules[i])
       max = i+1;

  printf("VF48 event: %6d, ", eventNo);
  printf("Triggers: ");
  for (int i=0; i<max; i++)
    if (modules[i])
      printf(" %4d", modules[i]->trigger);
    else
      printf(" null");
  printf(", Timestamps: ");
  for (int i=0; i<max; i++)
    if (modules[i])
       printf(" %.3f", modules[i]->timestamps[tgroup]*1000.0);
    else
      printf("    (null)");
  printf(" error %d", error);
  printf("\n");
}

static VF48event* events = NULL;

void PrintAllEvents()
{
  printf("--- all buffered events ---\n");
  for (VF48event* e = events; e != NULL; e = e->next)
    e->PrintSummary();
  printf("--- end ---\n");
}

static double EPS = 0.000100; // in seconds
static bool   gTimestampResyncEnable = true;

void VF48event::SetCoincTime(double time_sec)
{
   EPS = time_sec;
}

void VF48event::SetTimestampResync(bool enable)
{
   gTimestampResyncEnable = enable;
}

void VF48event::SetNumModules(int num_modules)
{
   assert(num_modules > 0);
   assert(num_modules < VF48_MAX_MODULES);
   gNumModules = num_modules;
}

void VF48event::SetGrpEnable(int module, int grp_enable)
{
   assert(module >= 0);
   assert(module < gNumModules);
   gGroupEnabled[module] = grp_enable;
}

void VF48event::SetNumSamples(int module, int num_samples)
{
   assert(module >= 0);
   assert(module < gNumModules);
   gNumSamples[module] = num_samples;
}

void VF48event::SetChanEnable(int module, int grp, int chan_enable)
{
   assert(module >= 0);
   assert(module < gNumModules);
   assert(grp >= 0);
   assert(grp < VF48_MAX_GROUPS);
   for (int i=0; i<VF48_MAX_CHANNELS/VF48_MAX_GROUPS; i++)
      if (chan_enable & (1<<i))
         gChanEnabled[module][grp*VF48_MAX_CHANNELS/VF48_MAX_GROUPS + i] = true;
      else
         gChanEnabled[module][grp*VF48_MAX_CHANNELS/VF48_MAX_GROUPS + i] = false;
}

void VF48event::SetTsFreq(int module, double ts_freq_hz)
{
   assert(module >= 0);
   assert(module < gNumModules);
   gVF48freq[module] = ts_freq_hz;
}

double VF48event::GetTsFreq(int module)
{
   assert(module >= 0);
   assert(module < gNumModules);
   return gVF48freq[module];
}

int VF48event::SetTrigWarning(int enable)
{
   int value = gTrigWarning;
   gTrigWarning = enable;
   return value;
}

static VF48event* FindEvent(int module, double timestamp)
{
  for (VF48event* e = events; e != NULL; e = e->next)
    if (fabs(e->timestamp - timestamp) < EPS)
      if (e->modules[module] == NULL)
        return e;

  VF48event* e = new VF48event();
  e->timestamp = timestamp;
  e->next = events;
  events = e;
  return e;
}

static uint64_t timestampBases[VF48_MAX_MODULES];
static uint64_t ts[VF48_MAX_MODULES][VF48_MAX_GROUPS];

static void FlushBuffers();

void VF48event::BeginRun(int grpEnabled, int numSamples, int delay)
{
  FlushBuffers();
  EndRun();

  gVF48badDataCount = 0;

  gEventNo = 0;
  xtime = 0;
  //xtime = GetTimeSec();
  for (int unit=0; unit<VF48_MAX_MODULES; unit++)
    {
      timestampBases[unit] = 0;
      
      for (int grp=0; grp<VF48_MAX_GROUPS; grp++)
        ts[unit][grp] = 0;
    }

  tgroup = -1;
  for (int grp=0; grp<VF48_MAX_GROUPS; grp++)
    if (grpEnabled & (1<<grp))
      {
        tgroup = grp;
        break;
      }

  if (tgroup < 0)
    {
      printf("VF48event::BeginRun: no groups enabled!\n");
      exit(123);
    }

  gDelay = delay;

  printf("UnpackVF48: groups enabled: 0x%x, num samples: %d\n", grpEnabled, numSamples);

  // which modules and channels are enabled?
  for (int unit=0; unit<VF48_MAX_MODULES; unit++)
    {
      gGroupEnabled[unit] = grpEnabled;
      gNumSamples[unit] = numSamples;

      int count = 0;
      for (int i=0; i<VF48_MAX_GROUPS; i++)
        for (int j=0; j<8; j++)
          {
            if (gGroupEnabled[unit] & (1<<i))
              {
                gChanEnabled[unit][i*8+j] = true;
                count ++;
              }
            else
              {
                gChanEnabled[unit][i*8+j] = false;
              }
          }
          

      //printf("VF48: module %d, groups enabled: 0x%x, channels enabled: %d, num samples: %d\n", unit, gGroupEnabled[unit], count, gNumSamples);
    }
}

void CompleteEvents(int skip)
{
  //PrintAllEvents();

  VF48event **p = &events;

  while (skip-- > 0)
    {
      VF48event *e = *p;
      if (!e)
        break;
      p = &e->next;
    }

  VF48event *e = *p;
  *p = NULL;

  // NB: the event FIFO has events in the reversed
  // order: newest one first. But we want to serve
  // them to HandleVF48event() in the time order,
  // oldest first. Also do not want to depend
  // on user-provided HandleVF48event() messing
  // with e->next.

  const int kMaxCount = 100;
  VF48event* ptr[kMaxCount];
  int count = 0;

  while (e)
    {
      ptr[count] = e;
      count++;
      assert(count<kMaxCount);
      VF48event* ee = e->next;
      e->next = NULL; // zero out the list pointers - we do not expose them to user code
      e = ee;
    }

  for (int i=count-1; i>=0; i--)
    HandleVF48event(ptr[i]);
}

void AddToEvent(VF48module* m)
{
  VF48event* e = FindEvent(m->unit, m->timestamps[tgroup]); // find event corresponding to this time, if not found create new event
  //printf("For unit %d, trigger %d, ts %f, Find event: ", m->unit, m->trigger, m->timestamps[tgroup]); e->PrintSummary();

  if (e->modules[m->unit] != NULL)
    {
      printf("*** Unit %d has duplicate data for trigger %d timestamp %.6f ms: FindEvent yelds trigger %d timestamp %.6f ms, diff %.6f ms\n", m->unit, m->trigger, m->timestamps[tgroup]*1000.0, e->modules[m->unit]->trigger, e->timestamp*1000.0, (m->timestamps[tgroup] - e->timestamp)*1000.0), gVF48badDataCount++;

      PrintAllEvents();

#if 1
      m->Print();
      printf("to event:\n");
      e->PrintSummary();
#endif
      // we are not adding this module to this event,
      // but somebody has to delete the module data.
      // We do it here.
      delete m;
      return;
    }

  e->modules[m->unit] = m;
  
  if (m->error > e->error)
    e->error = m->error;

  // PrintAllEvents();

  CompleteEvents(gDelay);
}

static VF48module* buf[VF48_MAX_MODULES];

static void FlushBuffers()
{
  //PrintAllEvents();

  for (int i=0; i<VF48_MAX_MODULES; i++)
    if (buf[i])
      {
        delete buf[i];
        buf[i] = NULL;
      }
  
  //PrintAllEvents();

  VF48event *e = events;

  events = NULL;

  while (e)
    {
      VF48event* next = e->next;
      delete e;
      e = next;
    }
}

void VF48event::EndRun()
{
  //PrintAllEvents();

  for (int i=0; i<VF48_MAX_MODULES; i++)
    if (buf[i])
      {
        buf[i]->Complete();
        AddToEvent(buf[i]);
        buf[i] = NULL;
      }
  
  //PrintAllEvents();

  CompleteEvents(-1);
}

void PrintVF48word(int unit, int i, uint32_t w)
{
  switch (w & 0xF0000000)
    {
    default: // unexpected data
      {
        printf("unit %d, word %5d: 0x%08x: Unknown data\n", unit, i, w);
        break;
      }
    case 0x80000000: // event header
      {
        uint32_t trigNo = w & 0x00FFFFFF;
        printf("unit %d, word %5d: 0x%08x: Event header:  unit %d, trigger %d\n", unit, i, w, unit, trigNo);
        break;
      }
    case 0xA0000000: // time stamp
      {
        uint32_t ts = w&0x00FFFFFF;
        printf("unit %d, word %5d: 0x%08x: Timestamp %d\n", unit, i, w, ts);
        break;
      }
    case 0xC0000000: // channel header
      {
        int group = (w&0x70)>>4;
        int chan = (w&0x7) | (group<<3);
        printf("unit %d, word %5d: 0x%08x: Group %d, Channel %2d\n", unit, i, w, group, chan);
        break;
      }
    case 0x00000000: // adc samples
      {
        if (w==0) // 64-bit padding
          {
            printf("unit %d, word %5d: 0x%08x: 64-bit padding\n", unit, i, w);
            break;
          }

        int sample1 = w & 0x3FFF;
        int sample2 = (w>>16) & 0x3FFF;
        printf("unit %d, word %5d: 0x%08x: samples 0x%04x 0x%04x (%d %d)\n", unit, i, w, sample1, sample2, sample1, sample2);
        break;
      }
    case 0x40000000: // time analysis
      {
        int t = w & 0x00FFFFFF;
        printf("unit %d, word %5d: 0x%08x: Time %d\n", unit, i, w, t);
        break;
      }
    case 0x50000000: // charge analysis
      {
        int c = w & 0x00FFFFFF;
        printf("unit %d, word %5d: 0x%08x: Charge %d\n", unit, i, w, c);
        break;
      }
    case 0xE0000000: // event trailer
      {
        uint32_t trigNo2 = w & 0x00FFFFFF;
        printf("unit %d, word %5d: 0x%08x: Event trailer: unit %d, trigger %d\n", unit, i, w, unit, trigNo2);
        break;
      }
    }
}

void UnpackVF48(int unit, int size, const void* data, bool disasm, bool disasmSamples)
{
  int       size32 = size;
  const uint32_t *data32 = (const uint32_t*)data;

  static bool gNewEvb[VF48_MAX_MODULES];
  static int  countSkipped[VF48_MAX_MODULES];
  static bool swap = false;

  static Context* c = NULL;

  //printf("UnpackVF48: unit %d, size %d\n", unit, size);

  assert(unit>=0);
  assert(unit<VF48_MAX_MODULES);

  bool doexit = false;

  if (!buf[unit])
    buf[unit] = new VF48module(unit);

  VF48module* m = buf[unit];
  assert(m != NULL);

  if (!c) {
     c = &gContexts[unit][0];
     c->module = unit;
     c->group = 0;
  }

  gContexts[unit][c->group].module = unit;

  if (c->module != unit)
     c = &gContexts[unit][c->group];


  //printf("data32[0] 0x%04x\n", data32[0]);

  if (data32[0] == 0) // is all data zero?!?
    {
      uint32_t www = 0;
      for (int i=0; www==0 && i<size32; i++)
	www |= data32[i];

      //printf("www 0x%04x\n", www);

      if (www == 0)
	{
          gVF48badDataCount++;
          printf("*** Unit %d, trigger %d: All data are zero, %d bytes\n", unit, c->headerTrigNo, 4*size32);
	  return;
	}
    }

  for (int i=0; i<size32; i++)
    {
      uint32_t w = data32[i];

#if 0
      if (((w&0xF0FFFFFF)==0xF0FFFFFF) && (w != 0xFFFFFFFF))
         swap = true;

      if (swap && (w != 0xdeadbeef)) {
         uint32_t w0 = w&0x000000FF;
         uint32_t w1 = w&0x0000FF00;
         uint32_t w2 = w&0x00FF0000;
         uint32_t w3 = w&0xFF000000;
         w = (w0<<(16+8)) | (w1<<8) | (w2>>8) | (w3>>(16+8));
         //printf("swap 0x%08x -> 0x%08x\n", data32[i], w);
      }
#endif

      //if (w==0 || w==0xffffffff) {
      //   gVF48badDataCount++;
      //   printf("*** Unit %d, trigger %d: Skipping invalid data at %5d: 0x%08x\n", unit, c->headerTrigNo, i, w);
      //   continue;
      //}

      // wait for an event header
      if ((w & 0xF0000000) == 0x80000000) // event header
        {
          if (countSkipped[unit])
            printf("*** Unit %d, trigger %d: Skipped %d words while waiting for event header\n", unit, c->headerTrigNo, countSkipped[unit]);

          c->accept = true;
          countSkipped[unit] = 0;
        }
      else if ((w & 0xF0000000) == 0xF0000000) // group marker
        {
	  if (!c->accept) {
	    int group = (w & 0x00000007);

	    if (disasm)
	      printf("unit %d, word %5d: 0x%08x: switch to data from group %d\n", unit, i, w, group);
	    
	    if (group >= 0 && group < VF48_MAX_GROUPS)
	      {
		c = &gContexts[unit][group];
		c->module = unit;
		c->group = group;
		c->currentGroup = group;

                gNewEvb[unit] = true;
	      }
	    continue;
	  }
        }
      else if (!c->accept)
	{
	  if (disasm)
	    printf("unit %d, word %5d: 0x%08x - skipped waiting for event header\n", unit, i, w);

          if (w == 0xdeadbeef) // DMA marker, do not count as skipped
             continue;
	  
	  countSkipped[unit]++;
	  continue;
	}

      switch (w & 0xF0000000)
        {
        default: // unexpected data
           {
              if (w == 0xdeaddead) // skip the buffer padding
                 {
                    if (disasm)
                       printf("unit %d, word %5d: 0x%08x: reading from empty EB FIFO\n", unit, i, w);
                    break;
                 }
              
              if (w == 0xdeadbeef) // dma marker
                 {
                    if (disasm)
                       printf("unit %d, word %5d: 0x%08x: DMA marker\n", unit, i, w);
                    break;
                 }
              
              if (c && m)
                 if (!m->channels[c->currentChannel].complete)
                    if (disasm)
                       printf("unit %d, adc samples: %d\n", unit, m->channels[c->currentChannel].numSamples);
              
              if (disasm)
                 printf("unit %d, word %5d: 0x%08x: unexpected data\n", unit, i, w);

              if (m)
                 m->error = 1;
              
              gVF48badDataCount++;
              printf("*** Unit %d, trigger %d: Unexpected data at %5d: 0x%08x, skipping to next event header\n", unit, c->headerTrigNo, i, w);
              
              c->accept = false;
              break;
           }
        case 0xF0000000: // switch to data from different group
           {
              int group = (w & 0x00000007);

              if (disasm)
                 printf("unit %d, word %5d: 0x%08x: switch to data from group %d\n", unit, i, w, group);

              if (group < 0 || group >= VF48_MAX_GROUPS)
                 {
                    if (m)
                       m->error = 1;
              
                    gVF48badDataCount++;
                    printf("*** Unit %d, trigger %d: Bad group marker at %5d: 0x%08x, skipping to next event header\n", unit, c->headerTrigNo, i, w);
              
                    c->accept = false;
                    break;
                 }

              c = &gContexts[unit][group];
              c->module = unit;
              c->group = group;
              c->currentGroup = group;

              gNewEvb[unit] = true;
              
              break;
           }
        case 0x80000000: // event header
          {
             uint32_t trigNo = w & 0x00FFFFFF;

             if (disasm)
                printf("unit %d, word %5d: 0x%08x: Event header:  unit %d, trigger %d\n", unit, i, w, unit, trigNo);

             if (0 && trigNo > 1) {
                if (trigNo != c->headerTrigNo+1) {
                   printf("*** Unit %d, trigger %d: Out of sequence trigger %d should be %d\n", unit, trigNo, trigNo, c->headerTrigNo+1);
                   gVF48badDataCount++;
                }
             }

             if (m->trigger != 0) {
                if ((!gNewEvb[unit]) || (m->completeGroupMask & (1<<c->group))) {
                   printf("*** Unit %d, trigger %d: Event header for trigger %d without an event trailer for previous trigger %d\n", unit, trigNo, trigNo, c->headerTrigNo);
                   gVF48badDataCount++;
                   
                   m->Complete();
                   AddToEvent(m);
                   m = NULL;
                   buf[unit] = NULL;
                   
                   buf[unit] = new VF48module(unit);
                   m = buf[unit];
                }
             }
	    
             c->Init();
	     c->accept = true;
             c->headerTrigNo = trigNo;

             m->trigger = trigNo;
             break;
          }
        case 0xA0000000: // time stamp
           {
              uint32_t ts = w&0x00FFFFFF;
              
              if (disasm)
                 printf("unit %d, word %5d: 0x%08x: Timestamp %d, count %d\n", unit, i, w, ts, c->timestampCount);
              
              if (c->timestampCount == 0)
                 {
                    c->timestamp1 = ts;
                 }
              else if (c->timestampCount == 1)
                 {
                    c->timestamp2 = ts;
                 }
              else
                 {
                    printf("*** Unit %d, trigger %d: Unexpected timestamp count: %d\n", unit, c->headerTrigNo, c->timestampCount);
                    gVF48badDataCount++;
                 }
              
              c->timestampFlag = 1;
              c->timestampCount++;
           }
           break;
        case 0xC0000000: // channel header
          {
            int group = (w&0x70)>>4;

            if (group == 0)
               group = c->group;

            int chan  = (w&0x7) | (group<<3);

            if (disasm)
               printf("unit %d, word %5d: 0x%08x: Group %d, Channel %2d\n", unit, i, w, group, chan);

            if (chan >= VF48_MAX_CHANNELS)
               {
                  printf("*** Unit %d, trigger %d: data at %5d: 0x%08x, Bad channel number %d\n", unit, c->headerTrigNo, i, w, chan);
                  gVF48badDataCount++;
                  break;
               }

            if (c->timestampCount)
               {
                  uint64_t ts48 = c->timestamp1 | (((uint64_t)c->timestamp2) << 24);
                  
                  if (gTimestampResyncEnable) {
                     if (ts48 == 0 && c->headerTrigNo == 1)
                        {
                           c->timestamp1 = 1;
                           ts48 = 1;
                        }
                     
                     if (!timestampBases[unit])
                        {
                           timestampBases[unit] = ts48;
                           xtime = GetTimeSec();
                        }
                  }
                  
                  if (ts48 == ts[unit][group])
                     {
                        printf("*** Unit %d, trigger %d: Group %d has invalid timestamp %d, should be more than %d\n", unit, c->headerTrigNo, group, (int)ts48, (int)ts[unit][group]);
                        gVF48badDataCount++;
                        ts48 = ts[unit][group] + (int)(EPS*gVF48freq[unit])+1;
                     }
                  
                  ts[unit][group] = ts48;

                  m->timestamp64[group] = ts48;
                  m->timestamps[group] = (ts48 - timestampBases[unit])/gVF48freq[unit];
               }
            
            if (m->channels[chan].numSamples != 0)
               {
                  printf("*** Unit %d, trigger %d: Duplicate data for channel %d: already have %d samples\n", unit, c->headerTrigNo, chan, m->channels[chan].numSamples);
                  gVF48badDataCount++;
               }

            c->currentGroup = group;

            c->timestampCount = 0;
            c->currentChannel = chan;
            m->groupMask |= (1<<group);
            if (c->timestampFlag)
               m->timestampGroupMask |= (1<<group);
            c->timestampFlag = 0;
            m->channels[c->currentChannel].complete   = false;
            m->channels[c->currentChannel].numSamples = 0;
            m->channels[c->currentChannel].channel    = c->currentChannel;
          }
          break;
        case 0x00000000: // adc samples
           {
              if (false && (w == 0))
                 {
                    if (disasm)
                       printf("unit %d, word %5d: 0x%08x: invalid data\n", unit, i, w);
                    
                    gVF48badDataCount++;
                    printf("*** Unit %d, trigger %d: Unexpected data at %5d: 0x%08x\n", unit, c->headerTrigNo, i, w);
                    
                    //accept[unit] = false;
                    break;
                 }
              
              VF48channel *cc = &m->channels[c->currentChannel];
              
              int sample1 = w & 0x3FFF;
              int sample2 = (w>>16) & 0x3FFF;
              
              if (disasmSamples)
                 printf("unit %d, word %5d: 0x%08x: samples %4d: 0x%04x 0x%04x (%d %d)\n", unit, i, w, cc->numSamples, sample1, sample2, sample1, sample2);
              
              if (m->channels[c->currentChannel].complete)
                 {
                    if (1)
                       {
                          gVF48badDataCount++;
                          m->error = 1;
                          
                          static int xi = 0;
                          static int xc = 0;
                          
                          if (xi+1 != i)
                             {
                                if (xc > 0)
                                   printf("*** Previously ignored %d out of sequence adc samples\n", xc);
                                
                                printf("*** Unit %d, trigger %d: Out of sequence adc samples for channel %d at %d, word 0x%08x\n", unit, c->headerTrigNo, cc->channel, i, w);
                                xc = 0;
                             }
                          
                          xi = i;
                          xc ++;
                       }
                 }
              else if (cc->numSamples < VF48_MAX_SAMPLES-2)
                 {
                    cc->samples[cc->numSamples++] = sample1;
                    cc->samples[cc->numSamples++] = sample2;
                 }
              else
                 {
                    //printf("*** Unit %d channel %d has too many samples: %d\n", unit, c->channel, c->numSamples);
                    gVF48badDataCount++;
                    cc->numSamples += 2;
                    m->error = 1;
                 }
           }
           break;
        case 0x40000000: // time analysis
           {
              if (!m->channels[c->currentChannel].complete)
                 if (disasm)
                    printf("unit %d, adc samples: %d\n", unit, m->channels[c->currentChannel].numSamples);
              int t = w & 0x00FFFFFF;
              if (disasm)
                 printf("unit %d, word %5d: 0x%08x: Time %d\n", unit, i, w, t);
              m->channels[c->currentChannel].time = t;
              m->channels[c->currentChannel].complete = true;
           }
          break;
        case 0x50000000: // charge analysis
           {
              if (!m->channels[c->currentChannel].complete)
                 if (disasm)
                    printf("unit %d, adc samples: %d\n", unit, m->channels[c->currentChannel].numSamples);
              int charge = w & 0x00FFFFFF;
              if (disasm)
                 printf("unit %d, word %5d: 0x%08x: Charge %d\n", unit, i, w, charge);
              
              if ((w & 0x0F000000) == 0x00000000)
                 m->suppressMask[c->currentChannel>>3] = w & 0xFF;
              
              m->channels[c->currentChannel].charge = charge;
              m->channels[c->currentChannel].complete = true;
           }
           break;
        case 0xE0000000: // event trailer
           {
              if (!m->channels[c->currentChannel].complete)
                 if (disasm)
                    printf("unit %d, adc samples: %d\n", unit, m->channels[c->currentChannel].numSamples);
              
              uint32_t trigNo = w & 0x00FFFFFF;

              c->trailerTrigNo = trigNo;

              m->completeGroupMask |= (1<<c->currentGroup);
              
              if (disasm)
                 printf("unit %d, word %5d: 0x%08x: Event trailer: unit %d, trigger %d, module complete mask 0x%x\n", unit, i, w, unit, trigNo, m->completeGroupMask);
              
              m->channels[c->currentChannel].complete = true;
              
              if (trigNo != c->headerTrigNo)
                 {
                    printf("*** Unit %d, trigger %d: event trailer trigger mismatch: see %d, should be %d\n", unit, c->headerTrigNo, trigNo, m->trigger);
                    gVF48badDataCount++;
                    m->error = 1;
                 }

              if ((!gNewEvb[unit]) || (m->completeGroupMask == gGroupEnabled[c->module])) {
                 m->Complete();
                 AddToEvent(m);
                 m = NULL;
                 buf[unit] = NULL;
                 
                 buf[unit] = new VF48module(unit);
                 m = buf[unit];
              }
           }
           break;
        }
    }
  
  if (doexit)
     exit(123);
}

// end

