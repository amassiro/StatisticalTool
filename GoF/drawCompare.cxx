  
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

  

void drawCompare () {

    gStyle->SetOptStat(0);
    
    TTree* limit_toys_snapshot_toysNoSystematics = (TTree*) _file0->Get("limit");  
    limit_toys_snapshot_toysNoSystematics->Draw("limit","","");
    
    TH1F* histo_toys_snapshot_toysNoSystematics = (TH1F*) ((TH1F*) gPad->GetPrimitive("htemp")) -> Clone ("histo_toys_snapshot_toysNoSystematics");
    // https://root-forum.cern.ch/t/easy-way-to-get-a-histogram-from-a-ttree/4197
    // he histogram exists only in the current pad and will be destroyed by the next TTree::Draw command
    
    TH1F* histo_data = new TH1F ("histo_data", "", histo_toys_snapshot_toysNoSystematics->GetNbinsX() * 10, histo_toys_snapshot_toysNoSystematics->GetBinLowEdge(1), histo_toys_snapshot_toysNoSystematics->GetBinLowEdge(histo_toys_snapshot_toysNoSystematics->GetNbinsX()+1) );    


    TTree* limit_toys_toysFrequentist = (TTree*) (_file1->Get("limit")) -> Clone ("TEST");  

    TH1F* histo_toys_toysFrequentist = new TH1F ("histo_toys_toysFrequentist", "histo_toys_toysFrequentist", histo_toys_snapshot_toysNoSystematics->GetNbinsX(), histo_toys_snapshot_toysNoSystematics->GetBinLowEdge(1), histo_toys_snapshot_toysNoSystematics->GetBinLowEdge(histo_toys_snapshot_toysNoSystematics->GetNbinsX()+1) );    
    
    
    limit_toys_toysFrequentist->Draw("limit >> histo_toys_toysFrequentist","","");
    
    
    
    
    TTree* limit_data = (TTree*) _file2->Get("limit");  
    std::cout << " limit_data = " << limit_data << std::endl;
    
    Double_t limit;
    limit_data->SetBranchAddress("limit", &limit);
    limit_data->GetEntry(0);
    histo_data->Fill(limit);
    std::cout << " limit (data) = " << limit << std::endl;
    
    
    histo_toys_snapshot_toysNoSystematics = DrawOverflow(histo_toys_snapshot_toysNoSystematics);
    
    
    histo_toys_snapshot_toysNoSystematics->SetLineWidth(1);
    histo_toys_snapshot_toysNoSystematics->SetLineColor(kBlue);

    histo_toys_toysFrequentist->SetLineWidth(1);
    histo_toys_toysFrequentist->SetLineColor(kGreen);


    histo_data->SetLineWidth(3);
    histo_data->SetLineColor(kRed);
    
    histo_toys_snapshot_toysNoSystematics->Scale (1. / histo_toys_snapshot_toysNoSystematics->Integral());
    histo_toys_toysFrequentist->Scale (1. / histo_toys_toysFrequentist->Integral());
    

    histo_toys_snapshot_toysNoSystematics->Draw("hist");
    histo_toys_snapshot_toysNoSystematics->GetXaxis()->SetTitle("#chi^{2}");

    histo_toys_toysFrequentist->Draw("hist same");
    histo_data->Draw("same");
    
}


