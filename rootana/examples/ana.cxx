// Default program for dealing with various standard TRIUMF VME setups:
// V792
//
//

#include <stdio.h>
#include <iostream>
#include <time.h>

#include "TRootanaEventLoop.hxx"

#define USE_V792
#ifdef  USE_V792
#include "TV792Histogram.h"
#endif 

class Analyzer: public TRootanaEventLoop {

#ifdef  USE_V792
  TV792Histograms *v792_histos;
#endif 

public:

  Analyzer() {

    std::cout << "Construct " << std::endl;

#ifdef  USE_V792
    v792_histos = 0;
#endif 
  };

  virtual ~Analyzer() {};

  void Initialize(){


  }


  void BeginRun(int transition,int run,int time){

#ifdef  USE_V792
    if(v792_histos) delete v792_histos;
    v792_histos = new TV792Histograms();
#endif 

  }


  bool ProcessMidasEvent(TDataContainer& dataContainer){

#ifdef  USE_V792
    v792_histos->UpdateHistograms(dataContainer);
#endif 
    return true;
  }


}; 


int main(int argc, char *argv[])
{

  Analyzer::CreateSingleton<Analyzer>();
  return Analyzer::Get().ExecuteLoop(argc, argv);

}

