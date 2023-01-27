


void create() {
  
  TH1F* bkg1 = new TH1F ("histo_bkg1", "bkg1", 3, 0, 3);
  
  bkg1-> SetBinContent (1, 5);
  bkg1-> SetBinContent (2, 20);
  bkg1-> SetBinContent (3, 10);
   
  // 5+20+10 = 35

  
  TH1F* bkg1_jes_up = new TH1F ("histo_bkg1_jesUp", "bkg1 up", 3, 0, 3);
  
  bkg1_jes_up-> SetBinContent (1, 6);
  bkg1_jes_up-> SetBinContent (2, 20);
  bkg1_jes_up-> SetBinContent (3, 11);

  TH1F* bkg1_jes_down = new TH1F ("histo_bkg1_jesDown", "bkg1 down", 3, 0, 3);
  
  bkg1_jes_down-> SetBinContent (1, 4);
  bkg1_jes_down-> SetBinContent (2, 19);
  bkg1_jes_down-> SetBinContent (3, 10);
  
   
  TH1F* sig = new TH1F ("histo_sig", "sig", 3, 0, 3);
  
  sig-> SetBinContent (1, 5);
  sig-> SetBinContent (2, 2);
  sig-> SetBinContent (3, 3);
  
  
  // 5+2+3 = 10
  
  
  
  TH1F* data = new TH1F ("histo_Data", "data", 3, 0, 3);
  
  data-> SetBinContent (1, 10);
  data-> SetBinContent (2, 22);
  data-> SetBinContent (3, 13);
  
  
  
  TFile* fileOut = new TFile ("test.root", "RECREATE");
  
  bkg1->Write();
  bkg1_jes_up->Write();
  bkg1_jes_down->Write();
  sig->Write();
  data->Write();


  
  TFile* fileOut2 = new TFile ("testNew.root", "RECREATE");
  
  TH1F* data_NEW = new TH1F ("histo_Data", "data", 1, 0, 1);
  
  data_NEW-> SetBinContent (1, 45);
  
  TH1F* bkg1_NEW = new TH1F ("histo_bkg1", "bkg1", 1, 0, 1);
  bkg1_NEW-> SetBinContent (1, 35);

  TH1F* sig_NEW = new TH1F ("histo_sig", "sig", 1, 0, 1);
  sig_NEW-> SetBinContent (1, 10);
  
  
  // lnN    0.9/1.2
  TH1F* bkg1_jes_down_NEW = new TH1F ("histo_bkg1_jesDown", "bkg1 down", 1, 0, 1);
  bkg1_jes_down_NEW-> SetBinContent (1, 31.5);       // 35      * 0.9 = 31.5
  
//   bkg1_jes_up-> SetBinContent (1, 4.5);       // 5      * 0.9 = 4.5
//   bkg1_jes_up-> SetBinContent (2, 18);        // 20     * 0.9 = 18
//   bkg1_jes_up-> SetBinContent (3, 9);         // 10     * 0.9 = 9
  
  TH1F* bkg1_jes_up_NEW = new TH1F ("histo_bkg1_jesUp", "bkg1 up", 1, 0, 1);  
  bkg1_jes_up_NEW-> SetBinContent (1, 42);     // 35      * 1.2 = 42

//   bkg1_jes_down-> SetBinContent (1, 6);     // 5      * 1.2 = 6
//   bkg1_jes_down-> SetBinContent (2, 24);    // 20     * 1.2 = 24
//   bkg1_jes_down-> SetBinContent (3, 12);    // 10     * 1.2 = 12
  
  
  // lnN      1.1
  TH1F* sig_jes_up_NEW = new TH1F ("histo_sig_jesUp", "sig up", 1, 0, 1);
  sig_jes_up_NEW-> SetBinContent (1, 11);    // 10     * 1.1 = 11
  
//   sig_jes_up-> SetBinContent (1, 6);    // 5     * 1.1 = 5.5
//   sig_jes_up-> SetBinContent (2, 20);   // 2     * 1.1 = 2.2
//   sig_jes_up-> SetBinContent (3, 11);   // 3     * 1.1 = 3.3
  
  TH1F* sig_jes_down_NEW = new TH1F ("histo_sig_jesDown", "sig down", 1, 0, 1);
  sig_jes_down_NEW-> SetBinContent (1, 9.090909091);     // 10      / 1.1 = 9.090909091
  
//   sig_jes_down-> SetBinContent (1, 4.545454545);     // 5      / 1.1 = 4.545454545
//   sig_jes_down-> SetBinContent (2, 1.818181818);     // 2      / 1.1 = 1.818181818
//   sig_jes_down-> SetBinContent (3, 2.727272727);     // 3      / 1.1 = 2.727272727
  
  
  bkg1_NEW->Write();
  sig_NEW->Write();

  bkg1_jes_up_NEW->Write();
  bkg1_jes_down_NEW->Write();
  sig_jes_up_NEW->Write();
  sig_jes_down_NEW->Write();
  
  data_NEW->Write();
  
  
}
