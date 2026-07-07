

void test(){

  // cross sections
  float A = 0.646;
  float sigmaA = 0.056;

  float B = 0.389;
  float sigmaB = 0.047;

  float C = A+B;
  float sigmaC = sqrt(sigmaA*sigmaA + sigmaB*sigmaB);


  std::cout << "A = " << A << " +/- " << sigmaA << "    (" << sigmaA/A*100 << " %)" << std::endl;
  std::cout << "B = " << B << " +/- " << sigmaB << "    (" << sigmaB/B*100 << " %)" << std::endl;
  std::cout << "C = " << C << " +/- " << sigmaC << "    (" << sigmaC/C*100 << " %)" << std::endl;

  std::cout << " ------- " << std::endl;


  //
  // 10000 is the luminosity
  //
  float lumi = 10000;

  float background_uncertainty = 0.09; // relative uncertainty

  // signal A
  float SigA = 7543;
  float BkgA = 7847;
  float effA = SigA / (A * lumi);

  // signal B
  float SigB = 4297;
  float BkgB = 5062;
  float effB = SigB / (B * lumi);

  // signal C
  float SigC = SigA + SigB;
  float BkgC = BkgA + BkgB;
  float effC = SigC / (C * lumi);


  //
  // in the estimation of A and B, I have "data - background",
  // thus I have the statistical uncertainty of data, namely sqrt(data) = sqrt (signal + background)
  // and the systematic uncertainty on B, background_uncertainty * Bkg
  //

  float C_extimated      = SigC/effC/lumi;
  float sigmaC_extimated = std::hypot(sqrt(SigC+BkgC) , background_uncertainty * BkgC)/effC/lumi;

  float A_extimated      = SigA/effA/lumi;
  float sigmaA_extimated = std::hypot(sqrt(SigA+BkgA) , background_uncertainty * BkgA)/effA/lumi;
  float stat_sigmaA_extimated = std::hypot(sqrt(SigA+BkgA) , 0. * BkgA)/effA/lumi;

  float B_extimated      = SigB/effB/lumi;
  float sigmaB_extimated = std::hypot(sqrt(SigB+BkgB) , background_uncertainty * BkgB)/effB/lumi;
  float stat_sigmaB_extimated = std::hypot(sqrt(SigB+BkgB) , 0. * BkgB)/effB/lumi;


  std::cout << "A extimated = " << A_extimated << " +/- " << sigmaA_extimated << "    (" << sigmaA_extimated/A_extimated*100 << " %)" << " stat unc = " << stat_sigmaA_extimated/A_extimated*100  << " % " << std::endl;
  std::cout << "B extimated = " << B_extimated << " +/- " << sigmaB_extimated << "    (" << sigmaB_extimated/B_extimated*100 << " %)" << " stat unc = " << stat_sigmaB_extimated/B_extimated*100  << " % " << std::endl;
  std::cout << "C extimated = " << C_extimated << " +/- " << sigmaC_extimated << "    (" << sigmaC_extimated/C_extimated*100 << " %)" << std::endl;


}


