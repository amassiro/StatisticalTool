{
  
  //---- functions for coverage   
  TCanvas* cc1 = new TCanvas ("cc1", "", 800, 600);  
  TF1* Nb = new TF1("Nb", "1./2. * (TMath::Erf((1-x)/sqrt(2)) - TMath::Erf((-1-x)/sqrt(2)))");
  Nb->Draw();
  Nb->GetXaxis()->SetTitle("bias");
  Nb->GetYaxis()->SetTitle("#int_{-1}^{+1} N(b,1)");
  
  
  TCanvas* cc2 = new TCanvas ("cc2", "", 800, 600);
  TF1* NbError = new TF1("NbError", "(1./2. * (TMath::Erf((1)/sqrt(2)) - TMath::Erf((-1)/sqrt(2)))) - 1./2. * (TMath::Erf((1-x)/sqrt(2)) - TMath::Erf((-1-x)/sqrt(2)))");
  NbError->Draw();
  NbError->GetXaxis()->SetTitle("bias");
  NbError->GetYaxis()->SetTitle("68\% - #int_{-1}^{+1} N(b,1)");
  
  TCanvas* cc3 = new TCanvas ("cc3", "", 800, 600);
  // 68% - Nb(b, sigma) = 0  --> find sigma
  TF2* NbSigma = new TF2("NbError", "abs((1./2. * (TMath::Erf((1)/sqrt(2)) - TMath::Erf((-1)/sqrt(2)))) - 1./2. * (TMath::Erf((1-x)/sqrt(2)*y) - TMath::Erf((-1-x)/sqrt(2)*y)))", 0, 0.7, 1, 1.5);
  NbSigma->Draw("colz");
  NbSigma->SetNpx(1000);
  NbSigma->SetNpy(1000);
  NbSigma->GetXaxis()->SetTitle("bias");
  NbSigma->GetYaxis()->SetTitle("#sigma such that 68\% - #int_{-1}^{+1} N(b,#sigma) = 0");
  NbSigma->SetMaximum(0.02);
//   NbSigma->SetMinimum(-0.002);
  NbSigma->Draw("colz");
  
}
