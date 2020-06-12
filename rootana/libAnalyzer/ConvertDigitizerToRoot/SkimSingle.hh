#ifndef SkimSingle_H
#define SkimSingle_H

#include "TRootanaEventLoop.hxx"
#include "TTree.h"
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TF1.h"
#include "TDataContainer.hxx"
#include "TV1730RawData.hxx"
//#include "ptfEvent.hh"
#include "PTFDict.h"
using namespace PTF;

class SkimSingle: public TRootanaEventLoop {

	public:
  	SkimSingle();
    virtual ~SkimSingle();
  	void Initialize();
  	void BeginRun(int transition,int run,int time);
    void EndRun(int transition,int run,int time);
  	bool ProcessMidasEvent(TDataContainer& dataContainer);
    bool CheckOption(std::string option);

    TH1F *ChargeHist;
    TH1F *BaselineHist;
    TH1F *TimeHist;
    TH1F *PeakHist;
    TH2F *Charge_Time;
    TH2F *Peak_Time;
    TH1F *SampleWave;
    TH1F *SampleWaveDeriv;
    TH1F *SamplePulseFind;
    TH1F *PulseNum;
    TH2F *DeltaT_Peak;
    TH2F *DeltaT_PRatio;
    TH2F *Peak1_Peak2;
    TH1F *PeakFixWindow; 
    TH1F *PeakFixWindow1;
    int counter;
    bool savesamples;
    Int_t DerivThreshold;
    Int_t SmallDeriv;
    
    protected:
    PTF::ptfEvent *rootevent;
    TTree *tree;
}; 

#endif
