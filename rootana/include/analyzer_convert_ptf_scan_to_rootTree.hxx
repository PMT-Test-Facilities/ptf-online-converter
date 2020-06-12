#ifndef analyzer_convert_ptf_scan_to_rootTree_H
#define analyzer_convert_ptf_scan_to_rootTree_H
  
#include <stdio.h>
#include <iostream>

#include "TRootanaEventLoop.hxx"
#include "TH1D.h"
#include "TV1190Data.hxx"
#include "TV792Data.hxx"

#include <TTree.h>
#include <TFile.h>

#include "TDataContainer.hxx"

#include "TH1F.h"
#include "TF1.h"

#define TRUE 1
#define FALSE 0

#define nPoints_max 10000000
#define num_phidg_max 1000

// Offset for the ADC channel number
#define Ch_Offset 1

// Flag to indicate the gantry was not moving and to record ADC and Phidget values. 
int gbl_accept_banks = FALSE;

class ScanToTreeConverter: public TRootanaEventLoop {

  int nnn;

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
/*  
using namespace 

class analyzer_convert_ptf_scan_to_rootTree {

	public:
	analyzer_convert_ptf_scan_to_rootTree();
	virtual ~analyzer_convert_ptf_scan_to_rootTree();
	void BeginRun(int transition,int run,int time);
	void EndRun(int transition,int run,int time);
	bool ProcessMidasEvent(TDataContainer& dataContainer);
	void Usage(void);
	bool CheckOption(std::string option);

	TH1F *ChargeHist;
*/
};

#endif
