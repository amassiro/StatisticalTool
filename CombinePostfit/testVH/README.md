Test with VH datacards
====

by Susan

    cd /afs/cern.ch/user/a/amassiro/work/Latinos/Framework/CMSSW_10_6_4/src/PlotsConfigurations/Configurations/ZH3l/
    
    git clone ssh://git@gitlab.cern.ch:7999/cms-hcg/cadi/hig-19-017.git
    git clone https://gitlab.cern.ch/cms-hcg/cadi/hig-19-017.git
    
    cp hig-19-017/VHlep/FullRunII_VH.* .
    
    
    cd /afs/cern.ch/user/a/amassiro/work/Latinos/Framework/AnalyticAnomalousCoupling/CMSSW_10_2_13/src/
    cmsenv
    cd -
    
    
    
    
 
Combine:

    FullRunII_ZH3l_SR1j.txt
    FullRunII_ZH3l_SR1j.root
    FullRunII_ZH3l_SR2j.txt
    FullRunII_ZH3l_SR2j.root
    
    fitDiagnosticsVH_obs.root


             
    mkdir fitDiagnosticsCombined         
    combine -M FitDiagnostics    FullRunII_VH.root   --out fitDiagnosticsCombined
        
        
    ln -s hig-19-017/VHlep/ZH3l
    ln -s hig-19-017/VHlep/WH3l
    ln -s hig-19-017/VHlep/WH2l_SS
    ln -s hig-19-017/VHlep/ZH4l

    
    
    
    
    PostFitShapesFromWorkspace  \
          -w FullRunII_VH.root  \
          -d FullRunII_VH.txt \
          -o output_histograms.root \
          --postfit --sampling \
          -f fitDiagnosticsCombined/fitDiagnostics.root:fit_s  \
          --total-shapes

