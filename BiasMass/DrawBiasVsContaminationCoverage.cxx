#include <TH1D.h>
#include <TF1.h>
#include <TRandom3.h>
#include <TCanvas.h>

void DrawBiasVsContaminationCoverage() {

  int nToy = 20;

  int min = 100;
  int max = 180;
  int Nbin = 160000;

  const int Nbkg = 3000; // num of bkg
  const int Nsig = 200; // num of sig

  std::vector<float> x_f;
  std::vector<float> y_f;
  std::vector<float> ex_f;
  std::vector<float> ey_f;

  int maxContamination = 2;

  TH1F *h_data = new TH1F("h_data", "Events generated from generic function; x; counts", Nbin, min, max);

  TH1F *h_bias = new TH1F("h_bias", "", 100, -0.50, 0.50);

  TF1 *fit_function = new TF1("fit_function", "[0]*exp(-x/[1]) + [2]*1/(sqrt(2*3.14*[4]*[4]))*exp(-(x-[3])*(x-[3])/(2*[4]*[4]))", min, max);
  TF1 *fit_function_sig = new TF1("fit_function_sig", "[0]*1/(sqrt(2*3.14*[2]*[2]))*exp(-(x-[1])*(x-[1])/(2*[2]*[2]))", min, max);
  TF1 *pdf_sig = new TF1("pdf_sig", "exp(-(x-125)*(x-125)/(2*2))", min, max);

  TCanvas *c_final = new TCanvas("c_final", "Bias plots", 800, 600);
  c_final->Divide(sqrt(maxContamination)+1, sqrt(maxContamination)+1);


  for (int iContamination = 0; iContamination<maxContamination; iContamination++){

    c_final->cd(iContamination+1);

    h_bias -> Reset("ICESM");

    TString bkgnamefunction = Form("exp(-x/80.) + 0.5 * %.2f * 2", 1. * iContamination / maxContamination);
    // TString bkgnamefunction = Form("exp(-x/80.) + 0.5 * %.2f * exp(-3*x/80.)", 1. * iContamination / maxContamination);
    // TString bkgnamefunction = Form("exp(-x/80.) + 0.5 * %.2f * exp(-10*x/80.)", 1. * iContamination / maxContamination);

    std::cout << " bkgnamefunction = " << bkgnamefunction.Data() << std::endl;

    TF1 *pdf_bkg = new TF1("pdf_bkg", bkgnamefunction.Data(), min, max);

    fit_function->SetParName(3, "Mass");
    fit_function->SetParName(4, "Width");

    for (int iToy=0; iToy<nToy; iToy++){

      h_data -> Reset("ICESM");


      for (int i = 0; i < Nbkg; i++) {
        double x = pdf_bkg->GetRandom();
        h_data->Fill(x);
      }

      for (int i = 0; i < Nsig; i++) {
        double x = pdf_sig->GetRandom();
        h_data->Fill(x);
      }


      fit_function->SetParameter(3, 125);
      fit_function->SetParameter(4, 2);
      fit_function->SetParameter(2, Nsig);

      fit_function->SetParLimits(1, 40., 100.);
      fit_function->SetParLimits(3, 120, 130);
      fit_function->SetParLimits(4, 1.5, 2.5);

      fit_function->SetLineColor(kRed);
      h_data->Fit(fit_function, "RMQ");
      h_data->Fit(fit_function, "RMQ");
      h_data->Fit(fit_function, "RMQ");

      fit_function_sig->SetParameter(0, fit_function->GetParameter(2));
      fit_function_sig->SetParameter(1, fit_function->GetParameter(3));
      fit_function_sig->SetParameter(2, fit_function->GetParameter(4));

      fit_function_sig->SetLineColor(kGreen+2);
      fit_function_sig->Draw("same");

      h_bias->Fill(fit_function->GetParameter(3) - 125.0);

    }

    c_final->cd(iContamination+1);
    h_bias->DrawClone();


    x_f.push_back(1.*iContamination/maxContamination);
    y_f.push_back(h_bias->GetMean());

    ex_f.push_back(0);
    ey_f.push_back(h_bias->GetRMS());

  }

  TGraphErrors *grBias = new TGraphErrors(x_f.size(), x_f.data(), y_f.data(), ex_f.data(), ey_f.data());
  grBias->SetTitle("bias vs contamination");

  TCanvas *cResult = new TCanvas("cResult", "graph", 800, 600);
  cResult->SetGrid();
  grBias->SetMarkerStyle(20);
  grBias->SetMarkerColor(kRed);
  grBias->SetLineColor(kRed);
  grBias->Draw("AP");
  grBias->GetXaxis()->SetTitle("contamination %");
  grBias->GetYaxis()->SetTitle("Fitted mass - 125 [GeV]");


  std::cout << "bin width = " << 1.*(max-min)/Nbin << std::endl;


}





