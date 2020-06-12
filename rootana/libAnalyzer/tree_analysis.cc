#include <TTree.h>
#include <TFile.h>
#include <TH1D.h>
#include <TF1.h>
#include <TGraph2D.h>
#include <TH2D.h>

void tree_analysis(){
  //run number
  int run = 1127;
  
  // Files to read and write
  TFile fi(Form("histograms_%08i.root",run),"RECREATE");
  TFile *f = new TFile(Form("output%08i.root",run));
  TTree *Histo = (TTree*)f->Get("scan_tree");

  //Define variables
  int ADC4_voltage[100000];
  double gant_x;
  double gant_y;
  int nument;
  int num_points;
  int max = 0;
  double ratio[10000];
  double mean;
  double int_ped;
  double int_pe;
  double err; 
  double err1;

  //Define branches
  Histo->SetBranchAddress("ADC4_voltage",ADC4_voltage);
  Histo->SetBranchAddress("num_points",&num_points);
  Histo->SetBranchAddress("gantry1_x",&gant_x);
  Histo->SetBranchAddress("gantry1_y",&gant_y);

  int step = Histo->GetEntries();
  for(int i=1; i<step; i++){
    TH1D *h = new TH1D("h","dist",200,0,200);
    double mean = 0;
    Histo->GetEntry(i);
    //std::cout << "Event #:  " << i << std::endl;
    for(int j=0; j<12000; j++){
      if(ADC4_voltage[j] > 0){
        h->Fill(ADC4_voltage[j]);
      }
    }
    h->Fit("gaus","QR","",80,90); //Pedestal gaussian fit
    TF1 *fit = h->GetFunction("gaus");
    h->Add(fit, -1);
    int_ped = gaus->GetParameter(0);
    
    h->Fit("gaus","QR+","",89,96); //PE gaussian fit
    int_pe = gaus->GetParameter(0);
    mean = gaus->GetParameter(1);
    err = gaus->GetParError(0);
    err1 = gaus->GetParError(1);
    //Deal with aberrant fits -- assume errant if pe > pedastal
    if(mean > 94){
      h->Fit("gaus","QR+","",91,99);
      int_pe = gaus->GetParameter(0);
      mean = gaus->GetParameter(1);   
    }
    if(mean < 88 || mean > 97 || int_pe < 1 || err > 100){
      int_pe = 1;
    }
    ratio[i] = int_pe/int_ped;
    //std::cout << "Error  " << err1 << std::endl;
    if(gant_x > 0.252 && gant_x < 0.257 && gant_y > 0.302 && gant_y < 0.307){
 //std::cout << gant_x << "  " << gant_y << "   " << err1 << std::endl;
  //  h->Fit("gaus","R+","",89,96); //PE gaussian fit
   // int_pe = gaus->GetParameter(0);
    //mean = gaus->GetParameter(1);

      fi.cd();
      h->Write();
      f->cd();
    }
    delete h;
  }
  int nbins = 113;//121;//16; //11 (for 0.03)
  double x_min = 0.100-0.0025;//-0.0025;//-0.01; //-0.015
  double x_max = 0.660+0.0025;//0.6625;//0.31;  // 0.315
  double y_min = 0.100-0.0025;//-0.0025;//0.13;  // 0.135
  double y_max = 0.660+0.0025;//0.6025;//0.46;  // 0.465

  TH2D *plot = new TH2D("g1","",nbins,x_min,x_max,nbins,x_min,x_max);
  TH2D *norm = new TH2D("g","",nbins,x_min,x_max,nbins,x_min,x_max);
//  Histo->Draw("gantry1_x[0]:gantry1_y[0]>>norm","num_points/num_points","goff");
  //Histo->Draw("gantry1_x[0]:gantry1_y[0]>>plot","ADC4_voltage","goff");
  for(int k=1; k<step; k++){
      Histo->GetEntry(k);
      
      //Histo->Draw("gant_x:gant_y>>plot","","goff");
    norm->Fill(gant_x, gant_y, 1);
    plot->Fill(gant_x, gant_y, ratio[k]);
  }
  plot->Divide(norm);
  gStyle->SetMarkerStyle(20);
  plot->Draw("colz");
} 
