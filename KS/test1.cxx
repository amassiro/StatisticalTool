


void test1(int reference = 10000, int test = 1000) {

  TH1::SetDefaultSumw2(1);
//   int reference = 10000;
  TH1F* histo_reference = new TH1F ("histo_reference", "", 100, -5, 5);
  histo_reference->FillRandom("gaus", reference);
  histo_reference->Draw();
  
//   int test = 1000;
  TH1F* histo_test = new TH1F ("histo_test", "", 100, -5, 5);
  histo_test->FillRandom("gaus", test);
  histo_test->Scale(1. * reference / test);
  histo_test->SetLineColor(kRed);
  histo_test->Draw("same");
  
  std::cout << " ------------- " << std::endl;
  std::cout << " stat reference = " << reference << std::endl;
  std::cout << " stat test      = " << test << std::endl;
  std::cout << " ------------- " << std::endl;
  std::cout << " delta KS       reference->test = " << histo_reference->KolmogorovTest(histo_test, "M") << std::endl;
  std::cout << " prob KS        reference->test = " << histo_reference->KolmogorovTest(histo_test)      << std::endl;
  std::cout << " prob KS (toys) reference->test = " << histo_reference->KolmogorovTest(histo_test, "X") << std::endl;
  std::cout << " ------------- " << std::endl;
  std::cout << " delta KS       test->reference = " << histo_test->KolmogorovTest(histo_reference, "M") << std::endl;
  std::cout << " prob KS        test->reference = " << histo_test->KolmogorovTest(histo_reference)      << std::endl;
  std::cout << " prob KS (toys) test->reference = " << histo_test->KolmogorovTest(histo_reference, "X") << std::endl;
  std::cout << " ------------- " << std::endl;
   
  float principal_delta = histo_reference->KolmogorovTest(histo_test, "M");

  TCanvas* cc = new TCanvas ("cc", "", 800, 600);
  TH1F* histo_delta = new TH1F ("histo_delta", "", 300, 0, 1.0);
  TH1F* histo_delta_more = new TH1F ("histo_delta_more", "", 300, 0, 1.0);
  
  int numtoys = 100000;
  for (int i=0; i<numtoys; i++) {
   
    histo_test->Reset();
    histo_test->FillRandom("gaus", test);
    histo_test->Scale(1. * reference / test);
    histo_test->SetLineColor(kRed);
    
    float delta = histo_reference->KolmogorovTest(histo_test, "M");
    histo_delta->Fill (delta);
    
    if (delta > principal_delta) {
      histo_delta_more->Fill (delta);
    }
  }
  
  histo_delta->Draw("hist");
  
  
  std::cout << " ------------- " << std::endl;
  std::cout << " prob delta KS       reference->test = " << histo_delta_more->Integral() / histo_delta->Integral() << std::endl;
  std::cout << " ------------- " << std::endl;
  
  
  
}


