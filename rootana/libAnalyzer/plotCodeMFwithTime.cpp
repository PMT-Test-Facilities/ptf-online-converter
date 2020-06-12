{
    TFile *file221 = new TFile("output00221.root");
    TFile *file220 = new TFile("output00220.root");
    TCanvas *canvas1 = new TCanvas("canvas1","Graph",0,0,1300,1000);
    //indicate the location of the legend
    TLegend* legend = new TLegend(0.68, 0.7, 0.9,0.9);
  
     
   
     

    int time;
    double gantry0_x;
    double gantry0_y;
    double gantry0_z;
    double phidg0_Bx;
    double phidg0_By;
    double phidg0_Bz;
    double phidg0_Btot;
    
    TMultiGraph *Mgraph = new TMultiGraph;
    TGraph2D *graph2D = new TGraph2D();
    TGraph *graph = new TGraph();
    TGraph *graph1 = new TGraph();
    TGraph *graph2 = new TGraph();
    TGraph *graph3 = new TGraph();
    TGraph *graph4 = new TGraph();
    TGraph *graph5 = new TGraph();
    
    //used for graphing indices
    int count = 0;
    int count1 = 0;
    int count2 = 0;
    int count3 = 0;
    int count4 = 0;
    int count5 = 0;
    
    // SetBRanchAddress returns the adress of the specified branch.
    scan_tree.SetBranchAddress("time",&time);
    scan_tree.SetBranchAddress("gantry0_x",&gantry0_x);
    scan_tree.SetBranchAddress("gantry0_y",&gantry0_y);
    scan_tree.SetBranchAddress("gantry0_z",&gantry0_z);
    scan_tree.SetBranchAddress("phidg0_Bx",&phidg0_Bx);
    scan_tree.SetBranchAddress("phidg0_By",&phidg0_By);
    scan_tree.SetBranchAddress("phidg0_Bz",&phidg0_Bz);
    scan_tree.SetBranchAddress("phidg0_Btot",&phidg0_Btot);

    // Get number of entries in the data set
    int entries = scan_tree->GetEntries();
    

    for(int ient = 2; ient < entries; ient++){    
        scan_tree->GetEntry(ient);
//        std::cout<< gantry0_x << " : " << gantry0_y<< " : " << gantry0_z<< std::endl;
        graph->SetPoint(count,count,phidg0_Btot);
        count++;    
    }
    
    file221.cd();
    
    // SetBRanchAddress returns the adress of the specified branch.
    scan_tree.SetBranchAddress("time",&time);
    scan_tree.SetBranchAddress("gantry0_x",&gantry0_x);
    scan_tree.SetBranchAddress("gantry0_y",&gantry0_y);
    scan_tree.SetBranchAddress("gantry0_z",&gantry0_z);
    scan_tree.SetBranchAddress("phidg0_Bx",&phidg0_Bx);
    scan_tree.SetBranchAddress("phidg0_By",&phidg0_By);
    scan_tree.SetBranchAddress("phidg0_Bz",&phidg0_Bz);
    scan_tree.SetBranchAddress("phidg0_Btot",&phidg0_Btot);
    
    int entries = scan_tree->GetEntries();
    
    
    for(int ient = 2; ient < entries; ient++){    
        scan_tree->GetEntry(ient);
        graph1->SetPoint(count1,count1,phidg0_Btot);
        count1++;
    }
    
    graph->SetMarkerStyle(20);
    graph1->SetMarkerStyle(21);
    graph->SetMarkerColor(1);
    graph1->SetMarkerColor(2);
    graph->SetLineColor(1);
    graph1->SetLineColor(2);
    
    Mgraph->Add(graph);
    Mgraph->Add(graph1);
    Mgraph->Draw("APL");
    Mgraph->SetTitle("Variation of magnetic field magnitude with time at specified point");
    Mgraph->GetYaxis().SetTitle("Magnetic Field Magnitude(Gauss)");
    Mgraph->GetYaxis().SetTitleOffset(1.3);
    Mgraph->GetYaxis().CenterTitle();   
    Mgraph->GetXaxis().SetTitle("Data point Number");
    Mgraph->GetXaxis().CenterTitle();
    
    legend->AddEntry(graph, "Point(0.32m, 0.3m, 0.3m)", "lp");
    legend->AddEntry(graph1, "Point(0.1m, 0.1m, 0m)", "lp");
    legend->Draw();
}
