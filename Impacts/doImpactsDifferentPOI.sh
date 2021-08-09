

combineTool.py -M Impacts -d cards/$1_$2.root -n $1_$2 -m 125 --doInitialFit -t -1 --setParameters muV=1.0,Fai=0.1 --redefineSignalPOIs=Fai --freezeParameters=muV --X-rtd MINIMIZER_analytic 
combineTool.py -M Impacts -d cards/$1_$2.root -n $1_$2 -m 125 --doFits -t -1 --setParameters muV=1.0,Fai=0.1 --redefineSignalPOIs=Fai --freezeParameters=muV --X-rtd MINIMIZER_analytic 
combineTool.py -M Impacts -d cards/$1_$2.root -n $1_$2 -m 125 -o aimpacts_$1_$2.json -t -1 --setParameters muV=1.0,Fai=0.1 --redefineSignalPOIs=Fai --freezeParameters=muV 
plotImpacts.py -i aimpacts_$1_$2.json -o aimpacts_$1_$2


