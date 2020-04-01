

void plot(float signal = 10.) {
  
//   
//   s/sqrt(b + c*b*b)
//   
  
  Color_t* color = new Color_t [200];
  color[0] = kBlue; //kRed ;
  color[1] = kAzure + 10 ;
  color[2] = kGreen ;
  color[3] = kOrange ;
  color[4] = kOrange + 1 ;
  color[5] = kOrange + 2 ;
  color[6] = kRed    + 0 ;
  color[7] = kRed    + 1 ;
  color[8] = kRed    + 2 ;
  color[9] = kRed    + 3 ;
  color[10]= kRed    + 4 ;
  for (int i=0; i<30; i++) {
    color[i+11] = kBlue + i;
  }
  
  

  float systematic = 0;
  float delta_systematic = 0.01;
  int totalSyst = 10;
  
  TF1* significance[totalSyst];
  
  for (int iSyst = 0; iSyst<totalSyst; iSyst++) {
    
    systematic = delta_systematic * iSyst;
    
    TString formula = Form ("%f / sqrt (x + %f * x*x)", signal, systematic);
    TString name = Form ("%f_%f", signal, systematic);
    
    significance[iSyst] = new TF1 (name.Data(), formula.Data(), 0.1, 100);
   
    significance[iSyst] -> SetLineColor(color[iSyst]);
    significance[iSyst] -> SetMarkerColor(color[iSyst]);

    if (iSyst == 0) {
      significance[iSyst] -> SetLineWidth(2); 
    }
    
  }

  
  
  for (int iSyst = 0; iSyst<totalSyst; iSyst++) {
    
    if (iSyst == 0) {
      significance[iSyst] -> Draw("PL");
      significance[iSyst] -> GetXaxis() -> SetTitle ("Background events");
      significance[iSyst] -> GetYaxis() -> SetTitle ("Significance");
    }
    else {
      significance[iSyst] -> Draw("PL same");
    }
    
  }
  
  
}





