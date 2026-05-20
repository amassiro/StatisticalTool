Unfolding
====

Example of different repsonse matrices for different priors, but same smearing function

    r99t matrix.cxx



Unfolding and normalized unfolding
====

Where:

    cd /afs/cern.ch/user/a/amassiro/work/Combine/CMSSW_14_1_0_pre4/src
    cmsenv
    cd /afs/cern.ch/user/a/amassiro/work/Combine/test


    datacard0.txt
    datacard1.txt
    datacard2.txt
    datacard3.txt

    combineCards.py   bin0=datacard0.txt  \
                      bin1=datacard1.txt  \
                      bin2=datacard2.txt  \
                      bin3=datacard3.txt  \
                  >   datacard_combined_0123.txt

    datacard_combined_0123.txt




    text2workspace.py -P HiggsAnalysis.CombinedLimit.PhysicsModel:multiSignalModel \
        --PO verbose \
        --PO 'map=.*/Signal0:r_s0[1,0,3]' \
        --PO 'map=.*/Signal1:r_s1[1,0,3]' \
        --PO 'map=.*/Signal2:r_s2[1,0,3]' \
        --PO 'map=.*/Signal3:r_s3[1,0,3]' \
        datacard_combined_0123.text \
        -o workspace_0123.root




