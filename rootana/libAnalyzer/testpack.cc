#include <TTree.h>
#include <TFile.h>
#include <TH1D.h>
#include <TF1.h>
#include <TGraph2D.h>
#include <TH2D.h>

void testpack() {
  int run = 1383;
  TFile f1(Form("Magnetic_Field_Scan_%4i",run), "recreate");
  TFile *f = new TFile("output00001383.root");
  TTree *Histo = (TTree*)f->Get("scan_tree");

  double gant0_x;
  double gant0_y;
  double gant0_z;
  double ph0_x[10];
  double ph0_y[10];
  double ph0_z[10];
  double ph0_tot[10];

  double gant1_x;
  double gant1_y;
  double gant1_z;
  double ph1_x[10];
  double ph1_y[10];
  double ph1_z[10];  
  double ph1_tot[10];


  Histo->SetBranchAddress("gantry0_x",&gant0_x);
  Histo->SetBranchAddress("gantry0_y",&gant0_y);
  Histo->SetBranchAddress("gantry0_z",&gant0_z);
  Histo->SetBranchAddress("phidg0_Bx",&ph0_x);
  Histo->SetBranchAddress("phidg0_By",ph0_y);
  Histo->SetBranchAddress("phidg0_Bz",&ph0_z);
  Histo->SetBranchAddress("phidg0_Btot",&ph0_tot);

  Histo->SetBranchAddress("gantry1_x",&gant1_x);
  Histo->SetBranchAddress("gantry1_y",&gant1_y);
  Histo->SetBranchAddress("gantry1_z",&gant1_z);
  Histo->SetBranchAddress("phidg1_Bx",ph1_x);
  Histo->SetBranchAddress("phidg1_By",ph1_y);
  Histo->SetBranchAddress("phidg1_Bz",ph1_z);
  Histo->SetBranchAddress("phidg1_Btot",ph1_tot);


  int step = Histo->GetEntries();
  for(int j=0; j<11; j++){
    h5 = new TH2D("gant1_Bx","",16,0.02,0.66,16,0.02,0.66);
    h6 = new TH2D("gant1_By","",16,0.02,0.66,16,0.02,0.66);
    h7 = new TH2D("gant1_Bz","",16,0.02,0.66,16,0.02,0.66);
    h8 = new TH2D("gant1_Btot","",16,0.02,0.66,16,0.02,0.66);
    double k = j*0.05;
    for(int i=1; i<step; i++){
      Histo->GetEntry(i);
      if(gant1_z > k - 0.025 && gant1_z < k + 0.025){ 
        for(int l=1; l < 3; l++){
        if(ph1_y[l] != 0 && ph1_x[l] != 0 && ph1_z[l] != 0){
	h5->Fill(gant1_x,gant1_y,ph1_x[l]);
        h6->Fill(gant1_x,gant1_y,ph1_y[l]);
        h7->Fill(gant1_x,gant1_y,ph1_z[l]);
        h8->Fill(gant1_x,gant1_y,ph1_tot[l]);
	}
	}
      }
    }
    f1.cd();
    h5->Write();
    h6->Write();
    h7->Write();
    h8->Write();
    f->cd();
  delete h5;
  delete h6;
  delete h7;
  delete h8;
  }

  for(int j=0; j<11; j++){
    h1 = new TH2D("gant0_Bx","",16,0.02,0.66,16,0.02,0.66);
    h2 = new TH2D("gant0_By","",16,0.02,0.66,16,0.02,0.66);
    h3 = new TH2D("gant0_Bz","",16,0.02,0.66,16,0.02,0.66);
    h4 = new TH2D("gant0_Btot","",16,0.02,0.66,16,0.02,0.66);
    double k = j*0.05;
    for(int i=1; i<step; i++){
      Histo->GetEntry(i);
      if(gant0_z > k - 0.025 && gant0_z < k + 0.025){
        for(int l=1; l < 3; l++){
        if(ph0_y[l] != 0 && ph0_x[l] != 0 && ph0_z[l] != 0){
        h1->Fill(gant0_x,gant0_y,ph0_x[l]);
        h2->Fill(gant0_x,gant0_y,ph0_y[l]);
        h3->Fill(gant0_x,gant0_y,ph0_z[l]);
        h4->Fill(gant0_x,gant0_y,ph0_tot[l]);
        }
        }
      }
    }
    f1.cd();
    h1->Write();
    h2->Write();
    h3->Write();
    h4->Write();
    f->cd();
  delete h1;
  delete h2;
  delete h3;
  delete h4;
  }

}
