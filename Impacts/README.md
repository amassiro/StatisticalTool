Impact plots
====

Understanding the impact plots and their use

    cd /afs/cern.ch/user/a/amassiro/work/Latinos/Framework/AnalyticAnomalousCoupling/CMSSW_10_2_13/src
    
    git clone https://github.com/cms-analysis/CombineHarvester.git CombineHarvester

    cmsenv
    
    scramv1 b -j 20
    
    cd /afs/cern.ch/user/a/amassiro/work/Latinos/Framework/AnalyticAnomalousCoupling/StatisticalTool/Impacts/
    
Run

    text2workspace.py     datacard.txt    -o datacard.root

    combineTool.py -M Impacts -d datacard.root -m 125 --doInitialFit -t -1 --expectSignal=1 -n nuis.125 
    
     
    combineTool.py -M Impacts -d datacard.root -m 125 --doInitialFit -t -1 --expectSignal=1 \
                 --named CMS_hww_WWnorm0j,CMS_hww_Topnorm0j,CMS_hww_DYttnorm0j,CMS_hww_WWnorm2j,CMS_hww_Topnorm2j,CMS_hww_DYttnorm2j  \
                 --setParameterRanges CMS_hww_WWnorm0j=-2,4:CMS_hww_Topnorm0j=-2,4:CMS_hww_DYttnorm0j=-2,4:CMS_hww_WWnorm2j=-2,4:CMS_hww_Topnorm2j=-2,4:CMS_hww_DYttnorm2j=-2,4         \
                 -n rateParams.125
    
    
    # do the fits for each nuisance
    combineTool.py -M Impacts -d datacard.root -m 125 --doFits -t -1 --expectSignal=1 --job-mode condor --task-name nuis -n nuis.125 
    
    # do the fit for each rateParam
    combineTool.py -M Impacts -d datacard.root -m 125 --doFits -t -1 --expectSignal=1 --job-mode condor --task-name rateParams \
            --named CMS_hww_WWnorm0j,CMS_hww_Topnorm0j,CMS_hww_DYttnorm0j,CMS_hww_WWnorm2j,CMS_hww_Topnorm2j,CMS_hww_DYttnorm2j \
            --setParameterRanges CMS_hww_WWnorm0j=-2,4:CMS_hww_Topnorm0j=-2,4:CMS_hww_DYttnorm0j=-2,4:CMS_hww_WWnorm2j=-2,4:CMS_hww_Topnorm2j=-2,4:CMS_hww_DYttnorm2j=-2,4 \
            -n rateParams.125
    