#ifndef Pulse_H
#define Pulse_H
#include "TObject.h"
namespace PTF {
class Pulse: public TObject
{
	public:
    Pulse():TObject(){
        time = 0.;
        charge = 0.;
        height = 0.;
        baseline = 0.;
        width = 0.;
        startbin = 0.;
    }
    Pulse(Double_t _time,Double_t _charge,Double_t _baseline,Double_t _height,Int_t _width,Int_t _startbin,Int_t _maxd,Int_t _maxdt,Int_t _mind,Int_t _mindt):TObject(){
    	time = _time;
	charge = _charge - _baseline*_width;
        baseline = _baseline;
        height = _height-_baseline;
        width = _width;
        startbin = _startbin;
        maxd = _maxd;
        maxdt = _maxdt;
        mind = _mind;
        mindt = _mindt;
    }
    ~Pulse(){}
	void SetTime(Double_t _time){time=_time;}
	Double_t GetTime(){return time;}
	void SetCharge(Double_t _charge){charge = _charge;}
	Double_t GetCharge(){return charge;}
	void SetBaseline(Double_t _baseline){baseline=_baseline;}
	Double_t GetBaseline(){return baseline;}
	void SetHeight(Double_t _height){height=_height;}
	Double_t GetHeight(){return height;}
	void SetWidth(Int_t _width){width=_width;}
	Int_t GetWidth(){return width;}
	void SetStartBin(Int_t _bin){startbin = _bin;}
	Int_t GetStartBin(){return startbin;}
	void SetMaxD(Int_t _maxd){maxd = _maxd;}
	Int_t GetMaxD(){return maxd;}
	void SetMaxDTime(Int_t _maxdt){maxdt=_maxdt;}
	Int_t GetMaxDTTime(){return maxdt;}
	void SetMinD(Int_t _mind){mind = _mind;}
	Int_t GetMinD(){return mind;}
	void SetMinDTime(Int_t _mindt){mindt = _mindt;}
	Int_t GetMinDTime(){return mindt;}

	protected:
	Double_t time;
	Double_t charge;
	Double_t height;
	Double_t baseline;
	Int_t width;
	Int_t startbin;
	Int_t maxd;
	Int_t maxdt;
	Int_t mind;
	Int_t mindt;
    
    ClassDef(Pulse,0)
};
}
#endif
