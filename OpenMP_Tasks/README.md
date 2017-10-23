# OPENMP Tasks 


This repo holds few example programs developed using Openmp tasks. The Reduce.cpp and Mergesort.cpp are developed using Openmp tasks where as bubbleSort and Longest Common Subsequence(LCS) are developed using Openmp parallel constructs. 
The programs are executed and the benchmarks for varying input lengths using 16 threads are as follows: 


## Reduce 
```
                        REDUCE                
Threads    N    Sequential    Parallel    Speed UP
16    10    0.000000205    0.000314869    0.0006510644
16    100    0.00000024    0.000251804    0.0009531223
16    1000    0.000000365    0.000178978    0.0020393568
16    10000    0.000001724    0.000200654    0.0085919045
16    100000    0.000015039    0.000162344    0.0926366235
16    1000000    0.000169139    0.002947    0.0573936206
16    100000000    0.027304196    0.024335174    1.122005374
16    1000000000    0.270129046    0.23782924    1.135810912
16    10000000000    0.380664686    0.262687752    1.449114712
16    100000000000    0.32825516    0.294685649    1.113916341
16    1000000000000    0.342543646    0.30157212    1.135859794
16    10000000000000    0.356872494    0.31212884    1.143349951
16    100000000000000    0.075210186    0.066889324    1.12439746
```

## Longest Common Subsequence 
```
                    LCS             
N    Sequential    Parallel    Speedup 
10    0.000012308    0.000209065    0.05887164279
100    0.000057604    0.000688722    0.08363897189
1000    0.004067026    0.006168899    0.6592790707
10000    0.919737915    0.127064036    7.238381087
100000    157.3564308    10.77492197    14.60395085
10000000    212.6545862    9.861367931    21.56441051
```

## Bubble Sort 
```
                Bubble Sort            
N    Sequential    Parallel    Speedup 
10    0.000006    0.0000644    0.09316770186
100    0.000043    0.00083649    0.05140527681
1000    0.000994    0.00976179    0.1018255873
10000    0.153958    0.167787    0.9175800271
100000    18.53053    8.904846    2.080948957
```
