
#include "TRandom3.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TLegend.h"


void simulate () {

  gStyle->SetOptStat(0);

  int nbin = 200;

  TH1F* histo = new TH1F ("histo", "", nbin, 0, nbin);

  TRandom3 *rng = new TRandom3(42);

  for (int i=0; i<nbin; i++){
    double val = 50*rng->Exp(10);
    if (i<10)  histo->SetBinContent(i+1,val*100.);
    else       histo->SetBinContent(i+1,val/10.);
  }



  TCanvas *cc = new TCanvas("cc", "", 800, 600);
  cc->cd();
  histo->Draw();


  //---- build chi2 distirbution
  TH1F* histo_chi2 = new TH1F ("histo_chi2", "", 5*nbin, 0, 5*nbin);

  int ntoys = 10000;

  for (int itoy = 0; itoy<ntoys; itoy++){
    double chi2 = 0;
    for (int i=0; i<nbin; i++){
      double value = histo->GetBinContent(i+1);
      double value_toy = rng->Poisson(value);
      chi2 += (value-value_toy)*(value-value_toy)/value;
    }
    histo_chi2->Fill(chi2);
  }

  TCanvas *cc_chi2 = new TCanvas("cc_chi2", "", 800, 600);
  cc_chi2->cd();
  histo_chi2->Draw();
  histo_chi2->GetXaxis()->SetTitle("#chi^{2}");

}


