{
    TFile *file270 = new TFile("output00273.root");
        
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

    // SetBRanchAddress returns the adress of the specified branch.
    scan_tree.SetBranchAddress("time",&time);
    scan_tree.SetBranchAddress("gantry0_x",&gantry0_x);
    scan_tree.SetBranchAddress("gantry0_y",&gantry0_y);
    scan_tree.SetBranchAddress("gantry0_z",&gantry0_z);
    scan_tree.SetBranchAddress("phidg0_Bx",&phidg0_Bx);
    scan_tree.SetBranchAddress("phidg0_By",&phidg0_By);
    scan_tree.SetBranchAddress("phidg0_Bz",&phidg0_Bz);
    scan_tree.SetBranchAddress("phidg0_Btot",&phidg0_Btot);
    scan_tree.SetBranchAddress("gantry0_tilt",&gantry0_tilt);
    scan_tree.SetBranchAddress("phidg0_tilt",&phidg0_tilt);
    // Get number of entries in the data set
    int entries = scan_tree->GetEntries();
    std::cout<<entries<<std::endl;

    
    for(int ient = 0; ient < entries; ient++){  
      scan_tree->GetEntry(ient); 
      //if(gantry0_z <0.01)
      std::cout<<phidg0_tilt<<" : " << gantry0_tilt <<  std::endl;    
    }
    
}    
    
