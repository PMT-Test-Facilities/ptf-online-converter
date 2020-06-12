// Simple program to print out the values from a magnetic field scan.


#include <stdio.h>
#include <iostream>

#include "TRootanaEventLoop.hxx"
#include "TH1D.h"
#include "TV1190Data.hxx"
#include "TV792Data.hxx"

#include <TTree.h>
#include <TFile.h>

#define TRUE 1
#define FALSE 0
#define nPoints_max 1000

// Flag to indicate the gantry was not moving and to record ADC and Phidget values. 
int gbl_accept_banks = FALSE;

class ScanToTreeConverter: public TRootanaEventLoop {
  
  int nnn;
  
private:
  int counter_gant;                                    //n-th measurement
  int subevent;
  double x0_pos, y0_pos, z0_pos, tilt0_pos, rot0_pos;  //positions of gantry0
  double x1_pos, y1_pos, z1_pos, tilt1_pos, rot1_pos;  //positions of gantry1
  int time;
  
  //TODO: add other positions (Phidget, laser, ...)




  int num_phidg0_points;
  double x0_field[100], y0_field[100], z0_field[100], tot0_field[100];     //B-field from phidget0
  int num_phidg1_points;
  double x1_field[100], y1_field[100], z1_field[100], tot1_field[100];     //B-field from phidget1
  double tilt_phid0[100], tilt_phid1[100];                       //tilt position from phidget
  double acc_x0[100], acc_y0[100], acc_z0[100];
  double acc_x1[100], acc_y1[100], acc_z1[100];

  
  //current and voltage of each of the 6 Helmholtz coils
  int counter_mag;
  double curr_coil1, curr_coil2, curr_coil3, curr_coil4, curr_coil5, curr_coil6;
  double volt_coil1, volt_coil2, volt_coil3, volt_coil4, volt_coil5, volt_coil6;
  
  //PMT currents and voltages
  double curr_hpd_enable, curr_hpd_hv_control, curr_hpd_lv_control;
  double curr_receiver0, curr_receiver1, curr_monitor0, curr_monitor1;

  double volt_hpd_enable, volt_hpd_hv_control, volt_hpd_lv_control;
  double volt_receiver0, volt_receiver1, volt_monitor0, volt_monitor1;


  //PMT readout
  int num_points;
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
    tree = new TTree("scan_tree","Scan Tree");
    tree->Branch("num_points",&num_points,"num_points/Int_t");
    tree->Branch("ADC0_voltage",ADC0_voltage,"ADC0_voltage[num_points]/Int_t");
    tree->Branch("ADC1_voltage",ADC1_voltage,"ADC1_voltage[num_points]/Int_t");    
    tree->Branch("ADC2_voltage",ADC2_voltage,"ADC2_voltage[num_points]/Int_t");
    tree->Branch("ADC3_voltage",ADC3_voltage,"ADC3_voltage[num_points]/Int_t");    
    tree->Branch("ADC4_voltage",ADC4_voltage,"ADC4_voltage[num_points]/Int_t");          
    tree->Branch("time",&time,"time/Int_t");
    tree->Branch("gantry_event",&counter_gant,"gantry_event/Int_t"); //an event is a measurement at a point
    tree->Branch("gantry_subevent",subevent,"gantry_subevent/Int_t");
    tree->Branch("gantry0_x",&x0_pos,"gantry0_x/Double_t");
    tree->Branch("gantry0_y",&y0_pos,"gantry0_y/Double_t");
    tree->Branch("gantry0_z",&z0_pos,"gantry0_z/Double_t");
    tree->Branch("gantry0_tilt",&tilt0_pos,"gantry0_tilt/Double_t");
    tree->Branch("gantry0_rot",&rot0_pos,"gantry0_rot/Double_t");
    tree->Branch("phidg0_tilt",&tilt_phid0,"phidg0_tilt/Double_t");
      
    tree->Branch("gantry1_x",&x1_pos,"gantry1_x/Double_t");
    tree->Branch("gantry1_y",&y1_pos,"gantry1_y/Double_t");
    tree->Branch("gantry1_z",&z1_pos,"gantry1_z/Double_t");
    tree->Branch("gantry1_tilt",&tilt1_pos,"gantry1_tilt/Double_t");
    tree->Branch("gantry1_rot",&rot1_pos,"gantry1_rot/Double_t");
    tree->Branch("phidg1_tilt",&tilt_phid1,"phidg1_tilt/Double_t");
    
    //field-related phidget measurements
    tree->Branch("num_phidg0_points",&num_phidg0_points,"num_phidg0_points/Int_t");
    tree->Branch("phidg0_Ax",acc_x0,"phidg0_Ax[num_phidg0_points]/Double_t");
    tree->Branch("phidg0_Ay",acc_y0,"phidg0_Ay[num_phidg0_points]/Double_t");
    tree->Branch("phidg0_Az",acc_z0,"phidg0_Az[num_phidg0_points]/Double_t");
    tree->Branch("phidg0_Bx",x0_field,"phidg0_Bx[num_phidg0_points]/Double_t");
    tree->Branch("phidg0_By",y0_field,"phidg0_By[num_phidg0_points]/Double_t");
    tree->Branch("phidg0_Bz",z0_field,"phidg0_Bz[num_phidg0_points]/Double_t");
    tree->Branch("phidg0_Btot",tot0_field,"phidg0_Btot[num_phidg0_points]/Double_t");
    
    tree->Branch("num_phidg1_points",&num_phidg1_points,"num_phidg1_points/Int_t");
    tree->Branch("phidg1_Ax",acc_x1,"phidg1_Ax[num_phidg0_points]/Double_t");
    tree->Branch("phidg1_Ay",acc_y1,"phidg1_Ay[num_phidg0_points]/Double_t");
    tree->Branch("phidg1_Az",acc_z1,"phidg1_Az[num_phidg0_points]/Double_t");
    tree->Branch("phidg1_Bx",x1_field,"phidg1_Bx[num_phidg1_points]/Double_t");
    tree->Branch("phidg1_By",y1_field,"phidg1_By[num_phidg1_points]/Double_t");
    tree->Branch("phidg1_Bz",z1_field,"phidg1_Bz[num_phidg1_points]/Double_t");
    tree->Branch("phidg1_Btot",tot1_field,"phidg1_Btot[num_phidg1_points]/Double_t");
    
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

  }
  
  void EndRun(int transition,int run,int time){
    std::cout << "Custom end run " << run <<std::endl;
    //tree->Write(): happens through in fOutputFile->Write() in base class (ProcessMidasFile)
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
      gbl_accept_banks = FALSE;
      return true;
    }
    if(gbl_accept_banks){
      
      // Try gettting an ADC bank
      TV792Data *bank = dataContainer.GetEventData<TV792Data>("ADC0");
      if(bank){
	num_points++;
	
	/// Get the Vector of ADC Measurements.
	std::vector<VADCMeasurement> measurements = bank->GetMeasurements();
	for(unsigned int i = 0; i < measurements.size(); i++){ // loop over measurements
	  int chan = measurements[i].GetChannel();
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
	tilt0_pos = ((double*)bank_cyc->GetData64())[4];
	rot0_pos = ((double*)bank_cyc->GetData64())[5];
	
	x1_pos = ((double*)bank_cyc->GetData64())[6];
	y1_pos = ((double*)bank_cyc->GetData64())[7];
	z1_pos = ((double*)bank_cyc->GetData64())[8];
	tilt1_pos = ((double*)bank_cyc->GetData64())[9];
	rot1_pos = ((double*)bank_cyc->GetData64())[10];
	
	time = (int)((double*)bank_cyc->GetData64())[11];
	
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
      //in loop over events
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

