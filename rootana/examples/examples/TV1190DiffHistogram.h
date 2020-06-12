#ifndef TV1190DiffHistograms_h
#define TV1190DiffHistograms_h

#include <string>
#include "THistogramArrayBase.h"

/// Class for making histograms of V1190 TDC data.
class TV1190DiffHistograms : public THistogramArrayBase {
 public:
  TV1190DiffHistograms();
  virtual ~TV1190DiffHistograms(){};
  
  /// Update the histograms for this canvas.
  void UpdateHistograms(TDataContainer& dataContainer);

  /// Take actions at begin run
  void BeginRun(int transition,int run,int time);

  /// Take actions at end run  
  void EndRun(int transition,int run,int time);

private:

  void CreateHistograms();
    
};

#endif


