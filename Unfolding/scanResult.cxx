

void scanResult () {

  TFile* _file0 = TFile::Open("higgsCombineTest.MultiDimFit.mH120.root");
  TFile* _file1 = TFile::Open("higgsCombineTest.MultiDimFit.mH140.root");


  TTree* limit_0   = (TTree*) _file0->Get("limit");
  TTree* limit_1   = (TTree*) _file1->Get("limit");

  float r_s0, r_s1, r_s2, r_s3;

  limit_0->SetBranchAddress("r_s0", &r_s0);
  limit_0->SetBranchAddress("r_s1", &r_s1);
  limit_0->SetBranchAddress("r_s2", &r_s2);
  limit_0->SetBranchAddress("r_s3", &r_s3);



  float r_cross_section;
  float F0, F1, F2;
  float err_F0, err_F1, err_F2;
  float F3;
  float err_F3;

  limit_1->SetBranchAddress("r_cross_section", &r_cross_section);
  limit_1->SetBranchAddress("trackedParam_F3",   &F3);
  limit_1->SetBranchAddress("trackedError_F3",   &err_F3);
  limit_1->SetBranchAddress("F0", &F0);
  limit_1->SetBranchAddress("F1", &F1);
  limit_1->SetBranchAddress("F2", &F2);


  limit_0->GetEntry(0);

  std::cout << std::endl;
  std::cout << " --------- " << std::endl;
  std::cout << "Unfolded cross section" << std::endl;
  std::cout << " r_s0 = " << r_s0 << std::endl;
  std::cout << " r_s1 = " << r_s1 << std::endl;
  std::cout << " r_s2 = " << r_s2 << std::endl;
  std::cout << " r_s3 = " << r_s3 << std::endl;

  // F3        rateParam   *   Signal3   (1-0.2*@0-0.3*@1-0.4*@2)/0.1   temp0,temp1,temp2
  //                                      --> 0.1 for F3
  std::cout << " effective r cross section = " << r_s0 * 0.2 + r_s1 * 0.3 + r_s2 * 0.4 + r_s3 * 0.1  << std::endl;

  std::cout << std::endl;
  std::cout << " xs0 = " << r_s0*0.2 << std::endl;
  std::cout << " xs1 = " << r_s1*0.3 << std::endl;
  std::cout << " xs2 = " << r_s2*0.4 << std::endl;
  std::cout << " xs3 = " << r_s3*0.1 << std::endl;


  limit_1->GetEntry(0);

  std::cout << std::endl;
  std::cout << " --------- " << std::endl;
  std::cout << "Normalized unfolded cross section" << std::endl;
  std::cout << " multidimfit version " << std::endl;
  std::cout << " r_cross_section  = " << r_cross_section << std::endl;
  std::cout << " F1 = " << F1 << std::endl;
  std::cout << " F2 = " << F2 << std::endl;
  std::cout << " F3 = " << F3 << std::endl;


  TFile* _file2 = TFile::Open("fitDiagnosticsTest.root");
  RooFitResult *fit_s = (RooFitResult*)_file2->Get("fit_s");

  int status = fit_s->status();
  int covQual = fit_s->covQual();
  double minNll = fit_s->minNll();

  RooArgList finalPars = fit_s->floatParsFinal();
  RooRealVar* roofit_r_cross_section = (RooRealVar*)finalPars.find("r_cross_section");
  RooRealVar* roofit_F0 = (RooRealVar*)finalPars.find("F0");
  RooRealVar* roofit_F1 = (RooRealVar*)finalPars.find("F1");
  RooRealVar* roofit_F2 = (RooRealVar*)finalPars.find("F2");

  double xs = roofit_r_cross_section->getVal();  double exs = roofit_r_cross_section->getError();
  double v0 = roofit_F0->getVal();  double e0 = roofit_F0->getError();
  double v1 = roofit_F1->getVal();  double e1 = roofit_F1->getError();
  double v2 = roofit_F2->getVal();  double e2 = roofit_F2->getError();

  //
  // Calculate central value for the dependent parameter F3
  // F3        rateParam   *   Signal3   (1-0.2*@0-0.3*@1-0.4*@2)/0.1   temp0,temp1,temp2

  double v3 = (1.0 - 0.2*v0 - 0.3*v1 - 0.4*v2)/0.1;
  double e3 = 0;

  std::cout << std::endl;
  std::cout << " r_cross_section  = " << xs << " +/- " << exs << std::endl;
  std::cout << " F0 = " << v0 << " +/- " << e0 << std::endl;
  std::cout << " F1 = " << v1 << " +/- " << e1 << std::endl;
  std::cout << " F2 = " << v2 << " +/- " << e2 << std::endl;



  int idx0 = finalPars.index("F0");
  int idx1 = finalPars.index("F1");
  int idx2 = finalPars.index("F2");
  const TMatrixDSym& cov = fit_s->covarianceMatrix();

  //
  // F3        rateParam   *   Signal3   (1-0.2*@0-0.3*@1-0.4*@2)/0.1   temp0,temp1,temp2
  // the derivatives: dF3/dFx
  //

  double der0 = -0.2/0.1;
  double der1 = -0.3/0.1;
  double der2 = -0.4/0.1;

  double e0_squared = (der0 * der0 * cov(idx0, idx0)) +
                      (der1 * der1 * cov(idx1, idx1)) +
                      (der2 * der2 * cov(idx2, idx2)) +
                      (2.0 * der0 * der1 * cov(idx0, idx1)) +
                      (2.0 * der0 * der2 * cov(idx0, idx2)) +
                      (2.0 * der1 * der2 * cov(idx1, idx2));
  e3 = std::sqrt(e0_squared);

  std::cout << " F3 = " << v3 << " +/- " << e3 << std::endl;

  std::cout << std::endl;
  std::cout << std::endl;

  std::cout << "closure :: " << 0.2 * v0 + 0.3 * v1 + 0.4 * v2 + 0.1 * v3   <<std::endl;

}

