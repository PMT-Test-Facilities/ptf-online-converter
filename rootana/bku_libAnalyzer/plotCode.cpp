{
    TFile *file226 = new TFile("output00223.root");
        
    TCanvas *canvas1 = new TCanvas("canvas1","Graph",0,0,1280,948);
    canvas1->SetRightMargin(0.15);
    

  
     
   
     

    int time;
    double gantry0_x;
    double gantry0_y;
    double gantry0_z;
    double phidg0_Bx;
    double phidg0_By;
    double phidg0_Bz;
    double phidg0_Btot;
    double Averaged_MagneticField;
    int count = 0;
    int iteration = 1;
    double last_x_value = 9999; 
    double last_y_value = 9999;
    double last_z_value = 9999;
    
    TGraph2D *graph2D = new TGraph2D();
        
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


    scan_tree ->GetEntry(0);
    last_x_value = gantry0_x;
    last_y_value = gantry0_y;
    last_z_value = gantry0_z;
    iteration=1;
    Averaged_MagneticField=phidg0_Btot;




    // for z = 0; 
    for(int ient = 1; ient < entries; ient++){    
        scan_tree->GetEntry(ient);
            if((last_x_value == gantry0_x) && (last_y_value == gantry0_y) && (last_z_value == gantry0_z)){
                Averaged_MagneticField = Averaged_MagneticField + phidg0_Btot;
                iteration++;    
            }
            else {
                Averaged_MagneticField = Averaged_MagneticField/iteration;
                scan_tree->GetEntry(ient-1);
                graph2D->SetPoint(count,gantry0_x,gantry0_y,Averaged_MagneticField);
                std::cout << gantry0_x << " : " << gantry0_y << " : " << Averaged_MagneticField << std::endl;
                scan_tree->GetEntry(ient);
                Averaged_MagneticField=phidg0_Btot;
                last_x_value = gantry0_x;
                last_y_value = gantry0_y;
                last_z_value = gantry0_z;
                count++;
                iteration=1;
            }
        }
    


 




/*
    // for z = 0; 
    for(int ient = 1; ient < entries; ient++){    
        scan_tree->GetEntry(ient);
        if(gantry0_z<0.01){    
            if((last_x_value == gantry0_x) && (last_y_value == gantry0_y) && (last_z_value == gantry0_z)){
                Averaged_MagneticField = Averaged_MagneticField + phidg0_Btot;
                iteration++;    
            }
            else {
                Averaged_MagneticField = Averaged_MagneticField/iteration;
                scan_tree->GetEntry(ient-1);
                graph2D->SetPoint(count,gantry0_x,gantry0_y,Averaged_MagneticField);
                std::cout << gantry0_x << " : " << gantry0_y << " : " << Averaged_MagneticField << std::endl;
                scan_tree->GetEntry(ient);
                Averaged_MagneticField=phidg0_Btot;
                last_x_value = gantry0_x;
                last_y_value = gantry0_y;
                last_z_value = gantry0_z;
                count++;
                iteration=1;
            }
        }
    }


*/ 
/*
    // for z = 0.1
    for(int ient = 1; ient < entries; ient++){    
        scan_tree->GetEntry(ient);
        if(gantry0_z>0.05 && gantry0_z<0.15){    
            if((last_x_value == gantry0_x) && (last_y_value == gantry0_y) && (last_z_value == gantry0_z)){
                Averaged_MagneticField = Averaged_MagneticField + phidg0_Btot;
                iteration++;    
            }
            else {
                Averaged_MagneticField = Averaged_MagneticField/iteration;
                scan_tree->GetEntry(ient-1);
                graph2D->SetPoint(count,gantry0_x,gantry0_y,Averaged_MagneticField);
                scan_tree->GetEntry(ient);
                Averaged_MagneticField=phidg0_Btot;
                last_x_value = gantry0_x;
                last_y_value = gantry0_y;
                last_z_value = gantry0_z;
                count++;
                iteration=1;
            }
        }
    }
    */
    
    
/*
    //for z = 0.2
    for(int ient = 1; ient < entries; ient++){    
        scan_tree->GetEntry(ient);
        if(gantry0_z>0.15 && gantry0_z<0.25){    
            if((last_x_value == gantry0_x) && (last_y_value == gantry0_y) && (last_z_value == gantry0_z)){
                Averaged_MagneticField = Averaged_MagneticField + phidg0_Btot;
                iteration++;    
            }
            else {
                Averaged_MagneticField = Averaged_MagneticField/iteration;
                scan_tree->GetEntry(ient-1);
                graph2D->SetPoint(count,gantry0_x,gantry0_y,Averaged_MagneticField);
                scan_tree->GetEntry(ient);
                Averaged_MagneticField=phidg0_Btot;
                last_x_value = gantry0_x;
                last_y_value = gantry0_y;
                last_z_value = gantry0_z;
                count++;
                iteration=1;
            }
        }
    }
 */
    
  /* 
    //for z = 0.3
    for(int ient = 1; ient < entries; ient++){    
        scan_tree->GetEntry(ient);
        if(gantry0_z>0.25 && gantry0_z<0.35){    
            if((last_x_value == gantry0_x) && (last_y_value == gantry0_y) && (last_z_value == gantry0_z)){
                Averaged_MagneticField = Averaged_MagneticField + phidg0_Btot;
                iteration++;    
            }
            else {
                Averaged_MagneticField = Averaged_MagneticField/iteration;
                scan_tree->GetEntry(ient-1);
                graph2D->SetPoint(count,gantry0_x,gantry0_y,Averaged_MagneticField);
                scan_tree->GetEntry(ient);
                Averaged_MagneticField=phidg0_Btot;
                last_x_value = gantry0_x;
                last_y_value = gantry0_y;
                last_z_value = gantry0_z;
                count++;
                iteration=1;
            }
        }
    }
 */  
 
/*
    // for z = 0.4
    for(int ient = 1; ient < entries; ient++){    
        scan_tree->GetEntry(ient);
        if(gantry0_z>0.35 && gantry0_z<0.45){    
            if((last_x_value == gantry0_x) && (last_y_value == gantry0_y) && (last_z_value == gantry0_z)){
                Averaged_MagneticField = Averaged_MagneticField + phidg0_Btot;
                iteration++;    
            }
            else {
                Averaged_MagneticField = Averaged_MagneticField/iteration;
                scan_tree->GetEntry(ient-1);
                graph2D->SetPoint(count,gantry0_x,gantry0_y,Averaged_MagneticField);
                scan_tree->GetEntry(ient);
                Averaged_MagneticField=phidg0_Btot;
                last_x_value = gantry0_x;
                last_y_value = gantry0_y;
                last_z_value = gantry0_z;
                count++;
                iteration=1;
            }
        }
    }

*/


    graph2D->SetMinimum(1.18);
    graph2D.Draw("ZCOL"); 
    graph2D->SetTitle("Magnetic Field Magnitude at Given Location,Z=0");
    graph2D->GetYaxis().SetTitle("Y-Axis location (m)");
    graph2D->GetYaxis().SetTitleOffset(1.3);
    graph2D->GetYaxis().CenterTitle();
    graph2D->GetXaxis().SetTitle("X-Axis location (m)");
    graph2D->GetXaxis().CenterTitle();
    graph2D->GetZaxis().SetTitle("Magnetic Field Magnitude(Gauss)");
    graph2D->GetZaxis().SetTitleOffset(1.3);
    graph2D->GetZaxis().CenterTitle();
    graph2D->GetZaxis().RotateTitle();

}
    
    
    
