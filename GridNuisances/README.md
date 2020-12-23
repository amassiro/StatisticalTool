Grid likelihood scan and nuisances
====

According to
    
    http://cms-analysis.github.io/HiggsAnalysis-CombinedLimit/part3/runningthetool/

    
    Note that constraint terms for the nuisances are dropped after promotion to a POI using --redefineSignalPOI. To produce a likelihood scan for a nuisance parameter, using MultiDimFit with --algo grid, you should instead use the --parameters (-P) option which will not cause the loss of the constraint term when scanning.
    parameters of interest of the input workspace that are not selected by this command become unconstrained nuisance parameters, but they are not added to the list of nuisances so they will not be randomized (see above).

Here: test on how to perform 2D scan of nuisances, keeping their priors.

    text2workspace.py datacard.txt -o datacard.root
    
    combine -M    MultiDimFit    datacard.root     \
           --redefineSignalPOIs r,bla              \
           -n "test1"    -m 125                    \
           --algo=grid --points 10000               \
           --setParameterRanges  r=-2,3:bla=-3,3      

    
    r00t higgsCombinetest1.MultiDimFit.mH125.root  draw2d.cxx\(\"r\",\"bla\",\"r\",\"bla\"\)    
    
    
Now adding the constraint:

    combine -M    MultiDimFit    datacard.root     \
           --redefineSignalPOIs r                  \
           --parameters bla                        \
           -n "test1"    -m 125                    \
           --algo=grid --points 10000              \
           --setParameterRanges  r=-2,3:bla=-3,3
           

    --> scan only 1d vs "bla"
    
    
    combine -M    MultiDimFit    datacard.root     \
           --redefineSignalPOIs r                  \
           --parameters bla                        \
           --parameters only_bkg                   \
           -n "test1"    -m 125                    \
           --algo=grid --points 10000              \
           --setParameterRanges  r=-2,3:bla=-3,3,only_bkg=-3,3
           
    -> r is set constant
    
    
    combine -M    MultiDimFit    datacard.root     \
           --redefineSignalPOIs r                  \
           --parameters bla                        \
           --parameters r                          \
           -n "test1"    -m 125                    \
           --algo=grid --points 10000              \
           --setParameterRanges  r=-2,3:bla=-3,3
           

    combine -M    MultiDimFit    datacard.root     \
           --redefineSignalPOIs r                  \
           --parameters bla                        \
           --parameters r                          \
           -n "test1"    -m 125                    \
           --algo=grid --points 10000              \
           --setParameterRanges  r=-2,3:bla=-3,3   \
           -t -1
           
           
    -> ok scan 2d con r vs bla, using the constraint on bla, roughly ~ bla shifted by +/-1 for 1 sigma along its direction    
           
           
Draw:

    r00t higgsCombinetest1.MultiDimFit.mH125.root  draw2d.cxx\(\"r\",\"bla\",\"r\",\"bla\"\)    
    

    