#include "SkimSingle.hh"

SkimSingle::SkimSingle():TRootanaEventLoop(){
    DisableAutoMainWindow();
    IsOffline();
    tree = 0;
    rootevent = 0;
    DerivThreshold = 12;
    SmallDeriv = 5;
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
    Charge_Time = new TH2F("Charge_Time","Charge_Time",10000,0,20000,300,0,300);
    Charge_Time->SetDirectory(0);
    Peak_Time = new TH2F("Peak_Time","Peak_Time",10000,0,20000,300,0,300);
    Peak_Time->SetDirectory(0);
    PulseNum = new TH1F("PulseNum","PulseNum",20,0,20);
    PulseNum->SetDirectory(0);
    DeltaT_Peak = new TH2F("DeltaT_Peak","DeltaT_Peak",200,-200,200,300,0,300);
    DeltaT_Peak->SetDirectory(0);
    DeltaT_PRatio = new TH2F("DeltaT_PRatio","DeltaT_PRatio",200,-200,200,100,0,10);
    DeltaT_PRatio->SetDirectory(0);
    Peak1_Peak2 = new TH2F("Peak1_Peak2","Peak1_Peak2",300,0,300,300,0,300);
    Peak1_Peak2->SetDirectory(0);
    PeakFixWindow = new TH1F("ChargeFixWindow","ChargeFixWindow",351,-50.5,300.5);
    PeakFixWindow->SetDirectory(0);	
 //   if(savesamples){
        SampleWave = new TH1F("","",10000,0,10000);
        SampleWave->SetDirectory(0);
	SampleWaveDeriv = new TH1F("","",10000,0,10000);
	SampleWaveDeriv->SetDirectory(0);
	SamplePulseFind = new TH1F("","",10000,0,10000);
        SamplePulseFind->SetDirectory(0);
 //   }
    counter=0;
}

void SkimSingle::EndRun(int transition,int run,int time){
    DeltaT_Peak->Write();
    DeltaT_PRatio->Write();
    Peak1_Peak2->Write();
    PulseNum->Write();
    ChargeHist->Scale(1./ChargeHist->Integral());
    ChargeHist->Write();
    BaselineHist->Write();
    TimeHist->Write();
    PeakHist->Write();
    Charge_Time->Write();
    Peak_Time->Write();
    PeakFixWindow->Write();
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
	    //Peak find
	    Int_t Deriv=0;
	    Int_t OldDeriv=0;
	    Int_t MaxV;
	    Int_t MaxVT;
	    Int_t Charge=0;
	    Int_t MinD;
	    Int_t MinDT;
	    Int_t MaxD;
	    Int_t MaxDT;
	    Int_t Start;
	    Double_t Baseline;
	    vector<Int_t> BaseSamples;
	    bool inpulse = false;
	    Int_t pulsecount=0;
	    for(int j = 1; j<samples.size()-1; ++j){
		OldDeriv = Deriv;
		Deriv = samples[j+1] - samples[j-1];
		Double_t AbsDeriv = TMath::Abs(Deriv);
		if(inpulse){
			Charge += samples[j];
			if(samples[j]<MaxV){
				MaxVT = j;
				MaxV = samples[j];
			}
			if(Deriv < MaxD){
				MaxD = Deriv;
				MaxDT = j;
			}
			if(Deriv > MinD){
				MinD = Deriv;
				MinDT = j;
			} 
			if((AbsDeriv < SmallDeriv && (samples[j]-Baseline) > -5)|| j>(samples.size()-2)){
				if(chan == 0)rootevent->AddTargetPulse(MaxVT*2.0,Charge,Baseline,MaxV,j-Start+1,Start,MaxD,MaxDT,MinD,MinDT);
				else if(chan ==1) rootevent->AddMonitor0Pulse(MaxVT*2.0,Charge,Baseline,MaxV,j-Start+1,Start,MaxD,MaxDT,MinD,MinDT);
				if(MaxVT < 4000) ++pulsecount;
				inpulse = false;
				for(int k=Start; k<=j; ++k) SamplePulseFind->Fill(k,MaxV-Baseline);
			}
		}
		else if(AbsDeriv > DerivThreshold){
			inpulse = true;
			MaxV= samples[j];
			Start = j-2;
			MaxVT = j;
			MinDT = j;
			MinD = Deriv;
			MaxD = Deriv;
			MaxDT = j;
			Charge = samples[j] + samples[j-1] + samples[j-2];
			Baseline = 0;
			for(int k=0; k<BaseSamples.size(); ++k) Baseline += ((Double_t)BaseSamples[k])/((Double_t)BaseSamples.size());
		}
		else if(AbsDeriv < SmallDeriv){
			BaseSamples.push_back(samples[j]);
			if(BaseSamples.size() > 20) BaseSamples.erase (BaseSamples.begin());
		}
	    }
	    if((chan == 0 || chan == 1) && counter < 1000) SamplePulseFind->Write(Form("Chan_%d_WavePulseFind_%d",chan,counter));
            SamplePulseFind->Reset();
            if(chan == 1){ 
		rootevent->SetMonitor0Samples(samples);	
                if(counter < 1000){
                    Double_t Baseline = 0.;
                    for(int j=20; j<40; ++j) Baseline += (Double_t) samples[j];
                    Baseline /= 20.;
                    Double_t Charge=0.;
                    for(int j=40; j<80; ++j) Charge += (Double_t) samples[j];
                    for(int j=0; j<samples.size(); ++j) SampleWave->Fill(j,(Double_t)samples[j]-Baseline);
		    for(int j=1; j<samples.size()-1; ++j) SampleWaveDeriv->Fill(j,(Double_t)(samples[j+1]-samples[j-1]));
                    SampleWave->Write(Form("Chan_%d_Wave_%d_%f",chan,counter,-(Charge - Baseline*41)));
                    SampleWave->Reset();
		    SampleWaveDeriv->Write(Form("Chan_%d_WaveDeriv_%d_%f",chan,counter,-(Charge - Baseline*41)));
		    SampleWaveDeriv->Reset();
                }
                ++counter;
            }
            if(chan == 0){
		PulseNum->Fill(pulsecount);
                rootevent->SetTargetPMTSamples(samples);
                Double_t Baseline = 0.;
                for(int j=00; j<40; ++j) Baseline += (Double_t) samples[j];
                Baseline /= 40.;
                Double_t Charge=0.;
		Double_t MaxPeak=100.;
                for(int j=40; j<80; ++j){ Charge += (Double_t) samples[j]; if(samples[j]-Baseline < MaxPeak) MaxPeak = samples[j]-Baseline;}
		PeakFixWindow->Fill(-MaxPeak);
                if(counter < 1000){
                    for(int j=0; j<samples.size(); ++j) SampleWave->Fill(j,(Double_t)samples[j]-Baseline);
                    SampleWave->Write(Form("Chan_%d_Wave_%d_%f",chan,counter,-(Charge - Baseline*41)));
                    SampleWave->Reset();
		    for(int j=1; j<samples.size()-1; ++j) SampleWaveDeriv->Fill(j,(Double_t)(samples[j+1]-samples[j-1]));
		    SampleWaveDeriv->Write(Form("Chan_%d_WaveDeriv_%d_%f",chan,counter,-(Charge - Baseline*41)));
                    SampleWaveDeriv->Reset();
                }
                ++counter;
                BaselineHist->Fill(Baseline);
		int NPrompt=0;
		int b=0;
		for(int k=0; k<rootevent->GetTargetPulseCount(); ++k){
			if(rootevent->GetTargetPulse(k)){
			//	Printf(Form("chan 0 # pulse = %d/%d %f",k,rootevent->GetTargetPulseCount(),rootevent->GetTargetPulse(k)->GetCharge()));
				ChargeHist->Fill(-rootevent->GetTargetPulse(k)->GetCharge());
                		TimeHist->Fill(rootevent->GetTargetPulse(k)->GetTime());
                		PeakHist->Fill(-rootevent->GetTargetPulse(k)->GetHeight());
                		Charge_Time->Fill(rootevent->GetTargetPulse(k)->GetTime(),-rootevent->GetTargetPulse(k)->GetCharge());
                		Peak_Time->Fill(rootevent->GetTargetPulse(k)->GetTime(),-rootevent->GetTargetPulse(k)->GetHeight());
				if(19108. < rootevent->GetTargetPulse(k)->GetTime() && rootevent->GetTargetPulse(k)->GetTime() < 19130){ ++NPrompt;b=k;}
				if(rootevent->GetTargetPulse(k-1) && rootevent->GetTargetPulse(k-1)->GetTime()>10000.) 
					DeltaT_Peak->Fill(rootevent->GetTargetPulse(k)->GetTime()-rootevent->GetTargetPulse(k-1)->GetTime(),-rootevent->GetTargetPulse(b)->GetHeight());
			}
		}
	/*	if(NPrompt == 1){
			Double_t  MaxPre = 10000;
			Double_t  MaxPreT=0;
			Double_t  MaxPost = 10000;
			Double_t  MaxPostT = 0;
			Int_t MidTime = rootevent->GetTargetPulse(b)->GetTime()/2.0;
			Int_t StartTime = MidTime-100;
			Int_t StopTime  = MidTime+100;
			for(int k=StartTime;k<MidTime-3; ++k)
				if(samples[k] < MaxPre){ MaxPre = samples[k]; MaxPreT=k;}
			for(int k=MidTime+15;k<=StopTime; ++k)
                                if(samples[k] < MaxPost){ MaxPost = samples[k]; MaxPostT=k;}
			//Printf(Form("\n %f %f %f %f",MaxPreT*2.0,rootevent->GetTargetPulse(b)->GetTime(),-MaxPre,rootevent->GetTargetPulse(b)->GetBaseline()));
			//Printf(Form("%f %f ",rootevent->GetTargetPulse(b)->GetTime()-MaxPostT*2.0,-(MaxPost-rootevent->GetTargetPulse(b)->GetBaseline())));
			DeltaT_Peak->Fill(MaxPreT*2.0-rootevent->GetTargetPulse(b)->GetTime(),-(MaxPre-rootevent->GetTargetPulse(b)->GetBaseline()));
			DeltaT_Peak->Fill(MaxPostT*2.0-rootevent->GetTargetPulse(b)->GetTime(),-(MaxPost-rootevent->GetTargetPulse(b)->GetBaseline())); 
			DeltaT_PRatio->Fill(MaxPostT*2.0-rootevent->GetTargetPulse(b)->GetTime(),(MaxPost-rootevent->GetTargetPulse(b)->GetBaseline())/rootevent->GetTargetPulse(b)->GetHeight());
    			Peak1_Peak2->Fill(-rootevent->GetTargetPulse(b)->GetHeight(),-(MaxPost-rootevent->GetTargetPulse(b)->GetBaseline()));
		}*/
            }
	}		
    }
	
    tree->Fill();
    //pulsefinding + baseline?
    	return true;
}

