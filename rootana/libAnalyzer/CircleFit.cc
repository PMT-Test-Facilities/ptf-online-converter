// Fast way from ROOT tutorial for fitting a circle...
// Kind of quick 'n dirty though...

#include <TTree.h>
#include <TFile.h>
#include <TH1D.h>
#include <TF1.h>
#include <TGraph2D.h>
#include <TH2D.h>

// need global
TH2D *h2;
TH2D *h2_circ;



void myFitCircle(Int_t &, Double_t *, Double_t &f, Double_t *par, Int_t){
  //minimisation function computing the sum of squares of residuals
  // play with pe_thr and upper_limit to confine circle fit to intensity values between pe_thr and upper_limit 
  int nbins_x = h2->GetNbinsX();
  int nbins_y = h2->GetNbinsY();
  f = 0;
  double pe_thr = 0.007; //for some reason, for run838 0.4 didn't work..
  double upper_limit = 0.01;
  for (int i = 0;i<nbins_y;i++) {
    for (int j = 0;j<nbins_x;j++) {
      // TODO: try to define boundary better?
      // eg. More clever way: Only pick the first bin above threshold?
      if(h2->GetBinContent(i,j) < upper_limit && h2->GetBinContent(i,j) > pe_thr){
	//std::cout << i << " " << j << " " << h2->GetBinContent(i,j) << std::endl;

	h2_circ->Fill(h2->GetXaxis()->GetBinCenter(i),
		      h2->GetYaxis()->GetBinCenter(j),1);

	double u = h2->GetXaxis()->GetBinCenter(i) - par[0];
	double v = h2->GetYaxis()->GetBinCenter(j) - par[1];
	double dr = par[2] - TMath::Sqrt(u*u+v*v);
	//std::cout << i << " " << j << " " << dr*dr << std::endl;
	f += dr*dr;
      }
    }
  }
}




void CircleFit(){
  int run = 1466;//986;//987;//982;//986;//982;//838;//730;
  // rough calculations, improve later on
  int ch1_pedestal = 173;
  double ch1_adc_per_pe = 9.7;
  int ch0_pedestal = 161;//172;//166.5;//159; //roughly, but changing
  double ch0_adc_per_pe = 7;//18;//8; //roughly
  int ch4_pedestal = 87;//178;//167;
  double ch4_adc_per_pe = 7;//8;//22;//28;//40.;

  int nbinsx = 65;
  int nbinsy = 65;//113;//121;//16; //11 (for 0.03)
  double x_min = 0.000-0.0025;//-0.0025;//-0.01; //-0.015
  double x_max = 0.640+0.0025;//0.6625;//0.31;  // 0.315
  double y_min = 0.000-0.0025;//-0.0025;//0.13;  // 0.135
  double y_max = 0.640+0.0025;//0.6025;//0.46;  // 0.465

  h2 = new TH2D("g1","",nbinsx,x_min,x_max,nbinsy,x_min,x_max);
  TH2D *h = new TH2D("g","",nbinsx,x_min,x_max,nbinsy,x_min,x_max);
  h2_circ = new TH2D("h2_circ","",nbinsx,x_min,x_max,nbinsy,y_min,y_max);

  //  TFile *file = new TFile(Form("out_run%05i.root",run));
  
  TFile fi(Form("histograms_%08i.root",run),"RECREATE");
  TFile *f = new TFile(Form("output%08i.root",run));
  TTree *Histo = (TTree*)f->Get("scan_tree");
  TFile f1("errant_fits.root","RECREATE");


  //define variables
  int ADC0_voltage[100000];
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
  double mean_ped;

  Histo->SetBranchAddress("ADC0_voltage",ADC0_voltage);
  Histo->SetBranchAddress("num_points",&num_points);
  Histo->SetBranchAddress("gantry1_x",&gant_x);
  Histo->SetBranchAddress("gantry1_y",&gant_y);

  int step = Histo->GetEntries();
  // loop over every entry - usually an individual sampling point
  for(int i=1; i<step; i++){
    TH1D *hh = new TH1D("hh","dist",250,0,250);
    double mean = 0;
    Histo->GetEntry(i);
    // loop over all waveforms within single entry 
    for(int j=0; j<12000; j++){
      if(ADC0_voltage[j] > 0){
        hh->Fill(ADC0_voltage[j]);
      }
    }
    hh->Fit("gaus","QR","",165,173); //Pedestal gaussian fit
    TF1 *fit = hh->GetFunction("gaus");
    hh->Add(fit, -1);
    int_ped = gaus->GetParameter(0);
    mean_ped = gaus->GetParameter(1);

    hh->Fit("gaus","QR+","",178,210); //PE gaussian fit
    int_pe = gaus->GetParameter(0);
    mean = gaus->GetParameter(1);
    err = gaus->GetParError(0);

    //Dealing with poor fits-------------------------------------------------------------------------------------------------------

    if(mean > 205){
      hh->Fit("gaus","QR+","",175,190);
      int_pe = gaus->GetParameter(0);
      mean = gaus->GetParameter(1);
      err = gaus->GetParError(0);
    } 
    if(mean < 170){
      hh->Fit("gaus","QR+","",175,190);
      int_pe = gaus->GetParameter(0);
      mean = gaus->GetParameter(1);
      err = gaus->GetParError(0);
    }

    if(sqrt((gant_x - 0.325)*(gant_x - 0.325)+(gant_y - 0.415)*(gant_y - 0.415)) < 0.24){
      if(mean < 170 || mean > 205){
	f1.cd();
	hh->Write();
	f->cd();
      }
    }	

    if(mean < 170 || mean > 205 || int_pe < 3 || err > 50){
      int_pe = 1;
      mean = mean_ped;
    }
    //------------------------------------------------------------------------------------------------------------------------------

    ratio[i] = int_pe/int_ped; // peak intensity plot
    //ratio[i] = (mean - mean_ped)/mean_ped; // mean intensity plot
    delete hh;
  } 

  //fill plots
  for(int k=1; k<step; k++){
    Histo->GetEntry(k);
    h->Fill(gant_x, gant_y, 1);
    h2->Fill(gant_x, gant_y, ratio[k]);
  }
  h2->Divide(h);
  gStyle->SetMarkerStyle(20);
  TCanvas *c = new TCanvas();
  h2->Draw("colz");

  //Fit a circle to the graph points
  TVirtualFitter::SetDefaultFitter("Minuit");  //default is Minuit
  TVirtualFitter *fitter = TVirtualFitter::Fitter(0, 3);
  fitter->SetFCN(myFitCircle);
  fitter->SetPrecision(1e-8);
  
  fitter->SetParameter(0, "x0",   0.32, 0.01, x_min,x_max);
  fitter->SetParameter(1, "y0",   0.4, 0.01, y_min,y_max);
  fitter->SetParameter(2, "R",    0.3, 0.05, 0,1);

  Double_t arglist[1] = {0};
  fitter->ExecuteCommand("MIGRAD", arglist, 0);

  // Note : tried HEsse and removing the one center point (run 838, but no
  // improvement in x_c, y_c error calc)
  
  
  //Draw the circle on top of the points
  TArc *arc = new TArc(fitter->GetParameter(0),
		       fitter->GetParameter(1),fitter->GetParameter(2));
   
  TMarker *center = new TMarker(fitter->GetParameter(0),
				fitter->GetParameter(1),29);
  center->SetMarkerSize(1.2);
  c->cd();
  h2_circ->SetMarkerStyle(7);
  h2_circ->Draw("same");
  arc->SetFillStyle(0);
  arc->SetFillColor(10);
  arc->SetLineColor(kRed);
  arc->SetLineWidth(2);
  arc->Draw("same");
  center->Draw("same");
  // including markers on Plots
  /*TMarker *m0 = new TMarker(0.497, 0.35, 25);
  TMarker *m1 = new TMarker(0.497, 0.39, 25);
  TMarker *m2 = new TMarker(0.477, 0.37, 25);   
  TMarker *m3 = new TMarker(0.517, 0.37, 25);
 
  m0->SetMarkerColor(kBlue);
  m1->SetMarkerColor(kBlue);
  m2->SetMarkerColor(kBlue);
  m3->SetMarkerColor(kBlue);

  m0->Draw("same"); 
  m1->Draw("same");
  m2->Draw("same");
  m3->Draw("same");

  TMarker *t0 = new TMarker(0.497, 0.34, 26);
  TMarker *t1 = new TMarker(0.497, 0.40, 26);
  TMarker *t2 = new TMarker(0.467, 0.37, 26);
  TMarker *t3 = new TMarker(0.527, 0.37, 26);

  t0->SetMarkerColor(kGreen);
  t1->SetMarkerColor(kGreen);
  t2->SetMarkerColor(kGreen);
  t3->SetMarkerColor(kGreen);

  t0->Draw("same");
  t1->Draw("same");
  t2->Draw("same");
  t3->Draw("same");

  TMarker *d0 = new TMarker(0.497, 0.43, 24);
  TMarker *d1 = new TMarker(0.497, 0.31, 24);

  d0->Draw("same");
  d1->Draw("same");
*/
/*
  TCanvas *c2 = new TCanvas();
  h3->Draw("colz");
  arc->Draw("same");
  center->Draw("same");
  m->Draw("same");

  TCanvas *c3 = new TCanvas();
  h4->GetZaxis()->SetRangeUser(0,3);
  h4->Draw("colz");
  arc->Draw("same");
  center->Draw("same");
*/
}
