


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
    
}


