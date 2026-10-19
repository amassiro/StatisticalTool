//
// measurement of the 68.27% and 95% CL interval edges with toys (Neyman), for one EFT Wilson coefficient C,
// compared with the Asimov -2DlnL = 1 and 3.84 crossings (Wilks)
//
//   root -l coverageMeasurement.cxx
//   root -l 'coverageMeasurement.cxx("input.root", "histo_SM", "histo_cP1", "histo_cM1", 0., 2000, -2, 2)'
//
// Model, bin by bin:
//
//    N(C) = SM + C * L + C^2 * Q
//
// 1) Asimov MC at C_true: data = N_i(C_true), no fluctuations
//    likelihood scan -2DlnL(C), C in [c_min, c_max], crossings at -2DlnL = 1 and 3.84
// 2) for a given C:
//      - t_Asimov(C) = -2DlnL_Asimov(C)
//      - toys at C, for each toy t = -2DlnL(C) = -2 ln [ L(C) / L(C_hat) ]
//      - q(C) = quantile of the toy distribution of t, with 31.73% (5%) of the toys above
//    C is on the edge of the 68.27% (95%) CL interval when t_Asimov(C) = q(C)
//      t_Asimov(C) < q(C) --> C inside the interval, t_Asimov(C) > q(C) --> C outside
// 3) for both edges, left (sx) and right (dx): start from the Asimov crossing (Wilks: q = 1 or 3.84), move C till
//    t_Asimov(C) - q(C) changes sign, then bisection
//    same random seed for every C --> q(C) smooth in C
//
// If Wilks holds the measured C is the Asimov crossing.
//


void coverageMeasurement(TString name_file = "input.root",
                         TString name_SM   = "histo_SM",
                         TString name_cP1  = "histo_cP1",
                         TString name_cM1  = "histo_cM1",
                         float c_true  = 0.,
                         int n_toys    = 2000,
                         float c_min   = -2.,
                         float c_max   =  2.
                        ) {

  gStyle->SetOptStat(0);
  gStyle->SetTitleFontSize(0.05);

  int npoints = 1000;   // points in [c_min, c_max], for the Asimov scan and for the minimum
  int n_bisection = 10; // iterations of the bisection

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
  //    -2lnL(C) = -2 ln [ L(C) / L_sat ] = 2 * sum_i [ N_i(C) - n_i + n_i * ln( n_i / N_i(C) ) ]
  //
  //    L_sat depends only on data, it cancels in -2DlnL = -2lnL(C) - -2lnL(C_hat)
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


  //---- 1) Asimov MC at C_true

  std::vector<double> v_asimov (n_bins, 0.);
  for (int iBin = 0; iBin < n_bins; iBin++) {
    v_asimov[iBin] = expected (iBin, c_true);
  }
  v_data = v_asimov;

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


  //---- 2) t_Asimov(C) and toys at C

  auto tAsimov = [&] (double c) {
    v_data = v_asimov;
    return minus2LogL (c) - min_m2lnL_asimov;
  };

  int n_at_boundary = 0;

  auto runToys = [&] (double c) {
    TRandom3 random (12345);   // same seed for every C
    std::vector<double> v_t;
    n_at_boundary = 0;
    for (int iToy = 0; iToy < n_toys; iToy++) {
      for (int iBin = 0; iBin < n_bins; iBin++) {
        v_data[iBin] = random.Poisson (expected (iBin, c));
      }
      double c_hat = findMinimum ();
      if (c_hat - c_min < step || c_max - c_hat < step) n_at_boundary++;
      //---- t = -2 ln [ L(C) / L(C_hat) ]
      double t = minus2LogL (c) - minus2LogL (c_hat);
      if (t < 0) t = 0;   // numerical precision of the minimum
      v_t.push_back (t);
    }
    std::sort (v_t.begin(), v_t.end());
    return v_t;
  };

  auto quantileOf = [&] (std::vector<double>& v_t, double p) {
    int k = std::max (0, std::min (n_toys - 1, (int) round (p * n_toys)));
    return v_t[k];
  };


  //---- 3) search of the edges, left (sx) and right (dx)

  std::vector<TString> sides = {"sx", "dx"};

  //---- [iCL][iSide]
  std::vector< std::vector<double> > v_c_cross        (cls.size(), std::vector<double> (sides.size(), 0.));
  std::vector< std::vector<double> > v_c_measured     (cls.size(), std::vector<double> (sides.size(), 0.));
  std::vector< std::vector<double> > v_c_measured_err (cls.size(), std::vector<double> (sides.size(), 0.));
  std::vector< std::vector<bool> >   v_found          (cls.size(), std::vector<bool>   (sides.size(), false));
  std::vector< std::vector< std::vector<double> > > v_t_measured (cls.size(), std::vector< std::vector<double> > (sides.size()));

  for (int iCL = 0; iCL < cls.size(); iCL++) {
    for (int iSide = 0; iSide < sides.size(); iSide++) {

      if (v_crossings[iCL].size() == 0) continue;

      double c_cross = (iSide == 0) ? *std::min_element (v_crossings[iCL].begin(), v_crossings[iCL].end())
                                    : *std::max_element (v_crossings[iCL].begin(), v_crossings[iCL].end());
      v_c_cross[iCL][iSide] = c_cross;
      double dir_out = (iSide == 0) ? -1. : 1.;   // away from the best fit

      std::cout << std::endl << " ---- CL = " << cls[iCL] << " , " << sides[iSide] << " , start from Asimov crossing -2DlnL = " << thresholds[iCL] << ": C = " << c_cross << std::endl;

      //---- f(C) = t_Asimov(C) - q(C) : < 0 inside, > 0 outside
      std::vector<double> v_t_a = runToys (c_cross);
      double c_a = c_cross;
      double f_a = tAsimov (c_a) - quantileOf (v_t_a, cls[iCL]);
      std::cout << "   C = " << c_a << " : t_Asimov = " << tAsimov (c_a) << " , quantile = " << quantileOf (v_t_a, cls[iCL]) << std::endl;

      double dir = (f_a < 0) ? dir_out : -dir_out;   // inside --> move out, outside --> move in
      double step_c = 0.05 * fabs (c_cross - c_hat_asimov);

      double c_b = c_a;
      double f_b = f_a;
      bool bracket = false;
      for (int iStep = 0; iStep < 50; iStep++) {
        c_b = c_a + dir * step_c;
        if (c_b < c_min || c_b > c_max) break;
        std::vector<double> v_t_b = runToys (c_b);
        f_b = tAsimov (c_b) - quantileOf (v_t_b, cls[iCL]);
        std::cout << "   C = " << c_b << " : t_Asimov = " << tAsimov (c_b) << " , quantile = " << quantileOf (v_t_b, cls[iCL]) << std::endl;
        if (f_a * f_b <= 0) {
          bracket = true;
          break;
        }
        c_a = c_b;
        f_a = f_b;
      }

      if (!bracket) {
        std::cout << "   no sign change in [" << c_min << ", " << c_max << "] --> enlarge the range" << std::endl;
        continue;
      }

      //---- bisection
      for (int iBis = 0; iBis < n_bisection; iBis++) {
        double c_m = 0.5 * (c_a + c_b);
        std::vector<double> v_t_m = runToys (c_m);
        double f_m = tAsimov (c_m) - quantileOf (v_t_m, cls[iCL]);
        std::cout << "   bisection C = " << c_m << " : t_Asimov = " << tAsimov (c_m) << " , quantile = " << quantileOf (v_t_m, cls[iCL]) << std::endl;
        if (f_a * f_m <= 0) {
          c_b = c_m;
          f_b = f_m;
        }
        else {
          c_a = c_m;
          f_a = f_m;
        }
      }

      //---- linear interpolation in the last interval
      double c_measured = (f_b != f_a) ? c_a - f_a * (c_b - c_a) / (f_b - f_a) : 0.5 * (c_a + c_b);

      //---- final toys at the measured C
      v_t_measured[iCL][iSide] = runToys (c_measured);
      if (n_at_boundary > 0) {
        std::cout << "   WARNING: " << n_at_boundary << " / " << n_toys << " toys with minimum at the boundary of [" << c_min << ", " << c_max << "] --> enlarge the range" << std::endl;
      }

      //---- stat. uncertainty from the toys: quantile at k +/- sqrt(n p (1-p)), converted to C with the slope of t_Asimov(C)
      //     (approximate: the dependence of the quantile on C is neglected)
      double sigma_k = sqrt (n_toys * cls[iCL] * (1. - cls[iCL]));
      double q_lo = quantileOf (v_t_measured[iCL][iSide], cls[iCL] - sigma_k / n_toys);
      double q_hi = quantileOf (v_t_measured[iCL][iSide], cls[iCL] + sigma_k / n_toys);
      double h = 1.e-3 * fabs (c_cross - c_hat_asimov);
      double slope = (tAsimov (c_measured + h) - tAsimov (c_measured - h)) / (2. * h);
      double c_err = 0.5 * (q_hi - q_lo) / fabs (slope);

      v_c_measured[iCL][iSide] = c_measured;
      v_c_measured_err[iCL][iSide] = c_err;
      v_found[iCL][iSide] = true;
    }
  }


  //---- summary

  std::cout << std::endl << " ---- summary, C_true = " << c_true << " , " << n_toys << " toys per point ----" << std::endl;
  for (int iCL = 0; iCL < cls.size(); iCL++) {
    for (int iSide = 0; iSide < sides.size(); iSide++) {
      if (!v_found[iCL][iSide]) continue;
      std::cout << " CL = " << cls[iCL] << " " << sides[iSide]
                << " : Asimov (Wilks, -2DlnL = " << thresholds[iCL] << ") C = " << v_c_cross[iCL][iSide]
                << " ; toys C = " << v_c_measured[iCL][iSide] << " +/- " << v_c_measured_err[iCL][iSide] << " (stat. toys)"
                << " , t_Asimov = " << tAsimov (v_c_measured[iCL][iSide])
                << " , quantile = " << quantileOf (v_t_measured[iCL][iSide], cls[iCL]) << std::endl;
    }
  }
  std::cout << std::endl;


  //---- plot: t distribution at the measured C

  //---- overflow added to the last bin, so all the toys are drawn
  auto addOverflow = [] (TH1* h) {
    int nb = h->GetNbinsX();
    h->SetBinContent (nb, h->GetBinContent(nb) + h->GetBinContent(nb+1));
    h->SetBinContent (nb+1, 0.);
  };

  TF1* f_chi2 = new TF1 ("f_chi2", "ROOT::Math::chisquared_pdf(x, 1)", 0.01, 10);
  f_chi2->SetLineColor(kRed);
  f_chi2->SetLineStyle(2);

  TCanvas* cc_t = new TCanvas ("cc_t", "t", 1200, 900);
  cc_t->Divide(2, 2);

  for (int iCL = 0; iCL < cls.size(); iCL++) {
    for (int iSide = 0; iSide < sides.size(); iSide++) {
      if (!v_found[iCL][iSide]) continue;

      int iPad = 2 * iCL + iSide + 1;
      cc_t->cd(iPad);
      gPad->SetLogy();

      std::vector<double>& v_t = v_t_measured[iCL][iSide];

      TH1F* h_t = new TH1F (Form("h_t_%d", iPad), "", 100, 0, 10);
      for (auto t : v_t) h_t->Fill (t);
      addOverflow (h_t);
      h_t->Scale (1. / n_toys / h_t->GetBinWidth(1));
      h_t->SetTitle(Form("toys at C = %.3f (%.2f%% CL, %s): upper %.2f%% above t_{Asimov} = %.3f", v_c_measured[iCL][iSide], 100. * cls[iCL], sides[iSide].Data(), 100. * (1. - cls[iCL]), tAsimov (v_c_measured[iCL][iSide])));
      h_t->SetLineColor(kBlue);
      h_t->SetLineWidth(2);
      h_t->GetXaxis()->SetTitle("t = -2 #Delta lnL (C)");
      h_t->GetYaxis()->SetTitle("pdf");
      h_t->Draw("histo");

      //---- upper (1-CL) of the toys shaded
      int i_upper = std::min (n_toys - 1, (int) round (cls[iCL] * n_toys));
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

      double t_asimov = tAsimov (v_c_measured[iCL][iSide]);
      TLine* line_t_asimov = new TLine (t_asimov, h_t->GetMinimum(1.e-6), t_asimov, h_t->GetMaximum());
      line_t_asimov->SetLineWidth(3);
      line_t_asimov->SetLineColor(kGreen+2);
      line_t_asimov->Draw();

      TLegend* leg = new TLegend (0.45, 0.65, 0.88, 0.85);
      leg->AddEntry(h_t,           "toys", "l");
      leg->AddEntry(h_t_upper,     Form("upper %.2f%% of toys", 100. * (1. - cls[iCL])), "f");
      leg->AddEntry(f_chi2,        "#chi^{2} 1 dof", "l");
      leg->AddEntry(line_t_asimov, "t_{Asimov}", "l");
      leg->Draw();
    }
  }

  cc_t->SaveAs("coverageMeasurement_t.png");


  //---- plot: Asimov scan, Asimov crossings (dashed) and measured C (solid)

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

  TLegend* leg_asimov = new TLegend (0.35, 0.50, 0.88, 0.88);
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
      leg_asimov->AddEntry(line_crossing, Form("Asimov -2#DeltalnL = %.2f: C = %.3f , %.3f", thresholds[iCL], v_c_cross[iCL][0], v_c_cross[iCL][1]), "l");
    }
    TLine* line_measured = 0;
    for (int iSide = 0; iSide < sides.size(); iSide++) {
      if (!v_found[iCL][iSide]) continue;
      line_measured = new TLine (v_c_measured[iCL][iSide], 0., v_c_measured[iCL][iSide], 2. * thresholds[1]);
      line_measured->SetLineStyle(1);
      line_measured->SetLineWidth(3);
      line_measured->SetLineColor(colors_crossing[iCL]);
      line_measured->Draw();
    }
    if (line_measured) {
      leg_asimov->AddEntry(line_measured, Form("toys %.2f%% CL: C = %.3f #pm %.3f , %.3f #pm %.3f", 100. * cls[iCL],
                                               v_c_measured[iCL][0], v_c_measured_err[iCL][0], v_c_measured[iCL][1], v_c_measured_err[iCL][1]), "l");
    }
  }
  leg_asimov->Draw();

  cc_asimov->SaveAs("coverageMeasurement_asimov.png");

  std::cout << " I'm done " << std::endl;

}
