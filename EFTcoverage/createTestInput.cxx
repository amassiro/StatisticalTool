//
// create a test input for coverage.cxx, coverageComplete.cxx and coverageMeasurement.cxx
//
//   root -l -b -q createTestInput.cxx
//   root -l -b -q 'createTestInput.cxx("input_2minima.root", 1)'
//
// 3 histograms: SM, C=+1, C=-1, from a falling SM spectrum:
//
//    N(C) = SM + C * L + C^2 * Q
//
// type = 0: linear and quadratic EFT terms growing with the bin index
// type = 1: Asimov scan (C_true = 0) with a global minimum at C = 0 and a local minimum at C ~ -1.16
//           N_i(C) = SM_i also at C*_i = -L_i / Q_i : with L = Q, C* = -1 in all bins --> 2 degenerate minima;
//           a small bin dependence of Q/L spreads C*_i in [-1.43, -0.77] --> the second minimum becomes local
//           (Asimov, C_true = 0: local minimum -2DlnL ~ 2.9 at C ~ -1.16, barrier -2DlnL ~ 7.9 at C ~ -0.63)
//


void createTestInput(TString name_file = "input.root", int type = 0) {

  TFile* f_output = new TFile (name_file, "RECREATE");

  int n_bins = 10;
  TH1F* h_SM  = new TH1F ("histo_SM",  "SM",   n_bins, 0, 10);
  TH1F* h_cP1 = new TH1F ("histo_cP1", "C=+1", n_bins, 0, 10);
  TH1F* h_cM1 = new TH1F ("histo_cM1", "C=-1", n_bins, 0, 10);

  for (int iBin = 1; iBin <= n_bins; iBin++) {
    double sm   = 1000. * exp (-0.4 * iBin);
    double lin  = 0.02  * iBin * sm;
    double quad = 0.001 * iBin * iBin * sm;

    if (type == 1) {
      lin  = 0.2 * sm;
      quad = 0.2 * sm * (1. + 0.3 * (iBin - (n_bins + 1) / 2.) / ((n_bins - 1) / 2.));
    }

    h_SM ->SetBinContent(iBin, sm);
    h_cP1->SetBinContent(iBin, sm + lin + quad);
    h_cM1->SetBinContent(iBin, sm - lin + quad);
  }

  std::cout << " type  = " << type << std::endl;
  std::cout << " SM    = " << h_SM->Integral()  << std::endl;
  std::cout << " C=+1  = " << h_cP1->Integral() << std::endl;
  std::cout << " C=-1  = " << h_cM1->Integral() << std::endl;

  f_output->Write();
  f_output->Close();

  std::cout << " written: " << name_file << std::endl;

}
