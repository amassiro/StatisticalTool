#include <TH1D.h>
#include <TF1.h>
#include <TRandom3.h>
#include <TCanvas.h>

void DrawBias() {

  int min = 100;
  int max = 180;
  int Nbin = 1000;

  const int Nbkg = 20000; // num of bkg
  const int Nsig = 2000; // num of sig


  TF1 *pdf_bkg = new TF1("pdf_bkg", "exp(-x/80.) + 0.3 * exp(-3*x/80.)", min, max);
  TF1 *pdf_sig = new TF1("pdf_sig", "exp(-(x-125)*(x-125)/(2*2))", min, max);

  TH1F *h_data = new TH1F("h_data", "Events generated from generic function; x; counts", Nbin, min, max);

  for (int i = 0; i < Nbkg; i++) {
    double x = pdf_bkg->GetRandom();
    h_data->Fill(x);
  }

  for (int i = 0; i < Nsig; i++) {
    double x = pdf_sig->GetRandom();
    h_data->Fill(x);
  }


  TCanvas *c_bkg = new TCanvas("c_bkg", "Generic Simulation + Fit", 800, 600);
  h_data->DrawClone();


  TF1 *fit_function_bkg = new TF1("fit_function_bkg", "[0]*exp(-x/[1])", min, max);
  h_data->Fit(fit_function_bkg, "R");
  fit_function_bkg->Draw("same");


  TCanvas *c_final = new TCanvas("c_final", "Mass spectrum", 800, 600);
  h_data->Draw();


  TF1 *fit_function = new TF1("fit_function", "[0]*exp(-x/[1]) + [2]*1/(sqrt(2*3.14*[4]*[4]))*exp(-(x-[3])*(x-[3])/(2*[4]*[4]))", min, max);

  fit_function->SetParName(3, "Mass");
  fit_function->SetParName(4, "Width");

  fit_function->SetParameter(0, fit_function_bkg->GetParameter(0));
  fit_function->SetParameter(1, fit_function_bkg->GetParameter(1));

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

  TF1 *fit_function_sig = new TF1("fit_function_sig", "[0]*1/(sqrt(2*3.14*[2]*[2]))*exp(-(x-[1])*(x-[1])/(2*[2]*[2]))", min, max);

  fit_function_sig->SetParameter(0, fit_function->GetParameter(2));
  fit_function_sig->SetParameter(1, fit_function->GetParameter(3));
  fit_function_sig->SetParameter(2, fit_function->GetParameter(4));

  fit_function_sig->SetLineColor(kGreen+2);
  fit_function_sig->Draw("same");


  std::cout << " ~~~~~~~~~~~~~ " << std::endl;
  std::cout << "Mass = " << fit_function->GetParameter(3) << " +/- " << fit_function->GetParError(3) << std::endl;
  std::cout << "bin width = " << 1.*(max-min)/Nbin << std::endl;





}


