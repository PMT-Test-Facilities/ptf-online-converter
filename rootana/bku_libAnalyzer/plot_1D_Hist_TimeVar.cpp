{
    // Data files being considered
    TFile *file219 = new TFile("output00219.root");  //original scan
    TFile *file220 = new TFile("output00220.root");  //time dependence measurement
    TFile *file221 = new TFile("output00221.root");  //time dependencd measurement
    TFile *file223 = new TFile("output00223.root");  // original scan
    TFile *file225 = new TFile("output00225.root");  // Ben's modified scan
    TFile *file226 = new TFile("output00226.root");  // Ben's modified scan
    
    // Set file to work with
    file220.cd();
       
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
    int count=0;
    
    
    // SetBRanchAddress returns the adress of the specified branch.
    scan_tree.SetBranchAddress("time", &time);
    scan_tree.SetBranchAddress("gantry_event", &gantry_event);
    scan_tree.SetBranchAddress("gantry_subevent", &gantry_subevent);
    scan_tree.SetBranchAddress("gantry0_x", &gantry0_x);
    scan_tree.SetBranchAddress("gantry0_y", &gantry0_y);
    scan_tree.SetBranchAddress("gantry0_z", &gantry0_z);
    scan_tree.SetBranchAddress("gantry0_tilt", &gantry0_tilt);
    scan_tree.SetBranchAddress("gantry0_rot", &gantry0_rot);
    scan_tree.SetBranchAddress("phidg0_tilt", &phidg0_tilt);
    scan_tree.SetBranchAddress("gantry1_x", &gantry1_x);
    scan_tree.SetBranchAddress("gantry1_y", &gantry1_y);
    scan_tree.SetBranchAddress("gantry1_z", &gantry1_z);
    scan_tree.SetBranchAddress("gantry1_tilt", &gantry1_tilt);
    scan_tree.SetBranchAddress("gantry1_rot", &gantry1_rot);
    scan_tree.SetBranchAddress("phidg1_tilt", &phidg1_tilt);
    scan_tree.SetBranchAddress("phidg_event", &phidg_event);
    scan_tree.SetBranchAddress("phidg0_Bx", &phidg0_Bx);
    scan_tree.SetBranchAddress("phidg0_By", &phidg0_By);
    scan_tree.SetBranchAddress("phidg0_Bz", &phidg0_Bz);
    scan_tree.SetBranchAddress("phidg0_Btot", &phidg0_Btot);
    scan_tree.SetBranchAddress("phidg1_Bx", &phidg1_Bx);
    scan_tree.SetBranchAddress("phidg1_By", &phidg1_By);
    scan_tree.SetBranchAddress("phidg1_Bz", &phidg1_Bz);
    scan_tree.SetBranchAddress("phidg1_Btot", &phidg1_Btot);
    scan_tree.SetBranchAddress("coil_event", &coil_event);
    scan_tree.SetBranchAddress("I_coil0", &I_coil0);
    scan_tree.SetBranchAddress("I_coil1", &I_coil1);
    scan_tree.SetBranchAddress("I_coil2", &I_coil2);
    scan_tree.SetBranchAddress("I_coil3", &I_coil3);
    scan_tree.SetBranchAddress("I_coil4", I_coil4);
    scan_tree.SetBranchAddress("I_coil5", I_coil5);
    scan_tree.SetBranchAddress("U_coil0", U_coil0);
    scan_tree.SetBranchAddress("U_coil1", U_coil1);
    scan_tree.SetBranchAddress("U_coil2", U_coil2);
    scan_tree.SetBranchAddress("U_coil3", U_coil3);
    scan_tree.SetBranchAddress("U_coil4", U_coil4);
    scan_tree.SetBranchAddress("U_coil5", U_coil5);
    
    // title on stats box,
    TH1D *histo1 = new TH1D("Statistics","Distribution of Magnetic Field Measurements at Point(0.32m, 0.3m, 0.2m)", 200,1.45,1.4);
    
    num_entries = scan_tree.GetEntries();
     
    // Start at ient = 2 to ignore first two points which were not at the same location 
    for(int ient = 2; ient < num_entries; ient++){    
        scan_tree->GetEntry(ient);
        histo1.Fill(phidg0_Btot);
    }
 
    histo1.GetXaxis().SetTitle("Magnitude of magnetic field (Gauss");
    histo1.GetYaxis().SetTitle("Number of Occurences");
    histo1.GetXaxis().CenterTitle();
    histo1.GetYaxis().CenterTitle();

    
    
    
    file221.cd();
    
    // SetBRanchAddress returns the adress of the specified branch.
    scan_tree.SetBranchAddress("time", &time);
    scan_tree.SetBranchAddress("gantry_event", &gantry_event);
    scan_tree.SetBranchAddress("gantry_subevent", &gantry_subevent);
    scan_tree.SetBranchAddress("gantry0_x", &gantry0_x);
    scan_tree.SetBranchAddress("gantry0_y", &gantry0_y);
    scan_tree.SetBranchAddress("gantry0_z", &gantry0_z);
    scan_tree.SetBranchAddress("gantry0_tilt", &gantry0_tilt);
    scan_tree.SetBranchAddress("gantry0_rot", &gantry0_rot);
    scan_tree.SetBranchAddress("phidg0_tilt", &phidg0_tilt);
    scan_tree.SetBranchAddress("gantry1_x", &gantry1_x);
    scan_tree.SetBranchAddress("gantry1_y", &gantry1_y);
    scan_tree.SetBranchAddress("gantry1_z", &gantry1_z);
    scan_tree.SetBranchAddress("gantry1_tilt", &gantry1_tilt);
    scan_tree.SetBranchAddress("gantry1_rot", &gantry1_rot);
    scan_tree.SetBranchAddress("phidg1_tilt", &phidg1_tilt);
    scan_tree.SetBranchAddress("phidg_event", &phidg_event);
    scan_tree.SetBranchAddress("phidg0_Bx", &phidg0_Bx);
    scan_tree.SetBranchAddress("phidg0_By", &phidg0_By);
    scan_tree.SetBranchAddress("phidg0_Bz", &phidg0_Bz);
    scan_tree.SetBranchAddress("phidg0_Btot", &phidg0_Btot);
    scan_tree.SetBranchAddress("phidg1_Bx", &phidg1_Bx);
    scan_tree.SetBranchAddress("phidg1_By", &phidg1_By);
    scan_tree.SetBranchAddress("phidg1_Bz", &phidg1_Bz);
    scan_tree.SetBranchAddress("phidg1_Btot", &phidg1_Btot);
    scan_tree.SetBranchAddress("coil_event", &coil_event);
    scan_tree.SetBranchAddress("I_coil0", &I_coil0);
    scan_tree.SetBranchAddress("I_coil1", &I_coil1);
    scan_tree.SetBranchAddress("I_coil2", &I_coil2);
    scan_tree.SetBranchAddress("I_coil3", &I_coil3);
    scan_tree.SetBranchAddress("I_coil4", I_coil4);
    scan_tree.SetBranchAddress("I_coil5", I_coil5);
    scan_tree.SetBranchAddress("U_coil0", U_coil0);
    scan_tree.SetBranchAddress("U_coil1", U_coil1);
    scan_tree.SetBranchAddress("U_coil2", U_coil2);
    scan_tree.SetBranchAddress("U_coil3", U_coil3);
    scan_tree.SetBranchAddress("U_coil4", U_coil4);
    scan_tree.SetBranchAddress("U_coil5", U_coil5);

    TH1D *histo2 = new TH1D("Statistics","Distribution of Magnetic Field Measurements at Point(0.1m, 0.1m, 0m)", 200,1.304,1.312); 
        
    num_entries = scan_tree.GetEntries();
     
    // Start at ient = 2 to ignore first two points which were not at the same location 
    for(int ient = 2; ient < num_entries; ient++){    
        scan_tree->GetEntry(ient);
        histo2.Fill(phidg0_Btot);
    }
 
    histo2.GetXaxis().SetTitle("Magnitude of magnetic field (Gauss)");
    histo2.GetYaxis().SetTitle("Number of Occurences");
    histo2.GetXaxis().CenterTitle();
    histo2.GetYaxis().CenterTitle();

    
    
    
     //Start Making graph
    TCanvas *canvas1 = new TCanvas("canvas1","Graph",0,0,1280,948);
    canvas1.Divide(1,2);

    canvas1.cd(1);
    histo1.Draw();    
    canvas1.cd(2);
    histo2.Draw();
    
           
}

void SetData(){


