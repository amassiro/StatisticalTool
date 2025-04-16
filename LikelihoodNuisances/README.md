Likelihood scan of 1 POI looking at nuisances stability
====

After performing a likelihood scan 1D of a parameter of interest (POI), look at the stability of the nuisances vs the likelihood scan.
Jumps should be "avoided" as symptoms of local minima in the minimization process or instabilities in the fit.


Run combine:

    combine -M MultiDimFit datacard.txt --algo=grid --points 100 --setPhysicsModelParameterRanges r=0.01,2 -n "LHScan" -m 125 --trackParameters >> signalStrength.txt
    
Then run this code:

    mkdir plot2DLL
    
    python plot2Dscan.py --input=higgsCombineTest.MultiDimFit.mH120.root   --poi=k_cHWB
    

