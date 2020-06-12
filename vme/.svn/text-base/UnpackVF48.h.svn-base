// UnpackVF48.h

#ifndef UnpackVF48_H
#define UnpackVF48_H

#define VF48_MAX_MODULES   22
#define VF48_MAX_GROUPS     6
#define VF48_MAX_CHANNELS  48
#define VF48_MAX_SAMPLES 1100

extern int gVF48badDataCount;

struct VF48channel
{
  int channel;
  uint32_t time;
  uint32_t charge;
  bool     complete;
  int numSamples;
  uint16_t samples[VF48_MAX_SAMPLES];
};

struct VF48module
{
  int      unit;
  uint32_t trigger;
  int      error;
  uint32_t timestampGroupMask;
  uint64_t timestamp64[VF48_MAX_GROUPS];
  double   timestamps[VF48_MAX_GROUPS];
  uint32_t groupMask;
  uint32_t completeGroupMask;
  bool     complete;
  int      numSamples;
  int      numChannels;
  int      suppressMask[VF48_MAX_GROUPS];
  VF48channel channels[VF48_MAX_CHANNELS];

  VF48module(int i); // ctor  

  void Print() const;

  void Complete();
};

struct VF48event
{
  VF48event*  next;
  int         eventNo;
  int         error;
  double      timestamp;
  VF48module* modules[VF48_MAX_MODULES];

  VF48event(); // ctor

  ~VF48event(); // dtor

  void PrintSummary() const;

  static void BeginRun(int grpEnabled, int numSamples, int delay);
  static void EndRun();
  static void SetCoincTime(double time_sec);
  static void SetTimestampResync(bool enable);
  static void SetNumModules(int num_modules);
  static void SetTsFreq(int module, double ts_freq_hz);
  static void SetNumSamples(int module, int num_samples);
  static void SetGrpEnable(int module, int grp_enable);
  static void SetChanEnable(int module, int grp, int chan_enable);
  static int  SetTrigWarning(int enable);

  static double GetTsFreq(int module);
};

//
// Dear user: you call UnpackVF48() with the VF48 data, it becomes
// unpacked and sorted into the "VF48event" data structure,
// events from separate VF48 modules are put together
// based on the VF48 data timestamps and after a complete
// event is assembled, the code calls your function HandleVF48event().
// Somewhere in this function, you should delete the event using "delete e;"
//

void PrintVF48word(int unit, int i, uint32_t w);
void UnpackVF48(int unit, int size, const void* data, bool disasm=false, bool disasmSamples=false);

extern void HandleVF48event(const VF48event *e);

#endif
// end
