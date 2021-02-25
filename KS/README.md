Kolmogorov-Smirnov tests
====

    r99t test1.cxx
    r99t test1.cxx\(10000,1000\)
    
reference:

    https://root.cern.ch/doc/master/classTH1.html#aeadcf087afe6ba203bcde124cfabbee4
    
    
Solution:

    histo_reference->KolmogorovTest(histo_test, "X")
    
    is the reliable method