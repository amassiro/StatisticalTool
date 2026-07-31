// smiley_contour.C
//
// Produces a CMS/ATLAS-style filled 1-sigma / 2-sigma likelihood contour
// plot -- like https://cds.cern.ch/record/2967655/files/Figure_003-a.pdf --
// but where the "signal region" traces a smiley face.
//
// Method:
//   1. Sample points along parametric curves for the face outline, two
//      eyes, and the mouth.
//   2. Build a scalar field L(x,y) = sum of Gaussian bumps centered on
//      those sample points. Near the smiley curves, L is large.
//   3. Define chi2(x,y) = -2 * ln( L(x,y) / Lmax ), exactly analogous to
//      -2*Delta(lnL) in a real profile-likelihood scan. This is 0 right
//      on the smiley curves and grows away from them.
//   4. Draw filled contours at the standard 2-parameter confidence levels
//      Delta_chi2 = 2.30 (68% CL, "1 sigma") and 6.18 (95% CL, "2 sigma")
//      using ROOT's "CONT Z LIST" trick to extract contour polygons as
//      TGraphs, then fill them (2-sigma graphs first, 1-sigma on top).
//
// Run with:   root -l smiley_contour.C
//
#include "TCanvas.h"
#include "TH2D.h"
#include "TStyle.h"
#include "TROOT.h"
#include "TList.h"
#include "TObjArray.h"
#include "TGraph.h"
#include "TLegend.h"
#include "TMarker.h"
#include "TLatex.h"
#include "TMath.h"
#include <vector>
#include <cmath>

void draw()
{
    gROOT->SetBatch(kFALSE);
    gStyle->SetOptStat(0);
    gStyle->SetOptTitle(0);
    gStyle->SetPadTickX(1);
    gStyle->SetPadTickY(1);

    // ---------------------------------------------------------------
    // 1. Grid setup
    // ---------------------------------------------------------------
    const int    nBinsX = 400, nBinsY = 400;
    const double xMin = -3.4, xMax = 3.4;
    const double yMin = -3.4, yMax = 5.4;

    TH2D *h2 = new TH2D("h2", ";Parameter X;Parameter Y", nBinsX, xMin, xMax, nBinsY, yMin, yMax);

    // ---------------------------------------------------------------
    // 2. Sample points defining the smiley face curves
    // ---------------------------------------------------------------
    std::vector<std::pair<double,double>> pts;

    // Face outline (big circle)
    const int    nOutline = 360;
    const double Rface    = 2.6;
    for (int i = 0; i < nOutline; ++i) {
        double th = 2 * TMath::Pi() * i / nOutline;
        pts.push_back({ Rface * cos(th), Rface * sin(th) });
    }

    // Left eye (small filled disk)
    const int    nEye = 80;
    const double Reye = 0.28;
    const double lx = -0.95, ly = 0.75;
    for (int i = 0; i < nEye; ++i) {
        double th = 2 * TMath::Pi() * i / nEye;
        pts.push_back({ lx + Reye * cos(th), ly + Reye * sin(th) });
        // extra interior points so the eye fills in solidly, not just a ring
        pts.push_back({ lx + 0.5 * Reye * cos(th), ly + 0.5 * Reye * sin(th) });
    }
    pts.push_back({ lx, ly });

    // Right eye
    const double rx = 0.95, ry = 0.75;
    for (int i = 0; i < nEye; ++i) {
        double th = 2 * TMath::Pi() * i / nEye;
        pts.push_back({ rx + Reye * cos(th), ry + Reye * sin(th) });
        pts.push_back({ rx + 0.5 * Reye * cos(th), ry + 0.5 * Reye * sin(th) });
    }
    pts.push_back({ rx, ry });

    // Mouth: lower arc of a circle, curving upward at both ends -> a smile
    const int    nMouth = 200;
    const double Rmouth = 1.7;
    const double cx = 0.0, cy = 0.55;
    const double thStart = 200.0 * TMath::DegToRad();
    const double thEnd   = 340.0 * TMath::DegToRad();
    for (int i = 0; i < nMouth; ++i) {
        double th = thStart + (thEnd - thStart) * i / (nMouth - 1);
        pts.push_back({ cx + Rmouth * cos(th), cy + Rmouth * sin(th) });
    }

    const double sigmaCurve = 0.13; // controls curve/eye thickness

    // ---------------------------------------------------------------
    // 3. Build the "likelihood" field and convert to chi2
    // ---------------------------------------------------------------
    std::vector<std::vector<double>> Lgrid(nBinsX, std::vector<double>(nBinsY, 0.0));
    double Lmax = -1.0;

    for (int ix = 0; ix < nBinsX; ++ix) {
        double x = h2->GetXaxis()->GetBinCenter(ix + 1);
        for (int iy = 0; iy < nBinsY; ++iy) {
            double y = h2->GetYaxis()->GetBinCenter(iy + 1);
            double L = 0.0;
            for (const auto &p : pts) {
                double dx = x - p.first;
                double dy = y - p.second;
                double d2 = dx * dx + dy * dy;
                L += std::exp(-d2 / (2 * sigmaCurve * sigmaCurve));
            }
            Lgrid[ix][iy] = L;
            if (L > Lmax) Lmax = L;
        }
    }

    for (int ix = 0; ix < nBinsX; ++ix) {
        for (int iy = 0; iy < nBinsY; ++iy) {
            double ratio = Lgrid[ix][iy] / Lmax;
            if (ratio < 1e-12) ratio = 1e-12;
            double chi2 = -2.0 * std::log(ratio);
            h2->SetBinContent(ix + 1, iy + 1, chi2);
        }
    }

    // ---------------------------------------------------------------
    // 4. Standard 2-parameter confidence levels
    // ---------------------------------------------------------------
    const double level1 = 2.30; // 68% CL ("1 sigma", 2 dof)
    const double level2 = 6.18; // 95% CL ("2 sigma", 2 dof)

    h2->SetContour(2);
    h2->SetContourLevel(0, level1);
    h2->SetContourLevel(1, level2);

    // ---------------------------------------------------------------
    // 5. Canvas + extract contour polygons via "CONT Z LIST"
    // ---------------------------------------------------------------
    TCanvas *c1 = new TCanvas("c1", "Smiley Contour", 800, 800);
    c1->SetLeftMargin(0.12);
    c1->SetRightMargin(0.05);
    c1->SetTopMargin(0.08);
    c1->SetBottomMargin(0.12);

    h2->Draw("CONT Z LIST");
    c1->Update();

    TObjArray *contours =
        (TObjArray*)gROOT->GetListOfSpecials()->FindObject("contours");

    // Colors, in a blue palette similar to CMS/ATLAS likelihood scans
    const int color1sigma = TColor::GetColor("#2E5C8A"); // darker blue
    const int color2sigma = TColor::GetColor("#A9C6E8"); // lighter blue
    // const int color1sigma = TColor::GetColor(kYellow-10);
    // const int color2sigma = TColor::GetColor(kYellow-5);

    // Draw an empty frame first (axes only), matching h2's range
    TH2D *hFrame = new TH2D("hFrame", ";c_{W} [TeV^{-2}];c_{HW} [TeV^{-2}]",
                             10, xMin, xMax, 10, yMin, yMax);
    hFrame->SetMinimum(0);
    hFrame->SetMaximum(1);
    hFrame->Draw("AXIS");

    // Draw 2-sigma polygons first (light fill), then 1-sigma on top (dark fill)
    if (contours && contours->GetSize() == 2) {
        TList *list2sigma = (TList*)contours->At(1);
        TList *list1sigma = (TList*)contours->At(0);

        for (int i = 0; i < list2sigma->GetSize(); ++i) {
            TGraph *g = (TGraph*)list2sigma->At(i);
            g->SetFillColor(color2sigma);
            g->SetLineColor(kBlack);
            g->SetLineWidth(2);
            g->SetLineStyle(9);
            g->Draw("F SAME");
            g->Draw("L SAME");
        }
        for (int i = 0; i < list1sigma->GetSize(); ++i) {
            TGraph *g = (TGraph*)list1sigma->At(i);
            g->SetFillColor(color1sigma);
            g->SetLineColor(kBlack);
            g->SetLineWidth(2);
            // g->SetLineStyle(9);
            g->Draw("F SAME");
            g->Draw("L SAME");
        }
    } else {
        printf("Warning: contour extraction did not return 2 levels.\n");
    }

    // ---------------------------------------------------------------
    // 6. Best-fit marker + legend + labels
    // ---------------------------------------------------------------
    TMarker *bf = new TMarker(0.0, 0.6, 29); // sits roughly at "face center"
    bf->SetMarkerColor(kRed);
    bf->SetMarkerSize(2.2);
    bf->Draw();

    TGraph *legProxy1 = new TGraph(); legProxy1->SetFillColor(color1sigma); legProxy1->SetLineColor(kBlack);
    TGraph *legProxy2 = new TGraph(); legProxy2->SetFillColor(color2sigma); legProxy2->SetLineColor(kBlack);

    TLegend *leg = new TLegend(0.55, 0.78, 0.93, 0.90);
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->SetTextSize(0.028);
    leg->AddEntry(legProxy1, "68% CL (1#sigma)", "f");
    leg->AddEntry(legProxy2, "95% CL (2#sigma)", "f");
    leg->AddEntry(bf, "Best fit", "p");
    leg->Draw();

    TLatex tl;
    tl.SetNDC();
    tl.SetTextFont(42);
    tl.SetTextSize(0.035);
    tl.DrawLatex(0.12, 0.945, "CMS");
    tl.SetTextAlign(31);
    tl.DrawLatex(0.95, 0.945, "13.6 TeV, 42 fb^{-1}");

    hFrame->Draw("AXIS SAME"); // redraw axes on top of fills

    c1->RedrawAxis();
    c1->SaveAs("smiley_contour.pdf");
    c1->SaveAs("smiley_contour.png");
}

