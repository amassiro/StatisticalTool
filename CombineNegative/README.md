Negative bins
====

test of combine feature described here:

    http://cms-analysis.github.io/HiggsAnalysis-CombinedLimit/part2/physicsmodels/#interference


And possible fix commenting this:
    
    https://github.com/cms-analysis/HiggsAnalysis-CombinedLimit/blob/102x/src/CMSHistFunc.cc#L575
    
    

    
    
Create data:

    r99t -q create.cxx
    
    
Fit:

    text2workspace.py datacard.txt -o datacard.root
              
    combine -M FitDiagnostics -t -1  --expectSignal 1    datacard.root
    
    
 <<< Combine >>> 
>>> method used is FitDiagnostics
>>> random number generator seed is 123456
SimNLL created with 0 channels, 0 generic constraints, 3 fast gaussian constraints, 0 fast poisson constraints, 0 fast group constraints, 
SimNLL created with 1 channels, 0 generic constraints, 3 fast gaussian constraints, 0 fast poisson constraints, 0 fast group constraints, 

 --- FitDiagnostics ---
Best fit r: 1  -0.50732/+0.775548  (68% CL)
Done in 0.00 min (cpu), 0.00 min (real)



    
    
    text2workspace.py datacardPos.txt -o datacardPos.root
              
    combine -M FitDiagnostics -t -1  --expectSignal 1    datacardPos.root
    