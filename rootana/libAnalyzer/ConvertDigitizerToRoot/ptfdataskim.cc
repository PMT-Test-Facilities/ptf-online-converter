//#include "ptfEvent.hh"
//#include "pulse.hh"
#include "SkimSingle.hh"
#include "TMidasEvent.h"
#include "TMidasFile.h"
#include "TFile.h"

int main(int argc, char *argv[]){
	//buildgui;
    
	//startapp
    SkimSingle::CreateSingleton<SkimSingle>();
    return SkimSingle::Get().ExecuteLoop(argc, argv);
}
