#ifndef PTFEVENT_H
#define PTFEVENT_H
#include "TObject.h"
#include <vector>
#include "TVector3.h"
#include "Pulse.hh"
using namespace PTF;

namespace PTF {

class ptfEvent: public TObject
{
	public:
    ptfEvent():TObject(){
        monitor0baseline = 0.;
        monitor1baseline = 0.;
        targetpmtbaseline = 0.;
        reciever0baseline = 0.;
        reciever1baseline = 0.;
        adcsize = 0.;
    }
	void SetMonitor0Samples(std::vector<UShort_t> _samples){monitor0samples = _samples;}
	std::vector<UShort_t> GetMonitor0Samples(){return monitor0samples;}
	std::vector<Double_t> GetMonitor0PhysicalSamples(){
		std::vector<Double_t> physical;
		for(int i=0; i< monitor0samples.size(); ++i) physical.push_back(adcsize * ((Double_t)monitor0samples[i]-monitor0baseline));
		return physical;
	}
	void SetMonitor1Samples(std::vector<UShort_t> _samples){monitor1samples = _samples;}
	std::vector<UShort_t> GetMonitor1Samples(){return monitor1samples;}
	std::vector<Double_t> GetMonitor1PhysicalSamples(){
                std::vector<Double_t> physical;
                for(int i=0; i< monitor1samples.size(); ++i) physical.push_back(adcsize * ((Double_t)monitor1samples[i]-monitor1baseline));
                return physical;
        }
	void SetTargetPMTSamples(std::vector<UShort_t> _samples){targetpmtsamples = _samples;}
	std::vector<UShort_t> GetTargetPMTSamples(){return targetpmtsamples;}
	std::vector<Double_t> GetTargetPMTPhysicalSamples(){
                std::vector<Double_t> physical;
                for(int i=0; i< targetpmtsamples.size(); ++i) physical.push_back(adcsize * ((Double_t)targetpmtsamples[i]-targetpmtbaseline));
                return physical;
        }
	void SetReciever0Samples(std::vector<UShort_t> _samples){reciever0samples = _samples;}
	std::vector<UShort_t> GetReciever0Samples(){return reciever0samples;}
	std::vector<Double_t> GetReciever0PhysicalSamples(){
                std::vector<Double_t> physical;
                for(int i=0; i< reciever0samples.size(); ++i) physical.push_back(adcsize * ((Double_t)reciever0samples[i]-reciever0baseline));
                return physical;
        }
	void SetReciever1Samples(std::vector<UShort_t> _samples){reciever1samples = _samples;}
	std::vector<UShort_t> GetReciever1Samples(){return reciever1samples;}
	std::vector<Double_t> GetReciever1PhysicalSamples(){
                std::vector<Double_t> physical;
                for(int i=0; i< reciever1samples.size(); ++i) physical.push_back(adcsize * ((Double_t)reciever1samples[i]-reciever1baseline));
                return physical;
        }
	void SetEventTime(Int_t _eventtime){eventtime = _eventtime;}
	Int_t GetEventTime(){return eventtime;}
	void SetMonitor0Baseline(Double_t _monitor0baseline){monitor0baseline=_monitor0baseline;}
	Double_t GetMonitor0Baseline(){return monitor0baseline;}
	void SetMonitor1Baseline(Double_t _monitor1baseline){monitor1baseline=_monitor1baseline;}
        Double_t GetMonitor1Baseline(){return monitor1baseline;}
	void Set(Double_t _targetpmtbaseline){targetpmtbaseline=_targetpmtbaseline;}
        Double_t GetTargetPMTBaseline(){return targetpmtbaseline;}
	void SetReciever0Baseline(Double_t _reciever0baseline){reciever0baseline=_reciever0baseline;}
        Double_t GetReciever0Baseline(){return reciever0baseline;}
	void SetReciever1Baseline(Double_t _reciever1baseline){reciever1baseline=_reciever1baseline;}
        Double_t GetReciever1Baseline(){return reciever1baseline;}
	void SetADCSize(Double_t _adcsize){adcsize=_adcsize;}
	Double_t GetADCSize(){return adcsize;}
    void SetEventID(Int_t _eventID){eventID = _eventID;}
    Int_t GetEventID(){return eventID;}
    
    void AddMonitor0Pulse(Pulse *_pulse){monitor0pulses.push_back(_pulse);}
    Pulse* GetMonitor0Pulse(Int_t n){if(n>0 && n< monitor0pulses.size()) return monitor0pulses[n]; else return NULL;}
    Int_t GetMonitor0PulseCount(){return monitor0pulses.size();}
    
    void AddMonitor1Pulse(Pulse *_pulse){monitor1pulses.push_back(_pulse);}
    Pulse* GetMonitor1Pulse(Int_t n){if(n>0 && n< monitor1pulses.size()) return monitor1pulses[n]; else return NULL;}
    Int_t GetMonitor1PulseCount(){return monitor1pulses.size();}
    
    void AddTargetPulse(Pulse *_pulse){targetpmtpulses.push_back(_pulse);}
    Pulse* GetTargetPulse(Int_t n){if(n>0 && n< targetpmtpulses.size()) return targetpmtpulses[n]; else return NULL;}
    Int_t GetTargetPulseCount(){return targetpmtpulses.size();}
    
    void AddReciever0Pulse(Pulse *_pulse){reciever0pulses.push_back(_pulse);}
    Pulse* GetReciever0Pulse(Int_t n){if(n>0 && n< reciever0pulses.size()) return reciever0pulses[n]; else return NULL;}
    Int_t GetReciever0PulseCount(){return reciever0pulses.size();}
    
    void AddReciever1Pulse(Pulse *_pulse){reciever1pulses.push_back(_pulse);}
    Pulse* GetReciever1Pulse(Int_t n){if(n>0 && n< reciever1pulses.size()) return reciever1pulses[n]; else return NULL;}
    Int_t GetReciever1PulseCount(){return reciever1pulses.size();}
    
	protected:
	std::vector<UShort_t> monitor0samples;
	std::vector<UShort_t> monitor1samples;
	std::vector<UShort_t> targetpmtsamples;
	std::vector<UShort_t> reciever0samples;
	std::vector<UShort_t> reciever1samples;
	Int_t eventtime;
    Int_t eventID;
	TVector3 gantry0position;
	TVector3 gantry0angle;
	TVector3 gantry1position;
	TVector3 gantry1angle;
	Double_t monitor0baseline;
	Double_t monitor1baseline;
	Double_t targetpmtbaseline;
	Double_t reciever0baseline;
	Double_t reciever1baseline;
	Double_t adcsize;
    std::vector<Pulse*> monitor0pulses;
    std::vector<Pulse*> monitor1pulses;
    std::vector<Pulse*> targetpmtpulses;
    std::vector<Pulse*> reciever0pulses;
    std::vector<Pulse*> reciever1pulses;
    
    ClassDef(ptfEvent,0)
    
};
}
#endif
