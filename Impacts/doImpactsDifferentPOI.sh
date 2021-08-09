

# text2workspace.py     datacard2.txt    -o datacard.root
text2workspace.py        datacard1opLinearOnly.txt -P HiggsAnalysis.AnalyticAnomalousCoupling.AnomalousCouplingLinearEFTNegative:analiticAnomalousCouplingLinearEFTNegative   -o   datacard.root    --X-allow-no-signal  --PO eftOperators=cG
          
combineTool.py -M Impacts -d datacard.root -n blablabla -m 125 --doInitialFit    -t -1 --setParameters r=1.0,cG=0.0 --redefineSignalPOIs=cG --freezeParameters=r --X-rtd MINIMIZER_analytic 

combineTool.py -M Impacts -d datacard.root -n blablabla -m 125 --doFits          -t -1 --setParameters r=1.0,cG=0.0 --redefineSignalPOIs=cG --freezeParameters=r --X-rtd MINIMIZER_analytic 

# 
# wait for condor to end ...
# 

# 
# collect jobs output
# 
combineTool.py -M Impacts -d datacard.root -n blablabla -m 125 -o my_impact.json -t -1 --setParameters r=1.0,cG=0.0 --redefineSignalPOIs=cG --freezeParameters=r 

# 
# plot
# 

plotImpacts.py -i my_impact.json -o my_impact


