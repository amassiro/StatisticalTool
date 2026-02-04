#include "TRandom3.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TLegend.h"

void matrix() {

  gStyle->SetOptStat(0);

  int nEvents = 1000000;
  double tau_1 = 100;
  double tau_2 = 400;

  int nbin = 5;

  TRandom3 *rng = new TRandom3(42);

  TH1F *h1_gen = new TH1F("h1_gen", "gen 1", nbin, 0, 1000);
  TH1F *h2_gen = new TH1F("h2_gen", "gen 2", nbin, 0, 1000);

  TH1F *h1_reco = new TH1F("h1_reco", "reco 1", nbin, 0, 1000);
  TH1F *h2_reco = new TH1F("h2_reco", "reco 2", nbin, 0, 1000);

  TH2F *h1_response = new TH2F("h1_response", "response 1", nbin, 0, 1000,  nbin, 0, 1000);
  TH2F *h2_response = new TH2F("h2_response", "response 2", nbin, 0, 1000,  nbin, 0, 1000);


  for (int i = 0; i < nEvents; i++) {
    double val = rng->Exp(tau_1);
    h1_gen->Fill(val);

    double rndgaus = rng->Gaus(1, 0.20);
    h1_reco->Fill(val*rndgaus);

    h1_response->Fill (val*rndgaus, val);


    val = rng->Exp(tau_2);
    h2_gen->Fill(val);

    rndgaus = rng->Gaus(1, 0.20);
    h2_reco->Fill(val*rndgaus);

    h2_response->Fill (val*rndgaus, val);

  }

  h1_gen->GetXaxis()->SetTitle("gen mjj [GeV]");
  h2_gen->GetXaxis()->SetTitle("gen mjj [GeV]");

  h1_reco->GetXaxis()->SetTitle("reco mjj [GeV]");
  h2_reco->GetXaxis()->SetTitle("reco mjj [GeV]");

  h1_response->GetXaxis()->SetTitle("reco mjj [GeV]");
  h2_response->GetXaxis()->SetTitle("reco mjj [GeV]");
  h1_response->GetYaxis()->SetTitle("gen mjj [GeV]");
  h2_response->GetYaxis()->SetTitle("gen mjj [GeV]");

  TCanvas *c1gen = new TCanvas("c1gen", "gen mjj", 800, 600);

  h1_gen->SetMarkerStyle(20);
  h1_gen->SetMarkerSize(0.6);
  h1_gen->SetLineColor(kBlack);

  h2_gen->SetMarkerStyle(21);
  h2_gen->SetMarkerSize(0.6);
  h2_gen->SetLineColor(kRed);


  h1_gen->Draw();
  h2_gen->Draw("same");

  c1gen->BuildLegend(0.6, 0.7, 0.85, 0.85);


  TCanvas *c1reco = new TCanvas("c1reco", "reco mjj", 800, 600);

  h1_reco->SetMarkerStyle(20);
  h1_reco->SetMarkerSize(0.6);
  h1_reco->SetLineColor(kBlack);

  h2_reco->SetMarkerStyle(21);
  h2_reco->SetMarkerSize(0.6);
  h2_reco->SetLineColor(kRed);


  h1_reco->Draw();
  h2_reco->Draw("same");

  c1reco->BuildLegend(0.6, 0.7, 0.85, 0.85);


  TCanvas *c1response = new TCanvas("c1response", "response mjj", 800, 300);

  c1response->Divide(2,1);

  c1response->cd(1);
  h1_response->Draw("colz text");
  c1response->cd(2);
  h2_response->Draw("colz text");



  TCanvas *c1response_col_normalized = new TCanvas("c1response_col_normalized", "response normalized mjj", 800, 300);

  TH2F *h1_response_normalized = new TH2F("h1_response_normalized", "response normalized 1", nbin, 0, 1000,  nbin, 0, 1000);
  TH2F *h2_response_normalized = new TH2F("h2_response_normalized", "response normalized 2", nbin, 0, 1000,  nbin, 0, 1000);

  for (int ix=0; ix<nbin; ix++){
    double norm_1 = 0;
    double norm_2 = 0;
    for (int iy=0; iy<nbin; iy++){
      norm_1 += h1_response->GetBinContent(ix+1, iy+1);
      norm_2 += h2_response->GetBinContent(ix+1, iy+1);
    }
    for (int iy=0; iy<nbin; iy++){
      h1_response_normalized->SetBinContent(ix+1, iy+1, h1_response->GetBinContent(ix+1, iy+1) / norm_1);
      h2_response_normalized->SetBinContent(ix+1, iy+1, h2_response->GetBinContent(ix+1, iy+1) / norm_2);
    }
  }

  h1_response_normalized->GetXaxis()->SetTitle("reco mjj [GeV]");
  h2_response_normalized->GetXaxis()->SetTitle("reco mjj [GeV]");
  h1_response_normalized->GetYaxis()->SetTitle("gen mjj [GeV]");
  h2_response_normalized->GetYaxis()->SetTitle("gen mjj [GeV]");

  gStyle->SetPaintTextFormat(".2f");

  c1response_col_normalized->Divide(2,1);

  c1response_col_normalized->cd(1);
  h1_response_normalized->Draw("colz text");
  c1response_col_normalized->cd(2);
  h2_response_normalized->Draw("colz text");


}











