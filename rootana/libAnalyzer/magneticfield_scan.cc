#include <TTree.h>
#include <TFile.h>
#include <TH1D.h>
#include <TF1.h>
#include <TGraph2D.h>
#include <TH2D.h>

void magneticfield_scan() {
  int run = 1383;
  TFile f1(Form("Magnetic_Field_Scan_%4i.root",run), "recreate");
  TFile *f = new TFile("output00001383.root");
  TTree *Histo = (TTree*)f->Get("scan_tree");
  gROOT->SetStyle("Plain");
  gStyle->SetFrameBorderMode(0);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);
 
  TCanvas *canvas_Bx1_z1 = new TCanvas("Bx1_z1","Plotting Canvas",150,10,990,660);
  TCanvas *canvas_Bx1_z2 = new TCanvas("Bx1_z2","Plotting Canvas",150,10,990,660);
  canvas_Bx1_z1->Divide(2,3);
  canvas_Bx1_z2->Divide(2,3);
  TCanvas *canvas_By1_z1 = new TCanvas("By1_z1","Plotting Canvas",150,10,990,660);
  TCanvas *canvas_By1_z2 = new TCanvas("By1_z2","Plotting Canvas",150,10,990,660);
  canvas_By1_z1->Divide(2,3);
  canvas_By1_z2->Divide(2,3);
  TCanvas *canvas_Bz1_z1 = new TCanvas("Bz1_z1","Plotting Canvas",150,10,990,660);
  TCanvas *canvas_Bz1_z2 = new TCanvas("Bz1_z2","Plotting Canvas",150,10,990,660);
  canvas_Bz1_z1->Divide(2,3);
  canvas_Bz1_z2->Divide(2,3);
  TCanvas *canvas_Btot1_z1 = new TCanvas("Btot1_z1","Plotting Canvas",150,10,990,660);
  TCanvas *canvas_Btot1_z2 = new TCanvas("Btot1_z2","Plotting Canvas",150,10,990,660);
  canvas_Btot1_z1->Divide(2,3);
  canvas_Btot1_z2->Divide(2,3);
  TCanvas *canvas_Bx0_z1 = new TCanvas("Bx0_z1","Plotting Canvas",150,10,990,660);
  TCanvas *canvas_Bx0_z2 = new TCanvas("Bx0_z2","Plotting Canvas",150,10,990,660);
  canvas_Bx0_z1->Divide(2,3);
  canvas_Bx0_z2->Divide(2,3);
  TCanvas *canvas_By0_z1 = new TCanvas("By0_z1","Plotting Canvas",150,10,990,660);
  TCanvas *canvas_By0_z2 = new TCanvas("By0_z2","Plotting Canvas",150,10,990,660);
  canvas_By0_z1->Divide(2,3);
  canvas_By0_z2->Divide(2,3);
  TCanvas *canvas_Bz0_z1 = new TCanvas("Bz0_z1","Plotting Canvas",150,10,990,660);
  TCanvas *canvas_Bz0_z2 = new TCanvas("Bz0_z2","Plotting Canvas",150,10,990,660);
  canvas_Bz0_z1->Divide(2,3);
  canvas_Bz0_z2->Divide(2,3);
  TCanvas *canvas_Btot0_z1 = new TCanvas("Btot0_z1","Plotting Canvas",150,10,990,660);
  TCanvas *canvas_Btot0_z2 = new TCanvas("Btot0_z2","Plotting Canvas",150,10,990,660);
  canvas_Btot0_z1->Divide(2,3);
  canvas_Btot0_z2->Divide(2,3);

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
  int num_points1;
  int num_points0;

  Histo->SetBranchAddress("gantry0_x",&gant0_x);
  Histo->SetBranchAddress("gantry0_y",&gant0_y);
  Histo->SetBranchAddress("gantry0_z",&gant0_z);
  Histo->SetBranchAddress("phidg0_Bx",ph0_x);
  Histo->SetBranchAddress("phidg0_By",ph0_y);
  Histo->SetBranchAddress("phidg0_Bz",ph0_z);
  Histo->SetBranchAddress("phidg0_Btot",ph0_tot);

  Histo->SetBranchAddress("gantry1_x",&gant1_x);
  Histo->SetBranchAddress("gantry1_y",&gant1_y);
  Histo->SetBranchAddress("gantry1_z",&gant1_z);
  Histo->SetBranchAddress("phidg1_Bx",ph1_x);
  Histo->SetBranchAddress("phidg1_By",ph1_y);
  Histo->SetBranchAddress("phidg1_Bz",ph1_z);
  Histo->SetBranchAddress("phidg1_Btot",ph1_tot);

  Histo->SetBranchAddress("num_phidg1_points",&num_points1);
  Histo->SetBranchAddress("num_phidg0_points",&num_points0);

  int step = Histo->GetEntries();

  TMarker *m0 = new TMarker(0.3, 0.35, 28);
  for(int j=0; j<11; j++){
    gStyle->SetOptTitle(kFALSE);
    double k = j*0.05;
    double kk = j*5.;
    h5 = new TH2D(Form("gant1_Bx z height = %2f cm",kk),"",16,0.02,0.66,16,0.02,0.66);
    h6 = new TH2D(Form("gant1_By z height = %2f cm",kk),"",16,0.02,0.66,16,0.02,0.66);
    h7 = new TH2D(Form("gant1_Bz z height = %2f cm",kk),"",16,0.02,0.66,16,0.02,0.66);
    h8 = new TH2D(Form("gant1_Btot z height = %2f cm",kk),"",16,0.02,0.66,16,0.02,0.66);
    for(int i=1; i<step; i++){
      Histo->GetEntry(i);
      if(gant1_z > k - 0.025 && gant1_z < k + 0.025){ 
        for(int l=0; l < 1; l++){
	  if(ph1_y[l] != 0 && ph1_x[l] != 0 && ph1_z[l] != 0){
            h5->Fill(gant1_x,gant1_y,ph1_x[l]);
   	    h6->Fill(gant1_x,gant1_y,ph1_y[l]);
            h7->Fill(gant1_x,gant1_y,ph1_z[l]);
            h8->Fill(gant1_x,gant1_y,ph1_tot[l]);
  	  }	  
	}
      }
    }
    // To whiteout empty cells in histogram
    TH2D* copy5 = dynamic_cast<TH2D*>(h5->Clone());
    copy5->Reset();
    copy5->SetFillColor(TColor::GetColor(253,253,253));
    Int_t bin = 0;
    Double_t maxVal = h5->GetMaximum();
    for(Int_t i = 1; i <= h5->GetNbinsX(); ++i) {
       for(Int_t g = 1; g <= h5->GetNbinsY(); ++g) {
          bin = h5->GetBin(i, g);
          if(!h5->GetBinContent(bin)) copy5->SetBinContent(bin, maxVal);
       }
    }
    TH2D* copy6 = dynamic_cast<TH2D*>(h6->Clone());
    copy6->Reset();
    copy6->SetFillColor(TColor::GetColor(253,253,253));
    Int_t bin = 0;
    Double_t maxVal = h6->GetMaximum();
    for(Int_t i = 1; i <= h6->GetNbinsX(); ++i) {
       for(Int_t g = 1; g <= h6->GetNbinsY(); ++g) {
          bin = h6->GetBin(i, g);
std::cout << bin << std::endl;
          if(!h6->GetBinContent(bin)) copy6->SetBinContent(bin, maxVal);
       }
    }
    TH2D* copy7 = dynamic_cast<TH2D*>(h7->Clone());
    copy7->Reset();
    copy7->SetFillColor(TColor::GetColor(253,253,253));
    Int_t bin = 0;
    Double_t maxVal = h7->GetMaximum();
    for(Int_t i = 1; i <= h7->GetNbinsX(); ++i) {
       for(Int_t g = 1; g <= h7->GetNbinsY(); ++g) {
          bin = h7->GetBin(i, g);
          if(!h7->GetBinContent(bin)) copy7->SetBinContent(bin, maxVal);
       }
    }
    TH2D* copy8 = dynamic_cast<TH2D*>(h8->Clone());
    copy8->Reset();
    copy8->SetFillColor(TColor::GetColor(253,253,253));
    Int_t bin = 0;
    Double_t maxVal = h8->GetMaximum();
    for(Int_t i = 1; i <= h8->GetNbinsX(); ++i) {
       for(Int_t g = 1; g <= h8->GetNbinsY(); ++g) {
          bin = h8->GetBin(i, g);
          if(!h8->GetBinContent(bin)) copy8->SetBinContent(bin, maxVal);
       }
    }

    f1.cd();
    h5->Write();
    h6->Write();
    h7->Write();
    h8->Write();
    copy5->Write();
    copy6->Write();
    copy7->Write();
    copy8->Write();

    /*h5->GetYaxis()->SetLabelFont(63);
    h5->GetYaxis()->SetLabelSize(12);
    h5->GetYaxis()->SetTitle("y (cm)");
    h5->GetYaxis()->CenterTitle();
    h5->GetYaxis()->SetTitleSize(12);
    h5->GetXaxis()->SetLabelFont(63);
    h5->GetXaxis()->SetLabelSize(12);
    h5->GetXaxis()->SetTitle("x (cm)");
    h5->GetXaxis()->CenterTitle();
    h5->GetXaxis()->SetTitleSize(12);
    h6->GetYaxis()->SetLabelFont(63);
    h6->GetYaxis()->SetLabelSize(12);
    h6->GetYaxis()->SetTitle("y (cm)");
    h6->GetYaxis()->CenterTitle();
    h6->GetYaxis()->SetTitleSize(12);
    h6->GetXaxis()->SetLabelFont(63);
    h6->GetXaxis()->SetLabelSize(12);
    h6->GetXaxis()->SetTitle("x (cm)");
    h6->GetXaxis()->CenterTitle();
    h6->GetXaxis()->SetTitleSize(12);
    h7->GetYaxis()->SetLabelFont(63);
    h7->GetYaxis()->SetLabelSize(12);
    h7->GetYaxis()->SetTitle("y (cm)");
    h7->GetYaxis()->CenterTitle();
    h7->GetYaxis()->SetTitleSize(12);
    h7->GetXaxis()->SetLabelFont(63);
    h7->GetXaxis()->SetLabelSize(12);
    h7->GetXaxis()->SetTitle("x (cm)");
    h7->GetXaxis()->CenterTitle();
    h7->GetXaxis()->SetTitleSize(12);
    h8->GetYaxis()->SetLabelFont(63);
    h8->GetYaxis()->SetLabelSize(12);
    h8->GetYaxis()->SetTitle("y (cm)");
    h8->GetYaxis()->CenterTitle();
    h8->GetYaxis()->SetTitleSize(12);
    h8->GetXaxis()->SetLabelFont(63);
    h8->GetXaxis()->SetLabelSize(12);
    h8->GetXaxis()->SetTitle("x (cm)");
    h8->GetXaxis()->CenterTitle();
    h8->GetXaxis()->SetTitleSize(12);
    */

    if(j < 6){
      canvas_Bx1_z1->cd(j+1);
      h5->DrawCopy("colz");
      copy5->DrawCopy("BOX SAME");
      m0->Draw("same");
      h5->GetTitle();  //gets title of histogram
      pt2 = new TPaveText(0.2,0.92,0.8,0.97,"NDC");
      pt2->SetFillColor(0);
      pt2->SetBorderSize(0);
      tx2 = pt2->AddText(0.5,0.5,Form("gant1_Bx z height = %2f cm",kk));
      tx2->SetTextSize(0.07);
      tx2->SetTextFont(42);
      tx2->SetTextAlign(22);
      pt2->Draw("same");
      canvas_By1_z1->cd(j+1);
      h6->DrawCopy("colz");
      copy6->DrawCopy("BOX SAME");
      m0->Draw("same");
      h6->GetTitle();  //gets title of histogram
      pt2 = new TPaveText(0.2,0.92,0.8,0.97,"NDC");
      pt2->SetFillColor(0);
      pt2->SetBorderSize(0);
      tx2 = pt2->AddText(0.5,0.5,Form("gant1_By z height = %2f cm",kk));
      tx2->SetTextSize(0.07);
      tx2->SetTextFont(42);
      tx2->SetTextAlign(22);
      pt2->Draw("same");
      canvas_Bz1_z1->cd(j+1);
      h7->DrawCopy("colz");
      copy7->DrawCopy("BOX SAME");
      m0->Draw("same");
      h7->GetTitle();  //gets title of histogram
      pt2 = new TPaveText(0.2,0.92,0.8,0.97,"NDC");
      pt2->SetFillColor(0);
      pt2->SetBorderSize(0);
      tx2 = pt2->AddText(0.5,0.5,Form("gant1_Bz z height = %2f cm",kk));
      tx2->SetTextSize(0.07);
      tx2->SetTextFont(42);
      tx2->SetTextAlign(22);
      pt2->Draw("same");
      canvas_Btot1_z1->cd(j+1);
      h8->DrawCopy("colz");
      copy8->DrawCopy("BOX SAME");
      m0->Draw("same");
      h8->GetTitle();  //gets title of histogram
      pt2 = new TPaveText(0.2,0.92,0.8,0.97,"NDC");
      pt2->SetFillColor(0);
      pt2->SetBorderSize(0);
      tx2 = pt2->AddText(0.5,0.5,Form("gant1_Btot z height = %2f cm",kk));
      tx2->SetTextSize(0.07);
      tx2->SetTextFont(42);
      tx2->SetTextAlign(22);
      pt2->Draw("same");
    } else if(j >5 && j < 11){
      canvas_Bx1_z2->cd(j-5);
      h5->DrawCopy("colz");
      copy5->DrawCopy("BOX SAME");
      m0->Draw("same");
      h5->GetTitle();  //gets title of histogram
      pt2 = new TPaveText(0.2,0.92,0.8,0.97,"NDC");
      pt2->SetFillColor(0);
      pt2->SetBorderSize(0);
      tx2 = pt2->AddText(0.5,0.5,Form("gant1_Bx z height = %2f cm",kk));
      tx2->SetTextSize(0.07);
      tx2->SetTextFont(42);
      tx2->SetTextAlign(22);
      pt2->Draw("same");
      h6->GetTitle();  //gets title of histogram
      pt2 = new TPaveText(0.2,0.92,0.8,0.97,"NDC");
      pt2->SetFillColor(0);
      pt2->SetBorderSize(0);
      tx2 = pt2->AddText(0.5,0.5,Form("gant1_By z height = %2f cm",kk));
      tx2->SetTextSize(0.07);
      tx2->SetTextFont(42);
      tx2->SetTextAlign(22);
      pt2->Draw("same");
      canvas_By1_z2->cd(j-5);
      h6->DrawCopy("colz");
      copy6->DrawCopy("BOX SAME");
      m0->Draw("same");
      h7->GetTitle();  //gets title of histogram
      pt2 = new TPaveText(0.2,0.92,0.8,0.97,"NDC");
      pt2->SetFillColor(0);
      pt2->SetBorderSize(0);
      tx2 = pt2->AddText(0.5,0.5,Form("gant1_Bz z height = %2f cm",kk));
      tx2->SetTextSize(0.07);
      tx2->SetTextFont(42);
      tx2->SetTextAlign(22);
      pt2->Draw("same");
      canvas_Bz1_z2->cd(j-5);
      h7->DrawCopy("colz");
      copy7->DrawCopy("BOX SAME");
      m0->Draw("same");
      h8->GetTitle();  //gets title of histogram
      pt2 = new TPaveText(0.2,0.92,0.8,0.97,"NDC");
      pt2->SetFillColor(0);
      pt2->SetBorderSize(0);
      tx2 = pt2->AddText(0.5,0.5,Form("gant1_Btot z height = %2f cm",kk));
      tx2->SetTextSize(0.07);
      tx2->SetTextFont(42);
      tx2->SetTextAlign(22);
      pt2->Draw("same");

      canvas_Btot1_z2->cd(j-5);
      h8->DrawCopy("colz");
      copy8->DrawCopy("BOX SAME");
      m0->Draw("same");
    }
 
    f->cd();
    
    delete h5;
    delete h6;
    delete h7;
    delete h8;
  }

canvas_Bx1_z1->Print("Magnetic_Field_Scan_1383.pdf(", "pdf");
canvas_Bx1_z2->Print("Magnetic_Field_Scan_1383.pdf", "pdf");
canvas_By1_z1->Print("Magnetic_Field_Scan_1383.pdf", "pdf");
canvas_By1_z2->Print("Magnetic_Field_Scan_1383.pdf", "pdf");
canvas_Bz1_z1->Print("Magnetic_Field_Scan_1383.pdf", "pdf");
canvas_Bz1_z2->Print("Magnetic_Field_Scan_1383.pdf", "pdf");
canvas_Btot1_z1->Print("Magnetic_Field_Scan_1383.pdf", "pdf");
canvas_Btot1_z2->Print("Magnetic_Field_Scan_1383.pdf", "pdf");



  for(int j=0; j<11; j++){
    double k = j*0.05;
    double kk = j*5.0;
    h1 = new TH2D(Form("gant0_Bx z height %2f",kk),"",16,0.02,0.66,16,0.02,0.66);
    h2 = new TH2D(Form("gant0_By z height %2f",kk),"",16,0.02,0.66,16,0.02,0.66);
    h3 = new TH2D(Form("gant0_Bz z height %2f",kk),"",16,0.02,0.66,16,0.02,0.66);
    h4 = new TH2D(Form("gant0_Btot z height %2f",kk),"",16,0.02,0.66,16,0.02,0.66);
    for(int i=1; i<step; i++){
      Histo->GetEntry(i);
      if(gant0_z > k - 0.025 && gant0_z < k + 0.025){
        for(int l=1; l < 2; l++){
          if(ph0_y[l] != 0 && ph0_x[l] != 0 && ph0_z[l] != 0){
            h1->Fill(gant0_x,gant0_y,ph0_x[l]/num_points0);
     	    h2->Fill(gant0_x,gant0_y,ph0_y[l]/num_points0);
            h3->Fill(gant0_x,gant0_y,ph0_z[l]/num_points0);
            h4->Fill(gant0_x,gant0_y,ph0_tot[l]/num_points0);
          }
        }
      }
    }
    // To whiteout empty cells in histogram
    TH2D* copy1 = dynamic_cast<TH2D*>(h1->Clone());
    copy1->Reset();
    copy1->SetFillColor(TColor::GetColor(253,253,253));
    Int_t bin = 0;
    Double_t maxVal = h1->GetMaximum();
    for(Int_t i = 1; i <= h1->GetNbinsX(); ++i) {
       for(Int_t g = 1; g <= h1->GetNbinsY(); ++g) {
          bin = h1->GetBin(i, g);
          if(!h1->GetBinContent(bin)) copy1->SetBinContent(bin, maxVal);
       }
    }
    TH2D* copy2 = dynamic_cast<TH2D*>(h2->Clone());
    copy2->Reset();
    copy2->SetFillColor(TColor::GetColor(253,253,253));
    Int_t bin = 0;
    Double_t maxVal = h2->GetMaximum();
    for(Int_t i = 1; i <= h2->GetNbinsX(); ++i) {
       for(Int_t g = 1; g <= h2->GetNbinsY(); ++g) {
          bin = h2->GetBin(i, g);
          if(!h2->GetBinContent(bin)) copy2->SetBinContent(bin, maxVal);
       }
    }
    TH2D* copy3 = dynamic_cast<TH2D*>(h3->Clone());
    copy3->Reset();
    copy3->SetFillColor(TColor::GetColor(253,253,253));
    Int_t bin = 0;
    Double_t maxVal = h3->GetMaximum();
    for(Int_t i = 1; i <= h3->GetNbinsX(); ++i) {
       for(Int_t g = 1; g <= h3->GetNbinsY(); ++g) {
          bin = h3->GetBin(i, g);
          if(!h3->GetBinContent(bin)) copy3->SetBinContent(bin, maxVal);
       }
    }
    TH2D* copy4 = dynamic_cast<TH2D*>(h4->Clone());
    copy4->Reset();
    copy4->SetFillColor(TColor::GetColor(253,253,253));
    Int_t bin = 0;
    Double_t maxVal = h4->GetMaximum();
    for(Int_t i = 1; i <= h4->GetNbinsX(); ++i) {
       for(Int_t g = 1; g <= h4->GetNbinsY(); ++g) {
          bin = h4->GetBin(i, g);
          if(!h4->GetBinContent(bin)) copy4->SetBinContent(bin, maxVal);
       }
    }

    f1.cd();
    h1->Write();
    h2->Write();
    h3->Write();
    h4->Write();
    copy1->Write();
    copy2->Write();
    copy3->Write();
    copy4->Write();

    /*h1->GetYaxis()->SetLabelFont(63);
    h1->GetYaxis()->SetLabelSize(12);
    h1->GetYaxis()->SetTitle("y (cm)");
    h1->GetYaxis()->CenterTitle();
    h1->GetYaxis()->SetTitleSize(12);
    h1->GetXaxis()->SetLabelFont(63);
    h1->GetXaxis()->SetLabelSize(12);
    h1->GetXaxis()->SetTitle("x (cm)");
    h1->GetXaxis()->CenterTitle();
    h1->GetXaxis()->SetTitleSize(12);
    h2->GetYaxis()->SetLabelFont(63);
    h2->GetYaxis()->SetLabelSize(12);
    h2->GetYaxis()->SetTitle("y (cm)");
    h2->GetYaxis()->CenterTitle();
    h2->GetYaxis()->SetTitleSize(12);
    h2->GetXaxis()->SetLabelFont(63);
    h2->GetXaxis()->SetLabelSize(12);
    h2->GetXaxis()->SetTitle("x (cm)");
    h2->GetXaxis()->CenterTitle();
    h2->GetXaxis()->SetTitleSize(12);
    h3->GetYaxis()->SetLabelFont(63);
    h3->GetYaxis()->SetLabelSize(12);
    h3->GetYaxis()->SetTitle("y (cm)");
    h3->GetYaxis()->CenterTitle();
    h3->GetYaxis()->SetTitleSize(12);
    h3->GetXaxis()->SetLabelFont(63);
    h3->GetXaxis()->SetLabelSize(12);
    h3->GetXaxis()->SetTitle("x (cm)");
    h3->GetXaxis()->CenterTitle();
    h3->GetXaxis()->SetTitleSize(12);
    h4->GetYaxis()->SetLabelFont(63);
    h4->GetYaxis()->SetLabelSize(12);
    h4->GetYaxis()->SetTitle("y (cm)");
    h4->GetYaxis()->CenterTitle();
    h4->GetYaxis()->SetTitleSize(12);
    h4->GetXaxis()->SetLabelFont(63);
    h4->GetXaxis()->SetLabelSize(12);
    h4->GetXaxis()->SetTitle("x (cm)");
    h4->GetXaxis()->CenterTitle();
    h4->GetXaxis()->SetTitleSize(12);
    */
    if(j < 6){
      canvas_Bx0_z1->cd(j+1);
      h1->DrawCopy("colz");
      copy1->DrawCopy("BOX SAME");
      m0->Draw("same");
      h1->GetTitle();  //gets title of histogram
      pt2 = new TPaveText(0.2,0.92,0.8,0.97,"NDC");
      pt2->SetFillColor(0);
      pt2->SetBorderSize(0);
      tx2 = pt2->AddText(0.5,0.5,Form("gant0_Bx z height = %2f cm",kk));
      tx2->SetTextSize(0.07);
      tx2->SetTextFont(42);
      tx2->SetTextAlign(22);
      pt2->Draw("same");
      canvas_By0_z1->cd(j+1);
      h2->DrawCopy("colz");
      copy2->DrawCopy("BOX SAME");
      m0->Draw("same");
      h2->GetTitle();  //gets title of histogram
      pt2 = new TPaveText(0.2,0.92,0.8,0.97,"NDC");
      pt2->SetFillColor(0);
      pt2->SetBorderSize(0);
      tx2 = pt2->AddText(0.5,0.5,Form("gant0_By z height = %2f cm",kk));
      tx2->SetTextSize(0.07);
      tx2->SetTextFont(42);
      tx2->SetTextAlign(22);
      pt2->Draw("same");
      canvas_Bz0_z1->cd(j+1);
      h3->DrawCopy("colz");
      copy3->DrawCopy("BOX SAME");
      m0->Draw("same");
      h3->GetTitle();  //gets title of histogram
      pt2 = new TPaveText(0.2,0.92,0.8,0.97,"NDC");
      pt2->SetFillColor(0);
      pt2->SetBorderSize(0);
      tx2 = pt2->AddText(0.5,0.5,Form("gant0_Bz z height = %2f cm",kk));
      tx2->SetTextSize(0.07);
      tx2->SetTextFont(42);
      tx2->SetTextAlign(22);
      pt2->Draw("same");
      canvas_Btot0_z1->cd(j+1);
      h4->DrawCopy("colz");
      copy4->DrawCopy("BOX SAME");
      m0->Draw("same");
      h4->GetTitle();  //gets title of histogram
      pt2 = new TPaveText(0.2,0.92,0.8,0.97,"NDC");
      pt2->SetFillColor(0);
      pt2->SetBorderSize(0);
      tx2 = pt2->AddText(0.5,0.5,Form("gant0_Btot z height = %2f cm",kk));
      tx2->SetTextSize(0.07);
      tx2->SetTextFont(42);
      tx2->SetTextAlign(22);
      pt2->Draw("same");
    } else if(j >5 && j < 11){
      canvas_Bx0_z2->cd(j-5);
      h1->DrawCopy("colz");
      copy1->DrawCopy("BOX SAME");
      m0->Draw("same");
      h1->GetTitle();  //gets title of histogram
      pt2 = new TPaveText(0.2,0.92,0.8,0.97,"NDC");
      pt2->SetFillColor(0);
      pt2->SetBorderSize(0);
      tx2 = pt2->AddText(0.5,0.5,Form("gant0_Bx z height = %2f cm",kk));
      tx2->SetTextSize(0.07);
      tx2->SetTextFont(42);
      tx2->SetTextAlign(22);
      pt2->Draw("same");
      canvas_By0_z2->cd(j-5);
      h2->DrawCopy("colz");
      copy2->DrawCopy("BOX SAME");
      m0->Draw("same");
      h2->GetTitle();  //gets title of histogram
      pt2 = new TPaveText(0.2,0.92,0.8,0.97,"NDC");
      pt2->SetFillColor(0);
      pt2->SetBorderSize(0);
      tx2 = pt2->AddText(0.5,0.5,Form("gant0_By z height = %2f cm",kk));
      tx2->SetTextSize(0.07);
      tx2->SetTextFont(42);
      tx2->SetTextAlign(22);
      pt2->Draw("same");
      canvas_Bz0_z2->cd(j-5);
      h3->DrawCopy("colz");
      copy3->Draw("BOX SAME");
      m0->Draw("same");
      h3->GetTitle();  //gets title of histogram
      pt2 = new TPaveText(0.2,0.92,0.8,0.97,"NDC");
      pt2->SetFillColor(0);
      pt2->SetBorderSize(0);
      tx2 = pt2->AddText(0.5,0.5,Form("gant0_Bz z height = %2f cm",kk));
      tx2->SetTextSize(0.07);
      tx2->SetTextFont(42);
      tx2->SetTextAlign(22);
      pt2->Draw("same");
      canvas_Btot0_z2->cd(j-5);
      h4->DrawCopy("colz");
      copy4->DrawCopy("BOX SAME");
      m0->Draw("same");
      h4->GetTitle();  //gets title of histogram
      pt2 = new TPaveText(0.2,0.92,0.8,0.97,"NDC");
      pt2->SetFillColor(0);
      pt2->SetBorderSize(0);
      tx2 = pt2->AddText(0.5,0.5,Form("gant0_Btot z height = %2f cm",kk));
      tx2->SetTextSize(0.07);
      tx2->SetTextFont(42);
      tx2->SetTextAlign(22);
      pt2->Draw("same");
    }

    f->cd();

  delete h1;
  delete h2;
  delete h3;
  delete h4;
  }

canvas_Bx0_z1->Print("Magnetic_Field_Scan_1383.pdf", "pdf");
canvas_Bx0_z2->Print("Magnetic_Field_Scan_1383.pdf", "pdf");
canvas_By0_z1->Print("Magnetic_Field_Scan_1383.pdf", "pdf");
canvas_By0_z2->Print("Magnetic_Field_Scan_1383.pdf", "pdf");
canvas_Bz0_z1->Print("Magnetic_Field_Scan_1383.pdf", "pdf");
canvas_Bz0_z2->Print("Magnetic_Field_Scan_1383.pdf", "pdf");
canvas_Btot0_z1->Print("Magnetic_Field_Scan_1383.pdf", "pdf");
canvas_Btot0_z2->Print("Magnetic_Field_Scan_1383.pdf)", "pdf");

}
