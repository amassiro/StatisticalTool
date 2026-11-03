Impact plots
====

Understanding the impact plots and their use

    cd /afs/cern.ch/user/a/amassiro/work/Latinos/Framework/AnalyticAnomalousCoupling/CMSSW_10_2_13/src
    
    git clone https://github.com/cms-analysis/CombineHarvester.git CombineHarvester

    cmsenv
    
    scramv1 b -j 20
    
    cd /afs/cern.ch/user/a/amassiro/work/Latinos/Framework/AnalyticAnomalousCoupling/StatisticalTool/Impacts/

    
Alternative:

    /home/amassiro/Cern/Code/StatisticalTool/Impacts/

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
 
    plotImpacts.py -i impacts.125.nuis.json -o impacts.125 

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
    
    
    
    
Impacts with a different Parameter of Interest (POI)

    doImpactsDifferentPOI.sh


        
    
    
    
    
    
    
    
    
    
    
    


Impact plots, observed and expected overlaid
====

Data and Asimov (MC-Asimov or data-Asimov) on the same canvas:
pulls/constraints on the left, impacts on the POI on the right.
The standard plotImpacts.py of combine accepts only one json, hence the local plotImpacts.py.

Run:

    text2workspace.py datacard.txt -m 125 -o ws.root

Data:

    combineTool.py -M Impacts -d ws.root -m 125 --robustFit 1 --doInitialFit -n .data
    combineTool.py -M Impacts -d ws.root -m 125 --robustFit 1 --doFits --parallel 8 -n .data
    combineTool.py -M Impacts -d ws.root -m 125 -o impacts_data.json -n .data

MC-Asimov (pre-fit nuisances, r=1):

    combineTool.py -M Impacts -d ws.root -m 125 --robustFit 1 -t -1 --expectSignal 1 --doInitialFit -n .asimov
    combineTool.py -M Impacts -d ws.root -m 125 --robustFit 1 -t -1 --expectSignal 1 --doFits --parallel 8 -n .asimov
    combineTool.py -M Impacts -d ws.root -m 125 -t -1 --expectSignal 1 -o impacts_asimov.json -n .asimov

data-Asimov (nuisances at the values fitted on data, r=1): same as MC-Asimov adding

    --toysFrequentist

Plot:

    python plotImpacts.py -i impacts_data.json -e impacts_asimov.json -t translate.json -o impacts

    python plotImpacts.py -i impacts_data.json -e impacts_asimov.json -t translate.json -o impacts \
                                --expLabel "data Asimov" -n 30 -c Preliminary

Where:

    -i              observed json (black points, coloured lines)
    -e              expected json (grey band, coloured boxes)
    -t              json {"nuisance_name": "label"} for the parameter names (TLatex)
    -n              number of parameters per page (--per-page, default 30)
    -m              maximum number of pages (--max-pages, default all)
    -s              sort by obs or exp impact (--sortBy obs/exp, default obs)
    -p              POI (default: first in the json)
    -b              do not print the observed best fit value
    --expLabel      legend label of the expected, e.g. "MC Asimov", "data Asimov"

NB: pull definition as default in combine plotImpacts.py: (theta_fit - theta_0)/sigma_prefit, asymmetric errors.
NB: impacts are signed: Delta r when the nuisance is moved to theta_fit +1 sigma (red) and -1 sigma (blue).
NB: the -n names (.data, .asimov) keep the two sets of fits separated in the same folder.
NB: the local plotImpacts.py works with python2 and python3.
NB: as in the original, all the pages in one pdf (impacts.pdf), plus one png/root per page (impacts_pageN.png),
    rank of each parameter written on the left.
NB: unconstrained parameters (e.g. rateParam) have no pull: the fitted value is written instead,
    observed on the upper line and expected on the lower one (double height row), as taken from the two json.
    PDG rounding from combine if available, otherwise 2 significant digits on the uncertainty.
NB: "python plotImpacts.py" runs the local script, "plotImpacts.py" (no python) runs the combine one in the PATH.

Test without combine (fake json), results in output/ (not in git):

    mkdir -p output
    cd output
    python ../makeFakeImpacts.py
    python ../plotImpacts.py -i impacts_data.json -e impacts_asimov.json -t ../translate.json -o test_impacts



Improved Impact
====

Impact plot with data and Asimov overlaid, as in CMS papers.
Example, full chain:

    text2workspace.py datacard.txt -m 125 -o ws.root

    # data
    combineTool.py -M Impacts -d ws.root -m 125 --robustFit 1 --doInitialFit -n .data
    combineTool.py -M Impacts -d ws.root -m 125 --robustFit 1 --doFits --parallel 8 -n .data
    combineTool.py -M Impacts -d ws.root -m 125 -o impacts_data.json -n .data

    # MC-Asimov (for data-Asimov add --toysFrequentist)
    combineTool.py -M Impacts -d ws.root -m 125 --robustFit 1 -t -1 --expectSignal 1 --doInitialFit -n .asimov
    combineTool.py -M Impacts -d ws.root -m 125 --robustFit 1 -t -1 --expectSignal 1 --doFits --parallel 8 -n .asimov
    combineTool.py -M Impacts -d ws.root -m 125 -t -1 --expectSignal 1 -o impacts_asimov.json -n .asimov

    # plot, results in output/
    mkdir -p output
    python plotImpacts.py -i impacts_data.json -e impacts_asimov.json -t translate.json -o output/impacts
    python plotImpacts.py -i impacts_data.json -e impacts_asimov.json -t translate.json -o output/impacts_top16 -n 16 -m 1 -s exp
    python plotImpacts.py -i impacts_data.json -e impacts_asimov.json -t translate.json -o output/impacts_dataAsimov --expLabel "data Asimov" -n 30 -c Preliminary
    python plotImpacts.py -e impacts_asimov.json -t translate.json -o output/impacts_expOnly --expLabel "MC Asimov"

