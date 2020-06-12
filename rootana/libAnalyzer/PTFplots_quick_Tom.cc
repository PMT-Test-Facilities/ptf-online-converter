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
  int nbins_x = h2->GetNbinsX();
  int nbins_y = h2->GetNbinsY();
  f = 0;
  double pe_thr = 0.00005; //for some reason, for run838 0.4 didn't work..


  for (int i = 0;i<nbins_y;i++) {
    for (int j = 0;j<nbins_x;j++) {
      // TODO: try to define boundary better?
      // eg. More clever way: Only pick the first bin above threshold?
      if(h2->GetBinContent(i,j) < 0.0001 && h2->GetBinContent(i,j) > pe_thr){
	std::cout << i << " " << j << " " << h2->GetBinContent(i,j) << std::endl;

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




void PTFplots_quick_Tom(void){
  int run = 1127;//986;//987;//982;//986;//982;//838;//730;
  // rough calculations, improve later on
  int ch1_pedestal = 173;
  double ch1_adc_per_pe = 9.7;
  int ch0_pedestal = 161;//172;//166.5;//159; //roughly, but changing
  double ch0_adc_per_pe = 7;//18;//8; //roughly
  int ch4_pedestal = 87;//178;//167;
  double ch4_adc_per_pe = 7;//8;//22;//28;//40.;

  /*
  int nbins = 61;//16; //11 (for 0.03)
  double x_min = -0.0025;//-0.01; //-0.015
  double x_max = 0.3025;//0.31;  // 0.315
  double y_min = 0.1475;//0.13;  // 0.135
  double y_max = 0.4525;//0.46;  // 0.465
  */
  int nbins = 113;//121;//16; //11 (for 0.03)
  double x_min = 0.100-0.0025;//-0.0025;//-0.01; //-0.015
  double x_max = 0.660+0.0025;//0.6625;//0.31;  // 0.315
  double y_min = 0.100-0.0025;//-0.0025;//0.13;  // 0.135
  double y_max = 0.660+0.0025;//0.6025;//0.46;  // 0.465

  //  TFile *file = new TFile(Form("out_run%05i.root",run));
  
  TFile *file = new TFile(Form("output%08i.root",run));
  TTree *tree = (TTree *)file->Get("scan_tree");

  TH2D *h = new TH2D("h","",nbins,x_min,x_max,nbins,y_min,y_max);
  h2 = new TH2D("h2","",nbins,x_min,x_max,nbins,y_min,y_max);
  h2_circ = new TH2D("h2_circ","",nbins,x_min,x_max,nbins,y_min,y_max);
  TH2D *h3 = new TH2D("h3","",nbins,x_min,x_max,nbins,y_min,y_max);
  TH2D *h4 = new TH2D("h4","",nbins,x_min,x_max,nbins,y_min,y_max);
  
  tree->Draw("gantry1_y[0]:gantry1_x[0]>>h","num_points","goff");
  tree->Draw("gantry1_y[0]:gantry1_x[0]>>h2",Form("(ADC4_voltage-%i)/%f && ADC4_voltage>90",ch4_pedestal,ch4_adc_per_pe),"goff");
std::cout << "DRAWWWW" << std::endl;
  tree->Draw("gantry1_y[0]:gantry1_x[0]>>h3",Form("(ADC3_voltage-%i)/%f",ch1_pedestal,ch1_adc_per_pe),"goff");
  tree->Draw("gantry1_y[0]:gantry1_x[0]>>h4",Form("(ADC2_voltage-%i)/%f",ch0_pedestal,ch0_adc_per_pe),"goff");

  std::cout << "Try" << std::endl;
  h2->Divide(h);
  h3->Divide(h);
  h4->Divide(h);
  h3->GetZaxis()->SetRangeUser(20,30);
  TCanvas *c = new TCanvas();
  h2->Draw("colz");
/*
  //Fit a circle to the graph points
  TVirtualFitter::SetDefaultFitter("Minuit");  //default is Minuit
  TVirtualFitter *fitter = TVirtualFitter::Fitter(0, 3);
  fitter->SetFCN(myFitCircle);
  fitter->SetPrecision(1e-8);
  
  fitter->SetParameter(0, "x0",   0.15, 0.05, x_min,x_max);
  fitter->SetParameter(1, "y0",   0.2, 0.05, y_min,y_max);
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



  TCanvas *c2 = new TCanvas();
  h3->Draw("colz");
  arc->Draw("same");
  center->Draw("same");


  TCanvas *c3 = new TCanvas();
  h4->GetZaxis()->SetRangeUser(0,3);
  h4->Draw("colz");
  arc->Draw("same");
  center->Draw("same");
*/
}
