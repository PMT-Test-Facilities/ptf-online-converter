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

// Returns true if the pos_x,pos_y,pos_z is within range of x,y,z
bool check_for_point(double x,double y, double z, double range, double pos_x, double pos_y, double pos_z ){
    return ( ( (pos_x < x+range)&&(pos_x> x-range) )&&( (pos_y <y+range)&&(pos_y>y-range) )&&( (pos_z <z+range) && (pos_z > z-range) ) );
}


double find_smallest_distance(char direction){
        
    double*coordinate;
    int num_entries = scan_tree->GetEntries();
    double min_distance = 1000;
    double last_position;

    // determine which direction to investigate
    if( direction == 'x'){
        coordinate = &gantry0_x;
    }    
    else if(direction =='y'){
        coordinate = &gantry0_y;
    }
    else{
        coordinate = &gantry0_z;
    }
        
    scan_tree->GetEntry(0);
    last_position = *coordinate;
    
    for(int index=1; index < num_entries; index++){
        scan_tree->GetEntry(index);
        if( (*coordinate != last_position) && (fabs(*coordinate-last_position) < min_distance) ){
            min_distance = fabs(*coordinate-last_position);
        }
        last_position = *coordinate;
    }
    
    return min_distance;

}

double** count(double range){

    double** count; 
    int max = 2;
    int entry_num;
    int found = 0;
    int num_entries = scan_tree->GetEntries();
     
    count = new double*[max];
    for (int i= 0; i < max;i++){
        count[i]= new double[4];
    }   
    
    scan_tree->GetEntry(0);
    count[0][0] = (double)max; 
    count[0][1] = (double)max;
    count[0][2] = (double)max;
    count[0][3] = (double)max;
  
    count[1][0] = gantry0_x;
    count[1][1] = gantry0_y;
    count[1][2] = gantry0_z;
    count[1][3] = 1.0;



    for(int index=1; index<num_entries;index++){
        scan_tree->GetEntry(index);
        entry_num = 1;

        while( entry_num < max && found == 0){
          
            if( check_for_point(count[entry_num][0], count[entry_num][1], count[entry_num][2], range/2, gantry0_x, gantry0_y, gantry0_z ) ) {
                count[entry_num][3]=count[entry_num][3]+1.0;
                found = 1;
            } 
            entry_num++;  
        }
  
        if( found == 1){
            found = 0;
        }
        else{
            max++;
            double** temp = new double*[max];
            for (int i= 0; i < max;i++){
                temp[i]= new double[4];
            }

            for( int i=0; i<max-1;i++){
                temp[i][0]= count[i][0];
                temp[i][1]= count[i][1];
                temp[i][2]= count[i][2];
                temp[i][3]= count[i][3];
            }
      
            temp[max-1][0]= gantry0_x;
            temp[max-1][1]= gantry0_y;
            temp[max-1][2]= gantry0_z;
            temp[max-1][3]= 1.0;
        
            for(int i = 0; i < max-1; ++i) {
                delete [] count[i];
            }
            delete [] count;
            count=temp;
        }  
    }  
  
    count[0][0] = (double)max;
    count[0][1] = (double)max;
    count[0][2] = (double)max;
    count[0][3] = (double)max;
  
    return count;
}
  
 
 

void plot_2D_Hist(){
    
    TFile *file270 = new TFile("output00270.root");  //detailed 0 scan (OFF)
    TFile *file273 = new TFile("output00273.root");  //less detailed scan, multiple z, (OFF mostly)
    TFile *file274 = new TFile("output00274.root");  //detailed scan, (ON)
    TFile *file275 = new TFile("output00275.root");  //less detailed scan, (ON)
    TFile *file226 = new TFile("output00226.root");  

    double x_smallest_increment;
    double y_smallest_increment;
    double z_smallest_increment;
    double increment;
    int num_entries;
    double** num_entries_per_point;
    int found=0;
    int entry_num;
    double multiplier =1.0;
    double max;
    double min_distance;
    int num_bin = 30;
    double z_low = -0.05;
    double z_high = 0.05;
    
    double Min_zval;
    double Max_zval; 
    
    // title on stats box,title, (xaxis) number of bins, low, high, y(axis) number of bins, low, high
    TH2D *histo1 = new TH2D("Statistics","Field Magnitude, Z=0m, Cyclotron On", num_bin,0.,0.64,num_bin,0.,0.62);
    TH2D *histo2 = new TH2D("Statistics","Tilt of phidget, Z=0m", num_bin,0,0.64,num_bin,0,0.62);
    TH2D *histo3 = new TH2D("Statistics","Field Magnitude, Z=0.1m, Cyclotron On", num_bin,0,0.64,num_bin,0,0.62);
    TH2D *histo3_0 = new TH2D("Statistics","Tilt of phidget, Z=0m", num_bin,0,0.64,num_bin,0,0.62);
    TH2D *histo3_1 = new TH2D("Statistics","Tilt of phidget, Z=0m", num_bin,0,0.64,num_bin,0,0.62);
    TH2D *histo3_2 = new TH2D("Statistics","Tilt of phidget, Z=0m", num_bin,0,0.64,num_bin,0,0.62);
    TH2D *histo4 = new TH2D("Statistics","Tilt of phidget, Z=0.5m,", num_bin,0,0.64,num_bin,0,0.62);
    TH2D *histo4_0 = new TH2D("Statistics","Tilt of phidget, Z=0m", num_bin,0,0.64,num_bin,0,0.62);
    TH2D *histo4_1 = new TH2D("Statistics","Tilt of phidget, Z=0.1m", num_bin,0,0.64,num_bin,0,0.62);
    TH2D *histo4_2 = new TH2D("Statistics","Tilt of phidget, Z=0.2m", num_bin,0,0.64,num_bin,0,0.62);
    TH2D *histo4_3 = new TH2D("Statistics","Tilt of phidget, Z=0.3m", num_bin,0,0.64,num_bin,0,0.62);
    TH2D *histo4_4 = new TH2D("Statistics","Tilt of phidget, Z=0.4m", num_bin,0,0.64,num_bin,0,0.62);
      
    // beginnign of graph creation unit
    file274->cd();
    scan_tree->SetBranchAddress("gantry0_x", &gantry0_x);
    scan_tree->SetBranchAddress("gantry0_y", &gantry0_y);
    scan_tree->SetBranchAddress("gantry0_z", &gantry0_z);
    scan_tree->SetBranchAddress("phidg0_Btot", &phidg0_Btot);
    scan_tree->SetBranchAddress("phidg0_Bx", &phidg0_Bx);
    scan_tree->SetBranchAddress("phidg0_By", &phidg0_By);
    scan_tree->SetBranchAddress("phidg0_Bz", &phidg0_Bz);
    scan_tree->SetBranchAddress("phidg0_tilt", &phidg0_tilt);

    
    x_smallest_increment=find_smallest_distance('x');
    y_smallest_increment=find_smallest_distance('y');  
    z_smallest_increment=find_smallest_distance('z');
    
    min_distance =  x_smallest_increment;
    if(y_smallest_increment<min_distance)
        min_distance = y_smallest_increment;
    if(z_smallest_increment<min_distance)
        min_distance = z_smallest_increment;

    num_entries = scan_tree->GetEntries();

    num_entries_per_point=count(min_distance);
    max = num_entries_per_point[0][0];
    int num = max-1;
    for(int i=0;i<=num;i++){
       // std::cout<<num_entries_per_point[i][0] <<" : "<< num_entries_per_point[i][1]<<" : "<<num_entries_per_point[i][2]<<" : "<<num_entries_per_point[i][3]<<std::endl; 
    }
    
    for(int i=0; i< num_entries; i++){
        scan_tree->GetEntry(i);
        entry_num = 1;
        found = 0;
   
        while( entry_num <= num && found == 0){
            if( check_for_point(num_entries_per_point[entry_num][0], num_entries_per_point[entry_num][1], num_entries_per_point[entry_num][2], min_distance/2, gantry0_x, gantry0_y, gantry0_z) ){
                multiplier = 1.0/num_entries_per_point[entry_num][3];
                found = 1;
            }
            else{
                entry_num++;
            }
        }
        
        if(gantry0_z > z_low && gantry0_z < z_high){
            histo1->Fill(fabs(gantry0_x),fabs(gantry0_y),(phidg0_Btot*multiplier));
        }
      
    }
    
    // end of graph creation unit
    
    // beginnign of graph creation unit
    file270->cd();
    scan_tree->SetBranchAddress("gantry0_x", &gantry0_x);
    scan_tree->SetBranchAddress("gantry0_y", &gantry0_y);
    scan_tree->SetBranchAddress("gantry0_z", &gantry0_z);
    scan_tree->SetBranchAddress("phidg0_Btot", &phidg0_Btot);
    scan_tree->SetBranchAddress("phidg0_Bx", &phidg0_Bx);
    scan_tree->SetBranchAddress("phidg0_By", &phidg0_By);
    scan_tree->SetBranchAddress("phidg0_Bz", &phidg0_Bz);
    scan_tree->SetBranchAddress("phidg0_tilt", &phidg0_tilt);
    
    
    x_smallest_increment=find_smallest_distance('x');
    y_smallest_increment=find_smallest_distance('y');  
    z_smallest_increment=find_smallest_distance('z');
    
    min_distance =  x_smallest_increment;
    if(y_smallest_increment<min_distance)
        min_distance = y_smallest_increment;
    if(z_smallest_increment<min_distance)
        min_distance = z_smallest_increment;

    num_entries = scan_tree->GetEntries();

    num_entries_per_point=count(min_distance);
    max = num_entries_per_point[0][0];
    int num = max-1;

    for(int i=0; i< num_entries; i++){
        scan_tree->GetEntry(i);
        entry_num = 1;
        found = 0;
   
        while( entry_num <= num && found == 0){
            if( check_for_point(num_entries_per_point[entry_num][0], num_entries_per_point[entry_num][1], num_entries_per_point[entry_num][2], min_distance/2, gantry0_x, gantry0_y, gantry0_z) ){
                multiplier = 1.0/num_entries_per_point[entry_num][3];
                found = 1;
            }
            else{
                entry_num++;
            }
        }
        
        if(gantry0_z > z_low && gantry0_z < z_high){
            histo2->Fill(fabs(gantry0_x),gantry0_y,(phidg0_tilt*multiplier));
            //std::cout<<num_entries_per_point[entry_num][0] <<" : "<< num_entries_per_point[entry_num][1]<<" : "<<num_entries_per_point[entry_num][2]<<" : "<<num_entries_per_point[entry_num][3]<<" : " << multiplier<<std::endl; 
            //std::cout<<"second " << gantry0_x << " : " << gantry0_y << " : " << gantry0_z << std::endl;
            //std::cout<<phidg0_Btot<<" : " <<(phidg0_Btot*multiplier)<< std::endl;
        }
      
    }
    
    // end of graph creation unit
    
    // beginnign of graph creation unit
    file270->cd();
    scan_tree->SetBranchAddress("gantry0_x", &gantry0_x);
    scan_tree->SetBranchAddress("gantry0_y", &gantry0_y);
    scan_tree->SetBranchAddress("gantry0_z", &gantry0_z);
    scan_tree->SetBranchAddress("phidg0_Btot", &phidg0_Btot);
    scan_tree->SetBranchAddress("phidg0_Bx", &phidg0_Bx);
    scan_tree->SetBranchAddress("phidg0_By", &phidg0_By);
    scan_tree->SetBranchAddress("phidg0_Bz", &phidg0_Bz);
    scan_tree->SetBranchAddress("phidg0_tilt", &phidg0_tilt);    
    
    
    x_smallest_increment=find_smallest_distance('x');
    y_smallest_increment=find_smallest_distance('y');  
    z_smallest_increment=find_smallest_distance('z');
    
    min_distance =  x_smallest_increment;
    if(y_smallest_increment<min_distance)
        min_distance = y_smallest_increment;
    if(z_smallest_increment<min_distance)
        min_distance = z_smallest_increment;

    num_entries = scan_tree->GetEntries();

    num_entries_per_point=count(min_distance);
    max = num_entries_per_point[0][0];
    int num = max-1;

    for(int i=0; i< num_entries; i++){
        scan_tree->GetEntry(i);
        entry_num = 1;
        found = 0;
   
        while( entry_num <= num && found == 0){
            if( check_for_point(num_entries_per_point[entry_num][0], num_entries_per_point[entry_num][1], num_entries_per_point[entry_num][2], min_distance/2, gantry0_x, gantry0_y, gantry0_z) ){
                multiplier = 1.0/num_entries_per_point[entry_num][3];
                found = 1;
            }
            else{
                entry_num++;
            }
        }
        
        if(gantry0_z > z_low && gantry0_z < z_high){
            histo3->Fill(fabs(gantry0_x),gantry0_y,(phidg0_Btot*multiplier));
            //std::cout<<num_entries_per_point[entry_num][0] <<" : "<< num_entries_per_point[entry_num][1]<<" : "<<num_entries_per_point[entry_num][2]<<" : "<<num_entries_per_point[entry_num][3]<<" : " << multiplier<<std::endl; 
            //std::cout<<"second " << gantry0_x << " : " << gantry0_y << " : " << gantry0_z << std::endl;
            //std::cout<<phidg0_Btot<<" : " <<(phidg0_Btot*multiplier)<< std::endl;
        }
        
        if(gantry0_z > -0.05 && gantry0_z < 0.05){
            histo3_0->Fill(fabs(gantry0_x),gantry0_y,(-1*phidg0_Bx*multiplier));
        }

        if(gantry0_z > -0.05 && gantry0_z < 0.05){
            histo3_1->Fill(fabs(gantry0_x),gantry0_y,(-1*phidg0_By*multiplier));
        }


        if(gantry0_z > -0.05 && gantry0_z < 0.05){
            histo3_2->Fill(fabs(gantry0_x),gantry0_y,(phidg0_Bz*multiplier));
        }



      
    }
    
    // end of graph creation unit

    
    // beginnign of graph creation unit
    file273->cd();
    scan_tree->SetBranchAddress("gantry0_x", &gantry0_x);
    scan_tree->SetBranchAddress("gantry0_y", &gantry0_y);
    scan_tree->SetBranchAddress("gantry0_z", &gantry0_z);
    scan_tree->SetBranchAddress("phidg0_Btot", &phidg0_Btot);
    scan_tree->SetBranchAddress("phidg0_Bx", &phidg0_Bx);
    scan_tree->SetBranchAddress("phidg0_By", &phidg0_By);
    scan_tree->SetBranchAddress("phidg0_Bz", &phidg0_Bz);
    scan_tree->SetBranchAddress("phidg0_tilt", &phidg0_tilt);    
    
    
    x_smallest_increment=find_smallest_distance('x');
    y_smallest_increment=find_smallest_distance('y');  
    z_smallest_increment=find_smallest_distance('z');
    
    min_distance =  x_smallest_increment;
    if(y_smallest_increment<min_distance)
        min_distance = y_smallest_increment;
    if(z_smallest_increment<min_distance)
        min_distance = z_smallest_increment;

    num_entries = scan_tree->GetEntries();

    num_entries_per_point=count(min_distance);
    max = num_entries_per_point[0][0];
    int num = max-1;

    for(int i=0; i< num_entries; i++){
        scan_tree->GetEntry(i);
        entry_num = 1;
        found = 0;
   
        while( entry_num <= num && found == 0){
            if( check_for_point(num_entries_per_point[entry_num][0], num_entries_per_point[entry_num][1], num_entries_per_point[entry_num][2], min_distance/2, gantry0_x, gantry0_y, gantry0_z) ){
                multiplier = 1.0/num_entries_per_point[entry_num][3];
                found = 1;
            }
            else{
                entry_num++;
            }
        }
        
        if(gantry0_z > 0.45 && gantry0_z < 0.55){
            histo4->Fill(fabs(gantry0_x),gantry0_y,(phidg0_tilt*multiplier));
            //std::cout<<num_entries_per_point[entry_num][0] <<" : "<< num_entries_per_point[entry_num][1]<<" : "<<num_entries_per_point[entry_num][2]<<" : "<<num_entries_per_point[entry_num][3]<<" : " << multiplier<<std::endl; 
            //std::cout<<"second " << gantry0_x << " : " << gantry0_y << " : " << gantry0_z << std::endl;
            //std::cout<<phidg0_Btot<<" : " <<(phidg0_Btot*multiplier)<< std::endl;
        }
        
        if(gantry0_z > -0.05 && gantry0_z < 0.05){
            histo4_0->Fill(fabs(gantry0_x),gantry0_y,(phidg0_tilt*multiplier));
        }

        if(gantry0_z > 0.05 && gantry0_z < 0.15){
            histo4_1->Fill(fabs(gantry0_x),gantry0_y,(phidg0_tilt*multiplier));
        }

        if(gantry0_z > 0.15 && gantry0_z < 0.25){
            histo4_2->Fill(fabs(gantry0_x),gantry0_y,(phidg0_tilt*multiplier));
        }

        if(gantry0_z > 0.25 && gantry0_z < 0.35){
            histo4_3->Fill(fabs(gantry0_x),gantry0_y,(phidg0_tilt*multiplier));
        }

        if(gantry0_z > 0.35 && gantry0_z < 0.45){
            histo4_4->Fill(fabs(gantry0_x),gantry0_y,(phidg0_tilt*multiplier));
        }
      
    }
    
    // end of graph creation unit
    
    
    Min_zval=9.7;
    Max_zval=10.2;
    double z_Min_zval=1.11;
    double z_Max_zval=1.2;
    double y_Max_zval=0.43;
    double y_Min_zval=0.32;
    double x_Max_zval=0.25;
    double x_Min_zval=0.14;    
    
    histo1->GetXaxis()->SetTitle("x-axis (m)");
    histo1->GetYaxis()->SetTitle("y-axis (m)");
    histo1->GetZaxis()->SetTitle("Magnetic Field Magnitude (Gauss)");
    histo1->GetXaxis()->CenterTitle();
    histo1->GetYaxis()->CenterTitle();
    histo1->GetZaxis()->CenterTitle();    
    histo1->GetZaxis()->SetTitleOffset(2);
    //histo1->SetMinimum(Min_zval);
    //histo1->SetMaximum(Max_zval);
    histo1->SetStats(kFALSE);
  
    histo2->GetXaxis()->SetTitle("x-axis (m)");
    histo2->GetYaxis()->SetTitle("y-axis (m)");
    histo2->GetZaxis()->SetTitle("Tilt (degrees)");    
    histo2->GetXaxis()->CenterTitle();
    histo2->GetYaxis()->CenterTitle();
    histo2->GetZaxis()->CenterTitle();
    histo2->GetZaxis()->SetTitleOffset(2);
    histo2->GetYaxis()->SetTitleOffset(1.5);
    histo2->SetMinimum(Min_zval);
    histo2->SetMaximum(Max_zval);
    histo2->SetStats(kFALSE);
       
    histo3->GetXaxis()->SetTitle("x-axis (m)");
    histo3->GetYaxis()->SetTitle("y-axis (m)");
    histo3->GetZaxis()->SetTitle("Magnetic Field Magnitude (Gauss)");
    histo3->GetXaxis()->CenterTitle();
    histo3->GetYaxis()->CenterTitle();
    histo3->GetZaxis()->CenterTitle();
    histo3->GetZaxis()->SetTitleOffset(2);
    histo3->GetYaxis()->SetTitleOffset(1.5);    
    histo3->SetMinimum(Min_zval);
    histo3->SetMaximum(Max_zval);
    histo3->SetStats(kFALSE); 
    
    
    histo3_0->GetXaxis()->SetTitle("x-axis (m)");
    histo3_0->GetYaxis()->SetTitle("y-axis (m)");
    histo3_0->GetZaxis()->SetTitle("Magnetic Field Magnitude (Gauss)");
    histo3_0->GetXaxis()->CenterTitle();
    histo3_0->GetYaxis()->CenterTitle();
    histo3_0->GetZaxis()->CenterTitle();
    histo3_0->GetZaxis()->SetTitleOffset(2);
    histo3_0->GetYaxis()->SetTitleOffset(1.5);
    histo3_0->SetMinimum(x_Min_zval);
    histo3_0->SetMaximum(x_Max_zval);
    histo3_0->SetStats(kFALSE);     

    histo3_1->GetXaxis()->SetTitle("x-axis (m)");
    histo3_1->GetYaxis()->SetTitle("y-axis (m)");
    histo3_1->GetZaxis()->SetTitle("Magnetic Field Magnitude (Gauss)");
    histo3_1->GetXaxis()->CenterTitle();
    histo3_1->GetYaxis()->CenterTitle();
    histo3_1->GetZaxis()->CenterTitle();
    histo3_1->GetZaxis()->SetTitleOffset(2);
    histo3_1->GetYaxis()->SetTitleOffset(1.5);
    histo3_1->SetMinimum(y_Min_zval);
    histo3_1->SetMaximum(y_Max_zval);
    histo3_1->SetStats(kFALSE);     

    histo3_2->GetXaxis()->SetTitle("x-axis (m)");
    histo3_2->GetYaxis()->SetTitle("y-axis (m)");
    histo3_2->GetZaxis()->SetTitle("Magnetic Field Magnitude (Gauss)");
    histo3_2->GetXaxis()->CenterTitle();
    histo3_2->GetYaxis()->CenterTitle();
    histo3_2->GetZaxis()->CenterTitle();
    histo3_2->GetZaxis()->SetTitleOffset(2);
    histo3_2->GetYaxis()->SetTitleOffset(1.5);
    histo3_2->SetMinimum(z_Min_zval);
    histo3_2->SetMaximum(z_Max_zval);
    histo3_2->SetStats(kFALSE);     


 
    histo4->GetXaxis()->SetTitle("x-axis(m)");
    histo4->GetYaxis()->SetTitle("y-axis(m)");
    histo4->GetZaxis()->SetTitle("Tilt(degrees)");
    histo4->GetXaxis()->CenterTitle();
    histo4->GetYaxis()->CenterTitle();
    histo4->GetZaxis()->CenterTitle();
    histo4->GetYaxis()->SetTitleOffset(1.5);
    histo4->GetZaxis()->SetTitleOffset(2);
    histo4->SetMinimum(Min_zval);
    histo4->SetMaximum(Max_zval);
    histo4->SetStats(kFALSE);
    
    histo4_0->GetXaxis()->SetTitle("x-axis(m)");
    histo4_0->GetYaxis()->SetTitle("y-axis(m)");
    histo4_0->GetZaxis()->SetTitle("Tilt(degrees)");
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
    histo4_1->GetZaxis()->SetTitle("Tilt(degrees)");
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
    histo4_2->GetZaxis()->SetTitle("Tilt(degrees)");
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
    histo4_3->GetZaxis()->SetTitle("Tilt(degrees)");
    histo4_3->GetXaxis()->CenterTitle();
    histo4_3->GetYaxis()->CenterTitle();
    histo4_3->GetZaxis()->CenterTitle();
    histo4_3->GetYaxis()->SetTitleOffset(1.5);
    histo4_3->GetZaxis()->SetTitleOffset(2);
    histo4_3->SetMinimum(Min_zval);
    histo4_3->SetMaximum(Max_zval);
    histo4_3->SetStats(kFALSE);
            
    histo4_4->GetXaxis()->SetTitle("x-axis(m)");
    histo4_4->GetYaxis()->SetTitle("y-axis (m)");
    histo4_4->GetZaxis()->SetTitle("Tilt(degrees)");
    histo4_4->GetXaxis()->CenterTitle();
    histo4_4->GetYaxis()->CenterTitle();
    histo4_4->GetZaxis()->CenterTitle();
    histo4_4->GetYaxis()->SetTitleOffset(1.5);
    histo4_4->GetZaxis()->SetTitleOffset(2);
    histo4_4->SetMinimum(Min_zval);
    histo4_4->SetMaximum(Max_zval);
    histo4_4->SetStats(kFALSE);    
  
          
    TCanvas *canvas1 = new TCanvas("canvas1","Graph",0,0,1280,948);
    canvas1->SetRightMargin(0.2);
    histo2->Draw("zcol");
    
/*
    canvas1->Divide(3,2);
    canvas1->cd(1);
    canvas1->cd(1)->SetRightMargin(0.2);
    histo3_0->Draw("zcol");
    canvas1->cd(2);
    canvas1->cd(2)->SetRightMargin(0.2);
    histo3_1->Draw("zcol");
    canvas1->cd(3);
    canvas1->cd(3)->SetRightMargin(0.2);
    histo3_2->Draw("zcol");
    canvas1->cd(4);
    canvas1->cd(4)->SetRightMargin(0.2);
    histo4_0->Draw("zcol");
    canvas1->cd(5);
    canvas1->cd(5)->SetRightMargin(0.2);
    histo4_1->Draw("zcol");
    canvas1->cd(6);
    canvas1->cd(6)->SetRightMargin(0.2);
    histo4_2->Draw("zcol");

    
*/
/*

    canvas1->Divide(1,2);
    canvas1->cd(1)->Divide(3,1);    
    canvas1->cd(2)->Divide(3,1);
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
    canvas1->cd(2)->cd(1)->SetRightMargin(0.2);
    histo4_3->Draw("zcol");
    canvas1->cd(2)->cd(2);
    canvas1->cd(2)->cd(2)->SetRightMargin(0.2);
    histo4_4->Draw("zcol");
    canvas1->cd(2)->cd(3);
    canvas1->cd(2)->cd(3)->SetRightMargin(0.2);
    histo4->Draw("zcol");
 */

}
