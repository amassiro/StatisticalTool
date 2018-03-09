
void Poisson(float mean){

  TH2F* h2 = new TH2F ("h2","",20,0,20, 2000, 0, 20);
  TH1F* h1 = new TH1F ("h1","",20,0,20);
  
  int NMAX = 10000;
  for (int i=0; i<NMAX; i++) {
    float value = gRandom->Poisson(mean);
    h1->Fill(value);  
    h2->Fill(value, mean);      
  }
  
  TCanvas* cc1d = new TCanvas ("cc1d", "", 800, 600);
  h1->Draw();
  
  TCanvas* cc2d = new TCanvas ("cc2d", "", 800, 600);
  h2->Draw("colz");
  
  
}



