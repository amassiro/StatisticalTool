Combine postfit distributions
====

See 

    https://hypernews.cern.ch/HyperNews/CMS/get/higgs-combination/1493.html

Prepare datacards
====

    r99t create.cxx


Fit
====

    
From

    http://cms-analysis.github.io/CombineHarvester/post-fit-shapes-ws.html

    
combine
    
    cd ../../CMSSW_10_2_13/src/
    cmsenv
    cd -
    

    
    
    text2workspace.py datacard1.txt -o datacard1.root
    text2workspace.py datacard2.txt -o datacard2.root
    
    
    mkdir fitDiagnostics1
    mkdir fitDiagnostics2
    
    combine -M FitDiagnostics    datacard1.root   --out fitDiagnostics1
    combine -M FitDiagnostics    datacard2.root   --out fitDiagnostics2
    
    
    
    combineCards.py   bin1=datacard1.txt  \
                      bin2=datacard2.txt  \
                  >   combined.txt

                  
    text2workspace.py combined.txt -o combined.root
             
    mkdir fitDiagnosticsCombined         
    combine -M FitDiagnostics    combined.root   --out fitDiagnosticsCombined
    
    
    

You can use PostFitShapesFromWorkspace in CombineHarvester for this [*],
you just need a combined datacard for your 2 bins, the workspace, and
the fitDiagnostics result for the combined fit, which I think you
already have.

You can then call PostFitShapesFromWorkspace -w your_workspace.root -d
your_datacard.txt -o output_histograms.root --postfit --sampling -f
your_fitdiagnostics_file.root:fit_s --total-shapes

    PostFitShapesFromWorkspace  \
          -w combined.root  \
          -d combined.txt \
          -o output_histograms.root \
          --postfit --sampling \
          -f fitDiagnosticsCombined/fitDiagnostics.root:fit_s  \
          --total-shapes

          
The file output_histograms.root will then contain a subdirectory
'postfit' with the the sum across bins for signal, background, and all
processes combined. The uncertainty band is sampled from the covariance
matrix, so takes into account the correlations correctly.


Instructions by Adinda

[*]
http://cms-analysis.github.io/CombineHarvester/post-fit-shapes-ws.html





Toys method
====


Here's a script I wrote to do the same resampling procedure as in
FitDiagnostics and save the samples into numpy arrays for later use:
https://gist.github.com/nsmith-/61562bd89cdac004f4d34d87336d03d6 By
manipulating the sample arrays, you can compute the variance for
arbitrary functions of the bin yields. Alternatively, you could modify
it to save a ROOT TTree.

Instructions by Nick





