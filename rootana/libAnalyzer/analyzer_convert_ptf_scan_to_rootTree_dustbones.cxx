// Simple program to print out the values from a magnetic field scan.

#include <stdio.h>
#include <iostream>

#include "TRootanaEventLoop.hxx"
#include "TH1F.h"
#include "TH1D.h"
#include "TV1190Data.hxx"
#include "TV792Data.hxx"
#include "TV1730RawData.hxx"
#include "TCanvas.h"

#include <TTree.h>
#include <TFile.h>

#define TRUE 1
#define FALSE 0

#define nPoints_max 10000000
#define num_phidg_max 1000

// Offset for the ADC channel number
#define Ch_Offset 1

// Flag to indicate the gantry was not moving and to record ADC and Phidget values. 
int gbl_accept_banks = TRUE; //set FALSE

class ScanToTreeConverter: public TRootanaEventLoop {
  
  int nnn;
  TH1F *SampleWave0;
  TH1F *SampleWave1;
  TH1F *StartVal0;
  TH1F *Merging;  


private:

  int counter_gant;                                    //n-th measurement
  int subevent;
  double x0_pos, y0_pos, z0_pos, tilt0_pos, rot0_pos;  //positions of gantry0
  double x1_pos, y1_pos, z1_pos, tilt1_pos, rot1_pos;  //positions of gantry1
  double time;
  
  //TODO: add other positions (Phidget, laser, ...)

  int num_phidg0_points;
  double x0_field[num_phidg_max], y0_field[num_phidg_max], z0_field[num_phidg_max], tot0_field[num_phidg_max];     //B-field from phidget0
  int num_phidg1_points;
  double x1_field[num_phidg_max], y1_field[num_phidg_max], z1_field[num_phidg_max], tot1_field[num_phidg_max];     //B-field from phidget1
  double tilt_phid0[num_phidg_max], tilt_phid1[num_phidg_max];                       //tilt position from phidget
  double acc_x0[num_phidg_max], acc_y0[num_phidg_max], acc_z0[num_phidg_max];
  double acc_x1[num_phidg_max], acc_y1[num_phidg_max], acc_z1[num_phidg_max];

  
  //current and voltage of each of the 6 Helmholtz coils
  int counter_mag;
  double curr_coil1, curr_coil2, curr_coil3, curr_coil4, curr_coil5, curr_coil6;
  double volt_coil1, volt_coil2, volt_coil3, volt_coil4, volt_coil5, volt_coil6;
 
 
  //PMT currents and voltages
  double curr_hpd_enable, curr_hpd_hv_control, curr_hpd_lv_control;
  double curr_receiver0, curr_receiver1, curr_monitor0, curr_monitor1;

  double volt_hpd_enable, volt_hpd_hv_control, volt_hpd_lv_control;
  double volt_receiver0, volt_receiver1, volt_monitor0, volt_monitor1;

  //Digitizer variables
  double ADC0_dig[nPoints_max], ADC1_dig[nPoints_max], Start_time0[nPoints_max], Window_width0[nPoints_max], Start_time1[nPoints_max], Window_width1[nPoints_max], start_0, start_1;

  //PMT readout
  int counter;
  int num_points;
  int num_points_dig0; 
  int num_points_dig1;  
  int ADC0_voltage[nPoints_max];
  int ADC1_voltage[nPoints_max];
  int ADC2_voltage[nPoints_max];
  int ADC3_voltage[nPoints_max];
  int ADC4_voltage[nPoints_max];
  //TFile *outputfile; //made by TRootAnaEventLoop with name of inputfile +.root
  TTree *tree;
 
public:
    
  ScanToTreeConverter() {
    UseBatchMode(); //necessary to switch off graphics, used in for example AnaDisplay
    nnn = 0;
  };
  
  virtual ~ScanToTreeConverter() {};

  void BeginRun(int transition,int run,int time){
    std::cout << "Custom: begin run " << run << std::endl;
    //setup ROOT branches

    //Add Canvas
    TCanvas *c1 = new TCanvas("c1","Canvas Example",200,10,600,480);
    
    tree = new TTree("scan_tree","Scan Tree");
    tree->Branch("num_points",&num_points,"num_points/Int_t");
    tree->Branch("num_points_dig0",&num_points_dig0,"num_points_dig0/Int_t");
    tree->Branch("num_points_dig1",&num_points_dig1,"num_points_dig1/Int_t");    
    tree->Branch("ADC0_voltage",ADC0_voltage,"ADC0_voltage[num_points]/Int_t");
    tree->Branch("ADC1_voltage",ADC1_voltage,"ADC1_voltage[num_points]/Int_t");    
    tree->Branch("ADC2_voltage",ADC2_voltage,"ADC2_voltage[num_points]/Int_t");
    tree->Branch("ADC3_voltage",ADC3_voltage,"ADC3_voltage[num_points]/Int_t");    
    tree->Branch("ADC4_voltage",ADC4_voltage,"ADC4_voltage[num_points]/Int_t");          
    tree->Branch("time",&time,"time/Double_t");        //arbitrary offset in time, in ms, either UInt or larger, so just be safe
    tree->Branch("gantry_event",&counter_gant,"gantry_event/Int_t"); //an event is a measurement at a point
    tree->Branch("gantry_subevent",&subevent,"gantry_subevent/Int_t");

    tree->Branch("gantry0_x",&x0_pos,"gantry0_x/Double_t");
    tree->Branch("gantry0_y",&y0_pos,"gantry0_y/Double_t");
    tree->Branch("gantry0_z",&z0_pos,"gantry0_z/Double_t");
    tree->Branch("gantry0_tilt",&tilt0_pos,"gantry0_tilt/Double_t");
    tree->Branch("gantry0_rot",&rot0_pos,"gantry0_rot/Double_t");
    //tree->Branch("phidg0_tilt",&tilt_phid0,"phidg0_tilt/Double_t");
    
    //digitizer data 
    tree->Branch("ADC0_dig",&ADC0_dig,"ADC0_dig[num_points_dig1]/Double_t"); //think of eqn*
    tree->Branch("ADC1_dig",&ADC1_dig,"ADC1_dig[num_points_dig1]/Double_t"); //think of eqn**
    tree->Branch("Start_time0",&Start_time0,"Start_time0[num_points_dig0]/Double_t");
    tree->Branch("Start_time1",&Start_time1,"Start_time1[num_points_dig1]/Double_t");
    tree->Branch("Window_width0",&Window_width0,"Window_width0[num_points_dig0]/Double_t");
    tree->Branch("Window_width1",&Window_width1,"Window_width1[num_points_dig1]/Double_t");
 
    tree->Branch("gantry1_x",&x1_pos,"gantry1_x/Double_t");
    tree->Branch("gantry1_y",&y1_pos,"gantry1_y/Double_t");
    tree->Branch("gantry1_z",&z1_pos,"gantry1_z/Double_t");
    tree->Branch("gantry1_tilt",&tilt1_pos,"gantry1_tilt/Double_t");
    tree->Branch("gantry1_rot",&rot1_pos,"gantry1_rot/Double_t");
    //tree->Branch("phidg1_tilt",&tilt_phid1,"phidg1_tilt/Double_t");
    
    //field-related phidget measurements
    tree->Branch("num_phidg0_points",&num_phidg0_points,"num_phidg0_points/Int_t");
    tree->Branch("phidg0_Ax",acc_x0,"phidg0_Ax[num_phidg0_points]/Double_t");
    tree->Branch("phidg0_Ay",acc_y0,"phidg0_Ay[num_phidg0_points]/Double_t");
    tree->Branch("phidg0_Az",acc_z0,"phidg0_Az[num_phidg0_points]/Double_t");
    tree->Branch("phidg0_Bx",x0_field,"phidg0_Bx[num_phidg0_points]/Double_t");
    tree->Branch("phidg0_By",y0_field,"phidg0_By[num_phidg0_points]/Double_t");
    tree->Branch("phidg0_Bz",z0_field,"phidg0_Bz[num_phidg0_points]/Double_t");
    tree->Branch("phidg0_Btot",tot0_field,"phidg0_Btot[num_phidg0_points]/Double_t");
    tree->Branch("phidg0_tilt",tilt_phid0,"phidg0_tilt[num_phidg0_points]/Double_t");
    
    tree->Branch("num_phidg1_points",&num_phidg1_points,"num_phidg1_points/Int_t");
    tree->Branch("phidg1_Ax",acc_x1,"phidg1_Ax[num_phidg1_points]/Double_t");
    tree->Branch("phidg1_Ay",acc_y1,"phidg1_Ay[num_phidg1_points]/Double_t");
    tree->Branch("phidg1_Az",acc_z1,"phidg1_Az[num_phidg1_points]/Double_t");
    tree->Branch("phidg1_Bx",x1_field,"phidg1_Bx[num_phidg1_points]/Double_t");
    tree->Branch("phidg1_By",y1_field,"phidg1_By[num_phidg1_points]/Double_t");
    tree->Branch("phidg1_Bz",z1_field,"phidg1_Bz[num_phidg1_points]/Double_t");
    tree->Branch("phidg1_Btot",tot1_field,"phidg1_Btot[num_phidg1_points]/Double_t");
    tree->Branch("phidg1_tilt",tilt_phid1,"phidg1_tilt[num_phidg1_points]/Double_t");
    
    //Helmholtz Coil related 
    tree->Branch("coil_event",&counter_mag,"coil_event/Int_t");
    tree->Branch("I_coil1",&curr_coil1,"I_coil1/Double_t");
    tree->Branch("I_coil2",&curr_coil2,"I_coil2/Double_t");
    tree->Branch("I_coil3",&curr_coil3,"I_coil3/Double_t");
    tree->Branch("I_coil4",&curr_coil4,"I_coil4/Double_t");
    tree->Branch("I_coil5",&curr_coil5,"I_coil5/Double_t");
    tree->Branch("I_coil6",&curr_coil6,"I_coil6/Double_t");
    
    tree->Branch("U_coil1",&volt_coil1,"U_coil1/Double_t");
    tree->Branch("U_coil2",&volt_coil2,"U_coil2/Double_t");
    tree->Branch("U_coil3",&volt_coil3,"U_coil3/Double_t");
    tree->Branch("U_coil4",&volt_coil4,"U_coil4/Double_t");
    tree->Branch("U_coil5",&volt_coil5,"U_coil5/Double_t");
    tree->Branch("U_coil6",&volt_coil6,"U_coil6/Double_t");
    
    //PMT voltage/current related
    tree->Branch("I_HPD_enable",&curr_hpd_enable,"I_HPD_enable/Double_t");
    tree->Branch("I_HPD_HV_control",&curr_hpd_hv_control,"I_HPD_HV_control/Double_t");
    tree->Branch("I_HPD_LV_control",&curr_hpd_lv_control,"I_HPD_LV_control/Double_t");
    tree->Branch("I_receiver0",&curr_receiver0,"I_receiver0/Double_t");
    tree->Branch("I_receiver1",&curr_receiver1,"I_receiver1/Double_t");
    tree->Branch("I_monitor0",&curr_monitor0,"I_monitor0/Double_t");
    tree->Branch("I_monitor1",&curr_monitor1,"I_monitor1/Double_t");

    tree->Branch("U_HPD_enable",&volt_hpd_enable,"U_HPD_enable/Double_t");
    tree->Branch("U_HPD_HV_control",&volt_hpd_hv_control,"U_HPD_HV_control/Double_t");
    tree->Branch("U_HPD_LV_control",&volt_hpd_lv_control,"U_HPD_LV_control/Double_t");
    tree->Branch("U_receiver0",&volt_receiver0,"U_receiver0/Double_t");
    tree->Branch("U_receiver1",&volt_receiver1,"U_receiver1/Double_t");
    tree->Branch("U_monitor0",&volt_monitor0,"U_monitor0/Double_t");
    tree->Branch("U_monitor1",&volt_monitor1,"U_monitor1/Double_t");

    //Histogram for waveform
    SampleWave0 = new TH1F("","",10000,0,10000);
    SampleWave0->SetDirectory(0);
    SampleWave1 = new TH1F("","",10000,0,10000);
    SampleWave1->SetDirectory(0);

StartVal0 = new TH1F("","",10000,0,10000);
StartVal0->SetDirectory(0);
Merging = new TH1F("","",10000,0,10000);
Merging->SetDirectory(0);

  }
  
  void EndRun(int transition,int run,int time){
    tree->Fill();
    std::cout << "Custom end run " << run <<std::endl;
    //tree->Write(); //: happens through in fOutputFile->Write() in base class (ProcessMidasFile)
  }
  
  bool ProcessMidasEvent(TDataContainer& dataContainer){
    TGenericData *bank = dataContainer.GetEventData<TGenericData>("EOM");  // END OF MOVE = START of MEASUREMENT
    if(bank){
      gbl_accept_banks = TRUE;
      return true;
    }
    bank = dataContainer.GetEventData<TGenericData>("BONM");               // BEGINNING OF NEXT MOVE = END of MEASUREMENT
    if(bank){
      tree->Fill();
      num_points = 0;
      num_points_dig0 = 0;
      num_points_dig1 = 0;
      num_phidg0_points = 0;
      num_phidg1_points = 0;
      gbl_accept_banks = FALSE;
      return true;
    }
    num_points_dig1++;
    num_points_dig0++;
    int  DerivThreshold = 16; //Thomas 12
    int DerivThreshold_1 = 12;
    counter++;
    int  SmallDeriv = 5; //Thomas 5
    int SmallDeriv_1 = 5;
    if(gbl_accept_banks){
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//attempt at adding digitizer data

      TV1730RawData *v1730 = dataContainer.GetEventData<TV1730RawData>("V730");
      if(v1730){
	std::vector<RawChannelMeasurement> measurements = v1730->GetMeasurements();
	int tem=0;
	for(int i=0; i < measurements.size(); i+=2){
	  int chan = measurements[i].GetChannel();
	  float offset = 0;
	  std::vector<UShort_t> samples;
	  for(int ib = 0; ib < measurements[i].GetNSamples(); ib++){
	    samples.push_back(measurements[i].GetSample(ib)-offset);
	  }
	  //Peak find
	  double Deriv=0;
	  double OldDeriv=0;
          double MaxV;
          double MaxVT;
          double Charge=0;
          double MinD;
          double MinDT;
          double MaxD;
          double MaxDT;
          double Start;
          double Baseline;
	  //int tem = 0;
          std::vector<Int_t> BaseSamples;
	  double Start_vec[100];
	  double j_vec[100];
	  double Baseline_vec[100];
	  double Charge_vec[100];
          double sum=0;
	  bool inpulse = false; 
          for(double j = 1; j<samples.size()-1; ++j){
            OldDeriv = Deriv;
            Deriv = samples[j+1] - samples[j-1];
            Double_t AbsDeriv = TMath::Abs(Deriv);
            if(inpulse && chan == 0){
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
	        Start_vec[tem] = Start;
		Charge_vec[tem] = Charge;
		j_vec[tem] = j;
		Baseline_vec[tem] = Baseline;
		tem ++;
		inpulse = false;
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
              for(int k=0; k<BaseSamples.size(); ++k){ Baseline += ((Double_t)BaseSamples[k])/((Double_t)BaseSamples.size()); }
            }
            else if(AbsDeriv < SmallDeriv){
              BaseSamples.push_back(samples[j]);
              if(BaseSamples.size() > 20) BaseSamples.erase (BaseSamples.begin());
	    }
	    if(counter<100){
	      SampleWave0->Fill(j-1,(Double_t)samples[j-1]-Baseline);
	    }
	  }
	  if(tem == 0){
	    Baseline = 0;
	    for(int k=0; k<BaseSamples.size(); ++k){ Baseline += ((Double_t)BaseSamples[k])/((Double_t)BaseSamples.size()); }
	    for(int jj=100; jj<120; jj++) sum += samples[jj];
	    sum = sum/20;
std::cout << "trial 1  " << sum << std::endl; 	   
 Charge_vec[tem] = sum - Baseline;
	  }
	  //code for chan 1 goes here
	  int chan_1 = measurements[i+1].GetChannel();
	  std::vector<UShort_t> samples_1;
	  for(int ic = 0; ic < measurements[i+1].GetNSamples(); ic++){
            samples_1.push_back(measurements[i+1].GetSample(ic)-offset);
	  }
  	  num_points_dig0++;
	  num_points_dig1++;
	  double sum_1=0;
	  double Deriv_1=0;
          double OldDeriv_1=0;
          double MaxV_1;
          double MaxVT_1;
          double Charge_1=0;
          double MinD_1;
          double MinDT_1;
          double MaxD_1;
          double MaxDT_1;
          double Start_1;
          double Baseline_1;
	  bool inpulse_1 = false;
	  bool dish = true;
	  std::vector<Int_t> BaseSamples_1;
          for(double m = 1; m<samples_1.size()-1; ++m){
            OldDeriv_1 = Deriv_1;
            Deriv_1 = samples_1[m+1] - samples_1[m-1];
            Double_t AbsDeriv_1 = TMath::Abs(Deriv_1);
            if(inpulse_1 && chan_1 == 1){
              Charge_1 += samples_1[m];
              if(samples_1[m]<MaxV_1){
                MaxVT_1 = m;
                MaxV_1 = samples_1[m];
              }
              if(Deriv_1 < MaxD_1){
                MaxD_1 = Deriv_1;
                MaxDT_1 = m;
              }
              if(Deriv_1 > MinD_1){
                MinD_1 = Deriv_1;
                MinDT_1 = m;
	      }
	      if((AbsDeriv_1 < SmallDeriv_1 && (samples_1[m]-Baseline_1) > -5)|| m>(samples_1.size()-2)){
		for(int s=0; s<tem; s++){
		  if(Start_vec[s]-60 < Start_1 && Start_vec[s] > Start_1){ //Set limits of overlap -- chan0 value must be between 0 and 60 units above chan1
		    ADC0_dig[num_points_dig0-1]= (double) -(Charge_vec[s]-(j_vec[s]-Start_vec[s]+1)*Baseline_vec[s]);
                    Start_time0[num_points_dig0-1] = (double) Start_vec[s];
                    Window_width0[num_points_dig0-1] = (double) j_vec[s]-Start_vec[s]+1;	    
  		    ADC1_dig[num_points_dig1-1]=(double) -(Charge_1-(m-Start_1+1)*Baseline_1); 
		    Start_time1[num_points_dig1-1] = (double) Start_1; 
		    Window_width1[num_points_dig1-1] = (double) m-Start_1+1;	    		  
		    inpulse_1 = false;
		    start_0 = Start_time0[num_points_dig0-1];
		    start_1 = Start_time1[num_points_dig1-1];
		    dish = false;
		  }
		}
	      }
	    }  
	    else if(AbsDeriv_1 > DerivThreshold_1){
              inpulse_1 = true;
              MaxV_1= samples_1[m];
              Start_1 = m-2;
              MaxVT_1 = m;
              MinDT_1 = m;
              MinD_1 = Deriv_1;
              MaxD_1 = Deriv_1;
              MaxDT_1 = m;
              Charge_1 = samples_1[m] + samples_1[m-1] + samples_1[m-2];
              Baseline_1 = 0;
	      for(int n=0; n<BaseSamples_1.size(); ++n){ Baseline_1 += ((Double_t)BaseSamples_1[n])/((Double_t)BaseSamples_1.size()); }
            }
            else if(AbsDeriv_1 < SmallDeriv_1){
              BaseSamples_1.push_back(samples_1[m]);
              if(BaseSamples_1.size() > 20) BaseSamples_1.erase (BaseSamples_1.begin());
	    }	
	    if(counter<100){
              SampleWave1->Fill(m-1,(Double_t)samples_1[m-1]-Baseline_1);
	    }
	  }
	  if(dish == true){
	    Baseline_1 = 0;
	    for(int k=0; k<BaseSamples_1.size(); ++k){ Baseline_1 += ((Double_t)BaseSamples_1[k])/((Double_t)BaseSamples_1.size()); }
	    for(int jk=100; jk<120; jk++) sum_1 += samples_1[jk];
	    sum_1 = sum_1/20;
	std::cout << "sum  " << sum << "  " << Baseline << std::endl;	    
ADC0_dig[num_points_dig0-1]=sum-Baseline;
	    ADC1_dig[num_points_dig1-1]=sum_1-Baseline_1;
	  }
	  if(counter<100){
            SampleWave0->Write(Form("Chan_%d_Wave_%d",chan,counter));
	    SampleWave1->Write(Form("Chan_%d_Wave_%d",chan_1,counter));
	    SampleWave0->Reset();
            SampleWave1->Reset();
	  }
	}
      } 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      // Try gettting an ADC bank
      TV792Data *bank = dataContainer.GetEventData<TV792Data>("ADC0");
      if(bank){
	num_points++;
	
	/// Get the Vector of ADC Measurements.
	std::vector<VADCMeasurement> measurements = bank->GetMeasurements();
	for(unsigned int i = 0; i < measurements.size(); i++){ // loop over measurements
	  int chan = measurements[i].GetChannel()-Ch_Offset;
	  uint32_t adc = measurements[i].GetMeasurement();
switch (chan){
	  case 0:
	    ADC0_voltage[num_points-1] = (int) adc;
	    break;
	  case 1:
	    ADC1_voltage[num_points-1] = (int) adc;
	    break;
	  case 2:
	    ADC2_voltage[num_points-1] = (int) adc;         
	    break;
	  case 3:
	    ADC3_voltage[num_points-1] = (int) adc;        
	    break;
	  case 4:
	    ADC4_voltage[num_points-1] = (int) adc;        
	    break;
	  default:
	    break;
	  }
	}
	return true;
      }
      // Try getting a Phidget bank
      TGenericData *bank_ph0 = dataContainer.GetEventData<TGenericData>("PH00");
      if(bank_ph0){
	num_phidg0_points++;
	acc_x0[num_phidg0_points -1] = ((double*)bank_ph0->GetData64())[0];
	acc_y0[num_phidg0_points -1] = ((double*)bank_ph0->GetData64())[1];
	acc_z0[num_phidg0_points -1] = ((double*)bank_ph0->GetData64())[2];
	x0_field[num_phidg0_points -1] = ((double*)bank_ph0->GetData64())[3];
	y0_field[num_phidg0_points -1] = ((double*)bank_ph0->GetData64())[4];
	z0_field[num_phidg0_points -1] = ((double*)bank_ph0->GetData64())[5];
	tot0_field[num_phidg0_points -1] = ((double*)bank_ph0->GetData64())[6];    
	tilt_phid0[num_phidg0_points -1] = ((double*)bank_ph0->GetData64())[7];
	return true;
      }

      TGenericData *bank_ph1 = dataContainer.GetEventData<TGenericData>("PH01");
      if(bank_ph1){
	num_phidg1_points++;
        
	acc_x1[num_phidg1_points -1] = ((double*)bank_ph1->GetData64())[0];
	acc_y1[num_phidg1_points -1] = ((double*)bank_ph1->GetData64())[1];
	acc_z1[num_phidg1_points -1] = ((double*)bank_ph1->GetData64())[2];
	x1_field[num_phidg1_points -1] = ((double*)bank_ph1->GetData64())[3];
	y1_field[num_phidg1_points -1] = ((double*)bank_ph1->GetData64())[4];
	z1_field[num_phidg1_points -1] = ((double*)bank_ph1->GetData64())[5];
	tot1_field[num_phidg1_points -1] = ((double*)bank_ph1->GetData64())[6];    
	tilt_phid1[num_phidg1_points -1] = ((double*)bank_ph1->GetData64())[7];

	return true;
      }
      
      //Grab the gantry bank 
      TGenericData *bank_cyc = dataContainer.GetEventData<TGenericData>("CYC0");
      if(bank_cyc){ 
	counter_gant = (int)((double*)bank_cyc->GetData64())[0];
	
	x0_pos = ((double*)bank_cyc->GetData64())[1];
	y0_pos = ((double*)bank_cyc->GetData64())[2];
	z0_pos = ((double*)bank_cyc->GetData64())[3];
	rot0_pos = ((double*)bank_cyc->GetData64())[4];
	tilt0_pos = ((double*)bank_cyc->GetData64())[5];
	x1_pos = ((double*)bank_cyc->GetData64())[6];
	y1_pos = ((double*)bank_cyc->GetData64())[7];
	z1_pos = ((double*)bank_cyc->GetData64())[8];
	rot1_pos = ((double*)bank_cyc->GetData64())[9];
	tilt1_pos = ((double*)bank_cyc->GetData64())[10];
	
	time = ((double*)bank_cyc->GetData64())[11];
	
	std::cout << counter_gant << " " 
		  << time << " " << ((double*)bank_cyc->GetData64())[11] 
		  << std::endl;
	
	/* DEBUG */
	//std::cout << counter_gant << " " << i << " " 
	//<<  x0_pos[i] << " " << y0_pos[i] << " " << z0_pos[i] << " " 
	//<< time[i] << " " 
	//<< std::endl;
	
	if(counter_gant%500 == 0)
	  std::cout << "Event " << counter_gant << std::endl;
      }
      
      TGenericData *bank_mag = dataContainer.GetEventData<TGenericData>("MAG0");
      if(bank_mag){ 	
	counter_mag = ((double*)bank_mag->GetData64())[0];
	
	//in feScan: filled from [1-80], [1-40] is current
	curr_coil1 = ((double*)bank_mag->GetData64())[9];
	curr_coil2 = ((double*)bank_mag->GetData64())[10];
	curr_coil3 = ((double*)bank_mag->GetData64())[1];
	curr_coil4 = ((double*)bank_mag->GetData64())[2];
	curr_coil5 = ((double*)bank_mag->GetData64())[3];
	curr_coil6 = ((double*)bank_mag->GetData64())[4];
	curr_hpd_enable = ((double*)bank_mag->GetData64())[5];
	curr_hpd_hv_control = ((double*)bank_mag->GetData64())[6];
	curr_hpd_lv_control = ((double*)bank_mag->GetData64())[7];
	curr_receiver0 = ((double*)bank_mag->GetData64())[33];
	curr_monitor0 = ((double*)bank_mag->GetData64())[34];
	curr_receiver1 = ((double*)bank_mag->GetData64())[35];
	curr_monitor1 = ((double*)bank_mag->GetData64())[36];
	
	
	volt_coil1 = ((double*)bank_mag->GetData64())[49];
	volt_coil2 = ((double*)bank_mag->GetData64())[50];
	volt_coil3 = ((double*)bank_mag->GetData64())[41];
	volt_coil4 = ((double*)bank_mag->GetData64())[42];
	volt_coil5 = ((double*)bank_mag->GetData64())[43];
	volt_coil6 = ((double*)bank_mag->GetData64())[44];

	volt_hpd_enable = ((double*)bank_mag->GetData64())[45];
	volt_hpd_hv_control = ((double*)bank_mag->GetData64())[46];
	volt_hpd_lv_control = ((double*)bank_mag->GetData64())[47];
	volt_receiver0 = ((double*)bank_mag->GetData64())[73];
	volt_monitor0 = ((double*)bank_mag->GetData64())[74];
	volt_receiver1 = ((double*)bank_mag->GetData64())[75];
	volt_monitor1 = ((double*)bank_mag->GetData64())[76];

      }
    }
  return true;
  }
  
  
  // Describe some other command line argument
  void Usage(void){
    std::cout << "\t-D option: this is a fun new option " << std::endl;
  }
  
  // Define some other command line argument
  bool CheckOption(std::string option){
    const char* arg = option.c_str();
    if (strncmp(arg,"-D",2)==0){
      std::cout << arg+2 << std::endl;
      std::cout << "I'm happy with this flag!" << std::endl;
      return true;
    }
    
    return false;
  }
}; 

int main(int argc, char *argv[])
{
  
  ScanToTreeConverter::CreateSingleton<ScanToTreeConverter>();
  return ScanToTreeConverter::Get().ExecuteLoop(argc, argv);
  
}

