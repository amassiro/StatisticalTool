  
TH1F * DrawOverflow(TH1F *h) {
  // This function paint the histogram h with an extra bin for overflows
  UInt_t nx    = h->GetNbinsX()+1;
  Double_t *xbins= new Double_t[nx+1];
  for (UInt_t i=0;i<nx;i++)
    xbins[i]=h->GetBinLowEdge(i+1);
  xbins[nx]=xbins[nx-1]+h->GetBinWidth(nx);
  char *tempName= new char[strlen(h->GetName())+10];
  sprintf(tempName,"%swtOverFlow",h->GetName());
  // Book a temporary histogram having ab extra bin for overflows
  TH1F *htmp = new TH1F(tempName, h->GetTitle(), nx, xbins);
  //---- style
  htmp->SetLineColor(h->GetLineColor());
  htmp->SetLineWidth(h->GetLineWidth());
  htmp->SetLineStyle(h->GetLineStyle());
  
  // Reset the axis labels
  htmp->SetXTitle(h->GetXaxis()->GetTitle());
  htmp->SetYTitle(h->GetYaxis()->GetTitle());
  // Fill the new hitogram including the extra bin for overflows
  for (UInt_t i=1; i<=nx; i++)
    htmp->Fill(htmp->GetBinCenter(i), h->GetBinContent(i));
  // Fill the underflows
  htmp->Fill(h->GetBinLowEdge(1)-1, h->GetBinContent(0));
  // Restore the number of entries
  htmp->SetEntries(h->GetEntries());
  // FillStyle and color
  htmp->SetFillStyle(h->GetFillStyle());
  htmp->SetFillColor(h->GetFillColor());
  return htmp;
}

  

void draw () {

    gStyle->SetOptStat(0);
    
    TTree* limit_toys = (TTree*) _file0->Get("limit");  
    limit_toys->Draw("limit","","");
    
    TH1F* histo_toys = (TH1F*) gPad->GetPrimitive("htemp");
    
    TH1F* histo_data = new TH1F ("histo_data", "", histo_toys->GetNbinsX() * 10, histo_toys->GetBinLowEdge(1), histo_toys->GetBinLowEdge(histo_toys->GetNbinsX()+1) );    
    TTree* limit_data = (TTree*) _file1->Get("limit");  
    std::cout << " limit_data = " << limit_data << std::endl;
    
    Double_t limit;
    limit_data->SetBranchAddress("limit", &limit);
    limit_data->GetEntry(0);
    histo_data->Fill(limit);
    std::cout << " limit (data) = " << limit << std::endl;
    
    
    histo_toys = DrawOverflow(histo_toys);
    
    
    histo_toys->SetLineWidth(1);
    histo_toys->SetLineColor(kBlue);

    histo_data->SetLineWidth(3);
    histo_data->SetLineColor(kRed);
    
    histo_toys->Scale (1. / histo_toys->Integral());
    

    histo_toys->Draw("hist");
    histo_toys->GetXaxis()->SetTitle("#chi^{2}");
    histo_data->Draw("same");
    
}


