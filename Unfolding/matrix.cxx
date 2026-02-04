#include "TRandom3.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TLegend.h"

void matrix() {

  gStyle->SetOptStat(0);

  int nEvents = 10000;
  double tau_1 = 200;
  double tau_2 = 400;



  TRandom3 *rng = new TRandom3(42);

  TH1F *h1_gen = new TH1F("h1_gen", "Exponential Distribution", 10, 0, 1000);
  TH1F *h2_gen = new TH1F("h2_gen", "Exponential Distribution", 10, 0, 1000);

  TH1F *h1_reco = new TH1F("h1_reco", "Exponential Distribution", 10, 0, 1000);
  TH1F *h2_reco = new TH1F("h2_reco", "Exponential Distribution", 10, 0, 1000);

  TH2F *h1_response = new TH2F("h1_response", "response 1", 10, 0, 1000,  10, 0, 1000);
  TH2F *h2_response = new TH2F("h2_response", "response 2", 10, 0, 1000,  10, 0, 1000);


  for (int i = 0; i < nEvents; i++) {
    double val = rng->Exp(tau_1);
    h1_gen->Fill(val);

    double rndgaus = rng->Gaus(1, 0.05);
    h1_reco->Fill(val*rndgaus);

    h1_response->Fill (val*rndgaus, val);


    val = rng->Exp(tau_2);
    h2_gen->Fill(val);

    rndgaus = rng->Gaus(1, 0.05);
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


  TCanvas *c1reco = new TCanvas("c1reco", "reco mjj", 800, 600);

  h1_reco->SetMarkerStyle(20);
  h1_reco->SetMarkerSize(0.6);
  h1_reco->SetLineColor(kBlack);

  h2_reco->SetMarkerStyle(21);
  h2_reco->SetMarkerSize(0.6);
  h2_reco->SetLineColor(kRed);


  h1_reco->Draw();
  h2_reco->Draw("same");


  TCanvas *c1response = new TCanvas("c1response", "response mjj", 800, 600);

  c1response->Divide(2,1);

  c1response->cd(1);
  h1_response->Draw("colz text");
  c1response->cd(2);
  h2_response->Draw("colz text");

}











