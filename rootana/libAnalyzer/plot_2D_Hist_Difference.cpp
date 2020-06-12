{
    // Data files being considered
    TFile *file219 = new TFile("output00219.root");  //original scan
    TFile *file220 = new TFile("output00220.root");  //time dependence measurement
    TFile *file221 = new TFile("output00221.root");  //time dependencd measurement
    TFile *file223 = new TFile("output00223.root");  // original scan
    TFile *file225 = new TFile("output00225.root");  // Ben's modified scan
    TFile *file226 = new TFile("output00226.root");  // Ben's modified scan
    
    // Set file to work with

       
    // Variables available in root file 
    int time_f1, time_f2,time_f3,time_f4;
    int gantry_event_f1,gantry_event_f2,gantry_event_f3,gantry_event_f4;
    int gantry_subevent_f1,gantry_subevent_f2,gantry_subevent_f3,gantry_subevent_f4;
    double gantry0_x_f1,gantry0_x_f2,gantry0_x_f3,gantry0_x_f4;
    double gantry0_y_f1, gantry0_y_f2,gantry0_y_f3,gantry0_y_f4;
    double gantry0_z_f1, gantry0_z_f2,gantry0_z_f3,gantry0_z_f4;
    double gantry0_tilt_f1, gantry0_tilt_f2,gantry0_tilt_f3,gantry0_tilt_f4;
    double gantry0_rot_f1, gantry0_rot_f2,gantry0_rot_f3,gantry0_rot_f4;
    double phidg0_tilt_f1, phidg0_tilt_f2,phidg0_tilt_f3,phidg0_tilt_f4;
    double gantry1_x_f1, gantry1_x_f2,gantry1_x_f3,gantry1_x_f4;
    double gantry1_y_f1, gantry1_y_f2,gantry1_y_f3,gantry1_y_f4;
    double gantry1_z_f1, gantry1_z_f2,gantry1_z_f3,gantry1_z_f4;
    double gantry1_tilt_f1, gantry1_tilt_f2,gantry1_tilt_f3,gantry1_tilt_f4;
    double gantry1_rot_f1, gantry1_rot_f2,gantry1_rot_f3,gantry1_rot_f4;
    double phidg1_tilt_f1, phidg1_tilt_f2,phidg1_tilt_f3,phidg1_tilt_f4;
    int phidg_event_f1, phidg_event_f2,phidg_event_f3,phidg_event_f4;
    double phidg0_Bx_f1, phidg0_Bx_f2,phidg0_Bx_f3,phidg0_Bx_f4;
    double phidg0_By_f1, phidg0_By_f2,phidg0_By_f3,phidg0_By_f4;
    double phidg0_Bz_f1, phidg0_Bz_f2,phidg0_Bz_f3,phidg0_Bz_f4;
    double phidg0_Btot_f1, phidg0_Btot_f2,phidg0_Btot_f3,phidg0_Btot_f4;
    double phidg1_Bx_f1, phidg1_Bx_f2,phidg1_Bx_f3,phidg1_Bx_f4;
    double phidg1_By_f1, phidg1_By_f2,phidg1_By_f3,phidg1_By_f4;
    double phidg1_Bz_f1, phidg1_Bz_f2,phidg1_Bz_f3,phidg1_Bz_f4;
    double phidg1_Btot_f1, phidg1_Btot_f2,phidg1_Btot_f3,phidg1_Btot_f4;
    int coil_event_f1, coil_event_f2,coil_event_f3,coil_event_f4;
    double I_coil0_f1, I_coil0_f2,I_coil0_f3,I_coil0_f4;
    double I_coil1_f1, I_coil1_f2,I_coil1_f3,I_coil1_f4;
    double I_coil2_f1, I_coil2_f2,I_coil2_f3,I_coil2_f4;    
    double I_coil3_f1, I_coil3_f2,I_coil3_f3,I_coil3_f4;    
    double I_coil4_f1, I_coil4_f2,I_coil4_f3,I_coil4_f4;    
    double I_coil5_f1, I_coil5_f2,I_coil5_f3,I_coil5_f4;    
    double U_coil0_f1, U_coil0_f2,U_coil0_f3,U_coil0_f4;
    double U_coil1_f1, U_coil1_f2,U_coil1_f3,U_coil1_f4;
    double U_coil2_f1, U_coil2_f2,U_coil2_f3,U_coil2_f4;    
    double U_coil3_f1, U_coil3_f2,U_coil3_f3,U_coil3_f4;    
    double U_coil4_f1, U_coil4_f2,U_coil4_f3,U_coil4_f4;    
    double U_coil5_f1, U_coil5_f2,U_coil5_f3,U_coil5_f4;    
    
    
    //Variables for Analysis purposes
    int num_entries;
    int num_bin = 30;
    double z_low = 0.25;
    double z_high = 0.35;
    


    file219.cd();
    ///*
    // SetBRanchAddress returns the adress of the specified branch.
    scan_tree.SetBranchAddress("time", &time_f1);
    scan_tree.SetBranchAddress("gantry_event", &gantry_event_f1);
    scan_tree.SetBranchAddress("gantry_subevent", &gantry_subevent_f1);
    scan_tree.SetBranchAddress("gantry0_x", &gantry0_x_f1);
    scan_tree.SetBranchAddress("gantry0_y", &gantry0_y_f1);
    scan_tree.SetBranchAddress("gantry0_z", &gantry0_z_f1);
    scan_tree.SetBranchAddress("gantry0_tilt", &gantry0_tilt_f1);
    scan_tree.SetBranchAddress("gantry0_rot", &gantry0_rot_f1);
    scan_tree.SetBranchAddress("phidg0_tilt", &phidg0_tilt_f1);
    scan_tree.SetBranchAddress("gantry1_x", &gantry1_x_f1);
    scan_tree.SetBranchAddress("gantry1_y", &gantry1_y_f1);
    scan_tree.SetBranchAddress("gantry1_z", &gantry1_z_f1);
    scan_tree.SetBranchAddress("gantry1_tilt", &gantry1_tilt_f1);
    scan_tree.SetBranchAddress("gantry1_rot", &gantry1_rot_f1);
    scan_tree.SetBranchAddress("phidg1_tilt", &phidg1_tilt_f1);
    scan_tree.SetBranchAddress("phidg_event", &phidg_event_f1);
    scan_tree.SetBranchAddress("phidg0_Bx", &phidg0_Bx_f1);
    scan_tree.SetBranchAddress("phidg0_By", &phidg0_By_f1);
    scan_tree.SetBranchAddress("phidg0_Bz", &phidg0_Bz_f1);
    scan_tree.SetBranchAddress("phidg0_Btot", &phidg0_Btot_f1);
    scan_tree.SetBranchAddress("phidg1_Bx", &phidg1_Bx_f1);
    scan_tree.SetBranchAddress("phidg1_By", &phidg1_By_f1);
    scan_tree.SetBranchAddress("phidg1_Bz", &phidg1_Bz_f1);
    scan_tree.SetBranchAddress("phidg1_Btot", &phidg1_Btot_f1);
    scan_tree.SetBranchAddress("coil_event", &coil_event_f1);
    scan_tree.SetBranchAddress("I_coil0", &I_coil0_f1);
    scan_tree.SetBranchAddress("I_coil1", &I_coil1_f1);
    scan_tree.SetBranchAddress("I_coil2", &I_coil2_f1);
    scan_tree.SetBranchAddress("I_coil3", &I_coil3_f1);
    scan_tree.SetBranchAddress("I_coil4", I_coil4_f1);
    scan_tree.SetBranchAddress("I_coil5", I_coil5_f1);
    scan_tree.SetBranchAddress("U_coil0", U_coil0_f1);
    scan_tree.SetBranchAddress("U_coil1", U_coil1_f1);
    scan_tree.SetBranchAddress("U_coil2", U_coil2_f1);
    scan_tree.SetBranchAddress("U_coil3", U_coil3_f1);
    scan_tree.SetBranchAddress("U_coil4", U_coil4_f1);
    scan_tree.SetBranchAddress("U_coil5", U_coil5_f1);


    file223.cd();
    ///*
    // SetBRanchAddress returns the adress of the specified branch.
    scan_tree.SetBranchAddress("time", &time_f2);
    scan_tree.SetBranchAddress("gantry_event", &gantry_event_f2);
    scan_tree.SetBranchAddress("gantry_subevent", &gantry_subevent_f2);
    scan_tree.SetBranchAddress("gantry0_x", &gantry0_x_f2);
    scan_tree.SetBranchAddress("gantry0_y", &gantry0_y_f2);
    scan_tree.SetBranchAddress("gantry0_z", &gantry0_z_f2);
    scan_tree.SetBranchAddress("gantry0_tilt", &gantry0_tilt_f2);
    scan_tree.SetBranchAddress("gantry0_rot", &gantry0_rot_f2);
    scan_tree.SetBranchAddress("phidg0_tilt", &phidg0_tilt_f2);
    scan_tree.SetBranchAddress("gantry1_x", &gantry1_x_f2);
    scan_tree.SetBranchAddress("gantry1_y", &gantry1_y_f2);
    scan_tree.SetBranchAddress("gantry1_z", &gantry1_z_f2);
    scan_tree.SetBranchAddress("gantry1_tilt", &gantry1_tilt_f2);
    scan_tree.SetBranchAddress("gantry1_rot", &gantry1_rot_f2);
    scan_tree.SetBranchAddress("phidg1_tilt", &phidg1_tilt_f2);
    scan_tree.SetBranchAddress("phidg_event", &phidg_event_f2);
    scan_tree.SetBranchAddress("phidg0_Bx", &phidg0_Bx_f2);
    scan_tree.SetBranchAddress("phidg0_By", &phidg0_By_f2);
    scan_tree.SetBranchAddress("phidg0_Bz", &phidg0_Bz_f2);
    scan_tree.SetBranchAddress("phidg0_Btot", &phidg0_Btot_f2);
    scan_tree.SetBranchAddress("phidg1_Bx", &phidg1_Bx_f2);
    scan_tree.SetBranchAddress("phidg1_By", &phidg1_By_f2);
    scan_tree.SetBranchAddress("phidg1_Bz", &phidg1_Bz_f2);
    scan_tree.SetBranchAddress("phidg1_Btot", &phidg1_Btot_f2);
    scan_tree.SetBranchAddress("coil_event", &coil_event_f2);
    scan_tree.SetBranchAddress("I_coil0", &I_coil0_f2);
    scan_tree.SetBranchAddress("I_coil1", &I_coil1_f2);
    scan_tree.SetBranchAddress("I_coil2", &I_coil2_f2);
    scan_tree.SetBranchAddress("I_coil3", &I_coil3_f2);
    scan_tree.SetBranchAddress("I_coil4", I_coil4_f2);
    scan_tree.SetBranchAddress("I_coil5", I_coil5_f2);
    scan_tree.SetBranchAddress("U_coil0", U_coil0_f2);
    scan_tree.SetBranchAddress("U_coil1", U_coil1_f2);
    scan_tree.SetBranchAddress("U_coil2", U_coil2_f2);
    scan_tree.SetBranchAddress("U_coil3", U_coil3_f2);
    scan_tree.SetBranchAddress("U_coil4", U_coil4_f2);
    scan_tree.SetBranchAddress("U_coil5", U_coil5_f2);
    
    
    file225.cd();
    ///*
    // SetBRanchAddress returns the adress of the specified branch.
    scan_tree.SetBranchAddress("time", &time_f3);
    scan_tree.SetBranchAddress("gantry_event", &gantry_event_f3);
    scan_tree.SetBranchAddress("gantry_subevent", &gantry_subevent_f3);
    scan_tree.SetBranchAddress("gantry0_x", &gantry0_x_f3);
    scan_tree.SetBranchAddress("gantry0_y", &gantry0_y_f3);
    scan_tree.SetBranchAddress("gantry0_z", &gantry0_z_f3);
    scan_tree.SetBranchAddress("gantry0_tilt", &gantry0_tilt_f3);
    scan_tree.SetBranchAddress("gantry0_rot", &gantry0_rot_f3);
    scan_tree.SetBranchAddress("phidg0_tilt", &phidg0_tilt_f3);
    scan_tree.SetBranchAddress("gantry1_x", &gantry1_x_f3);
    scan_tree.SetBranchAddress("gantry1_y", &gantry1_y_f3);
    scan_tree.SetBranchAddress("gantry1_z", &gantry1_z_f3);
    scan_tree.SetBranchAddress("gantry1_tilt", &gantry1_tilt_f3);
    scan_tree.SetBranchAddress("gantry1_rot", &gantry1_rot_f3);
    scan_tree.SetBranchAddress("phidg1_tilt", &phidg1_tilt_f3);
    scan_tree.SetBranchAddress("phidg_event", &phidg_event_f3);
    scan_tree.SetBranchAddress("phidg0_Bx", &phidg0_Bx_f3);
    scan_tree.SetBranchAddress("phidg0_By", &phidg0_By_f3);
    scan_tree.SetBranchAddress("phidg0_Bz", &phidg0_Bz_f3);
    scan_tree.SetBranchAddress("phidg0_Btot", &phidg0_Btot_f3);
    scan_tree.SetBranchAddress("phidg1_Bx", &phidg1_Bx_f3);
    scan_tree.SetBranchAddress("phidg1_By", &phidg1_By_f3);
    scan_tree.SetBranchAddress("phidg1_Bz", &phidg1_Bz_f3);
    scan_tree.SetBranchAddress("phidg1_Btot", &phidg1_Btot_f3);
    scan_tree.SetBranchAddress("coil_event", &coil_event_f3);
    scan_tree.SetBranchAddress("I_coil0", &I_coil0_f3);
    scan_tree.SetBranchAddress("I_coil1", &I_coil1_f3);
    scan_tree.SetBranchAddress("I_coil2", &I_coil2_f3);
    scan_tree.SetBranchAddress("I_coil3", &I_coil3_f3);
    scan_tree.SetBranchAddress("I_coil4", I_coil4_f3);
    scan_tree.SetBranchAddress("I_coil5", I_coil5_f3);
    scan_tree.SetBranchAddress("U_coil0", U_coil0_f3);
    scan_tree.SetBranchAddress("U_coil1", U_coil1_f3);
    scan_tree.SetBranchAddress("U_coil2", U_coil2_f3);
    scan_tree.SetBranchAddress("U_coil3", U_coil3_f3);
    scan_tree.SetBranchAddress("U_coil4", U_coil4_f3);
    scan_tree.SetBranchAddress("U_coil5", U_coil5_f3);
    
    
    
    file226.cd();
    ///*
    // SetBRanchAddress returns the adress of the specified branch.
    scan_tree.SetBranchAddress("time", &time_f4);
    scan_tree.SetBranchAddress("gantry_event", &gantry_event_f4);
    scan_tree.SetBranchAddress("gantry_subevent", &gantry_subevent_f4);
    scan_tree.SetBranchAddress("gantry0_x", &gantry0_x_f4);
    scan_tree.SetBranchAddress("gantry0_y", &gantry0_y_f4);
    scan_tree.SetBranchAddress("gantry0_z", &gantry0_z_f4);
    scan_tree.SetBranchAddress("gantry0_tilt", &gantry0_tilt_f4);
    scan_tree.SetBranchAddress("gantry0_rot", &gantry0_rot_f4);
    scan_tree.SetBranchAddress("phidg0_tilt", &phidg0_tilt_f4);
    scan_tree.SetBranchAddress("gantry1_x", &gantry1_x_f4);
    scan_tree.SetBranchAddress("gantry1_y", &gantry1_y_f4);
    scan_tree.SetBranchAddress("gantry1_z", &gantry1_z_f4);
    scan_tree.SetBranchAddress("gantry1_tilt", &gantry1_tilt_f4);
    scan_tree.SetBranchAddress("gantry1_rot", &gantry1_rot_f4);
    scan_tree.SetBranchAddress("phidg1_tilt", &phidg1_tilt_f4);
    scan_tree.SetBranchAddress("phidg_event", &phidg_event_f4);
    scan_tree.SetBranchAddress("phidg0_Bx", &phidg0_Bx_f4);
    scan_tree.SetBranchAddress("phidg0_By", &phidg0_By_f4);
    scan_tree.SetBranchAddress("phidg0_Bz", &phidg0_Bz_f4);
    scan_tree.SetBranchAddress("phidg0_Btot", &phidg0_Btot_f4);
    scan_tree.SetBranchAddress("phidg1_Bx", &phidg1_Bx_f4);
    scan_tree.SetBranchAddress("phidg1_By", &phidg1_By_f4);
    scan_tree.SetBranchAddress("phidg1_Bz", &phidg1_Bz_f4);
    scan_tree.SetBranchAddress("phidg1_Btot", &phidg1_Btot_f4);
    scan_tree.SetBranchAddress("coil_event", &coil_event_f4);
    scan_tree.SetBranchAddress("I_coil0", &I_coil0_f4);
    scan_tree.SetBranchAddress("I_coil1", &I_coil1_f4);
    scan_tree.SetBranchAddress("I_coil2", &I_coil2_f4);
    scan_tree.SetBranchAddress("I_coil3", &I_coil3_f4);
    scan_tree.SetBranchAddress("I_coil4", I_coil4_f4);
    scan_tree.SetBranchAddress("I_coil5", I_coil5_f4);
    scan_tree.SetBranchAddress("U_coil0", U_coil0_f4);
    scan_tree.SetBranchAddress("U_coil1", U_coil1_f4);
    scan_tree.SetBranchAddress("U_coil2", U_coil2_f4);
    scan_tree.SetBranchAddress("U_coil3", U_coil3_f4);
    scan_tree.SetBranchAddress("U_coil4", U_coil4_f4);
    scan_tree.SetBranchAddress("U_coil5", U_coil5_f4);
    
    
    file219.cd();
    num_entries = scan_tree.GetEntries();

    // f1 - 219 - original
    // f2 - 223 - original
    // f3 - 225 - modified
    // f4 - 226 - modified
    
    TH2D *histo1 = new TH2D("Statistics","Difference of Magnetic Field Magnitude, Z=0m", num_bin,0,0.64,num_bin,0,0.62);
    TH2D *histo2 = new TH2D("Statistics","Difference of Magnetic Field Magnitude, Z=0.3m", num_bin,0,0.66,num_bin,0,0.62);
    TH2D *histo3 = new TH2D("Statistics","Difference of Magnetic Field Magnitude, Z=0.2m", num_bin,0,0.66,num_bin,0,0.62);
    TH2D *histo4 = new TH2D("Statistics","Difference of Magnetic Field Magnitude, Z=0.3m", num_bin,0,0.66,num_bin,0,0.62);
    TH2D *histo5 = new TH2D("Statistics","Difference of Magnetic Field Magnitude, Z=0.4m", num_bin,0,0.66,num_bin,0,0.62);
   

     
    for(int ient = 0; ient < num_entries; ient++){    
        file219.cd();
        scan_tree->GetEntry(ient);
        
        file223.cd();
        scan_tree->GetEntry(ient);

        if( (gantry0_x_f1<0.45) && (gantry0_x_f1>0.41) && (gantry0_y_f1<0.23) && (gantry0_y_f1>0.19) )
          ;
        else if( ( (gantry0_x_f1<0.53) && (gantry0_x_f1>0.50) ) && ( (gantry0_y_f1<0.23) && (gantry0_y_f1>0.19) ) )
        ;
        else if( ( (gantry0_x_f1<0.63) && (gantry0_x_f1>0.61) ) && ( (gantry0_y_f1<0.23) && (gantry0_y_f1>0.19) ) )
        ;
        else{                
            if(gantry0_z_f1 > -0.05 && gantry0_z_f1 < 0.05){
                if ((gantry0_x_f1<0.02) && (gantry0_y_f1<0.02))
                    histo1.Fill(gantry0_x_f1,gantry0_y_f1,(fabs(phidg0_Btot_f1-phidg0_Btot_f2)*0.25));
                else
                    histo1.Fill(gantry0_x_f1,gantry0_y_f1,(fabs(phidg0_Btot_f1-phidg0_Btot_f2)*0.25) );
            }   

            if(gantry0_z_f1 > 0.25 && gantry0_z_f1 < 0.35){
    //           if(((gantry0_x_f1<0.33) && (gantry0_x_f1>0.31)) && ((gantry0_y_f1<0.21) && (gantry0_y_f1>0.19))) 
     //               histo2.Fill(gantry0_x_f1,gantry0_y_f1,(fabs(phidg0_Btot_f1-phidg0_Btot_f2)*0.25));
       //         else
                    histo2.Fill(gantry0_x_f1,gantry0_y_f1,(fabs(phidg0_Btot_f1-phidg0_Btot_f2)*0.5));
            }
        }
        /*if(gantry0_z_f3 > 0.15 && gantry0_z_f3 < 0.25){
            if(((gantry0_x_f3<0.33) && (gantry0_x_f3>0.31)) && ((gantry0_y_f3<0.21) && (gantry0_y_f3>0.19))) 
                histo3.Fill(gantry0_x_f3,gantry0_y_f3,(fabs(phidg0_Btot_f3-phidg0_Btot_f4)*0.25));
            else
                histo3.Fill(gantry0_x_f3,gantry0_y_f3,(fabs(phidg0_Btot_f3-phidg0_Btot_f4)*0.5));

        }    
        if(gantry0_z_f3 > 0.25&& gantry0_z_f3 < 0.35){
            if(((gantry0_x_f3<0.33) && (gantry0_x_f3>0.31)) && ((gantry0_y_f3<0.21) && (gantry0_y_f3>0.19))) 
                histo4.Fill(gantry0_x_f3,gantry0_y_f3,(fabs(phidg0_Btot_f3-phidg0_Btot_f4)*0.25));
            else
                histo4.Fill(gantry0_x_f3,gantry0_y_f3,(fabs(phidg0_Btot_f3-phidg0_Btot_f4)*0.5));
        }
                       
        if(gantry0_z_f3 > 0.35 && gantry0_z_f3 < 0.45){
            if(((gantry0_x_f3<0.33) && (gantry0_x_f3>0.31)) && ((gantry0_y_f3<0.21) && (gantry0_y_f3>0.19))) 
                histo5.Fill(gantry0_x_f3,gantry0_y_f3,(fabs(phidg0_Btot_f3-phidg0_Btot_f4)*0.25));
            else
                histo5.Fill(gantry0_x_f3,gantry0_y_f3,(fabs(phidg0_Btot_f3-phidg0_Btot_f4)*0.5));
         }*/
        
}    


    //for z = 0.3 1,2.3
    // for z = 0  1,1.5
    double Min_zval = 0.;
    double Max_zval = 0.014;   
 
    histo1.GetXaxis().SetTitle("x-axis (m)");
    histo1.GetYaxis().SetTitle("y-axis (m)");
    histo1.GetZaxis().SetTitle("Difference in Magnetic Field Magnitude (Gauss)");
    histo1.GetXaxis().CenterTitle();
    histo1.GetYaxis().CenterTitle();
    histo1.GetZaxis().CenterTitle();    
    histo1.GetYaxis().SetTitleOffset(1.5);
    histo1.GetZaxis().SetTitleOffset(2);
    histo1.SetMinimum(Min_zval);
    histo1.SetMaximum(Max_zval);
    histo1.SetStats(kFALSE);
    
    histo2.GetXaxis().SetTitle("x-axis (m)");
    histo2.GetYaxis().SetTitle("y-axis (m)");
    histo2.GetZaxis().SetTitle("Difference in Magnetic Field Magnitude (Gauss)");
    histo2.GetXaxis().CenterTitle();
    histo2.GetYaxis().CenterTitle();
    histo2.GetZaxis().CenterTitle();    
    histo2.GetYaxis().SetTitleOffset(1.5);
    histo2.GetZaxis().SetTitleOffset(2);
    histo2.SetMinimum(Min_zval);
    histo2.SetMaximum(Max_zval);
    histo2.SetStats(kFALSE);


    histo3.GetXaxis().SetTitle("x-axis (m)");
    histo3.GetYaxis().SetTitle("y-axis (m)");
    histo3.GetZaxis().SetTitle("Difference in Magnetic Field Magnitude (Gauss)");
    histo3.GetXaxis().CenterTitle();
    histo3.GetYaxis().CenterTitle();
    histo3.GetZaxis().CenterTitle();    
    histo3.GetYaxis().SetTitleOffset(1.5);
    histo3.GetZaxis().SetTitleOffset(2);
    histo3.SetMinimum(Min_zval);
    histo3.SetMaximum(Max_zval);
    histo3.SetStats(kFALSE);


    histo4.GetXaxis().SetTitle("x-axis (m)");
    histo4.GetYaxis().SetTitle("y-axis (m)");
    histo4.GetZaxis().SetTitle("Difference in Magnetic Field Magnitude (Gauss)");
    histo4.GetXaxis().CenterTitle();
    histo4.GetYaxis().CenterTitle();
    histo4.GetZaxis().CenterTitle();    
    histo4.GetYaxis().SetTitleOffset(1.5);
    histo4.GetZaxis().SetTitleOffset(1.5);
    histo4.SetMinimum(Min_zval);
    histo4.SetMaximum(Max_zval);
    histo4.SetStats(kFALSE);


    histo5.GetXaxis().SetTitle("x-axis (m)");
    histo5.GetYaxis().SetTitle("y-axis (m)");
    histo5.GetZaxis().SetTitle("Difference in Magnetic Field Magnitude (Gauss)");
    histo5.GetXaxis().CenterTitle();
    histo5.GetYaxis().CenterTitle();
    histo5.GetZaxis().CenterTitle();    
    histo5.GetYaxis().SetTitleOffset(1.5);
    histo5.GetZaxis().SetTitleOffset(1.5);
    histo5.SetMinimum(Min_zval);
    histo5.SetMaximum(Max_zval);
    histo5.SetStats(kFALSE);
    


    
    
    TCanvas *canvas1 = new TCanvas("canvas1","Graph",0,0,1280,948);

    canvas1.Divide(1,2);
    //canvas1.cd(1).Divide(3,1);    
    //canvas1.cd(2).Divide(2,1);
    
    canvas1.cd(1);
    canvas1.cd(1).SetRightMargin(0.2);
    histo1.Draw("zcol");
    
    canvas1.cd(2);
    canvas1.cd(2).SetRightMargin(0.2);
    histo2.Draw("zcol");
    /*
    canvas1.cd(1).cd(3);
    canvas1.cd(1).cd(3).SetRightMargin(0.2);
    histo3.Draw("zcol");
    
    canvas1.cd(2).cd(1);
    canvas1.cd(2).cd(1).SetRightMargin(0.15);
    histo4.Draw("zcol");
    
    canvas1.cd(2).cd(2);
    canvas1.cd(2).cd(2).SetRightMargin(0.15);
    histo5.Draw("zcol");
      */ 
}

