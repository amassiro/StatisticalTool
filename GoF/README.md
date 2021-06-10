Goodness of fit
====

    http://cms-analysis.github.io/HiggsAnalysis-CombinedLimit/part3/commonstatsmethods/#goodness-of-fit-tests
    
    http://cms-analysis.github.io/HiggsAnalysis-CombinedLimit/part3/runningthetool/#submission-to-condor

    
Local tests:


with condor:

    combineTool.py -M GoodnessOfFit datacard.txt --algo=saturated -t 1 -s 1:1000:1  --toysFreq --job-mode condor --sub-opts='+JobFlavour="espresso"'
    
    
    
    
    
    