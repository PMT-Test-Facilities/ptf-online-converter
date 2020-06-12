{
   TCanvas *c1 = new TCanvas("c1","Graph2D example",0,0,600,400);
   Double_t P = 5.;
   Int_t npx  = 20 ;
   Int_t npy  = 20 ;
   Double_t x = -P;
   Double_t y = -P;
   Double_t z;
   Int_t k = 0;
   Double_t dx = (2*P)/npx;
   Double_t dy = (2*P)/npy;
   TGraph2D *dt = new TGraph2D(npx*npy);
   dt->SetNpy(41);
   dt->SetNpx(40);
   for (Int_t i=0; i<npx; i++) {
      for (Int_t j=0; j<npy; j++) {
         z = sin(sqrt(x*x+y*y))+1;
         dt->SetPoint(k,x,y,z);
         k++;
         y = y+dy;
      }
      x = x+dx;
      y = -P;
   }
   gStyle->SetPalette(1);
   dt->SetMarkerStyle(20);
   dt->Draw("surf1");
   return c1;
}
