#include "SkimSingle.hh"

SkimSingle::SkimSingle():TRootanaEventLoop(){
    DisableAutoMainWindow();
    IsOffline();
    tree = 0;
    rootevent = 0;
}

SkimSingle::~SkimSingle() {}

void SkimSingle::Initialize(){
    savesamples = false;
}

bool SkimSingle::CheckOption(std::string option){

    const char* arg = option.c_str();
    if (strncmp(arg,"-o",2)==0){
        SetOutputFilename(strdup(arg+2));
        return true;
    }
    if (strncmp(arg,"-W",2)==0){
        savesamples = true;
        return true;
    }
    return false;
}

void SkimSingle::BeginRun(int transition,int run,int time){
	tree = new TTree("T","T");
	tree->Branch("ptfevent",&rootevent);
    
    ChargeHist = new TH1F("ChargeHist","ChargeHist",2000,-300,1700);
    ChargeHist->SetDirectory(0);
    BaselineHist = new TH1F("BaselineHist","BaselineHist",10000,0,10000);
    BaselineHist->SetDirectory(0);
    TimeHist = new TH1F("TimeHist","TimeHist",10000,0,20000);
    TimeHist->SetDirectory(0);
    PeakHist = new TH1F("PeakHist","PeakHist",351,-50.5,300.5);
    PeakHist->SetDirectory(0);
    Charge_Time = new TH2F("Charge_Time","Charge_Time",500,0,1000,300,0,300);
    Charge_Time->SetDirectory(0);
    Peak_Time = new TH2F("Peak_Time","Peak_Time",500,0,1000,300,0,300);
    Peak_Time->SetDirectory(0);
 //   if(savesamples){
        SampleWave = new TH1F("","",10000,0,10000);
        SampleWave->SetDirectory(0);
 //   }
    counter=0;
}

void SkimSingle::EndRun(int transition,int run,int time){
    ChargeHist->Scale(1./ChargeHist->Integral());
    ChargeHist->Fit("gaus","","",-50,50);
    ChargeHist->Write();
    BaselineHist->Write();
    TimeHist->Write();
    PeakHist->Fit("gaus","R","",-10,10);
    PeakHist->Fit("gaus","R+","",15,30);
    PeakHist->Write();
    PeakHist->SaveAs("1107_org");
    Charge_Time->Write();
    Peak_Time->Write();
}

bool SkimSingle::ProcessMidasEvent(TDataContainer& dataContainer){
	
	int eventid = dataContainer.GetMidasData().GetEventId();
  	int timestamp = dataContainer.GetMidasData().GetTimeStamp();
	TV1730RawData *v1730 = dataContainer.GetEventData<TV1730RawData>("V730");
	rootevent = new PTF::ptfEvent();
    rootevent->SetEventID(eventid);
    rootevent->SetEventTime(timestamp);
	if(v1730){
        rootevent->SetADCSize(2./16384.);
		std::vector<RawChannelMeasurement> measurements = v1730->GetMeasurements();
		for(int i = 0; i < measurements.size(); i++){	
			int chan = measurements[i].GetChannel();
			// Hack! Not sure why this is needed?
			float offset = 0;
			//if(chan == 1) offset = 35;
			vector<UShort_t> samples;
            Int_t PeakTime=0;
            Double_t Peak=0.;
			for(int ib = 0; ib < measurements[i].GetNSamples(); ib++){
				samples.push_back(measurements[i].GetSample(ib)-offset);
            }
            if(chan == 1){ rootevent->SetMonitor0Samples(samples);
                if(counter < 1000){
                    Double_t Baseline = 0.;
                    for(int j=9520; j<9540; ++j) Baseline += (Double_t) samples[j];
                    Baseline /= 20.;
                    Double_t Charge=0.;
                    for(int j=2480; j<2520; ++j) Charge += (Double_t) samples[j];
                    for(int j=0; j<samples.size(); ++j) SampleWave->Fill(j,(Double_t)samples[j]);
                    SampleWave->Write(Form("Chan_%d_Wave_%d_%f",chan,counter,-(Charge - Baseline*40)));
                    SampleWave->Reset();
                }
                ++counter;
            }
            if(chan == 0){
                rootevent->SetTargetPMTSamples(samples);
                Double_t Baseline = 0.;
                for(int j=9500; j<9540; ++j) Baseline += (Double_t) samples[j];
                Baseline /= 40.;
                Double_t Charge=0.;
		Double_t MaxPeak=100.;
                for(int j=2510; j<2550; ++j){ Charge += (Double_t) samples[j]; if(samples[j]-Baseline < MaxPeak) MaxPeak = samples[j]-Baseline;}
                if(counter < 1000){
                    for(int j=0; j<samples.size(); ++j) SampleWave->Fill(j,(Double_t)samples[j]-Baseline);
                    SampleWave->Write(Form("Chan_%d_Wave_%d_%f",chan,counter,-(Charge - Baseline*40)));
                    SampleWave->Reset();
                }
                ++counter;
                ChargeHist->Fill(-(Charge - Baseline*40));
                Charge=0.;
                //for(int j=100; j<150; ++j) Charge += (Double_t) samples[j];
                //ChargeHist->Fill(-(Charge - Baseline*50));
                BaselineHist->Fill(Baseline);
                TimeHist->Fill(PeakTime);
                PeakHist->Fill(-MaxPeak);
                Charge_Time->Fill(PeakTime,Charge);
                Peak_Time->Fill(PeakTime,Peak);
            }
		}		
	}
    tree->Fill();
    //pulsefinding + baseline?
    	return true;
}
 

