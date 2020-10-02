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

#define nPoints_max 6000 // John 2019-11-05 Reduced from 1000000
#define num_phidg_max 10000
#define max_temp_sensor 20
#define num_v1730_max 70 // IMPOTANT: IF THIS IS EVER CHANGED, ALSO CHANGE THE HARDCODED VALUES FOR WAVEFORM BRANCH WIDTHS AS WELL (see: "v1730 data")
#define timeStart 130 // defines start of PMT Pulse timing window, currently at the 130th sample of 200, with a window size of 70 samples.

// Offset for the ADC channel number
#define Ch_Offset 1

// Flag to indicate the gantry was not moving and to record ADC and Phidget values. 
int gbl_accept_banks = TRUE; //set FALSE

class ScanToTreeConverter: public TRootanaEventLoop {

  int nnn;
  TH1F *SampleWave0;
  TH1F *SampleWave1;

  //9.Nov.2017 added other channels
  TH1F *SampleWave2;
  TH1F*SampleWave3;
  TH1F *SampleWave4;
  TH1F *SampleWave5;

  TH1F *StartVal0;

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

  int num_phidg3_points;
  double x3_field[num_phidg_max], y3_field[num_phidg_max], z3_field[num_phidg_max], tot3_field[num_phidg_max];     //B-field from phidget3
  int num_phidg4_points;
  double x4_field[num_phidg_max], y4_field[num_phidg_max], z4_field[num_phidg_max], tot4_field[num_phidg_max];     //B-field from phidget4
  double tilt_phid3[num_phidg_max], tilt_phid4[num_phidg_max];                       //tilt position from phidget
  double acc_x3[num_phidg_max], acc_y3[num_phidg_max], acc_z3[num_phidg_max];
  double acc_x4[num_phidg_max], acc_y4[num_phidg_max], acc_z4[num_phidg_max];

  double int_temp, ext1_temp,ext2_temp;
  double temperatures[max_temp_sensor];  // temperatures[0] is int temp, temperatures[1] is ext1, temperature[2] is ext2

  Double_t timestamp;//Adding timing variable

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
  double Start_time0[nPoints_max], Window_width0[nPoints_max], Start_time1[nPoints_max], Window_width1[nPoints_max];
  
  // V1730 waveforms
  // 9.Nov.2017 Let's try adding using channels 2,3,4, 5
  double V1730_wave0[nPoints_max][num_v1730_max],  V1730_wave1[nPoints_max][num_v1730_max],  V1730_wave2[nPoints_max][num_v1730_max], V1730_wave3[nPoints_max][num_v1730_max], V1730_wave4[nPoints_max][num_v1730_max], V1730_wave5[nPoints_max][num_v1730_max];

  //PMT readout
  int start_val_stat;
  int window_width;
  int trigger;
  int counter;
  int num_points;
  int num_points_dig0; 
  int num_points_dig1;  

  //TFile *outputfile; //made by TRootAnaEventLoop with name of inputfile +.root
  TTree *tree;

  // Counters for TDC bank
  int ngoodTDCbanks;
  int nbadTDCbanks;

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

    tree->Branch("timestamp",&timestamp,"timestamp/Double_t");        //real time used in sec
    tree->Branch("gantry_event",&counter_gant,"gantry_event/Int_t"); //an event is a measurement at a point
    tree->Branch("gantry_subevent",&subevent,"gantry_subevent/Int_t");

    tree->Branch("gantry0_x",&x0_pos,"gantry0_x/Double_t");
    tree->Branch("gantry0_y",&y0_pos,"gantry0_y/Double_t");
    tree->Branch("gantry0_z",&z0_pos,"gantry0_z/Double_t");
    tree->Branch("gantry0_tilt",&tilt0_pos,"gantry0_tilt/Double_t");
    tree->Branch("gantry0_rot",&rot0_pos,"gantry0_rot/Double_t");
    //tree->Branch("phidg0_tilt",&tilt_phid0,"phidg0_tilt/Double_t");


    tree->Branch("Start_time0",&Start_time0,"Start_time0[num_points_dig0]/Double_t");
    tree->Branch("Start_time1",&Start_time1,"Start_time1[num_points_dig1]/Double_t");
    tree->Branch("Window_width0",&Window_width0,"Window_width0[num_points_dig0]/Double_t");
    tree->Branch("Window_width1",&Window_width1,"Window_width1[num_points_dig1]/Double_t");

    // v1730 data  V1730_wave0[nPoints_max][num_v1730_max]
    tree->Branch("V1730_wave0",&V1730_wave0,"V1730_wave0[num_points][70]/Double_t"); //think of eqn* // SIZE OF COLUMN MUST MATCH num_v1730_max OR ELSE BANDING ISSUES WILL OCCUR
    tree->Branch("V1730_wave1",&V1730_wave1,"V1730_wave1[num_points][70]/Double_t"); //think of eqn*
    tree->Branch("V1730_wave2",&V1730_wave2,"V1730_wave2[num_points][70]/Double_t"); //think of eqn*
    tree->Branch("V1730_wave3",&V1730_wave3,"V1730_wave3[num_points][70]/Double_t"); 
    tree->Branch("V1730_wave4",&V1730_wave4,"V1730_wave4[num_points][70]/Double_t");
    tree->Branch("V1730_wave5",&V1730_wave5,"V1730_wave5[num_points][70]/Double_t");
    //tree->Branch("V1730_wave0",&V1730_wave0,"V1730_wave0[num_points]/Double_t"); //think of eqn*
    //tree->Branch("V1730_wave1",&V1730_wave1,"V1730_wave1[num_points]/Double_t"); //think of eqn*
    //tree->Branch("V1730_wave2",&V1730_wave2,"V1730_wave2[num_points]/Double_t"); //think of eqn*
    //tree->Branch("ADC0_voltage",ADC0_voltage,"ADC0_voltage[num_points]/Int_t");

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

    tree->Branch("num_phidg3_points",&num_phidg3_points,"num_phidg3_points/Int_t");
    tree->Branch("phidg3_Ax",acc_x3,"phidg3_Ax[num_phidg3_points]/Double_t");
    tree->Branch("phidg3_Ay",acc_y3,"phidg3_Ay[num_phidg3_points]/Double_t");
    tree->Branch("phidg3_Az",acc_z3,"phidg3_Az[num_phidg3_points]/Double_t");
    tree->Branch("phidg3_Bx",x3_field,"phidg3_Bx[num_phidg3_points]/Double_t");
    tree->Branch("phidg3_By",y3_field,"phidg3_By[num_phidg3_points]/Double_t");
    tree->Branch("phidg3_Bz",z3_field,"phidg3_Bz[num_phidg3_points]/Double_t");
    tree->Branch("phidg3_Btot",tot3_field,"phidg3_Btot[num_phidg3_points]/Double_t");
    tree->Branch("phidg3_tilt",tilt_phid3,"phidg3_tilt[num_phidg3_points]/Double_t");

    tree->Branch("num_phidg4_points",&num_phidg4_points,"num_phidg4_points/Int_t");
    tree->Branch("phidg4_Ax",acc_x4,"phidg4_Ax[num_phidg4_points]/Double_t");
    tree->Branch("phidg4_Ay",acc_y4,"phidg4_Ay[num_phidg4_points]/Double_t");
    tree->Branch("phidg4_Az",acc_z4,"phidg4_Az[num_phidg4_points]/Double_t");
    tree->Branch("phidg4_Bx",x4_field,"phidg4_Bx[num_phidg4_points]/Double_t");
    tree->Branch("phidg4_By",y4_field,"phidg4_By[num_phidg4_points]/Double_t");
    tree->Branch("phidg4_Bz",z4_field,"phidg4_Bz[num_phidg4_points]/Double_t");
    tree->Branch("phidg4_Btot",tot4_field,"phidg4_Btot[num_phidg4_points]/Double_t");
    tree->Branch("phidg4_tilt",tilt_phid4,"phidg4_tilt[num_phidg4_points]/Double_t");

    tree->Branch("int_temp",&int_temp,"int_temp/Double_t");
    tree->Branch("ext1_temp",&ext1_temp,"ext1_temp/Double_t");
    tree->Branch("ext2_temp",&ext2_temp,"ext2_temp/Double_t");
    tree->Branch("temperatures",temperatures,"temperatures[20]/Double_t");

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
    SampleWave0 = new TH1F("","",200, 300, 500);
    SampleWave0->SetDirectory(0);
    SampleWave1 = new TH1F("","",200,300,500);
    SampleWave1->SetDirectory(0);
    // 9.Nov.2017
    SampleWave2 = new TH1F("", "", 200, 300, 500);
    SampleWave2->SetDirectory(0);
    SampleWave3 = new TH1F("", "", 200, 300, 500);
    SampleWave3->SetDirectory(0);
    SampleWave4 = new TH1F("", "", 200, 300, 500);
    SampleWave4->SetDirectory(0);
    SampleWave5 = new TH1F("", "", 200, 300, 500);
    SampleWave5->SetDirectory(0);


    StartVal0 = new TH1F("","",200,300,500);
    StartVal0->SetDirectory(0);

    ngoodTDCbanks = 0;
    nbadTDCbanks = 0; 

  }

  void EndRun(int transition,int run,int time){
    //tree->Fill();
    std::cout << "Custom end run " << run <<std::endl;
    //tree->Write(); //: happens through in fOutputFile->Write() in base class (ProcessMidasFile)

    std::cout << "Good TDC banks: " << ngoodTDCbanks << std::endl;;
    std::cout << "Bad  TDC banks: " << nbadTDCbanks << std::endl;;
  }

  bool ProcessMidasEvent(TDataContainer& dataContainer){
    TGenericData *bank = dataContainer.GetEventData<TGenericData>("EOM");  // END OF MOVE = START of MEASUREMENT
    if(bank){
      std::cout << "end " << std::endl;
      gbl_accept_banks = TRUE;
      return true;
    }
    bank = dataContainer.GetEventData<TGenericData>("BONM");               // BEGINNING OF NEXT MOVE = END of MEASUREMENT
    if(bank){
	  timestamp=dataContainer.GetMidasData().GetTimeStamp();//this is where we fill the tree for the time
      std::cout << "end of move" << std::endl;
      tree->Fill();
      counter = 0;
      num_points = 0;
      num_points_dig0 = 0;
      num_points_dig1 = 0;
      num_phidg0_points = 0;
      num_phidg1_points = 0;
      num_phidg3_points = 0;
      num_phidg4_points = 0;
      gbl_accept_banks = FALSE;
      return true;
    }
    if(gbl_accept_banks){
      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      //attempt at adding digitizer data
      //Set input parameters which correspond to ADC measurements
      start_val_stat = 90;
      window_width = 45;
      trigger = 40;

      // Try getting V1730 bank
      // Uncomment channels to get respective ROOT output from the file
      TV1730RawData *v1730_b = dataContainer.GetEventData<TV1730RawData>("V730");
      //printf("Decoded V1730 data\n");

      if(v1730_b){      

        // John 2019-11-05 Don't save unnecessary data
        // First scan point has been saving too many events
        if(num_points<nPoints_max){

          num_points++;
          
          
          //std::cout << "point# "<< num_points << std::endl;
          
          std::vector<RawChannelMeasurement> measurements = v1730_b->GetMeasurements();
          
          for(int i = 0; i < measurements.size(); i++){
            
            int chan = measurements[i].GetChannel();
            //std::cout << "chan " << chan << std::endl;
            if(chan < 0 || chan >5) continue; // 9.Nov.2017 updated for first 5 channels only
            //		  std::cout << "chan " << chan << std::endl;
            
            // saves num_v1730_max (currently 70) bins around each pulse
            // laser pulse is given an offset as it occurs earlier in time
            // Note: 1 ib  = 2 ns
            // This is what writes Midas data to the ROOT tree
            
            // Channel    Output
            // 0          Primary PMT wave
            // 1          Reference Wave
            // 2          Gantry0 receiver PMT
            // 3          Gantry0 monitor PMT
            // 4          Gantry1 receiver PMT
            // 5          Gantry1 monitor PMT
            
            for(int ib = timeStart; ib < measurements[i].GetNSamples() && ib  < num_v1730_max + timeStart ; ib++){
              
              //std::cout<<"ib = " <<ib <<std::endl;
              
              if(chan == 0) V1730_wave0[num_points-1][ib-timeStart] = measurements[i].GetSample(ib);  
              if(chan == 1) V1730_wave1[num_points-1][ib-timeStart] = measurements[i].GetSample(ib-timeStart); // -130 is the timeStart offset, meaning the reference signal window starts at the start of the digitizer readout
              // if(chan == 2) V1730_wave2[num_points-1][ib-timeStart] = measurements[i].GetSample(ib);
              if(chan == 3) V1730_wave3[num_points-1][ib-timeStart] = measurements[i].GetSample(ib-60); 
              if(chan == 5) V1730_wave5[num_points-1][ib-timeStart] = measurements[i].GetSample(ib-60); 
            }		             	      
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
        tilt_phid1[num_phidg1_points -1] = ((double*)bank_ph1->GetData64())[7];

        return true;
      }

      TGenericData *bank_ph3 = dataContainer.GetEventData<TGenericData>("PH03");
      if(bank_ph3){
        num_phidg3_points++;

        acc_x3[num_phidg3_points -1] = ((double*)bank_ph3->GetData64())[0];
        acc_y3[num_phidg3_points -1] = ((double*)bank_ph3->GetData64())[1];
        acc_z3[num_phidg3_points -1] = ((double*)bank_ph3->GetData64())[2];
        x3_field[num_phidg3_points -1] = ((double*)bank_ph3->GetData64())[3];
        y3_field[num_phidg3_points -1] = ((double*)bank_ph3->GetData64())[4];
        z3_field[num_phidg3_points -1] = ((double*)bank_ph3->GetData64())[5];
        tot3_field[num_phidg3_points -1] = ((double*)bank_ph3->GetData64())[6];    
        tilt_phid3[num_phidg3_points -1] = ((double*)bank_ph3->GetData64())[7];

        return true;
      }

      TGenericData *bank_ph4 = dataContainer.GetEventData<TGenericData>("PH04");
      if(bank_ph4){
        num_phidg4_points++;

        acc_x4[num_phidg4_points -1] = ((double*)bank_ph4->GetData64())[0];
        acc_y4[num_phidg4_points -1] = ((double*)bank_ph4->GetData64())[1];
        acc_z4[num_phidg4_points -1] = ((double*)bank_ph4->GetData64())[2];
        x4_field[num_phidg4_points -1] = ((double*)bank_ph4->GetData64())[3];
        y4_field[num_phidg4_points -1] = ((double*)bank_ph4->GetData64())[4];
        z4_field[num_phidg4_points -1] = ((double*)bank_ph4->GetData64())[5];
        tot4_field[num_phidg4_points -1] = ((double*)bank_ph4->GetData64())[6];    
        tilt_phid4[num_phidg4_points -1] = ((double*)bank_ph4->GetData64())[7];

        return true;
      }

      // Check for environment temperature monitoring data
      TGenericData *envt = dataContainer.GetEventData<TGenericData>("ENVT");
      if(envt){
	  int_temp = ((float*)envt->GetData64())[0];
	  ext1_temp = ((float*)envt->GetData64())[1];
	  ext2_temp = ((float*)envt->GetData64())[2];

	  temperatures[0] = ((float*)envt->GetData64())[0];
          temperatures[1] = ((float*)envt->GetData64())[1];
          temperatures[2] = ((float*)envt->GetData64())[2];
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

