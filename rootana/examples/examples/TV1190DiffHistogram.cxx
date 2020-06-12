#include "TV1190DiffHistogram.h"

#include "TV1190Data.hxx"
#include "TDirectory.h"

const int Nchannels = 64;

/// Reset the histograms for this canvas
TV1190DiffHistograms::TV1190DiffHistograms(){  
  
  CreateHistograms();
}


void TV1190DiffHistograms::CreateHistograms(){
  

  // Otherwise make histograms
  clear();
  
  std::cout << "Create Histos" << std::endl;
  for(int i = 0; i < Nchannels; i++){ // loop over channels    

    char name[100];
    char title[100];
    sprintf(name,"V1190_Diff_%i_%i",0,i);

    // Delete old histograms, if we already have them
    TH1D *old = (TH1D*)gDirectory->Get(name);
    if (old){
      delete old;
    }


    // Create new histograms
    
    sprintf(title,"V1190 Difference histogram for channel=%i",i);	
    
    TH1D *tmp = new TH1D(name,title,1000,-200,200);
    tmp->SetXTitle("Channel Time - Channel 0 time (ns)");
    tmp->SetYTitle("Number of Entries");
    push_back(tmp);
  }

}



  
/// Update the histograms for this canvas.
void TV1190DiffHistograms::UpdateHistograms(TDataContainer& dataContainer){


  TV1190Data *data = dataContainer.GetEventData<TV1190Data>("TDC0");
  if(!data) return;

  /// Get the Vector of ADC Measurements.
  std::vector<TDCMeasurement> measurements = data->GetMeasurements();
  
  // Find the reference time.
  static int ref_chan = 0;
  double ref_time = 0;


  for(unsigned int i = 0; i < measurements.size(); i++){ // loop over measurements
	
    int chan = measurements[i].GetChannel();
    if(ref_chan == chan)
      ref_time = measurements[i].GetMeasurement();
    
  }

  for(unsigned int i = 0; i < measurements.size(); i++){ // loop over measurements
	
    int chan = measurements[i].GetChannel();

    // Calculate time difference; convert to ns.
    double tdiff = (measurements[i].GetMeasurement() - ref_time)*0.2;
    
    GetHistogram(chan)->Fill(tdiff);
  }


}



/// Take actions at begin run
void TV1190DiffHistograms::BeginRun(int transition,int run,int time){

  CreateHistograms();

}

/// Take actions at end run  
void TV1190DiffHistograms::EndRun(int transition,int run,int time){

}
