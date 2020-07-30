


void create() {
  
  TH1F* bkg = new TH1F ("histo_bkg", "bkg", 3, 0, 3);
  
  bkg-> SetBinContent (1, 3);
  bkg-> SetBinContent (2, 20);
  bkg-> SetBinContent (3, 10);
   
  // 3+20+10 = 33

  TH1F* sig = new TH1F ("histo_sig", "sig", 3, 0, 3);
  
  sig-> SetBinContent (1, 1);
  sig-> SetBinContent (2, 2);
  sig-> SetBinContent (3, 3);
  
  // 1+2+3 = 6
  
  TH1F* data = new TH1F ("histo_Data", "data", 3, 0, 3);
  
  data-> SetBinContent (1, 4);
  data-> SetBinContent (2, 22);
  data-> SetBinContent (3, 13);
  
  // 4+22+13 = 39
  
  
  TFile* fileOut = new TFile ("test.root", "RECREATE");
  
  bkg->Write();
  sig->Write();
  data->Write();






// 
//   1 single bin case
// 
// 
//   TH1F* bkg = new TH1F ("histo_bkg", "bkg", 1, 0, 1);
//   
//   bkg-> SetBinContent (1, 3);
//   
//   
//   TH1F* sig = new TH1F ("histo_sig", "sig", 1, 0, 1);
//   
//   sig-> SetBinContent (1, 1);
//   
//   
//   TH1F* data = new TH1F ("histo_Data", "data", 1, 0, 1);
//   
//   data-> SetBinContent (1, 4);
//   
//   TFile* fileOut = new TFile ("test.root", "RECREATE");
//   
//   bkg->Write();
//   sig->Write();
//   data->Write();
// 
  
}
