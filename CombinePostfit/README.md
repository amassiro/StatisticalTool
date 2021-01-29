Combine postfit distributions
====

See 

    https://hypernews.cern.ch/HyperNews/CMS/get/higgs-combination/1493.html

Where:

    /afs/cern.ch/user/a/amassiro/Framework/AnalyticAnomalousCoupling/StatisticalTool
    
    /afs/cern.ch/user/a/amassiro/Framework/Postprocessing/LatinosOfficial/CMSSW_10_6_4/src   x latino
    
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



Follow up
====

Fit using control regions and then combine only part of them

That is also possible with this method - you'll have to make a new
datacard and workspace with just the two bins you want to combine,
though. As long as in that workspace you have the same parameters
affecting these two bins as you have in the fit result using 3 bins, you
can then still use that fit result to extract the post-fit shapes from
the datacard/workspace with only the 2 bins.


    text2workspace.py datacard3.txt -o datacard3.root

    
    combineCards.py   bin1=datacard1.txt  \
                      bin2=datacard2.txt  \
                  >   combined12.txt

                  
    text2workspace.py combined12.txt -o combined12.root

    
    
    combineCards.py   bin1=datacard1.txt  \
                      bin2=datacard2.txt  \
                      bin3=datacard3.txt  \
                  >   combined.txt

                  
    text2workspace.py combined.txt -o combined.root

             
    mkdir fitDiagnosticsCombined         
    combine -M FitDiagnostics    combined.root   --out fitDiagnosticsCombined
    

The workspace and the datacard are for the bins where I have the same variable.
While the "fitdiagnostic" result is from the combined (3 bins) fit

    
    PostFitShapesFromWorkspace  \
          -w combined12.root  \
          -d combined12.txt \
          -o output_histograms.root \
          --postfit --sampling \
          -f fitDiagnosticsCombined/fitDiagnostics.root:fit_s  \
          --total-shapes
    




Toys method
====


Here's a script I wrote to do the same resampling procedure as in
FitDiagnostics and save the samples into numpy arrays for later use:
https://gist.github.com/nsmith-/61562bd89cdac004f4d34d87336d03d6 By
manipulating the sample arrays, you can compute the variance for
arbitrary functions of the bin yields. Alternatively, you could modify
it to save a ROOT TTree.

Instructions by Nick






In Latinos
====

    cmsenv

    ../../../LatinoAnalysis/ShapeAnalysis/scripts/mkPostFitCombinedPlot.py   \
               --inputFilePostFitShapesFromWorkspace  output_SR2j.root   \
               --outputFile  mytest.root  \
               --kind   P  \
               --cutName  combinedMy  \
               --variable mll  \
               --structureFile Full2016_v6/structure.py \
               --plotFile Full2016_v6/plot.py \
               --lumiText  '36 + 42 /fb'
               
               
    
    mkPostFitCombinedPlot.py   \
               --inputFilePostFitShapesFromWorkspace  output_SR2j.root   \
               --outputFile  mytest.root  \
               --kind   P  \
               --cutName  combinedMy  \
               --variable mll   \
               --structureFile Full2016_v6/structure.py   \
               --plotFile Full2016_v6/plot.py \
               --lumiText  '36 + 42 /fb'
               
    
Then run simply mkPlot, after changing the cuts.py and variables.py


    mkPlot.py --pycfg=configuration_combined.py --inputFile mytest.root   --onlyPlot=cratio --linearOnly --showIntegralLegend=1

    
    
    
    