
void Poisson(float mean){

  //---- generate events
  int MAXX = 10;
  
  TH2F* h2 = new TH2F ("h2","",MAXX, 0, MAXX, MAXX*100, 0, MAXX);
  TH1F* h1 = new TH1F ("h1","",MAXX, 0, MAXX);
  
  int NMAX = 10000;
  for (int i=0; i<NMAX; i++) {
    float value = gRandom->Poisson(mean);
    h1->Fill(value + 0.5);  
    h2->Fill(value + 0.5, mean);      
  }
  
  TCanvas* cc1d = new TCanvas ("cc1d", "", 800, 600);
  h1->Draw();
  
  TCanvas* cc2d = new TCanvas ("cc2d", "", 800, 600);
  h2->Draw("colz");
  
  
  cc1d->SetGrid();
  

  //---- build confidence bands
  
  TGraph* band_lo = new TGraph();
  TGraph* band_hi = new TGraph();
  
  int BIN = 20;
  float alpha = (1-0.6827);
  
  for (int i=0; i<MAXX*BIN; i++) {    
//     std::cout << " [] -> " << int(1.*i/BIN) << " --> " << ROOT::Math::gamma_quantile   (alpha/2, int(1.*i/BIN)  , 1.) << " - " << ROOT::Math::gamma_quantile_c (alpha/2, int(1.*i/BIN)+1, 1.) << std::endl;
    band_lo->SetPoint (i, 1.*i/BIN,  ROOT::Math::gamma_quantile   (alpha/2, int(1.*i/BIN)  , 1.) );
    band_hi->SetPoint (i, 1.*i/BIN,  ROOT::Math::gamma_quantile_c (alpha/2, int(1.*i/BIN)+1, 1.) );
  }
  
  band_hi->SetLineWidth(4);
  band_hi->SetLineColor(kRed);
  band_lo->SetLineWidth(4);
  band_lo->SetLineColor(kRed);
  
  band_lo->Draw("PL");
  band_hi->Draw("PL");
  
  cc2d->SetGrid();
  
  
  
  
  TCanvas* cc2dbis = new TCanvas ("cc2dbis", "", 800, 600);
  band_lo->Draw("APL");
  band_hi->Draw("PL");
  
  cc2dbis->SetGrid();
  
  
  
}



