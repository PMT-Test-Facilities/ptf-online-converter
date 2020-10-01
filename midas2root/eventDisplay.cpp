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
        Double_t V1730_wave0[num_points][numTimeBins], V1730_wave1[num_points][numTimeBins], V1730_wave2[num_points][numTimeBins], V1730_wave3[num_points][numTimeBins];

        // Open up the waveform branches.
        // NOTICE: Only 4 channels' data are being retrieved 
        // for the time being. Add the additional branch addresses here
        // in the future. 
        auto dataBranch0 = scanTree->GetBranch("V1730_wave0");
        auto dataBranch1 = scanTree->GetBranch("V1730_wave1");
        auto dataBranch2 = scanTree->GetBranch("V1730_wave2");
        auto dataBranch3 = scanTree->GetBranch("V1730_wave3");

        // Store the information from the branches 
        // into the matrices defined above
        dataBranch0->SetAddress(&V1730_wave0);
        dataBranch1->SetAddress(&V1730_wave1);
        dataBranch2->SetAddress(&V1730_wave2);
        dataBranch3->SetAddress(&V1730_wave3);

        // Access the data
        scanTree->GetEntry(eventEntry);

        // Defining histograms and canvas to be shown
        auto canvas = new TCanvas("c1","",900,900);
        auto hwaveform0 = new TH1F("waveform0","Channel 0; Time(ns); Voltage (V)", numTimeBins, 0.0, float(numTimeBins)*1000./mPMT_DIGITIZER_SAMPLE_RATE);
        auto hwaveform1 = new TH1D("waveform1","Channel 1; Time(ns); Voltage (V)", numTimeBins, 0.0, float(numTimeBins)*1000./mPMT_DIGITIZER_SAMPLE_RATE);
        auto hwaveform2 = new TH1D("waveform2","Channel 2; Time(ns); Voltage (V)", numTimeBins, 0.0, float(numTimeBins)*1000./mPMT_DIGITIZER_SAMPLE_RATE);
        auto hwaveform3 = new TH1D("waveform3","Channel 3; Time(ns); Voltage (V)", numTimeBins, 0.0, float(numTimeBins)*1000./mPMT_DIGITIZER_SAMPLE_RATE);

        // Filling in the histograms
        for (int k=1; k<1024; k++) {

            // Changing the horizontal axis to ns
            Double_t time = float(k)*timeFactor;

            // Filling in the histograms with data
            hwaveform0->Fill(time, float(V1730_wave0[0][k-1]));
            hwaveform1->Fill(time, float(V1730_wave1[0][k-1]));
            hwaveform2->Fill(time, float(V1730_wave2[0][k-1]));
            hwaveform3->Fill(time, float(V1730_wave3[0][k-1]));

        }

        // Change vertical scale from counts to volts
        hwaveform0->Scale(scaleFactor);
        hwaveform1->Scale(scaleFactor);
        hwaveform2->Scale(scaleFactor);
        hwaveform3->Scale(scaleFactor);

        // Changing the style of the histograms
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

        // Show canvas
        theApp->Run();
        
    }

    // End of function.
    return 0;
}