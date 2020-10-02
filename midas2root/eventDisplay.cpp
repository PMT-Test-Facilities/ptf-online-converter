#include <stdio.h>
#include <iostream>
#include "TCanvas.h"
#include "TH2D.h"
#include "TH1.h"
#include "TPad.h"
#include "TFile.h"
#include "TTree.h"
#include "TLegend.h"
#include "TApplication.h"

// CHANGE THIS ACCORDING TO DIGITIZER USED.
#define PMT_CHANNEL_FORMAT "V1730_wave%i"
#define mPMT_DIGITIZER_SAMPLE_RATE 125 // MS/s
#define mPMT_DIGITIZER_FULL_SCALE_RANGE 2.0 // Vpp
#define mPMT_DIGITIZER_RESOLUTION 12 // bits


int main(int argc, char** argv) {

    std::cout<<argc<<std::endl;

    // Start the function to display the event.
    int eventEntry = atoi(argv[2]);

    // Is this entry valid? Set to false by default.
    bool validEntry = false;

    // Defining global variables:
    auto scanTree = new TTree();
    int numTimeBins = 1024;
    float timeFactor = 8.0; // ns
    Double_t digiCounts = pow(2.0, mPMT_DIGITIZER_RESOLUTION); // counts ?
    Double_t scaleFactor = mPMT_DIGITIZER_FULL_SCALE_RANGE/digiCounts; // V/counts ?

    // Test arguments passed through from terminal:
    if (argc!=3) {

        // Tell the user how to use the root file.
        std::cout << std::endl << "eventDisplay.exe usage:" <<std::endl;
        std::cout << "Provide path to root file and no. of event to be viewed."<<std::endl;
        std::cout << "Example: ./midas2root_mpmt.exe ./output00000400.root 1500"<<std::endl;
    
    } else {


        // Open root file:
        auto rootFile = new TFile(argv[1], "READ");

        // Tree name:
        char waveformTreeName[1024] = "scan_tree";

        // Retrieve the tree from the file:
        std::cout<<"Opening up root file..."<<std::endl;
        
        rootFile->GetObject(waveformTreeName, scanTree);

        // Getting number of entries:
        int numEntries = scanTree->GetEntries();
        std::cout<<"No. of entries found: "<<numEntries<<"."<<std::endl;

        // Testing initial entry:
        if (eventEntry < 0 || eventEntry > numEntries) {
            std::cout<<"Invalid event number."<<std::endl;
        } else {
            std::cout<<"Event in display: "<<eventEntry<<"."<<std::endl;
            validEntry = true;
        }
    }

    if (validEntry) {

        // Root app, to show the canvas.

        // NOTICE: TApplication is not compatible with 
        // argv[1] being a char, so do not move this declaration
        // to before the rootfile is open.
        auto theApp = new TApplication("App", &argc, argv);
        
        // Getting waveform parameters from the tree
        int num_points;
        auto dataBranch = scanTree->GetBranch("num_points");
        dataBranch->SetAddress(&num_points);
        scanTree->GetEntry(0);
    
        // Defining matrices to hold the waveform informations:
        Double_t V1730_wave0[num_points][numTimeBins],  V1730_wave1[num_points][numTimeBins],  V1730_wave2[num_points][numTimeBins],  V1730_wave3[num_points][numTimeBins];
        Double_t V1730_wave4[num_points][numTimeBins],  V1730_wave5[num_points][numTimeBins],  V1730_wave6[num_points][numTimeBins],  V1730_wave7[num_points][numTimeBins];
        Double_t V1730_wave8[num_points][numTimeBins],  V1730_wave9[num_points][numTimeBins],  V1730_wave10[num_points][numTimeBins], V1730_wave11[num_points][numTimeBins];
        Double_t V1730_wave12[num_points][numTimeBins], V1730_wave13[num_points][numTimeBins], V1730_wave14[num_points][numTimeBins], V1730_wave15[num_points][numTimeBins];
        Double_t V1730_wave16[num_points][numTimeBins], V1730_wave17[num_points][numTimeBins], V1730_wave18[num_points][numTimeBins], V1730_wave19[num_points][numTimeBins];

        // Open up the waveform branches.
        // NOTICE: Only 4 channels' data are being retrieved 
        // for the time being. Add the additional branch addresses here
        // in the future. 
        auto dataBranch0 = scanTree->GetBranch("V1730_wave0");
        auto dataBranch1 = scanTree->GetBranch("V1730_wave1");
        auto dataBranch2 = scanTree->GetBranch("V1730_wave2");
        auto dataBranch3 = scanTree->GetBranch("V1730_wave3");
        auto dataBranch4 = scanTree->GetBranch("V1730_wave4");
        auto dataBranch5 = scanTree->GetBranch("V1730_wave5");
        auto dataBranch6 = scanTree->GetBranch("V1730_wave6");
        auto dataBranch7 = scanTree->GetBranch("V1730_wave7");
        auto dataBranch8 = scanTree->GetBranch("V1730_wave8");
        auto dataBranch9 = scanTree->GetBranch("V1730_wave9");
        auto dataBranch10 = scanTree->GetBranch("V1730_wave10");
        auto dataBranch11 = scanTree->GetBranch("V1730_wave11");
        auto dataBranch12 = scanTree->GetBranch("V1730_wave12");
        auto dataBranch13 = scanTree->GetBranch("V1730_wave13");
        auto dataBranch14 = scanTree->GetBranch("V1730_wave14");
        auto dataBranch15 = scanTree->GetBranch("V1730_wave15");
        auto dataBranch16 = scanTree->GetBranch("V1730_wave16");
        auto dataBranch17 = scanTree->GetBranch("V1730_wave17");
        auto dataBranch18 = scanTree->GetBranch("V1730_wave18");
        auto dataBranch19 = scanTree->GetBranch("V1730_wave19");

        // Store the information from the branches 
        // into the matrices defined above
        dataBranch0->SetAddress(&V1730_wave0);
        dataBranch1->SetAddress(&V1730_wave1);
        dataBranch2->SetAddress(&V1730_wave2);
        dataBranch3->SetAddress(&V1730_wave3);
        dataBranch4->SetAddress(&V1730_wave4);
        dataBranch5->SetAddress(&V1730_wave5);
        dataBranch6->SetAddress(&V1730_wave6);
        dataBranch7->SetAddress(&V1730_wave7);
        dataBranch8->SetAddress(&V1730_wave8);
        dataBranch9->SetAddress(&V1730_wave9);
        dataBranch10->SetAddress(&V1730_wave10);
        dataBranch11->SetAddress(&V1730_wave11);
        dataBranch12->SetAddress(&V1730_wave12);
        dataBranch13->SetAddress(&V1730_wave13);
        dataBranch14->SetAddress(&V1730_wave14);
        dataBranch15->SetAddress(&V1730_wave15);
        dataBranch16->SetAddress(&V1730_wave16);
        dataBranch17->SetAddress(&V1730_wave17);
        dataBranch18->SetAddress(&V1730_wave18);
        dataBranch19->SetAddress(&V1730_wave19);

        // Access the data
        scanTree->GetEntry(eventEntry);

        // Defining histograms and canvas to be shown
        auto canvas = new TCanvas("c1","",900,900);
        auto hwaveform0  = new TH1F("waveform0","Channel 0; Time(ns); Voltage (V)", numTimeBins, 0.0, float(numTimeBins)*1000./mPMT_DIGITIZER_SAMPLE_RATE);
        auto hwaveform1  = new TH1D("waveform1","Channel 1; Time(ns); Voltage (V)", numTimeBins, 0.0, float(numTimeBins)*1000./mPMT_DIGITIZER_SAMPLE_RATE);
        auto hwaveform2  = new TH1D("waveform2","Channel 2; Time(ns); Voltage (V)", numTimeBins, 0.0, float(numTimeBins)*1000./mPMT_DIGITIZER_SAMPLE_RATE);
        auto hwaveform3  = new TH1D("waveform3","Channel 3; Time(ns); Voltage (V)", numTimeBins, 0.0, float(numTimeBins)*1000./mPMT_DIGITIZER_SAMPLE_RATE);
        auto hwaveform4  = new TH1D("waveform4","Channel 4; Time(ns); Voltage (V)", numTimeBins, 0.0, float(numTimeBins)*1000./mPMT_DIGITIZER_SAMPLE_RATE);
        auto hwaveform5  = new TH1D("waveform5","Channel 5; Time(ns); Voltage (V)", numTimeBins, 0.0, float(numTimeBins)*1000./mPMT_DIGITIZER_SAMPLE_RATE);
        auto hwaveform6  = new TH1D("waveform6","Channel 6; Time(ns); Voltage (V)", numTimeBins, 0.0, float(numTimeBins)*1000./mPMT_DIGITIZER_SAMPLE_RATE);
        auto hwaveform7  = new TH1D("waveform7","Channel 7; Time(ns); Voltage (V)", numTimeBins, 0.0, float(numTimeBins)*1000./mPMT_DIGITIZER_SAMPLE_RATE);
        auto hwaveform8  = new TH1D("waveform8","Channel 8; Time(ns); Voltage (V)", numTimeBins, 0.0, float(numTimeBins)*1000./mPMT_DIGITIZER_SAMPLE_RATE);
        auto hwaveform9  = new TH1D("waveform9","Channel 9; Time(ns); Voltage (V)", numTimeBins, 0.0, float(numTimeBins)*1000./mPMT_DIGITIZER_SAMPLE_RATE);
        auto hwaveform10 = new TH1D("waveform10","Channel 10; Time(ns); Voltage (V)", numTimeBins, 0.0, float(numTimeBins)*1000./mPMT_DIGITIZER_SAMPLE_RATE);
        auto hwaveform11 = new TH1D("waveform11","Channel 11; Time(ns); Voltage (V)", numTimeBins, 0.0, float(numTimeBins)*1000./mPMT_DIGITIZER_SAMPLE_RATE);
        auto hwaveform12 = new TH1D("waveform12","Channel 12; Time(ns); Voltage (V)", numTimeBins, 0.0, float(numTimeBins)*1000./mPMT_DIGITIZER_SAMPLE_RATE);
        auto hwaveform13 = new TH1D("waveform13","Channel 13; Time(ns); Voltage (V)", numTimeBins, 0.0, float(numTimeBins)*1000./mPMT_DIGITIZER_SAMPLE_RATE);
        auto hwaveform14 = new TH1D("waveform14","Channel 14; Time(ns); Voltage (V)", numTimeBins, 0.0, float(numTimeBins)*1000./mPMT_DIGITIZER_SAMPLE_RATE);
        auto hwaveform15 = new TH1D("waveform15","Channel 15; Time(ns); Voltage (V)", numTimeBins, 0.0, float(numTimeBins)*1000./mPMT_DIGITIZER_SAMPLE_RATE);
        auto hwaveform16 = new TH1D("waveform16","Channel 16; Time(ns); Voltage (V)", numTimeBins, 0.0, float(numTimeBins)*1000./mPMT_DIGITIZER_SAMPLE_RATE);
        auto hwaveform17 = new TH1D("waveform17","Channel 17; Time(ns); Voltage (V)", numTimeBins, 0.0, float(numTimeBins)*1000./mPMT_DIGITIZER_SAMPLE_RATE);
        auto hwaveform18 = new TH1D("waveform18","Channel 18; Time(ns); Voltage (V)", numTimeBins, 0.0, float(numTimeBins)*1000./mPMT_DIGITIZER_SAMPLE_RATE);
        auto hwaveform19 = new TH1D("waveform19","Channel 19; Time(ns); Voltage (V)", numTimeBins, 0.0, float(numTimeBins)*1000./mPMT_DIGITIZER_SAMPLE_RATE);

        // Filling in the histograms
        for (int k=1; k<numTimeBins; k++) {

            // Changing the horizontal axis to ns
            Double_t time = float(k)*timeFactor;

            // Filling in the histograms with data
            hwaveform0->Fill(time, float(V1730_wave0[0][k-1]));
            hwaveform1->Fill(time, float(V1730_wave1[0][k-1]));
            hwaveform2->Fill(time, float(V1730_wave2[0][k-1]));
            hwaveform3->Fill(time, float(V1730_wave3[0][k-1]));
            hwaveform4->Fill(time, float(V1730_wave4[0][k-1]));
            hwaveform5->Fill(time, float(V1730_wave5[0][k-1]));
            hwaveform6->Fill(time, float(V1730_wave6[0][k-1]));
            hwaveform7->Fill(time, float(V1730_wave7[0][k-1]));
            hwaveform8->Fill(time, float(V1730_wave8[0][k-1]));
            hwaveform9->Fill(time, float(V1730_wave9[0][k-1]));
            hwaveform10->Fill(time, float(V1730_wave10[0][k-1]));
            hwaveform11->Fill(time, float(V1730_wave11[0][k-1]));
            hwaveform12->Fill(time, float(V1730_wave12[0][k-1]));
            hwaveform13->Fill(time, float(V1730_wave13[0][k-1]));
            hwaveform14->Fill(time, float(V1730_wave14[0][k-1]));
            hwaveform15->Fill(time, float(V1730_wave15[0][k-1]));
            hwaveform16->Fill(time, float(V1730_wave16[0][k-1]));
            hwaveform17->Fill(time, float(V1730_wave17[0][k-1]));
            hwaveform18->Fill(time, float(V1730_wave18[0][k-1]));
            hwaveform19->Fill(time, float(V1730_wave19[0][k-1]));

        }

        // Change vertical scale from counts to volts
        hwaveform0->Scale(scaleFactor);
        hwaveform1->Scale(scaleFactor);
        hwaveform2->Scale(scaleFactor);
        hwaveform3->Scale(scaleFactor);
        hwaveform4->Scale(scaleFactor);
        hwaveform5->Scale(scaleFactor);
        hwaveform6->Scale(scaleFactor);
        hwaveform7->Scale(scaleFactor);
        hwaveform8->Scale(scaleFactor);
        hwaveform9->Scale(scaleFactor);
        hwaveform10->Scale(scaleFactor);
        hwaveform11->Scale(scaleFactor);
        hwaveform12->Scale(scaleFactor);
        hwaveform13->Scale(scaleFactor);
        hwaveform14->Scale(scaleFactor);
        hwaveform15->Scale(scaleFactor);
        hwaveform16->Scale(scaleFactor);
        hwaveform17->Scale(scaleFactor);
        hwaveform18->Scale(scaleFactor);
        hwaveform19->Scale(scaleFactor);


    //     // Changing the style of the histograms
        hwaveform0->SetLineWidth(2);
        hwaveform0->SetLineColor(1);
        hwaveform0->SetStats(0);

        hwaveform1->SetLineWidth(2);
        hwaveform1->SetLineColor(1);
        hwaveform1->SetStats(0);

        hwaveform2->SetLineWidth(2);
        hwaveform2->SetLineColor(1);
        hwaveform2->SetStats(0);

        hwaveform3->SetLineWidth(2);
        hwaveform3->SetLineColor(1);
        hwaveform3->SetStats(0);

        hwaveform4->SetLineWidth(2);
        hwaveform4->SetLineColor(1);
        hwaveform4->SetStats(0);

        hwaveform5->SetLineWidth(2);
        hwaveform5->SetLineColor(1);
        hwaveform5->SetStats(0);

        hwaveform6->SetLineWidth(2);
        hwaveform6->SetLineColor(1);
        hwaveform6->SetStats(0);

        hwaveform7->SetLineWidth(2);
        hwaveform7->SetLineColor(1);
        hwaveform7->SetStats(0);

        hwaveform8->SetLineWidth(2);
        hwaveform8->SetLineColor(1);
        hwaveform8->SetStats(0);

        hwaveform9->SetLineWidth(2);
        hwaveform9->SetLineColor(1);
        hwaveform9->SetStats(0);

        hwaveform10->SetLineWidth(2);
        hwaveform10->SetLineColor(1);
        hwaveform10->SetStats(0);

        hwaveform11->SetLineWidth(2);
        hwaveform11->SetLineColor(1);
        hwaveform11->SetStats(0);

        hwaveform12->SetLineWidth(2);
        hwaveform12->SetLineColor(1);
        hwaveform12->SetStats(0);

        hwaveform13->SetLineWidth(2);
        hwaveform13->SetLineColor(1);
        hwaveform13->SetStats(0);

        hwaveform14->SetLineWidth(2);
        hwaveform14->SetLineColor(1);
        hwaveform14->SetStats(0);

        hwaveform15->SetLineWidth(2);
        hwaveform15->SetLineColor(1);
        hwaveform15->SetStats(0);

        hwaveform16->SetLineWidth(2);
        hwaveform16->SetLineColor(1);
        hwaveform16->SetStats(0);

        hwaveform17->SetLineWidth(2);
        hwaveform17->SetLineColor(1);
        hwaveform17->SetStats(0);

        hwaveform18->SetLineWidth(2);
        hwaveform18->SetLineColor(1);
        hwaveform18->SetStats(0);

        hwaveform19->SetLineWidth(2);
        hwaveform19->SetLineColor(1);
        hwaveform19->SetStats(0);

        // Dividing canvas.
        canvas->Divide(4,5);

        // Drawing histograms.
        canvas->cd(1);
        hwaveform0->Draw("HIST");
        canvas->cd(2);
        hwaveform1->Draw("HIST");
        canvas->cd(3);
        hwaveform2->Draw("HIST");
        canvas->cd(4);
        hwaveform3->Draw("HIST");
        canvas->cd(5);
        hwaveform4->Draw("HIST");
        canvas->cd(6);
        hwaveform4->Draw("HIST");
        canvas->cd(7);
        hwaveform6->Draw("HIST");
        canvas->cd(8);
        hwaveform7->Draw("HIST");
        canvas->cd(9);
        hwaveform8->Draw("HIST");
        canvas->cd(10);
        hwaveform9->Draw("HIST");
        canvas->cd(11);
        hwaveform10->Draw("HIST");
        canvas->cd(12);
        hwaveform11->Draw("HIST");
        canvas->cd(13);
        hwaveform12->Draw("HIST");
        canvas->cd(14);
        hwaveform13->Draw("HIST");
        canvas->cd(15);
        hwaveform14->Draw("HIST");
        canvas->cd(16);
        hwaveform15->Draw("HIST");
        canvas->cd(17);
        hwaveform16->Draw("HIST");
        canvas->cd(18);
        hwaveform17->Draw("HIST");
        canvas->cd(19);
        hwaveform18->Draw("HIST");
        canvas->cd(20);
        hwaveform19->Draw("HIST");

    //     // Show canvas
        theApp->Run();
        
    }

    // // End of function.
    return 0;
}