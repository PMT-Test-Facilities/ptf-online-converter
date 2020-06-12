bool check_for_point(double x,double y, double z, double range, double pos_x, double pos_y, double pos_z ){
 return ( ( (pos_x < x+range)&&(pos_x> x-range) )&&( (pos_y <y+range)&&(pos_y>y-range) )&&( (pos_z <z+range) && (pos_z > z-range) ) );
}


void plot_2D_Hist_MFPositionVar(){
    // Data files being considered
    TFile *file219 = new TFile("output00219.root");  //original scan
    TFile *file220 = new TFile("output00220.root");  //time dependence measurement
    TFile *file221 = new TFile("output00221.root");  //time dependencd measurement
    TFile *file223 = new TFile("output00223.root");  // original scan
    TFile *file225 = new TFile("output00225.root");  // Ben's modified scan
    TFile *file226 = new TFile("output00226.root");  // Ben's modified scan
    
    // Variables available in root file 
    int time;
    int gantry_event;
    int gantry_subevent;
    double gantry0_x;
    double gantry0_y;
    double gantry0_z;
    double gantry0_tilt;
    double gantry0_rot;
    double phidg0_tilt;
    double gantry1_x;
    double gantry1_y;
    double gantry1_z;
    double gantry1_tilt;
    double gantry1_rot;
    double phidg1_tilt;
    int phidg_event;
    double phidg0_Bx;
    double phidg0_By;
    double phidg0_Bz;
    double phidg0_Btot;
    double phidg1_Bx;
    double phidg1_By;
    double phidg1_Bz;
    double phidg1_Btot;
    int coil_event;
    double I_coil0;
    double I_coil1;
    double I_coil2;    
    double I_coil3;    
    double I_coil4;    
    double I_coil5;    
    double U_coil0;
    double U_coil1;
    double U_coil2;    
    double U_coil3;    
    double U_coil4;    
    double U_coil5;    
    
    //Variables for Analysis purposes
    int num_entries;
    int num_bin = 30;
    double z_low = 0.35;
    double z_high = 0.45;
    
    double Min_zval = 1.1;
    double Max_zval = 1.7; 
    
    // title on stats box,title, (xaxis) number of bins, low, high, y(axis) number of bins, low, high
    TH2D *histo1 = new TH2D("Statistics","Magnetic Field Magnitude at (x,y) position, Z=0.3m", num_bin,0,0.64,num_bin,0,0.62);
    // title on stats box,title, (xaxis) number of bins, low, high, y(axis) number of bins, low, high
    TH2D *histo2 = new TH2D("Statistics","Magnetic Field Magnitude at (x,y) position, Z=0.3m", num_bin,0,0.64,num_bin,0,0.62);
    TH2D *histo3 = new TH2D("Statistics","Magnetic Field Magnitude at (x,y) position, Z=0.4m", num_bin,0,0.64,num_bin,0,0.62);
    TH2D *histo4 = new TH2D("Statistics","Magnetic Field Magnitude at (x,y) position, Z=0.4m", num_bin,0,0.64,num_bin,0,0.62);
    TH2D *histo4_0 = new TH2D("Statistics","Magnetic Field Magnitude at (x,y) position, Z=0", num_bin,0,0.64,num_bin,0,0.62);
    TH2D *histo4_1 = new TH2D("Statistics","Magnetic Field Magnitude at (x,y) position, Z=0.1", num_bin,0,0.64,num_bin,0,0.62);
    TH2D *histo4_2 = new TH2D("Statistics","Magnetic Field Magnitude at (x,y) position, Z=0.2", num_bin,0,0.64,num_bin,0,0.62);
    TH2D *histo4_3 = new TH2D("Statistics","Magnetic Field Magnitude at (x,y) position, Z=0.3", num_bin,0,0.64,num_bin,0,0.62);
    TH2D *histo4_4 = new TH2D("Statistics","Magnetic Field Magnitude at (x,y) position, Z=0.4", num_bin,0,0.64,num_bin,0,0.62);

    file219->cd();
    
    // SetBRanchAddress returns the adress of the specified branch->
    scan_tree->SetBranchAddress("time", &time);
    scan_tree->SetBranchAddress("gantry_event", &gantry_event);
    scan_tree->SetBranchAddress("gantry_subevent", &gantry_subevent);
    scan_tree->SetBranchAddress("gantry0_x", &gantry0_x);
    scan_tree->SetBranchAddress("gantry0_y", &gantry0_y);
    scan_tree->SetBranchAddress("gantry0_z", &gantry0_z);
    scan_tree->SetBranchAddress("gantry0_tilt", &gantry0_tilt);
    scan_tree->SetBranchAddress("gantry0_rot", &gantry0_rot);
    scan_tree->SetBranchAddress("phidg0_tilt", &phidg0_tilt);
    scan_tree->SetBranchAddress("gantry1_x", &gantry1_x);
    scan_tree->SetBranchAddress("gantry1_y", &gantry1_y);
    scan_tree->SetBranchAddress("gantry1_z", &gantry1_z);
    scan_tree->SetBranchAddress("gantry1_tilt", &gantry1_tilt);
    scan_tree->SetBranchAddress("gantry1_rot", &gantry1_rot);
    scan_tree->SetBranchAddress("phidg1_tilt", &phidg1_tilt);
    scan_tree->SetBranchAddress("phidg_event", &phidg_event);
    scan_tree->SetBranchAddress("phidg0_Bx", &phidg0_Bx);
    scan_tree->SetBranchAddress("phidg0_By", &phidg0_By);
    scan_tree->SetBranchAddress("phidg0_Bz", &phidg0_Bz);
    scan_tree->SetBranchAddress("phidg0_Btot", &phidg0_Btot);
    scan_tree->SetBranchAddress("phidg1_Bx", &phidg1_Bx);
    scan_tree->SetBranchAddress("phidg1_By", &phidg1_By);
    scan_tree->SetBranchAddress("phidg1_Bz", &phidg1_Bz);
    scan_tree->SetBranchAddress("phidg1_Btot", &phidg1_Btot);
    scan_tree->SetBranchAddress("coil_event", &coil_event);
    scan_tree->SetBranchAddress("I_coil0", &I_coil0);
    scan_tree->SetBranchAddress("I_coil1", &I_coil1);
    scan_tree->SetBranchAddress("I_coil2", &I_coil2);
    scan_tree->SetBranchAddress("I_coil3", &I_coil3);
    scan_tree->SetBranchAddress("I_coil4", I_coil4);
    scan_tree->SetBranchAddress("I_coil5", I_coil5);
    scan_tree->SetBranchAddress("U_coil0", U_coil0);
    scan_tree->SetBranchAddress("U_coil1", U_coil1);
    scan_tree->SetBranchAddress("U_coil2", U_coil2);
    scan_tree->SetBranchAddress("U_coil3", U_coil3);
    scan_tree->SetBranchAddress("U_coil4", U_coil4);
    scan_tree->SetBranchAddress("U_coil5", U_coil5);
    
    num_entries = scan_tree->GetEntries();
     
    for(int ient = 0; ient < num_entries; ient++){    
        scan_tree->GetEntry(ient);
        if(gantry0_z > z_low && gantry0_z < z_high)
        histo1->Fill(gantry0_x,gantry0_y,(phidg0_Btot*0.5));
     }

    file223->cd();

    scan_tree->SetBranchAddress("gantry0_x", &gantry0_x);
    scan_tree->SetBranchAddress("gantry0_y", &gantry0_y);
    scan_tree->SetBranchAddress("gantry0_z", &gantry0_z);
    scan_tree->SetBranchAddress("phidg0_Bx", &phidg0_Bx);
    scan_tree->SetBranchAddress("phidg0_By", &phidg0_By);
    scan_tree->SetBranchAddress("phidg0_Bz", &phidg0_Bz);
    scan_tree->SetBranchAddress("phidg0_Btot", &phidg0_Btot);

    num_entries = scan_tree->GetEntries();

    for(int ient = 0; ient < num_entries; ient++){    
        scan_tree->GetEntry(ient);
        if(gantry0_z > z_low && gantry0_z < z_high)
        histo2->Fill(gantry0_x,gantry0_y,(phidg0_Btot*0.5));
    }

    file225->cd();

    scan_tree->SetBranchAddress("gantry0_x", &gantry0_x);
    scan_tree->SetBranchAddress("gantry0_y", &gantry0_y);
    scan_tree->SetBranchAddress("gantry0_z", &gantry0_z);
    scan_tree->SetBranchAddress("phidg0_Bx", &phidg0_Bx);
    scan_tree->SetBranchAddress("phidg0_By", &phidg0_By);
    scan_tree->SetBranchAddress("phidg0_Bz", &phidg0_Bz);
    scan_tree->SetBranchAddress("phidg0_Btot", &phidg0_Btot);

    num_entries = scan_tree->GetEntries();
     
    for(int ient = 0; ient < num_entries; ient++){    
        scan_tree->GetEntry(ient);
        if(gantry0_z > z_low && gantry0_z < z_high)
        histo3->Fill(gantry0_x,gantry0_y,(phidg0_Btot*0.5));
    }
 

    file226->cd();

    scan_tree->SetBranchAddress("gantry0_x", &gantry0_x);
    scan_tree->SetBranchAddress("gantry0_y", &gantry0_y);
    scan_tree->SetBranchAddress("gantry0_z", &gantry0_z);
    scan_tree->SetBranchAddress("phidg0_Bx", &phidg0_Bx);
    scan_tree->SetBranchAddress("phidg0_By", &phidg0_By);
    scan_tree->SetBranchAddress("phidg0_Bz", &phidg0_Bz);
    scan_tree->SetBranchAddress("phidg0_Btot", &phidg0_Btot);

    num_entries = scan_tree->GetEntries();
     
    for(int ient = 0; ient < num_entries; ient++){    
        scan_tree->GetEntry(ient);
        
        // z Variable
        if(gantry0_z > z_low && gantry0_z < z_high)
            if(check_for_point( 0.32 , 0.2 , (z_high-0.05) , 0.01 , gantry0_x , gantry0_y , gantry0_z ) )
                histo4->Fill(gantry0_x,gantry0_y,(phidg0_Btot*0.25));
            else
                histo4->Fill(gantry0_x,gantry0_y,(phidg0_Btot*0.5));

        // z = 0      
        if(gantry0_z > -0.05 && gantry0_z < 0.05)
            histo4_0->Fill(gantry0_x,gantry0_y,(phidg0_Btot*0.5));
            
        // z = 0.1
        if(gantry0_z > 0.05 && gantry0_z < 0.15)
            histo4_1->Fill(gantry0_x,gantry0_y,(phidg0_Btot*0.5));
        
        // z = 0->2
        if(gantry0_z > 0.15 && gantry0_z < 0.25)
            histo4_2->Fill(gantry0_x,gantry0_y,(phidg0_Btot*0.5));

        // z = 0->3
        if(gantry0_z > 0.25 && gantry0_z < 0.35)
            histo4_3->Fill(gantry0_x,gantry0_y,(phidg0_Btot*0.5));

        // z = 0->4      
        if(gantry0_z > 0.35 && gantry0_z < 0.45)
            histo4_4->Fill(gantry0_x,gantry0_y,(phidg0_Btot*0.5));
    }
 
    histo1->GetXaxis()->SetTitle("x-axis (m)");
    histo1->GetYaxis()->SetTitle("y-axis (m)");
    histo1->GetZaxis()->SetTitle("Magnetic Field Magnitude (Gauss)");
    histo1->GetXaxis()->CenterTitle();
    histo1->GetYaxis()->CenterTitle();
    histo1->GetZaxis()->CenterTitle();    
    histo1->SetMinimum(Min_zval);
    histo1->SetMaximum(Max_zval);
    histo1->SetStats(kFALSE);
  
    histo2->GetXaxis()->SetTitle("x-axis (m)");
    histo2->GetYaxis()->SetTitle("y-axis (m)");
    histo2->GetZaxis()->SetTitle("Magnetic Field Magnitude (Gauss)");    
    histo2->GetXaxis()->CenterTitle();
    histo2->GetYaxis()->CenterTitle();
    histo2->GetZaxis()->CenterTitle();
    histo2->SetMinimum(Min_zval);
    histo2->SetMaximum(Max_zval);
    histo2->SetStats(kFALSE);
       
    histo3->GetXaxis()->SetTitle("x-axis (m)");
    histo3->GetYaxis()->SetTitle("y-axis (m)");
    histo3->GetZaxis()->SetTitle("Magnetic Field Magnitude (Gauss)");
    histo3->GetXaxis()->CenterTitle();
    histo3->GetYaxis()->CenterTitle();
    histo3->GetZaxis()->CenterTitle();
    histo3->SetMinimum(Min_zval);
    histo3->SetMaximum(Max_zval);
    histo3->SetStats(kFALSE); 
 
    histo4->GetXaxis()->SetTitle("x-axis(m)");
    histo4->GetYaxis()->SetTitle("y-axis(m)");
    histo4->GetZaxis()->SetTitle("Magnetic Field Magnitude (Gauss)");
    histo4->GetXaxis()->CenterTitle();
    histo4->GetYaxis()->CenterTitle();
    histo4->GetZaxis()->CenterTitle();
    //histo4->GetYaxis()->SetTitleOffset(1.5);
    //histo4->GetZaxis()->SetTitleOffset(1.5);
    histo4->SetMinimum(Min_zval);
    histo4->SetMaximum(Max_zval);
    histo4->SetStats(kFALSE);
    
    histo4_0->GetXaxis()->SetTitle("x-axis(m)");
    histo4_0->GetYaxis()->SetTitle("y-axis(m)");
    histo4_0->GetZaxis()->SetTitle("Magnetic Field Magnitude (Gauss)");
    histo4_0->GetXaxis()->CenterTitle();
    histo4_0->GetYaxis()->CenterTitle();
    histo4_0->GetZaxis()->CenterTitle();
    histo4_0->GetYaxis()->SetTitleOffset(1.5);
    histo4_0->GetZaxis()->SetTitleOffset(2);
    histo4_0->SetMinimum(Min_zval);
    histo4_0->SetMaximum(Max_zval);
    histo4_0->SetStats(kFALSE);    
    
    histo4_1->GetXaxis()->SetTitle("x-axis(m)");
    histo4_1->GetYaxis()->SetTitle("y-axis(m)");
    histo4_1->GetZaxis()->SetTitle("Magnetic Field Magnitude (Gauss)");
    histo4_1->GetXaxis()->CenterTitle();
    histo4_1->GetYaxis()->CenterTitle();
    histo4_1->GetZaxis()->CenterTitle();
    histo4_1->GetYaxis()->SetTitleOffset(1.5);
    histo4_1->GetZaxis()->SetTitleOffset(2);
    histo4_1->SetMinimum(Min_zval);
    histo4_1->SetMaximum(Max_zval);
    histo4_1->SetStats(kFALSE);       

    histo4_2->GetXaxis()->SetTitle("x-axis(m)");
    histo4_2->GetYaxis()->SetTitle("y-axis(m)");
    histo4_2->GetZaxis()->SetTitle("Magnetic Field Magnitude (Gauss)");
    histo4_2->GetXaxis()->CenterTitle();
    histo4_2->GetYaxis()->CenterTitle();
    histo4_2->GetZaxis()->CenterTitle();
    histo4_2->GetYaxis()->SetTitleOffset(1.5);
    histo4_2->GetZaxis()->SetTitleOffset(2);
    histo4_2->SetMinimum(Min_zval);
    histo4_2->SetMaximum(Max_zval);
    histo4_2->SetStats(kFALSE);
            
    histo4_3->GetXaxis()->SetTitle("x-axis(m)");
    histo4_3->GetYaxis()->SetTitle("y-axis(m)");
    histo4_3->GetZaxis()->SetTitle("Magnetic Field Magnitude (Gauss)");
    histo4_3->GetXaxis()->CenterTitle();
    histo4_3->GetYaxis()->CenterTitle();
    histo4_3->GetZaxis()->CenterTitle();
//  histo4_3->GetYaxis()->SetTitleOffset(1.5);
    histo4_3->GetZaxis()->SetTitleOffset(1.5);
    histo4_3->SetMinimum(Min_zval);
    histo4_3->SetMaximum(Max_zval);
    histo4_3->SetStats(kFALSE);
            
    histo4_4->GetXaxis()->SetTitle("x-axis(m)");
    histo4_4->GetYaxis()->SetTitle("y-axis (m)");
    histo4_4->GetZaxis()->SetTitle("Magnetic Field Magnitude (Gauss)");
    histo4_4->GetXaxis()->CenterTitle();
    histo4_4->GetYaxis()->CenterTitle();
    histo4_4->GetZaxis()->CenterTitle();
//  histo4_4->GetYaxis()->SetTitleOffset(0.5);
    histo4_4->GetZaxis()->SetTitleOffset(1.5);
    histo4_4->SetMinimum(Min_zval);
    histo4_4->SetMaximum(Max_zval);
    histo4_4->SetStats(kFALSE);    


    TCanvas *canvas1 = new TCanvas("canvas1","Graph",0,0,1280,948);
    
// for 2 graphs
    canvas1->Divide(1,2);
    canvas1->cd(1);
    canvas1->cd(1)->SetRightMargin(0.2);
    histo3->Draw("zcol");
    canvas1->cd(2)->SetRightMargin(0.2);
    canvas1->cd(2);
    histo4->Draw("zcol");    

// for 5 graphs
/*
    canvas1->Divide(1,2);
    canvas1->cd(1)->Divide(3,1);    
    canvas1->cd(2)->Divide(2,1);
    canvas1->cd(1)->cd(1);
    canvas1->cd(1)->cd(1)->SetRightMargin(0.2);
    histo4_0->Draw("zcol");
    canvas1->cd(1)->cd(2);
    canvas1->cd(1)->cd(2)->SetRightMargin(0.2);
    histo4_1->Draw("zcol");
    canvas1->cd(1)->cd(3);
    canvas1->cd(1)->cd(3)->SetRightMargin(0.2);
    histo4_2->Draw("zcol");
    canvas1->cd(2)->cd(1);
    canvas1->cd(2)->cd(1)->SetRightMargin(0.15);
    histo4_3->Draw("zcol");
    canvas1->cd(2)->cd(2);
    canvas1->cd(2)->cd(2)->SetRightMargin(0.15);
    histo4_4->Draw("zcol");
//*/ 
   
           
}




