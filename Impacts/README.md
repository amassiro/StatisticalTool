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

And run:

    combineTool.py -M Impacts -d datacard.root -m 125 --doInitialFit -t -1 --expectSignal=1 -n nuis.125 
    
    # do the fits for each nuisance
    combineTool.py -M Impacts -d datacard.root -m 125 --doFits -t -1 --expectSignal=1 --job-mode condor --task-name nuis -n nuis.125 
    
Wait for condor to end ...

    # collect job output
    combineTool.py -M Impacts -d datacard.root -m 125 -t -1 --expectSignal=1 -o impacts.125.nuis.json -n nuis.125
    
    #combine the two jsons
    echo "{\"params\":" > impacts.125.json
    jq -s ".[0].params+.[1].params" impacts.125.nuis.json >> impacts.125.json 
    echo ",\"POIs\":" >> impacts.125.json
    jq -s ".[0].POIs" impacts.125.nuis.json >> impacts.125.json
    echo "}" >> impacts.125.json
    # make plots
    plotImpacts.py -i impacts.125.json -o impacts.125 
 

Explanation:

    bla up -> measure smaller r:
      red +1sigma -> on the left
    and viceversa
    

    
    
    
Including rateparam:

    text2workspace.py     datacard2.txt    -o datacard.root

    
        
    combineTool.py -M Impacts -d datacard.root -m 125 --doInitialFit -t -1 --expectSignal=1 \
                 --named bkg_norm  \
                 --setParameterRanges bkg_norm=-2,4   \
                 -n rateParams.125
    
    combineTool.py -M Impacts -d datacard.root -m 125 --doInitialFit -t -1 --expectSignal=1 -n nuis.125 

    # do the fits for each nuisance
    combineTool.py -M Impacts -d datacard.root -m 125 --doFits -t -1 --expectSignal=1 --job-mode condor --task-name nuis -n nuis.125 
    
    # do the fit for each rateParam
    combineTool.py -M Impacts -d datacard.root -m 125 --doFits -t -1 --expectSignal=1 --job-mode condor --task-name rateParams \
            --named bkg_norm \
            --setParameterRanges bkg_norm=-2,4 \
            -n rateParams.125
    
Wait for condor to end ...


    #collect job output
    combineTool.py -M Impacts -d datacard.root -m 125 -t -1 --expectSignal=1 -o impacts.125.nuis.json -n nuis.125
    
    combineTool.py -M Impacts -d datacard.root -m 125 -t -1 --expectSignal=1 --named bkg_norm   -o impacts.125.rateParams.json -n rateParams.125
    
    
    #combine the two jsons
    echo "{\"params\":" > impacts.125.json
    jq -s ".[0].params+.[1].params" impacts.125.nuis.json impacts.125.rateParams.json >> impacts.125.json 
    echo ",\"POIs\":" >> impacts.125.json
    jq -s ".[0].POIs" impacts.125.nuis.json impacts.125.rateParams.json >> impacts.125.json
    echo "}" >> impacts.125.json
    # make plots
    plotImpacts.py -i impacts.125.json -o impacts.125 
    
    
    
    
    
    
    
    
    
    
    
    
    
    