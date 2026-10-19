//
// coverage of the -2DlnL < 1 interval for one EFT Wilson coefficient C
//
//   root -l coverage.cxx
//   root -l 'coverage.cxx("input.root", "histo_SM", "histo_cP1", "histo_cM1", -0.5, 2000, -2, 2, "sx")'
//
// Model, bin by bin:
//
//    N(C) = SM + C * L + C^2 * Q
//
// 1) Asimov MC at C_true: data = N_i(C_true), no fluctuations
//    likelihood scan -2DlnL(C), C in [c_min, c_max]
// 2) C_toys = crossing -2DlnL = 1 of the Asimov scan, on the left ("sx") or on the right ("dx")
// 3) toys at C_toys: Poisson in each bin with mean N_i(C_toys). For each toy:
//      - find C_hat, the C for which -2lnL is minimum (-2lnL normalised to the saturated model, see below)
//      - t = -2DlnL(C_toys) = -2 ln [ L(C_toys) / L(C_hat) ]
//                           = -2 ln [ L(C_toys) / L_sat ] - ( -2 ln [ L(C_hat) / L_sat ] )
//        L_sat = Poisson evaluated at mu = n (saturated model), depends only on data --> cancels in t
//
// On the Asimov t_obs = -2DlnL_Asimov(C_toys) = 1 by construction.
// If Wilks holds, t is chi2 with 1 dof:
//    fraction of toys with t < 1 = 68.27% , p-value = fraction of toys with t >= 1 = 31.73% , 68.27% quantile = 1
//


void coverage(TString name_file = "input.root",
              TString name_SM   = "histo_SM",
              TString name_cP1  = "histo_cP1",
              TString name_cM1  = "histo_cM1",
              float c_true  = 0.,
              int n_toys    = 2000,
              float c_min   = -2.,
              float c_max   =  2.,
              TString side  = "sx"
             ) {

  gStyle->SetOptStat(0);

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


  //---- plot: input histograms, SM, C=+1, C=-1

  TCanvas* cc_inputs = new TCanvas ("cc_inputs", "inputs", 800, 600);
  cc_inputs->SetLogy();

  TH1* h_SM_draw  = (TH1*) h_SM ->Clone ("h_SM_draw");
  TH1* h_cP1_draw = (TH1*) h_cP1->Clone ("h_cP1_draw");
  TH1* h_cM1_draw = (TH1*) h_cM1->Clone ("h_cM1_draw");
  h_SM_draw ->SetLineColor(kBlack);
  h_cP1_draw->SetLineColor(kRed);
  h_cM1_draw->SetLineColor(kBlue);
  h_SM_draw ->SetLineWidth(3);
  h_cP1_draw->SetLineWidth(2);
  h_cM1_draw->SetLineWidth(2);

  double max_inputs = std::max (h_SM_draw->GetMaximum(), std::max (h_cP1_draw->GetMaximum(), h_cM1_draw->GetMaximum()));
  double min_inputs = std::min (h_SM_draw->GetMinimum(), std::min (h_cP1_draw->GetMinimum(), h_cM1_draw->GetMinimum()));
  h_SM_draw->SetMaximum (2. * max_inputs);
  h_SM_draw->SetMinimum (0.5 * min_inputs);
  h_SM_draw->SetTitle ("input");
  h_SM_draw->GetYaxis()->SetTitle("events");
  h_SM_draw ->Draw("histo");
  h_cP1_draw->Draw("histo same");
  h_cM1_draw->Draw("histo same");

  TLegend* leg_inputs = new TLegend (0.70, 0.70, 0.88, 0.88);
  leg_inputs->AddEntry(h_SM_draw,  "SM",   "l");
  leg_inputs->AddEntry(h_cP1_draw, "C=+1", "l");
  leg_inputs->AddEntry(h_cM1_draw, "C=-1", "l");
  leg_inputs->Draw();

  cc_inputs->SaveAs("coverage_inputs.png");




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

  //---- crossings (Wilks): 68% CL -> 1.00, 95% CL -> 3.84
  std::vector< std::vector<double> > v_crossings (cls.size());   // crossings at -2DlnL = 1 and 3.84
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


  //---- plot: N(C) = SM + C * L + C^2 * Q, C in the 95% CL range of the Asimov scan (-2DlnL < 3.84, ~2 sigma)
  //     C < 0 blue, C > 0 red, darker for larger |C|

  double c_draw_min = c_min;
  double c_draw_max = c_max;
  if (v_crossings[1].size() > 0) {
    c_draw_min = *std::min_element (v_crossings[1].begin(), v_crossings[1].end());
    c_draw_max = *std::max_element (v_crossings[1].begin(), v_crossings[1].end());
  }
  int n_c_draw = 16;
  double c_draw_abs_max = std::max (fabs (c_draw_min), fabs (c_draw_max));

  std::vector<double> v_c_draw;
  for (int iC = 0; iC < n_c_draw; iC++) {
    double c = c_draw_min + (c_draw_max - c_draw_min) * iC / (n_c_draw - 1);
    if (fabs (c) < 1.e-6) continue;   // = SM
    v_c_draw.push_back (c);
  }

  TCanvas* cc_inputs_c = new TCanvas ("cc_inputs_c", "inputs vs C", 800, 800);

  //---- top pad: N(C), bottom pad: N(C) / SM
  TPad* pad_top    = new TPad ("pad_top",    "", 0., 0.30, 1., 1.);
  TPad* pad_bottom = new TPad ("pad_bottom", "", 0., 0.00, 1., 0.30);
  pad_top->SetBottomMargin(0.02);
  pad_bottom->SetTopMargin(0.02);
  pad_bottom->SetBottomMargin(0.30);
  pad_top->Draw();
  pad_bottom->Draw();

  pad_top->cd();
  pad_top->SetLogy();

  std::vector<TH1*> v_h_c_draw;
  double max_inputs_c = h_SM->GetMaximum();
  double min_inputs_c = h_SM->GetMinimum();
  for (int iC = 0; iC < v_c_draw.size(); iC++) {
    TH1* h_c = (TH1*) h_SM->Clone (Form("h_c_draw_%d", iC));
    for (int iBin = 0; iBin < n_bins; iBin++) {
      h_c->SetBinContent (iBin+1, expected (iBin, v_c_draw[iC]));
    }
    double x = fabs (v_c_draw[iC]) / c_draw_abs_max;   // 0 ... 1
    int color = (v_c_draw[iC] < 0) ? TColor::GetColor ((float) (0.60 * (1. - x)), (float) (0.80 - 0.70 * x), (float) (1.00 - 0.40 * x))
                                   : TColor::GetColor ((float) (1.00 - 0.40 * x), (float) (0.75 * (1. - x)), (float) (0.65 * (1. - x)));
    h_c->SetLineColor(color);
    h_c->SetLineWidth(2);
    max_inputs_c = std::max (max_inputs_c, h_c->GetMaximum());
    min_inputs_c = std::min (min_inputs_c, h_c->GetMinimum());
    v_h_c_draw.push_back (h_c);
  }

  TH1* h_SM_draw_c = (TH1*) h_SM->Clone ("h_SM_draw_c");
  h_SM_draw_c->SetLineColor(kBlack);
  h_SM_draw_c->SetLineWidth(3);
  h_SM_draw_c->SetMaximum (3. * max_inputs_c);
  h_SM_draw_c->SetMinimum (0.5 * min_inputs_c);
  h_SM_draw_c->SetTitle (Form("N(C) = SM + C L + C^{2} Q , C in [%.2f, %.2f] (Asimov -2#DeltalnL < 3.84)", c_draw_min, c_draw_max));
  h_SM_draw_c->GetYaxis()->SetTitle("events");
  h_SM_draw_c->GetXaxis()->SetLabelSize(0);
  h_SM_draw_c->Draw("histo");
  for (auto h_c : v_h_c_draw) h_c->Draw("histo same");
  h_SM_draw_c->Draw("histo same");

  TLegend* leg_inputs_c = new TLegend (0.55, 0.55, 0.88, 0.88);
  leg_inputs_c->SetNColumns(2);
  leg_inputs_c->AddEntry(h_SM_draw_c, "SM (C=0)", "l");
  for (int iC = 0; iC < v_c_draw.size(); iC++) {
    leg_inputs_c->AddEntry(v_h_c_draw[iC], Form("C = %+.3f", v_c_draw[iC]), "l");
  }
  leg_inputs_c->Draw();

  //---- ratio N(C) / SM
  pad_bottom->cd();
  pad_bottom->SetGridy();

  std::vector<TH1*> v_h_ratio;
  double max_ratio = 1.;
  double min_ratio = 1.;
  for (int iC = 0; iC < v_h_c_draw.size(); iC++) {
    TH1* h_ratio = (TH1*) v_h_c_draw[iC]->Clone (Form("h_ratio_%d", iC));
    h_ratio->Divide (h_SM);
    max_ratio = std::max (max_ratio, h_ratio->GetMaximum());
    min_ratio = std::min (min_ratio, h_ratio->GetMinimum());
    v_h_ratio.push_back (h_ratio);
  }

  TH1* h_ratio_SM = (TH1*) h_SM->Clone ("h_ratio_SM");
  h_ratio_SM->Divide (h_SM);
  h_ratio_SM->SetLineColor(kBlack);
  h_ratio_SM->SetLineWidth(3);
  double margin_ratio = 0.1 * (max_ratio - min_ratio);
  h_ratio_SM->SetMaximum (max_ratio + margin_ratio);
  h_ratio_SM->SetMinimum (min_ratio - margin_ratio);
  h_ratio_SM->SetTitle ("");
  h_ratio_SM->GetYaxis()->SetTitle("N(C) / SM");
  h_ratio_SM->GetYaxis()->SetNdivisions(505);
  h_ratio_SM->GetYaxis()->SetTitleSize(0.10);
  h_ratio_SM->GetYaxis()->SetTitleOffset(0.45);
  h_ratio_SM->GetYaxis()->SetLabelSize(0.09);
  h_ratio_SM->GetXaxis()->SetLabelSize(0.10);
  h_ratio_SM->GetXaxis()->SetTitleSize(0.10);
  h_ratio_SM->Draw("histo");
  for (auto h_ratio : v_h_ratio) h_ratio->Draw("histo same");
  h_ratio_SM->Draw("histo same");

  cc_inputs_c->SaveAs("coverage_inputs_c.png");


  //---- 2) C_toys = crossing at -2DlnL = 1, left (sx) or right (dx)

  if (v_crossings[0].size() == 0) {
    std::cout << " no crossing at -2DlnL = 1 --> enlarge the range" << std::endl;
    return;
  }
  if (side != "sx" && side != "dx") {
    std::cout << " side must be sx or dx, not " << side << std::endl;
    return;
  }

  double c_toys = (side == "sx") ? *std::min_element (v_crossings[0].begin(), v_crossings[0].end())
                                 : *std::max_element (v_crossings[0].begin(), v_crossings[0].end());

  double t_asimov = minus2LogL (c_toys) - min_m2lnL_asimov;   // = 1 by construction

  std::cout << " C_toys = " << c_toys << " (" << side << " crossing at -2DlnL = 1) , t_Asimov = -2DlnL_Asimov(C_toys) = " << t_asimov << std::endl;


  //---- 3) toys at C_toys

  TRandom3 random (12345);

  TH1F* h_t     = new TH1F ("h_t",     "", 100, 0, 10);
  TH1F* h_c_hat = new TH1F ("h_c_hat", "", 100, c_min, c_max);

  std::vector<double> v_t;
  int n_at_boundary = 0;

  int n_toys_scan = 10;   // likelihood scan drawn for the first n_toys_scan toys
  std::vector<TGraph*> v_gr_toy_scan;
  std::vector<double> v_t_toy_scan;   // t = -2DlnL(C_toys) of these toys, for the color

  for (int iToy = 0; iToy < n_toys; iToy++) {

    //---- data
    for (int iBin = 0; iBin < n_bins; iBin++) {
      v_data[iBin] = random.Poisson (expected (iBin, c_toys));
    }

    //---- minimum
    double c_hat = findMinimum ();
    if (c_hat - c_min < step || c_max - c_hat < step) n_at_boundary++;

    //---- t = -2 ln [ L(C_toys) / L(C_hat) ]
    double t = minus2LogL (c_toys) - minus2LogL (c_hat);
    if (t < 0) t = 0;   // numerical precision of the minimum

    h_t->Fill (t);
    h_c_hat->Fill (c_hat);
    v_t.push_back (t);

    //---- likelihood scan for the first toys: -2DlnL(C) = -2lnL(C) - -2lnL(C_hat)
    if (iToy < n_toys_scan) {
      double min_m2lnL = minus2LogL (c_hat);
      TGraph* gr_toy_scan = new TGraph ();
      for (int iPoint = 0; iPoint < npoints; iPoint++) {
        double c = c_min + step * iPoint;
        gr_toy_scan->SetPoint (iPoint, c, minus2LogL (c) - min_m2lnL);
      }
      v_gr_toy_scan.push_back (gr_toy_scan);
      v_t_toy_scan.push_back (t);
    }

  }

  if (n_at_boundary > 0) {
    std::cout << " WARNING: " << n_at_boundary << " / " << n_toys << " toys with minimum at the boundary of [" << c_min << ", " << c_max << "] --> enlarge the range" << std::endl;
  }


  //---- t from toys vs t_Asimov = 1

  std::sort (v_t.begin(), v_t.end());

  int n_inside = 0;
  for (auto t : v_t) if (t < t_asimov) n_inside++;
  double cov     = 1. * n_inside / n_toys;
  double cov_err = sqrt (cov * (1. - cov) / n_toys);
  double quantile = v_t[ std::min (n_toys - 1, (int) round (cls[0] * n_toys)) ];

  std::cout << " toys at C_toys = " << c_toys << " , " << n_toys << " toys" << std::endl;
  std::cout << " fraction of toys with t < " << t_asimov << " = " << cov << " +/- " << cov_err << "  (Wilks: " << cls[0] << ")" << std::endl;
  std::cout << " p-value = fraction of toys with t >= " << t_asimov << " = " << 1. - cov << " +/- " << cov_err << "  (Wilks: " << 1. - cls[0] << ")" << std::endl;
  std::cout << " " << cls[0] << " quantile of toys = " << quantile << "  (Wilks: " << thresholds[0] << ")" << std::endl;


  //---- plot: distribution of t, with chi2 1 dof

  TCanvas* cc_t = new TCanvas ("cc_t", "t", 800, 600);
  cc_t->SetLogy();

  //---- overflow added to the last bin, so all the toys are drawn
  auto addOverflow = [] (TH1* h) {
    int nb = h->GetNbinsX();
    h->SetBinContent (nb, h->GetBinContent(nb) + h->GetBinContent(nb+1));
    h->SetBinContent (nb+1, 0.);
  };

  addOverflow (h_t);
  h_t->Scale (1. / n_toys / h_t->GetBinWidth(1));
  h_t->SetLineColor(kBlue);
  h_t->SetLineWidth(2);
  h_t->GetXaxis()->SetTitle("t = -2 #Delta lnL (C_{toys})");
  h_t->GetYaxis()->SetTitle("pdf");
  h_t->Draw("histo");

  //---- upper 32% of the toys shaded: filled only with the toys with t >= 68% quantile,
  //     same binning and normalisation as h_t --> integral (with overflow) = 32% exactly
  double fraction_upper = 0.32;
  int i_upper = std::min (n_toys - 1, (int) round ((1. - fraction_upper) * n_toys));
  double t_upper = v_t[i_upper];
  TH1F* h_t_upper = (TH1F*) h_t->Clone ("h_t_upper");
  h_t_upper->Reset();
  for (int iToy = i_upper; iToy < n_toys; iToy++) {
    h_t_upper->Fill (v_t[iToy]);
  }
  addOverflow (h_t_upper);
  h_t_upper->Scale (1. / n_toys / h_t_upper->GetBinWidth(1));
  std::cout << " upper " << 100. * fraction_upper << "%: t >= " << t_upper << " , fraction of toys shaded = "
            << h_t_upper->Integral() * h_t_upper->GetBinWidth(1)
            << " (overflow t > " << h_t_upper->GetXaxis()->GetXmax() << " in the last bin)" << std::endl;
  h_t_upper->SetFillColor(kBlue);
  h_t_upper->SetFillStyle(3004);
  h_t_upper->SetLineWidth(0);
  h_t_upper->Draw("histo same");

  TF1* f_chi2 = new TF1 ("f_chi2", "ROOT::Math::chisquared_pdf(x, 1)", 0.01, 10);
  f_chi2->SetLineColor(kRed);
  f_chi2->SetLineStyle(2);
  f_chi2->Draw("same");

  TLine* line_t_asimov = new TLine (t_asimov, h_t->GetMinimum(1.e-6), t_asimov, h_t->GetMaximum());
  line_t_asimov->SetLineWidth(3);
  line_t_asimov->SetLineColor(kGreen+2);
  line_t_asimov->Draw();

  TLegend* leg = new TLegend (0.50, 0.70, 0.88, 0.88);
  leg->AddEntry(h_t,           Form("toys C_{toys} = %.3f (C_{true} = %.2f, %s)", c_toys, c_true, side.Data()), "l");
  leg->AddEntry(f_chi2,        "#chi^{2} 1 dof", "l");
  leg->AddEntry(line_t_asimov, "t_{Asimov} = -2#DeltalnL_{Asimov}(C_{toys})", "l");
  leg->Draw();

  cc_t->SaveAs("coverage.png");


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

  //---- C_toys: where the toys are generated
  TLine* line_c_toys = new TLine (c_toys, 0., c_toys, 2. * thresholds[1]);
  line_c_toys->SetLineStyle(1);
  line_c_toys->SetLineWidth(3);
  line_c_toys->SetLineColor(kGreen+2);
  line_c_toys->Draw();

  //---- -2DlnL = 1 and 3.84: vertical lines at the crossings, min and max C in the legend
  TLegend* leg_asimov = new TLegend (0.45, 0.55, 0.88, 0.88);
  leg_asimov->AddEntry(gr_asimov,   Form("Asimov MC scan, C_{true} = %.2f", c_true), "l");
  leg_asimov->AddEntry(line_c_toys, Form("C_{toys} = %.3f", c_toys), "l");

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

  cc_asimov->SaveAs("coverage_asimov.png");


  //---- plot: likelihood scan for the first toys

  //---- color of each scan from t = -2DlnL(C_toys), palette in [0, max t of these toys]
  TCanvas* cc_toy_scan = new TCanvas ("cc_toy_scan", "toy scans", 800, 600);
  cc_toy_scan->SetRightMargin(0.15);
  //---- palette: light blue / blue for small t, then yellow and red
  const int n_stops = 4;
  double stops[n_stops] = {0.00, 0.30, 0.65, 1.00};
  double red  [n_stops] = {0.55, 0.10, 1.00, 0.85};
  double green[n_stops] = {0.85, 0.35, 0.90, 0.00};
  double blue [n_stops] = {1.00, 0.95, 0.00, 0.00};
  TColor::CreateGradientColorTable (n_stops, stops, red, green, blue, 255);

  double t_scan_max = *std::max_element (v_t_toy_scan.begin(), v_t_toy_scan.end());
  if (t_scan_max <= 0) t_scan_max = 1.;

  //---- empty frame, only for the axes and the color scale (bins below the minimum are not drawn)
  TH2F* h_frame_toy_scan = new TH2F ("h_frame_toy_scan", Form("first %d toys at C_{toys} = %.3f", (int) v_gr_toy_scan.size(), c_toys), 10, c_min, c_max, 10, 0., 2. * thresholds[1]);
  h_frame_toy_scan->SetBinContent (1, 1, -1.);
  h_frame_toy_scan->SetMinimum (0.);
  h_frame_toy_scan->SetMaximum (t_scan_max);
  h_frame_toy_scan->GetXaxis()->SetTitle("C");
  h_frame_toy_scan->GetYaxis()->SetTitle("-2 #Delta lnL");
  h_frame_toy_scan->GetZaxis()->SetTitle("t = -2 #Delta lnL (C_{toys})");
  h_frame_toy_scan->Draw("colz");

  int n_colors = gStyle->GetNumberOfColors();
  for (int iToy = 0; iToy < v_gr_toy_scan.size(); iToy++) {
    int i_color = (int) (v_t_toy_scan[iToy] / t_scan_max * (n_colors - 1));
    v_gr_toy_scan[iToy]->SetLineColor(gStyle->GetColorPalette(i_color));
    v_gr_toy_scan[iToy]->SetLineWidth(2);
    v_gr_toy_scan[iToy]->Draw("L");
  }
  TLine* line_c_toys_scan = new TLine (c_toys, 0., c_toys, 2. * thresholds[1]);
  line_c_toys_scan->SetLineWidth(3);
  line_c_toys_scan->SetLineColor(kGreen+2);
  line_c_toys_scan->Draw();

  cc_toy_scan->SaveAs("coverage_toy_scan.png");


  //---- plot: best fit C

  TCanvas* cc_c_hat = new TCanvas ("cc_c_hat", "c hat", 800, 600);
  h_c_hat->SetLineColor(kBlack);
  h_c_hat->SetLineWidth(2);
  h_c_hat->GetXaxis()->SetTitle("#hat{C}");
  h_c_hat->GetYaxis()->SetTitle("toys");
  h_c_hat->Draw("histo");

  std::cout << " I'm done " << std::endl;

}
