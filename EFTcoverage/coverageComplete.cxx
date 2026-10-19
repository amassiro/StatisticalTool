//
// coverage of the -2DlnL < 1 and < 3.84 intervals for one EFT Wilson coefficient C,
// tested at the 4 edges of the Asimov intervals
//
//   root -l coverageComplete.cxx
//   root -l 'coverageComplete.cxx("input.root", "histo_SM", "histo_cP1", "histo_cM1", -0.5, 2000, -2, 2)'
//
// Model, bin by bin:
//
//    N(C) = SM + C * L + C^2 * Q
//
// 1) Asimov MC at C_true: data = N_i(C_true), no fluctuations
//    likelihood scan -2DlnL(C), C in [c_min, c_max]
// 2) 4 tests, C_test = crossing of the Asimov scan:
//      -2DlnL = 1    left (sx) and right (dx)   --> 68.27% CL
//      -2DlnL = 3.84 left (sx) and right (dx)   --> 95% CL
// 3) for each test, toys at C_test: Poisson in each bin with mean N_i(C_test). For each toy:
//      - find C_hat, the C for which -2lnL is minimum (-2lnL normalised to the saturated model, see below)
//      - t = -2DlnL(C_test) = -2 ln [ L(C_test) / L(C_hat) ]
//                           = -2 ln [ L(C_test) / L_sat ] - ( -2 ln [ L(C_hat) / L_sat ] )
//        L_sat = Poisson evaluated at mu = n (saturated model), depends only on data --> cancels in t
//
// On the Asimov t_Asimov = -2DlnL_Asimov(C_test) = 1 (3.84) by construction.
// If Wilks holds, t is chi2 with 1 dof:
//    fraction of toys with t < 1 (3.84) = 68.27% (95%) , quantile 68.27% (95%) of t = 1 (3.84)
//


void coverageComplete(TString name_file = "input.root",
                      TString name_SM   = "histo_SM",
                      TString name_cP1  = "histo_cP1",
                      TString name_cM1  = "histo_cM1",
                      float c_true  = 0.,
                      int n_toys    = 2000,
                      float c_min   = -2.,
                      float c_max   =  2.
                     ) {

  gStyle->SetOptStat(0);
  gStyle->SetTitleFontSize(0.06);

  int npoints = 1000;   // points in [c_min, c_max], for the Asimov scan and for the minimum

  //---- input

  TFile* f_input = new TFile (name_file, "READ");
  if (!f_input || f_input->IsZombie()) {
    std::cout << " file not found: " << name_file << std::endl;
    return;
  }

  TH1* h_SM  = (TH1*) f_input->Get(name_SM);
  TH1* h_cP1 = (TH1*) f_input->Get(name_cP1);
  TH1* h_cM1 = (TH1*) f_input->Get(name_cM1);

  if (!h_SM || !h_cP1 || !h_cM1) {
    std::cout << " histogram not found: " << name_SM << " " << name_cP1 << " " << name_cM1 << std::endl;
    return;
  }

  int n_bins = h_SM->GetNbinsX();


  //---- linear and quadratic components: L = (N(+1) - N(-1)) / 2 , Q = (N(+1) + N(-1)) / 2 - SM

  std::vector<double> v_SM, v_lin, v_quad;
  for (int iBin = 1; iBin <= n_bins; iBin++) {
    double sm = h_SM ->GetBinContent(iBin);
    double p1 = h_cP1->GetBinContent(iBin);
    double m1 = h_cM1->GetBinContent(iBin);
    v_SM.push_back(sm);
    v_lin.push_back( (p1 - m1) / 2. );
    v_quad.push_back( (p1 + m1) / 2. - sm );
  }

  auto expected = [&] (int iBin, double c) {
    return v_SM[iBin] + c * v_lin[iBin] + c * c * v_quad[iBin];
  };


  //---- -2lnL, as likelihood ratio to the saturated model (as in FCCee-EFT likelihood.cxx)
  //
  //    Pois(n|mu) / Pois(n|n) = (mu/n)^n * exp(-(mu-n))       --> the n! cancels
  //
  //    -2lnL(C) = -2 ln [ L(C) / L_sat ] = 2 * sum_i [ N_i(C) - n_i + n_i * ln( n_i / N_i(C) ) ]
  //
  //    - -2lnL >= 0, = 0 when mu = n in all bins
  //    - sum of logs, no product of small numbers --> no underflow
  //    - L_sat depends only on data, it cancels in -2DlnL = -2lnL(C) - -2lnL(C_hat)
  //

  std::vector<double> v_data (n_bins, 0.);

  auto minus2LogL = [&] (double c) {
    double value = 0.;
    for (int iBin = 0; iBin < n_bins; iBin++) {
      double n  = v_data[iBin];
      double mu = expected (iBin, c);
      if (mu <= 0) {
        if (n > 0) return 1.e9;
        continue;
      }
      value += 2. * (mu - n);
      if (n > 0) value += 2. * n * log (n / mu);
    }
    return value;
  };

  TF1* f_m2lnL = new TF1 ("f_m2lnL", [&] (double* x, double* p) { return minus2LogL (x[0]); }, c_min, c_max, 0);

  double step = (c_max - c_min) / (npoints - 1);

  //---- minimum in [c_min, c_max]: grid first (-2lnL can have 2 minima with the quadratic term), then refine around the best point
  auto findMinimum = [&] () {
    double c_grid = c_min;
    double min_grid = 1.e20;
    for (int iPoint = 0; iPoint < npoints; iPoint++) {
      double c = c_min + step * iPoint;
      double value = minus2LogL (c);
      if (value < min_grid) {
        min_grid = value;
        c_grid = c;
      }
    }
    return f_m2lnL->GetMinimumX (std::max((double) c_min, c_grid - step), std::min((double) c_max, c_grid + step));
  };

  std::vector<double> thresholds = {1.00, 3.84};
  std::vector<double> cls        = {0.6827, 0.95};
  std::vector<double> fractions_upper = {0.32, 0.05};   // upper part of the t distribution shaded


  //---- 1) Asimov MC at C_true: data = expected yields at C_true

  for (int iBin = 0; iBin < n_bins; iBin++) {
    v_data[iBin] = expected (iBin, c_true);
  }

  double c_hat_asimov = findMinimum ();
  double min_m2lnL_asimov = minus2LogL (c_hat_asimov);

  std::cout << " Asimov C_true = " << c_true << ": best fit C = " << c_hat_asimov << " , min -2lnL = " << min_m2lnL_asimov << std::endl;

  std::vector<double> v_c_scan, v_scan;
  TGraph* gr_asimov = new TGraph ();
  for (int iPoint = 0; iPoint < npoints; iPoint++) {
    double c = c_min + step * iPoint;
    double value = minus2LogL (c) - min_m2lnL_asimov;
    v_c_scan.push_back (c);
    v_scan.push_back (value);
    gr_asimov->SetPoint (iPoint, c, value);
  }

  //---- crossings: 68% CL -> 1.00, 95% CL -> 3.84
  std::vector< std::vector<double> > v_crossings (cls.size());
  for (int iCL = 0; iCL < cls.size(); iCL++) {
    std::cout << " Asimov: CL = " << cls[iCL] << " (-2DlnL = " << thresholds[iCL] << ") crossings: ";
    for (int iPoint = 0; iPoint < npoints-1; iPoint++) {
      double y1 = v_scan[iPoint]   - thresholds[iCL];
      double y2 = v_scan[iPoint+1] - thresholds[iCL];
      if (y1 * y2 < 0) {
        double c_cross = v_c_scan[iPoint] + (v_c_scan[iPoint+1] - v_c_scan[iPoint]) * (-y1) / (y2 - y1);
        std::cout << "  " << c_cross;
        v_crossings[iCL].push_back (c_cross);
      }
    }
    if (v_crossings[iCL].size() == 0) std::cout << "  none in [" << c_min << ", " << c_max << "] --> enlarge the range";
    std::cout << std::endl;
  }


  //---- 2) and 3) the 4 tests: (1, sx) (1, dx) (3.84, sx) (3.84, dx)

  std::vector<TString> sides = {"sx", "dx"};

  TRandom3 random (12345);

  TF1* f_chi2 = new TF1 ("f_chi2", "ROOT::Math::chisquared_pdf(x, 1)", 0.01, 10);
  f_chi2->SetLineColor(kRed);
  f_chi2->SetLineStyle(2);

  //---- overflow added to the last bin, so all the toys are drawn
  auto addOverflow = [] (TH1* h) {
    int nb = h->GetNbinsX();
    h->SetBinContent (nb, h->GetBinContent(nb) + h->GetBinContent(nb+1));
    h->SetBinContent (nb+1, 0.);
  };

  TCanvas* cc_t = new TCanvas ("cc_t", "t", 1200, 900);
  cc_t->Divide(2, 2);

  std::vector<TString> summary;

  for (int iCL = 0; iCL < cls.size(); iCL++) {
    for (int iSide = 0; iSide < sides.size(); iSide++) {

      int iPad = 2 * iCL + iSide + 1;
      TString name_test = Form("-2DlnL = %.2f %s", thresholds[iCL], sides[iSide].Data());

      if (v_crossings[iCL].size() == 0) {
        std::cout << " " << name_test << ": no crossing --> skip" << std::endl;
        continue;
      }

      double c_test = (iSide == 0) ? *std::min_element (v_crossings[iCL].begin(), v_crossings[iCL].end())
                                   : *std::max_element (v_crossings[iCL].begin(), v_crossings[iCL].end());

      //---- t on the Asimov at C_test, = threshold by construction
      for (int iBin = 0; iBin < n_bins; iBin++) {
        v_data[iBin] = expected (iBin, c_true);
      }
      double t_asimov = minus2LogL (c_test) - min_m2lnL_asimov;

      //---- toys at C_test
      TH1F* h_t = new TH1F (Form("h_t_%d", iPad), "", 100, 0, 10);
      std::vector<double> v_t;
      int n_at_boundary = 0;

      for (int iToy = 0; iToy < n_toys; iToy++) {

        for (int iBin = 0; iBin < n_bins; iBin++) {
          v_data[iBin] = random.Poisson (expected (iBin, c_test));
        }

        double c_hat = findMinimum ();
        if (c_hat - c_min < step || c_max - c_hat < step) n_at_boundary++;

        //---- t = -2 ln [ L(C_test) / L(C_hat) ]
        double t = minus2LogL (c_test) - minus2LogL (c_hat);
        if (t < 0) t = 0;   // numerical precision of the minimum

        h_t->Fill (t);
        v_t.push_back (t);
      }

      if (n_at_boundary > 0) {
        std::cout << " " << name_test << " WARNING: " << n_at_boundary << " / " << n_toys << " toys with minimum at the boundary of [" << c_min << ", " << c_max << "] --> enlarge the range" << std::endl;
      }

      std::sort (v_t.begin(), v_t.end());

      int n_inside = 0;
      for (auto t : v_t) if (t < t_asimov) n_inside++;
      double cov      = 1. * n_inside / n_toys;
      double cov_err  = sqrt (cov * (1. - cov) / n_toys);
      double quantile = v_t[ std::min (n_toys - 1, (int) round (cls[iCL] * n_toys)) ];

      summary.push_back (Form(" %s : C_test = %7.4f , t_Asimov = %.4f , fraction of toys with t < t_Asimov = %.4f +/- %.4f (Wilks %.4f) , %.4f quantile of t = %.3f (Wilks %.2f)",
                              name_test.Data(), c_test, t_asimov, cov, cov_err, cls[iCL], cls[iCL], quantile, thresholds[iCL]));

      //---- plot
      cc_t->cd(iPad);
      gPad->SetLogy();

      addOverflow (h_t);
      h_t->Scale (1. / n_toys / h_t->GetBinWidth(1));
      h_t->SetTitle(Form("toys at C = %.3f (-2#DeltalnL_{Asimov} = %.2f, %s) : P(t < %.2f) = %.3f #pm %.3f",
                         c_test, thresholds[iCL], sides[iSide].Data(), thresholds[iCL], cov, cov_err));
      h_t->SetLineColor(kBlue);
      h_t->SetLineWidth(2);
      h_t->GetXaxis()->SetTitle("t = -2 #Delta lnL (C)");
      h_t->GetYaxis()->SetTitle("pdf");
      h_t->Draw("histo");

      //---- upper part of the toys shaded: filled only with the toys with t >= quantile,
      //     same binning and normalisation as h_t --> integral (with overflow) = fraction_upper exactly
      int i_upper = std::min (n_toys - 1, (int) round ((1. - fractions_upper[iCL]) * n_toys));
      TH1F* h_t_upper = (TH1F*) h_t->Clone (Form("h_t_upper_%d", iPad));
      h_t_upper->Reset();
      for (int iToy = i_upper; iToy < n_toys; iToy++) {
        h_t_upper->Fill (v_t[iToy]);
      }
      addOverflow (h_t_upper);
      h_t_upper->Scale (1. / n_toys / h_t_upper->GetBinWidth(1));
      h_t_upper->SetFillColor(kBlue);
      h_t_upper->SetFillStyle(3004);
      h_t_upper->SetLineWidth(0);
      h_t_upper->Draw("histo same");

      f_chi2->Draw("same");

      TLine* line_t_asimov = new TLine (t_asimov, h_t->GetMinimum(1.e-6), t_asimov, h_t->GetMaximum());
      line_t_asimov->SetLineWidth(3);
      line_t_asimov->SetLineColor(kGreen+2);
      line_t_asimov->Draw();

      TLegend* leg = new TLegend (0.45, 0.65, 0.88, 0.85);
      leg->AddEntry(h_t,           "toys", "l");
      leg->AddEntry(h_t_upper,     Form("upper %.0f%% of toys", 100. * fractions_upper[iCL]), "f");
      leg->AddEntry(f_chi2,        "#chi^{2} 1 dof", "l");
      leg->AddEntry(line_t_asimov, "t_{Asimov}", "l");
      leg->Draw();
    }
  }

  cc_t->SaveAs("coverageComplete_t.png");

  std::cout << std::endl << " ---- summary, C_true = " << c_true << " , " << n_toys << " toys per test ----" << std::endl;
  for (auto line : summary) std::cout << line << std::endl;
  std::cout << std::endl;


  //---- plot: Asimov scan

  TCanvas* cc_asimov = new TCanvas ("cc_asimov", "Asimov scan", 800, 600);
  gr_asimov->SetLineColor(kBlue);
  gr_asimov->SetLineWidth(2);
  gr_asimov->SetTitle(Form("Asimov MC, C_{true} = %.2f", c_true));
  gr_asimov->GetXaxis()->SetTitle("C");
  gr_asimov->GetYaxis()->SetTitle("-2 #Delta lnL");
  gr_asimov->GetYaxis()->SetRangeUser(0., 2. * thresholds[1]);
  gr_asimov->Draw("AL");

  for (auto threshold : thresholds) {
    TLine* line = new TLine (c_min, threshold, c_max, threshold);
    line->SetLineStyle(2);
    line->SetLineColor(kRed);
    line->Draw();
  }

  //---- -2DlnL = 1 and 3.84: vertical lines at the crossings (where the toys are generated), min and max C in the legend
  TLegend* leg_asimov = new TLegend (0.45, 0.60, 0.88, 0.88);
  leg_asimov->AddEntry(gr_asimov, Form("Asimov MC scan, C_{true} = %.2f", c_true), "l");

  std::vector<int> colors_crossing = {kMagenta+1, kOrange+7};

  for (int iCL = 0; iCL < cls.size(); iCL++) {
    TLine* line_crossing = 0;
    for (auto c_cross : v_crossings[iCL]) {
      line_crossing = new TLine (c_cross, 0., c_cross, thresholds[iCL]);
      line_crossing->SetLineStyle(2);
      line_crossing->SetLineWidth(2);
      line_crossing->SetLineColor(colors_crossing[iCL]);
      line_crossing->Draw();
    }
    if (v_crossings[iCL].size() > 0) {
      double c_cross_min = *std::min_element (v_crossings[iCL].begin(), v_crossings[iCL].end());
      double c_cross_max = *std::max_element (v_crossings[iCL].begin(), v_crossings[iCL].end());
      leg_asimov->AddEntry(line_crossing, Form("-2#DeltalnL = %.2f: min C = %.3f", thresholds[iCL], c_cross_min), "l");
      leg_asimov->AddEntry((TObject*) 0,  Form("-2#DeltalnL = %.2f: max C = %.3f", thresholds[iCL], c_cross_max), "");
    }
  }
  leg_asimov->Draw();

  cc_asimov->SaveAs("coverageComplete_asimov.png");

  std::cout << " I'm done " << std::endl;

}
