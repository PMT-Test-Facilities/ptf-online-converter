{

  {

    
     TFile *f = new TFile("output00225.root");
     TCanvas *c2 = new TCanvas("c2","Graph",0,0,1000,1000);
     TMultiGraph *mg = new TMultiGraph;
     // c->Divide(1,3);
     // c->cd(1);
   
    int time;
    double gantry0_x;
    double gantry0_y;
    double gantry0_z;
    double phidg1_Btot;
    double phidg0_Btot;
    double phidg1_Bx;
    double phidg1_By;
    double phidg1_Bz;
    double phidg0_Bx;
    double phidg0_By;
    double phidg0_Bz;
    double max =0 ;
    double min = 100;
    double last_value_x=1000;
    double last_value_y=1000;
    double last_value_Btot;
    double difference = 1000;
    int count = 0;
    int count1 = 0;
    int count2 = 0;
    int count3 = 0;
    int count4 = 0;
    int count5 = 0;
    
     // SetBRanchAddress returns the adress of the branch. To Get a value form the tree call .GetEntry(index)
    scan_tree.SetBranchAddress("time",&time);
    scan_tree.SetBranchAddress("gantry0_x",&gantry0_x);
    scan_tree.SetBranchAddress("gantry0_y",&gantry0_y);
    scan_tree.SetBranchAddress("gantry0_z",&gantry0_z);
    scan_tree.SetBranchAddress("phidg1_Btot",&phidg1_Btot);
    scan_tree.SetBranchAddress("phidg0_Btot",&phidg0_Btot);
    scan_tree.SetBranchAddress("phidg1_Bx",&phidg1_Bx);
    scan_tree.SetBranchAddress("phidg1_By",&phidg1_By);
    scan_tree.SetBranchAddress("phidg1_Bz",&phidg1_Bz);
    scan_tree.SetBranchAddress("phidg0_Bx",&phidg0_Bx);
    scan_tree.SetBranchAddress("phidg0_By",&phidg0_By);
    scan_tree.SetBranchAddress("phidg0_Bz",&phidg0_Bz);

   
    // returns the totla number of entries 
    int entries = scan_tree->GetEntries();

    TGraph2D *graph2D = new TGraph2D();
    TGraph *graph = new TGraph();
    TGraph *graph1 = new TGraph();
    TGraph *graph2 = new TGraph();
    TGraph *graph3 = new TGraph();
    TGraph *graph4 = new TGraph();
    TGraph *graph5 = new TGraph();

    for(int ient = 0; ient < entries; ient++){    
      scan_tree->GetEntry(ient);    
      //    std::cout<< phidg1_Bx <<" : " << phidg1_By << " : " << phidg1_Bz << " : "  << phidg1_Btot<< " : " << sqrt(phidg1_Bx*phidg1_Bx+phidg1_By*phidg1_By+phidg1_Bz*phidg1_Bz) << std::endl; 
      /*
      if ((last_value_x == gantry0_x) && (last_value_y == gantry0_y))
	difference = fabs(last_value_Btot-phidg1_Btot);
      //	std::cout << fabs(last_value_Btot-phidg1_Btot) << " : " << difference<< std::endl;
      // std::cout << gantry0_x<<" : " <<  gantry0_y<< " : " << gantry0_x- 0.160152 << " : " << phidg1_Btot << std::endl;;

      last_value_Btot=phidg1_Btot;
      last_value_x=gantry0_x;
      last_value_y=gantry0_y;



    if(difference != 1000){
  	if (difference > max)
	  max = difference;
  	if(Difference < min)
	  min = difference;} 

      */
      if( gantry0_z > 0.35 && gantry0_z<0.45){
         
          if( (gantry0_x != last_value_x)  || (gantry0_y != last_value_y)){        
              graph2D->SetPoint(count4,gantry0_x,gantry0_y,phidg0_Btot);
              count4++;
              std::cout<<phidg0_Btot << std::endl;
          }
          last_value_x = gantry0_x;
          last_value_y = gantry0_y;
      }

          graph->SetPoint(count4,gantry0_x,gantry0_y);

     if ((gantry0_x- 0.000174757)<0 && gantry0_z <0.01){
	        graph1->SetPoint(count5,gantry0_y,phidg0_Btot);
	        count5++;
	    }

     // std::cout << gantry0_x << " : " << gantry0_x-0.160152 <<std::endl;
     
     
      if (((gantry0_x- 0.160152)<0.01) && ((gantry0_x- 0.160152)>-0.05)&& gantry0_z <0.01){
	        graph2->SetPoint(count,gantry0_y,phidg0_Btot);
  	      count ++;
	    }
      
      if (((gantry0_x- 0.320129)<0.0001) && ((gantry0_x- 0.320129)>-0.05)&& gantry0_z <0.01){
          if (count1 < 24)
	            graph3->SetPoint(count1,gantry0_y,phidg0_Btot);
	        count1 ++;}
      
      if (((gantry0_x- 0.480107)<0.01) && ((gantry0_x- 0.480107)>-0.05)&& gantry0_z <0.01){
      	  graph4->SetPoint(count2,gantry0_y,phidg0_Btot);
	        count2 ++;}

      if (((gantry0_x- 0.640084)<0.01) && ((gantry0_x- 0.640084)>-0.05)&& gantry0_z <0.01){
      	  graph5->SetPoint(count3,gantry0_y,phidg0_Btot);
	        count3 ++;}
      

      
    }
    //    std:: cout << "Max: " << max << std::endl;
    // std:: cout << "Min: " << min << std::endl;

    gStyle->SetPalette(1);

    graph1->SetMarkerStyle(20);
    graph1->SetMarkerColor(1);
    graph1->SetLineColor(1);
    graph2->SetMarkerStyle(21);
    graph2->SetMarkerColor(2);
    graph2->SetLineColor(2);
    graph3->SetMarkerStyle(22);
    graph3->SetMarkerColor(3);
    graph3->SetLineColor(3);
    graph4->SetMarkerStyle(23);
    graph4->SetMarkerColor(4);
    graph4->SetLineColor(4);
    graph5->SetMarkerStyle(24);
    graph5->SetMarkerColor(6);
    graph5->SetLineColor(6);

    mg->Add(graph1);
    mg->Add(graph2);
    mg->Add(graph3);
    mg->Add(graph4);
    mg->Add(graph5);

    //mg -> Draw("apl");
    //graph->SetMarkerStyle(20);
    //graph->Draw("zCOL");
    graph2D->SetMinimum(1.2);
    graph2D->SetMarkerStyle(20);
    graph2D ->Draw("zcol");
    return c; 
  }

}
