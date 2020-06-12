#include <TTree.h>
#include <TFile.h>
#include <TH1D.h>
#include <TF1.h>
#include <TGraph2D.h>
#include <TH2D.h>


void centralpos_histo(){
  // Purpose of macro is to allow user to test gaussian fits on waveform - a finicky issue that requires a lot of optimization. If there are certain positions in CircleFit which are producing poor fits 
  // can isolate gantry position below and test different gaussian fit constraints. When all fits are good, prints out useful histograms of pe and pedestal statistics.
  //Files to read and write
  int run = 1466;
  TFile fi(Form("centralposition_histos_%08i.root",run),"RECREATE");
  TFile *f = new TFile(Form("output%08i.root",run));
  TTree *Histo = (TTree*)f->Get("scan_tree");

  //Define variables
  int ADC0_voltage[100000];
  int ADC1_voltage[100000];
  double gant_x;
  double gant_y;
  double ratio;
  double mean_ped;
  double mean_pe;
  double int_ped;
  double int_pe;
  double sigma_ped;
  double sigma_pe;
  double ratio_mon;
  double mean_ped_mon;
  double int_ped_mon;
  double int_pe_mon;
  double sigma_ped_mon;
  int event;
  double delta_mean;
  double error;
  double average;
  double num;

  //Define gaussian values - might need to play with these a bit until fits are good
  int mon_pedmin = 155;
  int mon_pedmax = 170;
  int mon_pemin = 165;
  int mon_pemax = 175;
  int det_pedmin = 165;
  int det_pedmax = 173;
  int det_pemin = 178;
  int det_pemax = 210;

  //Define branches
  Histo->SetBranchAddress("ADC1_voltage",ADC1_voltage);
  Histo->SetBranchAddress("ADC0_voltage",ADC0_voltage);
  Histo->SetBranchAddress("gantry1_x",&gant_x);
  Histo->SetBranchAddress("gantry1_y",&gant_y);
  Histo->SetBranchAddress("gantry_event",&event);

  //Define number of events
  int step = Histo->GetEntries();

  //Generate Histograms
  TH1D *ratio_val = new TH1D("int","Normalized First PE Intensity",200,0,100);
  TH1D *ped_mean = new TH1D("ped_mean","Mean Value of Pedestal",100,165,170);
  TH1D *pe_mean = new TH1D("pe_mean","Mean Value of First PE",100,0,50);
  TH1D *ped_sigma = new TH1D("ped_sigma","Sigma Value of Pedestal",20,0.8,1.4);
  TH1D *pe_sigma = new TH1D("pe_sigma","Sigma Value of First PE",20,0,1.5);
  TH1D *ratio_val_mon = new TH1D("int_mon","Normalized First PE Monitor Intensity",100,0,2); 
  TH2D *intensity_monitor = new TH2D("intensity_comparison","Intensity of Detector vs. Monitor PMT",450,0,150,100,0,20);
  TH2D *drift = new TH2D("drift","Mean Pedestal Drift over Time",30,166,169,5000,0,5000);
  TH2D *drift2 = new TH2D("drift2","Mean Pedestal Drift over Gant X",30,166,169,100,0,1);
  TH2D *drift3 = new TH2D("drift3","Mean Pedestal Drift over Gant Y",30,166,169,100,0,1);
  TH2D *drift4 = new TH2D("drift4","Average val drift over Time",105,165,180,5000,0,5000);
  TH2D *drift5 = new TH2D("drift5","Average val Drift over Gant X",105,165,180,100,0,0.7);
  TH2D *drift6 = new TH2D("drift6","Average val Drift over Gant Y",105,165,180,100,0,0.7);
  TH2D *error_val = new TH2D("mean error", "Error in mean vs. Mean",100,10,30,100,0,30);
  TH1D *avg = new TH1D("average","Average value",200,150,200);

TH1D *h = new TH1D("h","dist",250,0,250);
  //Detector PMT
  for(int i=1; i<step; i++){
    TH1D *h = new TH1D("h","dist",200,0,200); //1D Histogram to store event i detector PMT data
    TH1D *hm = new TH1D("hm","dist",200,0,200); //1D Histogram to store event i moitor PMT data
    Histo->GetEntry(i);
    average = 0;
    num = 0;
//    if(gant_x > 0.299 && gant_x < 0.301 && gant_y > 0.569 && gant_y < 0.571){ // When troubleshooting finicky point: Isolate point of interest by specifying gantry location
      for(int j=0; j<12000; j++){
        if(ADC0_voltage[j] > 0){
	  average += ADC0_voltage[j];
	  num += 1;
	  h->Fill(ADC0_voltage[j]);
	  hm->Fill(ADC1_voltage[j]);
	}
      }
      h->Fit("gaus", "QR", "",det_pedmin,det_pedmax);  //Detector pedestal gaussian fit
      TF1 *fit = h->GetFunction("gaus");
      h->Add(fit, -1);
      int_ped = gaus->GetParameter(0);
      mean_ped = gaus->GetParameter(1);
      sigma_ped = gaus->GetParameter(2);

      hm->Fit("gaus", "QR", "",mon_pedmin,mon_pedmax); //Monitor pedestal gaussian fit
      TF1 *fit_mon = hm->GetFunction("gaus");
      hm->Add(fit_mon, -1);
      int_ped_mon = gaus->GetParameter(0);
      mean_ped_mon = gaus->GetParameter(1);
      sigma_ped_mon = gaus->GetParameter(2);

      h->Fit("gaus","QR+","",det_pemin,det_pemax); //Detector PE gaussian fit
      int_pe = gaus->GetParameter(0);
      mean_pe = gaus->GetParameter(1);
      sigma_pe = gaus->GetParameter(2);
      error = gaus->GetParError(1);
      ratio = int_ped/int_pe;

      hm->Fit("gaus", "QR+", "",mon_pemin,mon_pemax); //Monitor PE gaussian fit
      int_pe_mon = gaus->GetParameter(0);
      ratio_mon = int_ped_mon/int_pe_mon;

      //Fixing bad fits
      if(mean_pe < 180){ //PE gaussian mean too large
        h->Fit("gaus","QR+","",175,183);
	int_pe = gaus->GetParameter(0);
        mean_pe = gaus->GetParameter(1);
        sigma_pe = gaus->GetParameter(2);
        ratio = int_ped/int_pe;
      }

      average = average/num;
      avg->Fill(average);
      delta_mean = mean_pe-mean_ped;
      drift->Fill(mean_ped,event);
      drift2->Fill(mean_ped,gant_x);
      drift3->Fill(mean_ped,gant_y);
      drift4->Fill(average,event);
      drift5->Fill(average,gant_x);
      drift6->Fill(average,gant_y);
      intensity_monitor->Fill(ratio, ratio_mon);
      ratio_val->Fill(ratio);
      ratio_val_mon->Fill(ratio_mon);
      ped_mean->Fill(mean_ped);
      ped_sigma->Fill(sigma_ped);
      pe_mean->Fill(delta_mean);
      pe_sigma->Fill(sigma_pe/delta_mean);
      error_val->Fill(delta_mean, error);      
      //Write out histograms
      fi.cd();
      h->Draw();
      hm->Write();
      f->cd();
 //std::cout <<  "RATIO " << 1/ratio << std::endl;
 //std::cout << "MEAN pe " << mean_pe << std::endl;
    
    // Reset histograms for next event
    delete h;
    delete hm;
  }
  
  //Setting titles
  ratio_val->SetXTitle("Normalized PE intensity [gaus(int_pedestal)/gaus(int_PE)]");
  ratio_val->GetXaxis()->CenterTitle();
  ratio_val_mon->SetXTitle("Normalized PE Monitor intensity [gaus_mon(int_pedestal)/gaus_mon(int_PE)]");
  ratio_val_mon->GetXaxis()->CenterTitle();
  ped_mean->SetXTitle("Pedestal Mean [gaus(mean_pedestal)]");
  ped_mean->GetXaxis()->CenterTitle();
  ped_sigma->SetXTitle("Pedestal Width [gaus(sigma_pedestal)]");
  ped_sigma->GetXaxis()->CenterTitle();
  pe_mean->SetXTitle("Pedestal subtracted PE Mean [gaus(mean_PE)-gaus(mean_pedestal)]");
  pe_mean->GetXaxis()->CenterTitle();
  pe_sigma->SetXTitle("Delta divided PE Width [gaus(sigma_PE)/(gaus(mean_PE)-gaus(mean_pedestal))]");
  pe_sigma->GetXaxis()->CenterTitle();
  drift->SetXTitle("Pedestal Mean [gaus(mean_pedestal)]");
  drift->GetXaxis()->CenterTitle();
  drift->SetYTitle("Event number (time) [event]");
  drift->GetYaxis()->CenterTitle();
  drift2->SetXTitle("Pedestal Mean [gaus(mean_pedestal)]");
  drift2->GetXaxis()->CenterTitle();
  drift2->SetYTitle("Gant X (m)");
  drift2->GetYaxis()->CenterTitle();
  drift3->SetXTitle("Pedestal Mean [gaus(mean_pedestal)]");
  drift3->GetXaxis()->CenterTitle();
  drift3->SetYTitle("Gant Y (m)");
  drift3->GetYaxis()->CenterTitle();
  drift4->SetXTitle("Average value");
  drift4->GetXaxis()->CenterTitle();
  drift4->SetYTitle("Event number (time) [event]");
  drift4->GetYaxis()->CenterTitle();
  drift5->SetXTitle("Average value");
  drift5->GetXaxis()->CenterTitle();
  drift5->SetYTitle("Gant X (m)");
  drift5->GetYaxis()->CenterTitle();
  drift6->SetXTitle("Average value");
  drift6->GetXaxis()->CenterTitle();
  drift6->SetYTitle("Gant Y (m)");
  drift6->GetYaxis()->CenterTitle();
  intensity_monitor->SetXTitle("Normalized PE intensity [gaus(int_pedestal)/gaus(int_PE)]");
  intensity_monitor->GetXaxis()->CenterTitle();
  intensity_monitor->SetYTitle("Normalized Monitor PE intensity [gaus_mon(int_pedestal)/gaus_mon(int_PE)]");
  intensity_monitor->GetYaxis()->CenterTitle();
  error_val->SetXTitle("Pedestal subtracted PE Mean [gaus(mean_PE)-gaus(mean_pedestal)]");
  error_val->GetXaxis()->CenterTitle();
  error_val->SetYTitle("Chi2 of PE Mean [gaus(chi2)]");
  error_val->GetYaxis()->CenterTitle();  


  //Writing histograms to root file
  fi.cd();
  ratio_val->Write();
  ratio_val_mon->Write();
  ped_mean->Write();
  pe_mean->Write();
  ped_sigma->Write();
  pe_sigma->Write();
  intensity_monitor->Write();
  drift->Write();
  drift2->Write();
  drift3->Write();
  drift4->Write();
  drift5->Write();
  drift6->Write();
  error_val->Write();
  avg->Write();
}
