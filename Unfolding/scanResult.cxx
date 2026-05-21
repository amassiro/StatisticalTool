

void scanResult () {

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

  for (int i=0; i<limit_1->GetEntries(); i++) {
    limit_1->GetEntry(i);
    std::cout << " [i=" << i << "] :: RVV_0 = " << FR0 << " +/- " << err_FR0 << std::endl;
    std::cout << "                 :: RVV_1 = " << RVV_1 << std::endl;
    std::cout << "                 :: RVV_2 = " << RVV_2 << std::endl;
    std::cout << "                 :: RVV_3 = " << RVV_3 << std::endl;
    std::cout << "                 :: r_s0  = " << r_s0 << std::endl;
  }


  TFile* _file2 = TFile::Open("fitDiagnosticsTest.root");
  // RooWorkspace *w = (RooWorkspace*)_file1->Get("w");
  RooFitResult *fit_s = (RooFitResult*)_file2->Get("fit_s");
  // RooAbsReal *fraction_0 = w->function("FR0");
  // double val = fraction_0->getVal();
  // double err = fraction_0->getPropagatedError(*fit_s);

  int status = fit_s->status();
  int covQual = fit_s->covQual();
  double minNll = fit_s->minNll();

  std::cout << std::string(70, '=') << std::endl;
  std::cout << "                   ROOFITRESULT DIAGNOSTICS" << std::endl;
  std::cout << std::string(70, '=') << std::endl;
  std::cout << "  Minuit Fit Status     : " << status << " (0 = Converged successfully)" << std::endl;
  std::cout << "  Covariance Quality    : " << covQual << " (3 = Full, accurate matrix)" << std::endl;
  std::cout << "  Minimum -log(L) Value : " << minNll << std::endl;
  std::cout << std::string(70, '-') << std::endl;

  RooArgList finalPars = fit_s->floatParsFinal();
  finalPars.Print();
  RooRealVar* crosssection = (RooRealVar*)finalPars.find("r_s0");
  RooRealVar* fr1 = (RooRealVar*)finalPars.find("RVV_1");
  RooRealVar* fr2 = (RooRealVar*)finalPars.find("RVV_2");
  RooRealVar* fr3 = (RooRealVar*)finalPars.find("RVV_3");

  double xs = crosssection->getVal();  double exs = crosssection->getError();
  double v1 = fr1->getVal();  double e1 = fr1->getError();
  double v2 = fr2->getVal();  double e2 = fr2->getError();
  double v3 = fr3->getVal();  double e3 = fr3->getError();

  // Calculate central value for the dependent parameter FR3
  double v0 = 1.0 - v1 - v2 - v3;
  double e0 = 0;

  std::cout << std::fixed << std::setprecision(5);
  std::cout << "  Post-Fit Central Values & Individual Symmetric Errors:" << std::endl;
  std::cout << "    xs  = " << xs << " +/- " << exs << std::endl;
  std::cout << "    FR0 = " << v0 << " +/- " << e0 << std::endl;
  std::cout << "    FR1 = " << v1 << " +/- " << e1 << std::endl;
  std::cout << "    FR2 = " << v2 << " +/- " << e2 << std::endl;
  std::cout << "    FR3 = " << v3 << " +/- " << e3 << std::endl;


  double val_fr1 = v1;
  double val_fr2 = v2;
  double val_fr3 = v3; // FR3 is literally just RVV_3
  double val_fr0 = (250.0 - 40.0*v1 - 80.0*v2 - 50.0*v3) / 80.0;

  int idx1 = finalPars.index("RVV_1");
  int idx2 = finalPars.index("RVV_2");
  int idx3 = finalPars.index("RVV_3");
  const TMatrixDSym& cov = fit_s->covarianceMatrix();

  // 4. Propagate Error for FR0 using the Gradient Matrix
  // d(FR0)/d(RVV_1) = -40/80 = -0.5
  // d(FR0)/d(RVV_2) = -80/80 = -1.0
  // d(FR0)/d(RVV_3) = -50/80 = -0.625
  double der1 = -40.0 / 80.0;
  double der2 = -80.0 / 80.0;
  double der3 = -50.0 / 80.0;

  double var_fr0 = (der1 * der1 * cov(idx1, idx1)) +
          (der2 * der2 * cov(idx2, idx2)) +
          (der3 * der3 * cov(idx3, idx3)) +
          (2.0 * der1 * der2 * cov(idx1, idx2)) +
          (2.0 * der1 * der3 * cov(idx1, idx3)) +
          (2.0 * der2 * der3 * cov(idx2, idx3));
  double err_fr0 = std::sqrt(var_fr0);

  // 5. Errors for FR1, FR2, FR3 are just their direct POI errors
  double err_fr1 = fr1->getError();
  double err_fr2 = fr2->getError();
  double err_fr3 = fr3->getError();

  // Print final results
  std::cout << std::string(60, '=') << std::endl;
  std::cout << "        FINAL EXACT COUPLING FRACTIONS & ERRORS" << std::endl;
  std::cout << std::string(60, '=') << std::endl;
  std::cout << std::fixed << std::setprecision(5);
  std::cout << "  FR0 = " << val_fr0 << " +/- " << err_fr0 << " (propagated with correlations)" << std::endl;
  std::cout << "  FR1 = " << val_fr1 << " +/- " << err_fr1 << std::endl;
  std::cout << "  FR2 = " << val_fr2 << " +/- " << err_fr2 << std::endl;
  std::cout << "  FR3 = " << val_fr3 << " +/- " << err_fr3 << std::endl;
  std::cout << std::string(60, '=') << std::endl << std::endl;


  // std::cout << " RVV_0 = " << val << " +/- " << err << std::endl;




}

