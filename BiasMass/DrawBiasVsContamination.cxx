#include <TH1D.h>
#include <TF1.h>
#include <TRandom3.h>
#include <TCanvas.h>

void DrawBiasVsContamination() {

  int min = 100;
  int max = 180;
  int Nbin = 1600*5/4;

  const int Nbkg = 40000; // num of bkg
  const int Nsig = 2000; // num of sig


  std::vector<float> x_f;
  std::vector<float> y_f;
  std::vector<float> ex_f;
  std::vector<float> ey_f;

  int maxContamination = 10;
  // int maxContamination = 1;

  TF1 *pdf_bkg_1 = new TF1("pdf_bkg_1", "exp(-x/80.)", min, max);
  TF1 *pdf_bkg_2 = new TF1("pdf_bkg_2", "5 * 0.5 * exp(-5*x/195.)", min, max);
  TF1 *pdf_bkg_3 = new TF1("pdf_bkg_3", "exp(-x/80.) + 5 * 0.5 * exp(-5*x/195.)", min, max);

  TCanvas *c_backgrounds = new TCanvas("c_backgrounds", "Functions", 800, 600);
  c_backgrounds->Divide(2,2);
  c_backgrounds->cd(1);
  pdf_bkg_1->Draw("PL");  pdf_bkg_1->SetLineColor(kRed);
  c_backgrounds->cd(2);
  pdf_bkg_2->Draw("PL");  pdf_bkg_2->SetLineColor(kBlue);
  c_backgrounds->cd(3);
  pdf_bkg_3->Draw("PL");  pdf_bkg_3->SetLineColor(kGreen+2);
  c_backgrounds->cd(4);
  pdf_bkg_1->Draw("PL");       pdf_bkg_1->SetLineColor(kRed);
  pdf_bkg_2->Draw("PL same");  pdf_bkg_2->SetLineColor(kBlue);
  pdf_bkg_3->Draw("PL same");  pdf_bkg_3->SetLineColor(kGreen+2);


  TCanvas *c_final = new TCanvas("c_final", "Mass spectrum", 800, 600);
  c_final->Divide(sqrt(maxContamination)+1, sqrt(maxContamination)+1);

  TH1F *h_data = new TH1F("h_data", "Events generated from generic function; x; counts", Nbin, min, max);
  TF1 *pdf_sig = new TF1("pdf_sig", "exp(-(x-125)*(x-125)/(2*2))", min, max);

  for (int iContamination = 0; iContamination<maxContamination; iContamination++){

    // TString bkgnamefunction = Form("exp(-x/80.) + 0.5 * %.2f * 2", 1. * iContamination / maxContamination);
    // TString bkgnamefunction = Form("exp(-x/80.) + 0.5 * %.2f * 0.1*exp(-3*x/80.)*(-100+(x-140)*(x-140))", 1. * iContamination / maxContamination);
    // TString bkgnamefunction = Form("exp(-x/80.) + 0.5 * %.2f * exp(-x/120.)", 1. * iContamination / maxContamination);
    // TString bkgnamefunction = Form("exp(-x/80.) + 0.5 * %.2f * exp(-3*x/80.)", 1. * iContamination / maxContamination);
    TString bkgnamefunction = Form("exp(-x/80.) + 5 * %.2f * exp(-5*x/195.)", 1. * iContamination / maxContamination);

    std::cout << " bkgnamefunction = " << bkgnamefunction.Data() << std::endl;

    TF1 *pdf_bkg = new TF1("pdf_bkg", bkgnamefunction.Data(), min, max);

    h_data->Reset();

    // TH1F *h_data = new TH1F("h_data", "Events generated from generic function; x; counts", Nbin, min, max);

    for (int i = 0; i < Nbkg; i++) {
      double x = pdf_bkg->GetRandom();
      h_data->Fill(x);
    }

    for (int i = 0; i < Nsig; i++) {
      double x = pdf_sig->GetRandom();
      h_data->Fill(x);
    }


    // TCanvas *c_bkg = new TCanvas("c_bkg", "Generic Simulation + Fit", 800, 600);
    // h_data->DrawClone();

    c_final->cd(iContamination+1);

    // TF1 *fit_function_bkg = new TF1("fit_function_bkg", "[0]*exp(-x/[1])", min, max);
    // h_data->Fit(fit_function_bkg, "R");
    // fit_function_bkg->Draw("same");


    h_data->Draw();


    TF1 *fit_function = new TF1("fit_function", "[0]*exp(-x/[1]) + [2]*1/(sqrt(2*3.14*[4]*[4]))*exp(-(x-[3])*(x-[3])/(2*[4]*[4]))", min, max);

    fit_function->SetParName(3, "Mass");
    fit_function->SetParName(4, "Width");

    // fit_function->SetParameter(0, fit_function_bkg->GetParameter(0));
    // fit_function->SetParameter(1, fit_function_bkg->GetParameter(1));

    fit_function->SetParameter(3, 125);
    fit_function->SetParameter(4, 2);
    fit_function->SetParameter(2, Nsig);

    // fit_function->SetParLimits(0, 10, Nbkg*1.5);
    // fit_function->SetParLimits(2, 10, Nsig*1.5);
    fit_function->SetParLimits(1, 40., 100.);
    fit_function->SetParLimits(3, 120, 130);
    fit_function->SetParLimits(4, 1.5, 2.5);


    fit_function->SetLineColor(kRed);
    h_data->Fit(fit_function, "RMQ");
    h_data->Fit(fit_function, "RMQ");
    h_data->Fit(fit_function, "RMQ");

    TF1 *fit_function_sig = new TF1("fit_function_sig", "[0]*1/(sqrt(2*3.14*[2]*[2]))*exp(-(x-[1])*(x-[1])/(2*[2]*[2]))", min, max);

    fit_function_sig->SetParameter(0, fit_function->GetParameter(2));
    fit_function_sig->SetParameter(1, fit_function->GetParameter(3));
    fit_function_sig->SetParameter(2, fit_function->GetParameter(4));

    fit_function_sig->SetLineColor(kGreen+2);
    fit_function_sig->Draw("same");


    std::cout << " ~~~~~~~~~~~~~ " << std::endl;
    std::cout << "Mass = " << fit_function->GetParameter(3) << " +/- " << fit_function->GetParError(3) << std::endl;
    std::cout << "bin width = " << 1.*(max-min)/Nbin << std::endl;


    x_f.push_back(1.*iContamination/maxContamination);
    y_f.push_back(fit_function->GetParameter(3) - 125.0);

    ex_f.push_back(0);
    ey_f.push_back(fit_function->GetParError(3));

  }

  TGraphErrors *grBias = new TGraphErrors(x_f.size(), x_f.data(), y_f.data(), ex_f.data(), ey_f.data());
  grBias->SetTitle("bias vs contamination");

  TCanvas *cResult = new TCanvas("cResult", "graph", 800, 600);
  grBias->SetMarkerStyle(20);
  grBias->SetMarkerColor(kRed);
  grBias->SetLineColor(kRed);
  grBias->Draw("AP");
  grBias->GetXaxis()->SetTitle("contamination %");
  grBias->GetYaxis()->SetTitle("Fitted mass - 125 [GeV]");



}


