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

	protected:
	Double_t time;
	Double_t charge;
	Double_t height;
	Double_t baseline;
	Int_t width;
	Int_t startbin;
    
    ClassDef(Pulse,0)
};
}
#endif