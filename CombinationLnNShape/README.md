Combine two datacards, with lnN and shape for same nuisance
====

What happens when you combine two datacards, one with shape, one with lnN for the same nuisance.



Where:

    /afs/cern.ch/user/a/amassiro/Framework/AnalyticAnomalousCoupling/StatisticalTool/CombinationLnNShape
    
    cd /afs/cern.ch/user/a/amassiro/Framework/Combine/CMSSW_10_2_13/src/
    cmsenv
    cd -
    (to get combine)
    

    
Create data:

    r99t -q create.cxx
    
Combine datacards:

    combineCards.py   bin1=datacard_1.txt  \
                      bin2=datacard_2.txt  \
                      >   datacard_combined.txt

                      
    combineCards.py   bin1=datacard_1.txt  \
                      bin2=datacard_2.txt  \
                      -S  \
                      >   datacard_combined.txt
 
 
    combineCards.py   bin1=datacard_2.txt  \
                      bin2=datacard_3.txt  \
                      >   datacard_combined_with_histos.txt

 
NB: "-S" is needed to get this line in the combined datacard:
    
    shapes *         bin1      FAKE

    
                      
                      
That does:

    jes     lnN     1.1         0.9/1.2
    +
    jes     shape   -           1.0
    -->
    jes                     shape?     1.1      0.9/1.2  -        1.0    

    
Fit and likelihood scan:

    text2workspace.py datacard_1.txt -o datacard_1.root
    text2workspace.py datacard_2.txt -o datacard_2.root
    text2workspace.py datacard_combined.txt -o datacard.root
    
    text2workspace.py datacard_combined_with_histos.txt -o datacard_combined_with_histos.root
    
    
    combine -M    MultiDimFit    datacard.root     \
           --redefineSignalPOIs r                  \
           -n "test1"    -m 125                    \
           --algo=grid --points 200                \
           --setParameterRanges  r=0,2     
    
    combine -M    MultiDimFit    datacard_combined_with_histos.root     \
           --redefineSignalPOIs r                  \
           -n "test1histo"    -m 125               \
           --algo=grid --points 200                \
           --setParameterRanges  r=0,2     

           
           
    r00t higgsCombinetest1.MultiDimFit.mH125.root  draw.cxx   
    r00t higgsCombinetest1.MultiDimFit.mH125.root higgsCombinetest1histo.MultiDimFit.mH125.root  drawCompare.cxx   
    

    
    