
void Poisson(float mean){

  int MAXX = 5+10*mean;
  
  
  //---- build confidence bands

  std::vector<float> v_band_lo;
  std::vector<float> v_band_hi;
  std::vector<float> v_band_hi_flip_flop;
  
  
  TGraph* band_lo = new TGraph();
  TGraph* band_hi = new TGraph();
  TGraph* band_hi_flip_flop = new TGraph();
  
  int BIN = 20;
  float alpha = (1-0.6827);
  
  for (int i=0; i<MAXX*BIN; i++) {    
    //     std::cout << " [] -> " << int(1.*i/BIN) << " --> " << ROOT::Math::gamma_quantile   (alpha/2, int(1.*i/BIN)  , 1.) << " - " << ROOT::Math::gamma_quantile_c (alpha/2, int(1.*i/BIN)+1, 1.) << std::endl;
    band_lo->SetPoint (i, 1.*i/BIN,  ROOT::Math::gamma_quantile   (alpha/2, int(1.*i/BIN)  , 1.) );
    band_hi->SetPoint (i, 1.*i/BIN,  ROOT::Math::gamma_quantile_c (alpha/2, int(1.*i/BIN)+1, 1.) );
    if (int(1.*i/BIN) !=0 ) {
      band_hi_flip_flop->SetPoint (i, 1.*i/BIN,  ROOT::Math::gamma_quantile_c (alpha/2, int(1.*i/BIN)+1, 1.) );
//       band_hi_flip_flop->SetPoint (i, 1.*i/BIN,  ROOT::Math::gamma_quantile_c (alpha/2, int(1.*i/BIN)+1, 1.) -1 );
    }
    else {
      band_hi_flip_flop->SetPoint (i, 1.*i/BIN,  ROOT::Math::gamma_quantile_c (alpha, int(1.*i/BIN)+1, 1.) );      
    }
    
    if ((i%BIN) == 0) v_band_lo.push_back(  ROOT::Math::gamma_quantile   (alpha/2, int(1.*i/BIN)  , 1.) );
    if ((i%BIN) == 0) v_band_hi.push_back(  ROOT::Math::gamma_quantile_c (alpha/2, int(1.*i/BIN)+1, 1.) );
    if ((i%BIN) == 0) {
      if (int(1.*i/BIN) !=0 ) {
        v_band_hi_flip_flop.push_back(  ROOT::Math::gamma_quantile_c (alpha/2, int(1.*i/BIN)+1, 1.) );
//         v_band_hi_flip_flop.push_back(  ROOT::Math::gamma_quantile_c (alpha/2, int(1.*i/BIN)+1, 1.) - 1  );
      }
      else {
        v_band_hi_flip_flop.push_back(  ROOT::Math::gamma_quantile_c (alpha  , int(1.*i/BIN)+1, 1.)  );        
      }
    }
  }
  
  band_hi_flip_flop->SetLineWidth(4);
  band_hi_flip_flop->SetLineColor(kBlue);
  band_hi_flip_flop->SetLineStyle(3);
  band_hi->SetLineWidth(4);
  band_hi->SetLineColor(kRed);
  band_lo->SetLineWidth(4);
  band_lo->SetLineColor(kRed);
  
  
  
  //---- generate events
  
  TH2F* h2 = new TH2F ("h2","",MAXX, 0, MAXX, MAXX*100, 0, MAXX);
  TH1F* h1 = new TH1F ("h1","",MAXX, 0, MAXX);
  
  int coverage_counting = 0;
  int coverage_counting_flip_flop = 0;
  int NMAX = 10000;
  for (int i=0; i<NMAX; i++) {
    int ivalue = gRandom->Poisson(mean);
    h1->Fill(ivalue + 0.5);  
    h2->Fill(ivalue + 0.5, mean);  

    //
    //---- if the "mean" is included in the band defined by the observed measurement, count++
    if (ivalue < v_band_lo.size()) { 
      if ((mean >= v_band_lo.at(ivalue)) && (mean <= v_band_hi.at(ivalue))) {
        coverage_counting ++;
      } 
      if ((mean >= v_band_lo.at(ivalue)) && (mean <= v_band_hi_flip_flop.at(ivalue))) {
        coverage_counting_flip_flop ++;
      } 
    } 
    else {
      std::cout << " increase the MAXX to at least " << ivalue+1 << std::endl;
    }
    //
    //
    
  }
  
  std::cout << " ----------------------- " << std::endl;
  std::cout << " coverage             = " << 1. * coverage_counting / NMAX << std::endl;
  std::cout << " coverage (flip flop) = " << 1. * coverage_counting_flip_flop / NMAX << std::endl;
  std::cout << " ----------------------- " << std::endl;
  
  
  TCanvas* cc1d = new TCanvas ("cc1d", "", 800, 600);
  h1->Draw();
  cc1d->SetGrid();
  
  
  //---- draw 
  
  TCanvas* cc2d = new TCanvas ("cc2d", "", 800, 600);
  h2->Draw("colz");
  
  
  band_lo->Draw("PL");
  band_hi->Draw("PL");
  
  cc2d->SetGrid();
  
  
  
  
  TCanvas* cc2dbis = new TCanvas ("cc2dbis", "", 800, 600);
  band_lo->Draw("APL");
  band_hi->Draw("PL");
  band_hi_flip_flop->Draw("PL");
  
  cc2dbis->SetGrid();
  
  
  
  
  //---- test coverage
  
  
  
  
  
  
}



