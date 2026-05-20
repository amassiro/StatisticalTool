

void scanRoot () {

  TFile* _file0 = TFile::Open("higgsCombineTest.MultiDimFit.mH120.root");
  TFile* _file1 = TFile::Open("higgsCombineTest.MultiDimFit.mH125.root");


  TTree* limit_0   = (TTree*) _file0->Get("limit");
  TTree* limit_1   = (TTree*) _file1->Get("limit");

  float r_s0, r_s1, r_s2, r_s3;

  limit_0->SetBranchAddress("r_s0", &r_s0);
  limit_0->SetBranchAddress("r_s1", &r_s1);
  limit_0->SetBranchAddress("r_s2", &r_s2);
  limit_0->SetBranchAddress("r_s3", &r_s3);



  float RVV_1, RVV_2, RVV_3;
  float FR0;
  float err_FR0;

  limit_1->SetBranchAddress("r_s0", &r_s0);
  limit_1->SetBranchAddress("trackedParam_FR0",   &FR0);
  limit_1->SetBranchAddress("trackedError_FR0",   &err_FR0);
  limit_1->SetBranchAddress("RVV_1", &RVV_1);
  limit_1->SetBranchAddress("RVV_2", &RVV_2);
  limit_1->SetBranchAddress("RVV_3", &RVV_3);



  limit_0->GetEntry(0);

  std::cout << "Standard" << std::endl;
  std::cout << " r_s0 = " << r_s0 << std::endl;
  std::cout << " r_s1 = " << r_s1 << std::endl;
  std::cout << " r_s2 = " << r_s2 << std::endl;
  std::cout << " r_s3 = " << r_s3 << std::endl;



  limit_1->GetEntry(0);

  std::cout << "Normalized" << std::endl;
  std::cout << " r_s0  = " << r_s0 << std::endl;
  std::cout << " RVV_0 = " << FR0 << " +/- " << err_FR0 << std::endl;
  std::cout << " RVV_1 = " << RVV_1 << std::endl;
  std::cout << " RVV_2 = " << RVV_2 << std::endl;
  std::cout << " RVV_3 = " << RVV_3 << std::endl;



  // TFile* _file2 = TFile::Open("fitDiagnosticsTest.root");
  // RooWorkspace *w = (RooWorkspace*)_file1->Get("w");
  // RooFitResult *fit_s = (RooFitResult*)_file1->Get("fit_s");
  // RooAbsReal *fraction_0 = w->function("FR0");
  // double val = fraction_0->getVal();
  // double err = fraction_0->getPropagatedError(*fit_s);

  // std::cout << " RVV_0 = " << val << " +/- " << err << std::endl;




}

