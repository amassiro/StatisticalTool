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
        datacard_combined_0123.txt \
        -o workspace_0123.root


    combine -M    MultiDimFit    workspace_0123.root     \
       --algo=cross


    fiducial r_s0 = 40/80 = 0.5
    fiducial r_s1 = 40/40 = 1.0
    fiducial r_s2 = 80/80 = 1.0
    fiducial r_s3 = 50/50 = 1.0

    r_s0 = 0.499964
    r_s1 = 1.00003
    r_s2 = 1
    r_s3 = 1.00002


Normalized:

    Expected:
    80+40+80+50 = 250

    Observed:
    40+40+80+50 = 210

    fiducial r_s0 = 210/250 = 0.84


    New expected =
    (80,  40, 80, 50 ) x 0.84 =
    (67.2,  33.6,  67.2, 42.0 )

    and compared to the Observed:

    RVV_0 = 40 / 67.2 = 0.59523810
    RVV_1 = 40 / 33.6 = 1.1904762
    RVV_2 = 80 / 67.2 = 1.1904762
    RVV_3 = 50 / 42.0 = 1.1904762

     RVV_1 = 1.19075
     RVV_2 = 1.19033
     RVV_3 = 1.19091

    FR0 rateParam * Signal0 (250-40*@0-80*@1-50*@2)/80 RVV_1,RVV_2,RVV_3
    RVV_0 = (250 - 40 * 1.1904762 - 80 * 1.1904762 - 50 * 1.1904762) / 80 = 0.59523808



    cat  datacard_combined_0123.txt   addendum.txt  >  datacard_combined_0123_normalized.txt

    text2workspace.py -P HiggsAnalysis.CombinedLimit.PhysicsModel:multiSignalModel \
              --PO verbose \
              --PO 'map=.*/Signal0:r_s0[1,0,3]' \
              --PO 'map=.*/Signal1:r_s0[1,0,3]' \
              --PO 'map=.*/Signal2:r_s0[1,0,3]' \
              --PO 'map=.*/Signal3:r_s0[1,0,3]' \
        datacard_combined_0123_normalized.txt \
        -o workspace_0123_normalized.root


    combine -M    MultiDimFit    workspace_0123_normalized.root     \
       --redefineSignalPOIs r_s0,RVV_1,RVV_2,RVV_3 \
       --algo=cross \
       --trackParameters FR0 \
       --trackErrors FR0 \
       -m 125

    combine -M    MultiDimFit    workspace_0123_normalized.root     \
       --redefineSignalPOIs r_s0,RVV_1,RVV_2,RVV_3 \
       --algo=singles \
       --trackParameters FR0 \
       --trackErrors FR0 \
       -m 125



    combine -M FitDiagnostics workspace_0123_normalized.root \
       --forceRecreateNLL \
       --redefineSignalPOIs r_s0,RVV_1,RVV_2,RVV_3 \
       --saveWorkspace \
       -m 125

    combine -M    MultiDimFit    workspace_0123_normalized.root     \
       --redefineSignalPOIs r_s0,RVV_1,RVV_2,RVV_3 \
       --algo=singles \
       --saveFitResult \
       --robustHesse=1 \
       --trackParameters FR0 \
       --trackErrors FR0 \
       -m 125


    combine -M    MultiDimFit    workspace_0123_normalized.root     \
       --redefineSignalPOIs r_s0,RVV_1,RVV_2,RVV_3 \
       --algo=none \
       --saveFitResult \
       --trackParameters FR0 \
       --trackErrors FR0 \
       -m 125


    r99t scanRoot.cxx


New model:

    cat  datacard_combined_0123.txt   addendum2.txt  >  datacard_combined_0123_normalized_newmodel.txt

    text2workspace.py datacard_combined_0123_normalized_newmodel.txt -o workspace_0123_newmodel.root

    combine -M MultiDimFit workspace_0123_newmodel.root \
         --algo cross \
         --redefineSignalPOIs cross_section_r,Fraction_0,Fraction_1,Fraction_2 \
         --trackParameters Fraction_3 \
         -m 130



    cat  datacard_combined_0123.txt   addendum3.txt  >  datacard_combined_0123_normalized_newmodel.txt

    text2workspace.py -P HiggsAnalysis.CombinedLimit.PhysicsModel:multiSignalModel \
              --PO verbose \
              --PO 'map=.*/Signal0:r_cross_section[1,0,3]' \
              --PO 'map=.*/Signal1:r_cross_section[1,0,3]' \
              --PO 'map=.*/Signal2:r_cross_section[1,0,3]' \
              --PO 'map=.*/Signal3:r_cross_section[1,0,3]' \
        datacard_combined_0123_normalized_newmodel.txt \
        -o workspace_0123_normalized_newmodel.root

    combine -M MultiDimFit workspace_0123_normalized_newmodel.root \
         --algo cross \
         --redefineSignalPOIs r_cross_section,F0,F1,F2 \
         --trackParameters F3 \
         --trackErrors F3 \
         -m 140

    combine -M FitDiagnostics workspace_0123_normalized_newmodel.root \
       --forceRecreateNLL \
       --redefineSignalPOIs r_cross_section,F0,F1,F2 \
       --saveWorkspace \
       -m 140


    r99t scanResult.cxx









Links:

    https://cms-analysis.github.io/HiggsAnalysis-CombinedLimit/latest/part3/commonstatsmethods/?h=singles#likelihood-fits-and-scans











