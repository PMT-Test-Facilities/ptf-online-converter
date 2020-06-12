#include <stdio.h>
#include <iostream>

#include "TRootanaDisplay.hxx"
#include "TH1D.h"
#include "TV792Data.hxx"

#include "TFancyHistogramCanvas.hxx"

#define USE_V792
#ifdef  USE_V792
#include "TV792Histogram.h"
#endif 

class MyTestLoop: public TRootanaDisplay {
  
#ifdef  USE_V792
  TV792Histograms *v792_histos;
#endif 


  TH1F *current_value;
public:

  MyTestLoop() {

    DisableRootOutput(false);
    
    // Initialize histograms.
    current_value = new TH1F("current_value","Size of FR10 bank",50,100,150);

#ifdef  USE_V792
    v792_histos = 0;
    v792_histos = new TV792Histograms();
#endif 
  }
  

  void AddAllCanvases(){
    // Set up tabbed canvases
    SetNumberSkipEvent(100);


    TFancyHistogramCanvas* v792_all = new TFancyHistogramCanvas(v792_histos,"V792");
    AddSingleCanvas(v792_all);

    SetDisplayName("Example Display");
  };

  virtual ~MyTestLoop() {};


  void BeginRun(int transition,int run,int time){

#ifdef  USE_V792
    v792_histos->BeginRun(transition,run,time);
#endif 

  }

  void ResetHistograms(){
    current_value->Reset();
  }

  void UpdateHistograms(TDataContainer& dataContainer){

    current_value->Reset();
    TV792Data *data = dataContainer.GetEventData<TV792Data>("ADC0");
    if(!data) return;
    
    /// Get the Vector of ADC Measurements.
    std::vector<VADCMeasurement> measurements = data->GetMeasurements();
    for(unsigned int i = 0; i < measurements.size(); i++){ // loop over measurements
    
      int chan = measurements[i].GetChannel();
      uint32_t adc = measurements[i].GetMeasurement();
      
      if(chan ==0)
	current_value->Fill(adc);
    }
  
  }


  void PlotCanvas(TDataContainer& dataContainer){

    if(GetDisplayWindow()->GetCurrentTabName().compare("Current") == 0){       
      TCanvas* c1 = GetDisplayWindow()->GetCanvas("Current");
      c1->Clear();
      current_value->Draw();
      c1->Modified();
      c1->Update();
    }
    
  }


}; 






int main(int argc, char *argv[])
{
  MyTestLoop::CreateSingleton<MyTestLoop>();  
  return MyTestLoop::Get().ExecuteLoop(argc, argv);
}

