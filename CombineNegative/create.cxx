


void create() {
  
  TH1F* bkg1 = new TH1F ("histo_bkg1", "bkg1", 3, 0, 3);
  
  bkg1-> SetBinContent (1, 3);
  bkg1-> SetBinContent (2, 20);
  bkg1-> SetBinContent (3, 10);
   
  // 3+20+10 = 33

  
  TH1F* bkg2 = new TH1F ("histo_bkg2", "bkg2", 3, 0, 3);
  
  bkg2-> SetBinContent (1, -1);
  bkg2-> SetBinContent (2, -2);
  bkg2-> SetBinContent (3, -7);

  // -1 -2 -7 = -10
  
  TH1F* sig = new TH1F ("histo_sig", "sig", 3, 0, 3);
  
  sig-> SetBinContent (1, 5);
  sig-> SetBinContent (2, 2);
  sig-> SetBinContent (3, 3);
  
  // 5+2+3 = 10
  
  TH1F* data = new TH1F ("histo_Data", "data", 3, 0, 3);
  
  data-> SetBinContent (1, 4);
  data-> SetBinContent (2, 22);
  data-> SetBinContent (3, 13);
  
  // 4+22+13 = 39
  
  
  TFile* fileOut = new TFile ("test.root", "RECREATE");
  
  bkg1->Write();
  bkg2->Write();
  sig->Write();
  data->Write();




  
}
