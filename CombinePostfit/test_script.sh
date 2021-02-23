#/bin/sh


cd /afs/cern.ch/user/a/amassiro/Framework/AnalyticAnomalousCoupling/StatisticalTool/CombinePostfit/

cd ../../../Combine/CMSSW_10_2_13/src/
eval `scramv1 runtime -sh`

cd -
    
combineCards.py   binA=datacard1.txt  \
                  binB=datacard2.txt  \
                  >   combined_for_plots.txt



combineCards.py   bin2=datacard2.txt  \
                  bin3=datacard3.txt  \
                  >   combined.txt
                  
text2workspace.py combined.txt           -o combined.root
text2workspace.py combined_for_plots.txt -o combined_for_plots.root
             
             
mkdir fitDiagnosticsCombined         
combine -M FitDiagnostics    combined.root   --out fitDiagnosticsCombined
    
    
    
PostFitShapesFromWorkspace  \
          -w combined_for_plots.root  \
          -d combined_for_plots.txt \
          -o output_histograms.root \
          --postfit --sampling \
          -f fitDiagnosticsCombined/fitDiagnostics.root:fit_s  \
          --total-shapes
    
# latino

cd /afs/cern.ch/work/a/amassiro/Latinos/Framework/CMSSW_10_6_4/src/LatinoAnalysis/

eval `scramv1 runtime -sh`


cd -
    
../../../Postprocessing/LatinosOfficial/CMSSW_10_6_4/src/LatinoAnalysis/ShapeAnalysis/scripts/mkPostFitCombinedPlot.py   \
               --inputFilePostFitShapesFromWorkspace  output_histograms.root   \
               --outputFile  mytest.root  \
               --kind   P  \
               --cutName  combinedMy  \
               --variable mll  \
               --structureFile structure.py \
               --plotFile plot.py \
               --lumiText  '36 + 42 /fb'
               
               
     
/afs/cern.ch/work/a/amassiro/Latinos/Framework/CMSSW_10_6_4/src/LatinoAnalysis/ShapeAnalysis/scripts/mkPostFitControlPlots.py  --inputFileHisto mytest.root \
               --outputFileHistoClone mytest_backup.root   \
               --listOfFilesOriginal test1.root,test2.root  \
               --plotFile plot_combined.py   \
               --cutName  combinedMy  \
               --variable mll
    
    
        
       
        
mkPlot.py --pycfg=configuration_combined.py --inputFile mytest.root   --onlyPlot=cratio --linearOnly --showIntegralLegend=1            
               
              