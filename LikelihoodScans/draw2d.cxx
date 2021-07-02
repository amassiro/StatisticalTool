

TH1F* GetSmallestInterval1D(TH1F* h, double CL = 0.68)
{
  double width = 999999999.;
  double min = 0.;
  double max = 0.;
  int binMin = -1;
  int binMax = -1;
  
  double allIntegral = h->Integral(1, h->GetNbinsX());
  for(int bin1 = 1; bin1 <= h->GetNbinsX(); ++bin1){
    double tempMin = h -> GetBinCenter(bin1);
    for(int bin2 = bin1+1; bin2 <= h->GetNbinsX(); ++bin2)  
    {
      if( h->Integral(bin1,bin2)  < CL*allIntegral ) continue;
      
      double tempMax = h -> GetBinCenter(bin2);
      if( (tempMax-tempMin) < width )
      {
        width = tempMax-tempMin;
        min = tempMin;
        max = tempMax;
        binMin = bin1;
        binMax = bin2;
      }
      
      break;
    }
  }
  
  
  std::cout << " min = " << binMin << std::endl;
  std::cout << " max = " << binMax << std::endl;
  
  TH1F* clone = (TH1F*)(h->Clone());
  clone -> Clear();
  for(int bin = 1; bin <= h->GetNbinsX(); ++bin)
    if( (bin <= binMin) || (bin >= binMax) )
      clone -> SetBinContent(bin, 0);
      
  
  return clone;
}



std::pair<double,double> Support(TH1F* h, double CL = 0.68)
{
 double width = 999999999.;
 double min = 0.;
 double max = 0.;
 int binMin = -1;
 int binMax = -1;
 
 double allIntegral = h->Integral(1, h->GetNbinsX());
 for(int bin1 = 1; bin1 <= h->GetNbinsX(); ++bin1){
  double tempMin = h -> GetBinCenter(bin1);
  for(int bin2 = bin1+1; bin2 <= h->GetNbinsX(); ++bin2)  
  {
   if( h->Integral(bin1,bin2)  < CL*allIntegral ) continue;
   
   double tempMax = h -> GetBinCenter(bin2);
   if( (tempMax-tempMin) < width )
   {
    width = tempMax-tempMin;
    min = tempMin;
    max = tempMax;
    binMin = bin1;
    binMax = bin2;
   }
   
   break;
  }
 }
 std::cout << " min = " << binMin << std::endl;
 std::cout << " max = " << binMax << std::endl;
 
 std::cout << " hmin = " << h -> GetBinContent(binMin) << std::endl;
 std::cout << " hmax = " << h -> GetBinContent(binMax) << std::endl;
 
 return std::pair<double,double> (h->Integral(1,binMin), h->Integral(1,binMax));
}



#include <map>

TH1F* FC1D(TH1F* h, double CL = 0.68){

 std::vector<int> flags; 
 std::vector<int> flagsNo; 
 std::multimap<double,int> values;
 for(int bin = 1; bin <= h->GetNbinsX(); bin++){
  values.insert(std::pair<double, int>(h->GetBinContent(bin), bin));
 }
 
 double allIntegral = h->Integral(1, h->GetNbinsX()); 
 double tempIntegral = 0;
 
 for (std::multimap<double,int>::iterator iMap = values.begin(); iMap!= values.end(); iMap++){
  tempIntegral += iMap->first;
  if (tempIntegral > allIntegral*(1-CL)) flags.push_back(iMap->second);
  else flagsNo.push_back(iMap->second);
 }

 TH1F* clone = (TH1F*)(h->Clone()); 
 for (int iFlag = 0; iFlag<flags.size(); iFlag++ ){
  clone -> SetBinContent(flags.at(iFlag), h->GetBinContent(flags.at(iFlag)));
 }
 for (int iFlag = 0; iFlag<flagsNo.size(); iFlag++ ){
  clone -> SetBinContent(flagsNo.at(iFlag), 0);
 } 
  return clone;
}


TH2F* FC2D(TH2F* h, double CL = 0.68){

 std::vector<int> flags; 
 std::vector<int> flagsNo; 
 std::multimap<double,int> values;
 
 for (int iBinX = 1; iBinX <= h->GetXaxis()->GetNbins(); iBinX++){
  for (int iBinY = 1; iBinY <= h->GetYaxis()->GetNbins(); iBinY++){
    int bin = h->GetBin(iBinX,iBinY);
//    std::cout << " bin = " << bin <<  " : [" << iBinX << " , " << iBinY << "]" << std::endl;
    values.insert(std::pair<double, int>(h->GetBinContent(iBinX,iBinY),bin));
  }
 }

 double allIntegral = h->Integral(1, h->GetNbinsX(),1, h->GetNbinsY()); 
 double tempIntegral = 0;
 for (std::multimap<double,int>::iterator iMap = values.begin(); iMap!= values.end(); iMap++){
  tempIntegral += iMap->first;
  if (tempIntegral > allIntegral*(1-CL)) flags.push_back(iMap->second);
  else flagsNo.push_back(iMap->second);
 }
// std::cout << "size = " << flags.size() << " , " << flagsNo.size() << std::endl;

 TH2F* clone = (TH2F*)(h->Clone()); 
 for (int iFlag = 0; iFlag<flags.size(); iFlag++ ){
//  int iBinX;
//  int iBinY;
//  int iBinZ;
//  clone->GetBinXYZ(flags.at(iFlag),iBinX,iBinY,iBinZ); 
//  clone -> SetBinContent(iBinX,iBinY, h->GetBinContent(iBinX,iBinY));
//  clone -> SetBinContent(iBinX,iBinY, 1);
    
//    clone -> SetBinContent(flags.at(iFlag), CL);
//    clone -> SetBinContent(flags.at(iFlag), CL * allIntegral);
   clone -> SetBinContent(flags.at(iFlag), h->GetBinContent(flags.at(iFlag)));
 }
 for (int iFlag = 0; iFlag<flagsNo.size(); iFlag++ ){
//  int iBinX;
//  int iBinY;
//  int iBinZ;
//  clone->GetBinXYZ(flagsNo.at(iFlag),iBinX,iBinY,iBinZ); 
//  clone -> SetBinContent(iBinX,iBinY, h->GetBinContent(iBinX,iBinY));
//  clone -> SetBinContent(iBinX,iBinY, 0);

  clone -> SetBinContent(flagsNo.at(iFlag), 0);
 } 
  return clone;
}






void draw2d(){

  TH2F* h2d = new TH2F ("h2d", "", 100, -4, 4, 100, -4, 4);
  TH1F* h_profiled = new TH1F ("h_profiled", "", 100, -4, 4);
  TH1F* h_frozen   = new TH1F ("h_frozen",   "", 100, -4, 4);
  
  
  int NTOYS = 100000;

  //
  // simulation of no correlation
  //
  for (int itoy=0; itoy<NTOYS; itoy++) {
    //     Gaus (Double_t mean=0, Double_t sigma=1)
    float value_x = gRandom->Gaus (0.0, 1.0);
    float value_y = gRandom->Gaus (0.0, 1.0);
    
    h2d->Fill(value_x, value_y);
    h_frozen->Fill(value_x);
    h_profiled->Fill(value_x);
    
  }
  
  
  
  TCanvas* cc2d = new TCanvas ("cc2d", "", 800,600);
  h2d->Draw("colz");
  
  TH2F* h2d_68 = FC2D(h2d, 0.68);
  h2d_68->SetLineColor(kRed);
  h2d_68->Draw("same");
  std::cout << " 68% = " << h2d_68->Integral() / h2d->Integral() << std::endl;
  
  
  
  TCanvas* cc_frozen = new TCanvas ("cc_frozen", "", 800,600);
  h_frozen->Draw();
  TH1F* h_frozen_68 = FC1D(h_frozen, 0.68);
  h_frozen_68->SetLineColor(kRed);
  h_frozen_68->Draw("same");
  std::cout << " 68% = " << h_frozen_68->Integral() / h_frozen->Integral() << std::endl;
  


  TCanvas* cc_profiled = new TCanvas ("cc_profiled", "", 800,600);
  h_profiled->Draw();
  TH1F* h_profiled_68 = FC1D(h_profiled, 0.68);
  h_profiled_68->SetLineColor(kRed);
  h_profiled_68->Draw("same");
  std::cout << " 68% = " << h_profiled_68->Integral() / h_profiled->Integral() << std::endl;
  
  
  
  
  
  // likelihood
  TCanvas* cc2d_likelihood = new TCanvas ("cc2d_likelihood", "", 800,600);
  
  TF2 *gauss2d = new TF2("gauss2d","log(TMath::Gaus(x,[0],1)*TMath::Gaus(y,[1],1))",-1,1,-1,1);
//   gauss2d->Draw("surf");
  gauss2d->Draw("lego");
  
  
//   TF2 *flat2d = new TF2("flat2d","x*0 - 0.5",-4,4,-4,4);
//   flat2d->SetLineColor(kBlue);
//   flat2d->Draw("surf");
  
  
  
  TCanvas* cc1d_likelihood = new TCanvas ("cc1d_likelihood", "", 800,600);
  
  TF1 *gauss1d = new TF1("gauss1d","log(TMath::Gaus(x,[0],1))",-4,4);
  gauss1d->Draw();
  cc1d_likelihood->SetGrid();
  
}



