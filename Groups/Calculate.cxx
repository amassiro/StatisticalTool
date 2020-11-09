
void Calculate() {
  
  //
  //  Tot = total
  //  f = fractional ranking
  //  iFrac = fractional ranking (integer)
  //  m = group size A
  //  n = group size B
  //  M = numer of groups A
  //  N = number of groups B
  //  Tot = n*N+m*M
  //  
  //  exc = fractional of exclusions
  //
  
  int nFrac = 100;
  
  TH1F* N_survive_n = new TH1F ("N_survive_n","",nFrac, 0, nFrac);
  TH1F* N_total_n   = new TH1F ("N_total_n","",nFrac, 0, nFrac);
  TH1F* prob_survive_n = new TH1F ("prob_survive_n","",nFrac, 0, nFrac);
  
  TH1F* N_survive_m = new TH1F ("N_survive_m","",nFrac, 0, nFrac);
  TH1F* N_total_m   = new TH1F ("N_total_m","",nFrac, 0, nFrac);
  TH1F* prob_survive_m = new TH1F ("prob_survive_m","",nFrac, 0, nFrac);

  int m = 30;
  int n = 50;
  
  int M = 10;
  int N = 10;
  
  int Tot = m*M + n*N;
  
  int nToys = 10000;
  
  float exc = 0.10;

//   for (int iFrac = 0; iFrac<nFrac; iFrac+=5) {
  for (int iFrac = 0; iFrac<nFrac; iFrac++) {
    int global_position = int( 1. * iFrac/nFrac*Tot ) ; // points from 0 to 100 (cumulative)
    
    std::cout << " global_position = " << global_position << " [" << iFrac << "] :: " << Tot << std::endl;
    
    for (int iToy = 0; iToy <= nToys; iToy++) {
      //       std::cout << " iToy = " << iToy << "[" << nToys << "]" << std::endl;
      std::vector<int> selected;
      
      bool found_me = false;
      
      for (int iM=0; iM<M; iM++) {
        std::vector<int> group;        
        for (int im=0; im<m; im++) {
          while (1) {
            int new_gamer = int (Tot * (gRandom->Uniform (1.)));
            // if not selected yet
            if (std::find(selected.begin(), selected.end(), new_gamer) == selected.end()) {
              selected.push_back(new_gamer);
              group.push_back(new_gamer);
              break;
            }
          }
        }
        
        if (std::find(group.begin(), group.end(), global_position) != group.end()) {
          // check if saved
          std::sort (group.begin(), group.end()); 
//           for (int ii=0; ii<group.size(); ii++) {
//             std::cout << "   [ " << ii << " ] = " << group.at(ii) << std::endl;
//           }
          std::vector<int>::iterator it = std::find(group.begin(), group.end(), global_position);
          int index = std::distance(group.begin(), it);
          
          if (1.0 * index / group.size() > exc) {
            N_survive_m->Fill(iFrac+1);
          }
          N_total_m->Fill(iFrac+1);
          found_me = true;
          group.clear();
          break;
        }   
        group.clear();
      }
      
      // if not yet found, check other groups
      if (!found_me) {
        for (int iN=0; iN<N; iN++) {
          std::vector<int> group;        
          for (int in=0; in<n; in++) {
            while (1) {
              int new_gamer = int (Tot * (gRandom->Uniform (1.)));
              // if not selected yet
              if (std::find(selected.begin(), selected.end(), new_gamer) == selected.end()) {
                selected.push_back(new_gamer);
                group.push_back(new_gamer);
                break;
              }
            }
          }
           
          if (std::find(group.begin(), group.end(), global_position) != group.end()) {
            // check if saved
            std::sort (group.begin(), group.end()); 
//             for (int ii=0; ii<group.size(); ii++) {
//               std::cout << "   [ " << ii << " ] = " << group.at(ii) << std::endl;
//             }
            std::vector<int>::iterator it = std::find(group.begin(), group.end(), global_position);
            int index = std::distance(group.begin(), it);
            
            if (1.0 * index / group.size() > exc) {
              N_survive_n->Fill(iFrac+1);
            }
            N_total_n->Fill(iFrac+1);
            found_me = true;
            group.clear();
            break;
          }   
          group.clear();
        } 
      }
      selected.clear();
    }
  }
  
  // calculate prob_survive as N_survive / N_total
  for (int iFrac = 0; iFrac<nFrac; iFrac++) {
    
    float pass  = N_survive_m -> GetBinContent (iFrac+1);
    float total = N_total_m   -> GetBinContent (iFrac+1);
    
    float ratio = 0;
    if (total !=0 ) ratio = pass / total;
    prob_survive_m -> SetBinContent ( iFrac+1, ratio);
    
  }
  for (int iFrac = 0; iFrac<nFrac; iFrac++) {
    
    float pass  = N_survive_n -> GetBinContent (iFrac+1);
    float total = N_total_n   -> GetBinContent (iFrac+1);
    
    float ratio = 0;
    if (total !=0 ) ratio = pass / total;
    prob_survive_n -> SetBinContent ( iFrac+1, ratio);
    
  }
  
  //   prob_survive = N_survive.Clone();
  //   prob_survive.Divide(N_total);
  
  
  prob_survive_m->SetLineColor(kRed);
  prob_survive_n->SetLineColor(kBlue);
  
  TCanvas* cc = new TCanvas ("cc", "", 800,600);
  
  prob_survive_m->Draw("L");
  prob_survive_n->Draw("L same");
  
  TLegend* legend = new TLegend(0.1,0.7,0.48,0.9);
  legend->AddEntry(prob_survive_m, " group 30","L");
  legend->AddEntry(prob_survive_n, " group 50","L");
  legend->Draw();

  
  TFile* file_result = new TFile("file_result.root", "RECREATE");
  N_survive_n->Write();
  N_total_n->Write();
  prob_survive_n->Write();
  N_survive_m->Write();
  N_total_m->Write();
  prob_survive_m->Write();
  
  
  
}
