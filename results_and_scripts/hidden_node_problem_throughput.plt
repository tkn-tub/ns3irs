set terminal svg
set output "hidden_node_problem_throughput.svg"
set title "Throughput: With and Without IRS"
set xlabel "Time (s)"
set ylabel "Throughput (Mbps)"
set key right center box 3
plot "-"  title "Tx(1) without IRS" with lines, "-"  title "Tx(2) without IRS" with lines, "-"  title "Tx(1) with IRS" with lines, "-"  title "Tx(2) with IRS" with lines, "-"  title "Tx(1) RTS/CTS" with lines, "-"  title "Tx(2) RTS/CTS" with lines
1 0
1.05 0.0544
1.1 0.0830836
1.15 0.119207
1.2 0.15232
1.25 0.155366
1.3 0.149391
1.35 0.160782
1.4 0.22032
1.45 0.252116
1.5 0.258944
1.55 0.257961
1.6 0.27132
1.65 0.283869
1.7 0.28896
1.75 0.306816
1.8 0.310987
1.85 0.308757
1.9 0.318669
1.95 0.328074
2 0.319872
2.05 0.317643
2.1 0.32096
2.15 0.345377
2.2 0.34272
2.25 0.355413
2.3 0.372522
2.35 0.37918
2.4 0.38556
2.45 0.396343
2.5 0.397555
2.55 0.39872
2.6 0.395446
2.65 0.396607
2.7 0.397724
2.75 0.415418
2.8 0.41208
2.85 0.416876
2.9 0.413628
2.95 0.406617
3 0.415072
3.05 0.419504
3.1 0.427479
3.15 0.427947
3.2 0.44625
3.25 0.4429
3.3 0.446575
3.35 0.439909
3.4 0.4368
3.45 0.447026
3.5 0.443904
3.55 0.44087
3.6 0.43792
3.65 0.45383
3.7 0.460048
3.75 0.45696
3.8 0.45696
3.85 0.451025
3.9 0.459889
3.95 0.465636
4 0.468384
4.05 0.468243
4.1 0.479251
4.15 0.481735
4.2 0.47872
4.25 0.486528
4.3 0.486184
4.35 0.480596
4.4 0.482924
4.45 0.480065
4.5 0.482347
4.55 0.477046
4.6 0.474344
4.65 0.488898
4.7 0.483697
4.75 0.490631
4.8 0.49742
4.85 0.504069
4.9 0.510583
4.95 0.505425
5 0.500371
5.05 0.497679
5.1 0.49504
5.15 0.490234
5.2 0.498702
5.25 0.496128
5.3 0.497914
5.35 0.493261
5.4 0.499271
5.45 0.503075
5.5 0.504733
5.55 0.508419
5.6 0.50796
5.65 0.507509
5.7 0.505061
5.75 0.50663
5.8 0.504232
5.85 0.515545
5.9 0.516984
5.95 0.5184
6 0.521696
6.05 0.519273
6.1 0.531871
6.15 0.534978
6.2 0.532506
6.25 0.539213
6.3 0.536747
6.35 0.537917
6.4 0.53907
6.45 0.543747
6.5 0.539564
6.55 0.535445
6.6 0.531389
6.65 0.530829
6.7 0.531983
6.75 0.528043
6.8 0.5292
6.85 0.525337
6.9 0.528153
6.95 0.529285
7 0.528768
7.05 0.53312
7.1 0.534193
7.15 0.538446
7.2 0.53788
7.25 0.535746
7.3 0.533642
7.35 0.537783
7.4 0.537237
7.45 0.539765
7.5 0.53769
7.55 0.535642
7.6 0.532118
7.65 0.534613
7.7 0.53856
7.75 0.540982
7.8 0.543372
7.85 0.548643
7.9 0.549509
7.95 0.548927
8 0.54978
8.05 0.549203
8.1 0.548634
8.15 0.556482
8.2 0.557268
8.25 0.55943
8.3 0.557436
8.35 0.558203
8.4 0.55488
8.45 0.557004
8.5 0.55776
8.55 0.554498
8.6 0.551274
8.65 0.55205
8.7 0.55413
8.75 0.550963
8.8 0.550429
8.85 0.54861
8.9 0.546812
8.95 0.545033
9 0.543275
9.05 0.54406
9.1 0.542326
9.15 0.54186
9.2 0.540157
9.25 0.542177
9.3 0.541719
9.35 0.541265
9.4 0.539602
9.45 0.540373
9.5 0.545947
9.55 0.547874
9.6 0.54621
9.65 0.545748
9.7 0.547645
9.75 0.551867
9.8 0.553714
9.85 0.553223
9.9 0.552737
9.95 0.556848
10 0.554064
10.05 0.554718
10.1 0.554234
10.15 0.552629
10.2 0.54992
10.25 0.549467
10.3 0.550127
10.35 0.549677
10.4 0.548132
10.45 0.550976
10.5 0.552704
10.55 0.550085
10.6 0.552878
10.65 0.554574
10.7 0.55732
10.75 0.556854
10.8 0.556391
10.85 0.556986
10.9 0.558623
10.95 0.557116
11 0.558737
11.05 0.558277
11.1 0.556791
11.15 0.557368
11.2 0.5559
11.25 0.554445
11.3 0.556035
11.35 0.553586
11.4 0.55216
11.45 0.549749
11.5 0.548352
11.55 0.550924
11.6 0.551503
11.65 0.549136
11.7 0.54679
11.75 0.546407
11.8 0.546028
11.85 0.546617
11.9 0.54624
11.95 0.547778
12 0.549304
12.05 0.551765
12.1 0.552317
12.15 0.552865
12.2 0.552472
12.25 0.550217
12.3 0.550767
12.35 0.551312
12.4 0.549089
12.45 0.550554
12.5 0.552008
12.55 0.55345
12.6 0.553973
12.65 0.554493
12.7 0.553209
12.75 0.553728
12.8 0.55335
12.85 0.552975
12.9 0.550832
12.95 0.551351
13 0.554503
13.05 0.552379
13.1 0.552015
13.15 0.549916
13.2 0.551295
13.25 0.550939
13.3 0.550585
13.35 0.549379
13.4 0.551592
13.45 0.552939
13.5 0.550891
13.55 0.55223
13.6 0.55272
13.65 0.554043
13.7 0.554523
13.75 0.553337
13.8 0.55216
13.85 0.555116
13.9 0.556406
13.95 0.554412
14 0.55488
14.05 0.553718
14.1 0.554186
14.15 0.553035
14.2 0.555914
14.25 0.556369
14.3 0.55682
14.35 0.55488
14.4 0.55692
14.45 0.555784
14.5 0.557018
14.55 0.556675
14.6 0.557116
14.65 0.559893
14.7 0.561097
14.75 0.560744
14.8 0.565025
14.85 0.566969
14.9 0.565833
14.95 0.566233
e
1 0
1.05 0
1.1 0
1.15 0.0298017
1.2 0.06664
1.25 0.0731136
1.3 0.123028
1.35 0.143858
1.4 0.13872
1.45 0.157572
1.5 0.198016
1.55 0.22111
1.6 0.22134
1.65 0.22848
1.7 0.25536
1.75 0.280704
1.8 0.291947
1.85 0.327282
1.9 0.348733
1.95 0.351508
2 0.359856
2.05 0.351079
2.1 0.3536
2.15 0.35069
2.2 0.363491
2.25 0.355413
2.3 0.352654
2.35 0.350012
2.4 0.35224
2.45 0.345051
2.5 0.356429
2.55 0.37184
2.6 0.377871
2.65 0.396607
2.7 0.393493
2.75 0.390493
2.8 0.40392
2.85 0.400842
2.9 0.417567
2.95 0.433725
3 0.434112
3.05 0.430741
3.1 0.431164
3.15 0.4352
3.2 0.4284
3.25 0.43587
3.3 0.439651
3.35 0.433089
3.4 0.44688
3.45 0.443715
3.5 0.450432
3.55 0.45696
3.6 0.472827
3.65 0.46635
3.7 0.466223
3.75 0.466099
3.8 0.465979
3.85 0.474764
3.9 0.468677
3.95 0.471421
4 0.476952
4.05 0.490809
4.1 0.490396
4.15 0.492746
4.2 0.49504
4.25 0.49728
4.3 0.496811
4.35 0.51211
4.4 0.508887
4.45 0.510871
4.5 0.515349
4.55 0.52224
4.6 0.526497
4.65 0.520836
4.7 0.53231
4.75 0.526707
4.8 0.52598
4.85 0.527624
4.9 0.531566
4.95 0.542352
5 0.552922
5.05 0.558758
5.1 0.56896
5.15 0.581182
5.2 0.575594
5.25 0.574464
5.3 0.5712
5.35 0.578674
5.4 0.579662
5.45 0.57644
5.5 0.577431
5.55 0.576346
5.6 0.57528
5.65 0.576255
5.7 0.579217
5.75 0.57418
5.8 0.575139
5.85 0.570224
5.9 0.5712
5.95 0.57216
6 0.573104
6.05 0.577809
6.1 0.573073
6.15 0.572129
6.2 0.574885
6.25 0.570286
6.3 0.573013
6.35 0.573899
6.4 0.572985
6.45 0.570314
6.5 0.576473
6.55 0.582537
6.6 0.59024
6.65 0.592674
6.7 0.591661
6.75 0.594048
6.8 0.5964
6.85 0.603721
6.9 0.602657
6.95 0.603253
7 0.60384
7.05 0.599557
7.1 0.598553
7.15 0.599161
7.2 0.59976
7.25 0.601927
7.3 0.605628
7.35 0.607726
7.4 0.611338
7.45 0.613369
7.5 0.616896
7.55 0.614324
7.6 0.611785
7.65 0.60928
7.7 0.609775
7.75 0.610263
7.8 0.60928
7.85 0.605399
7.9 0.607352
7.95 0.607843
8 0.605472
8.05 0.60313
8.1 0.603639
8.15 0.599935
8.2 0.600457
8.25 0.596817
8.3 0.600104
8.35 0.596511
8.4 0.60112
8.45 0.600267
8.5 0.59808
8.55 0.602599
8.6 0.607066
8.65 0.604878
8.7 0.606654
8.75 0.613632
8.8 0.61404
8.85 0.614443
8.9 0.616126
8.95 0.616513
9 0.615627
9.05 0.612225
9.1 0.611372
9.15 0.611777
9.2 0.613419
9.25 0.610103
9.3 0.612965
9.35 0.613353
9.4 0.612521
9.45 0.611698
9.5 0.608478
9.55 0.606489
9.6 0.61047
9.65 0.609675
9.7 0.611243
9.75 0.608108
9.8 0.606171
9.85 0.607734
9.9 0.604664
9.95 0.601626
10 0.602045
10.05 0.60246
10.1 0.60287
10.15 0.605528
10.2 0.61264
10.25 0.61411
10.3 0.613347
10.35 0.614799
10.4 0.618434
10.45 0.617661
10.5 0.617984
10.55 0.623718
10.6 0.625087
10.65 0.622152
10.7 0.620313
10.75 0.621678
10.8 0.621973
10.85 0.623319
10.9 0.623604
10.95 0.624929
11 0.623127
11.05 0.622375
11.1 0.625747
11.15 0.626015
11.2 0.62526
11.25 0.625527
11.3 0.625793
11.35 0.627062
11.4 0.631326
11.45 0.634556
11.5 0.634777
11.55 0.632029
11.6 0.633244
11.65 0.63641
11.7 0.639549
11.75 0.639744
11.8 0.637033
11.85 0.637237
11.9 0.63744
11.95 0.638597
12 0.636888
12.05 0.634245
12.1 0.632569
12.15 0.631846
12.2 0.631129
12.25 0.633216
12.3 0.6325
12.35 0.630864
12.4 0.633848
12.45 0.63222
12.5 0.633347
12.55 0.630823
12.6 0.630133
12.65 0.630352
12.7 0.62877
12.75 0.628992
12.8 0.630105
12.85 0.628542
12.9 0.630534
12.95 0.629864
13 0.627441
13.05 0.626788
13.1 0.629628
13.15 0.634184
13.2 0.633513
13.25 0.636295
13.3 0.637339
13.35 0.638375
13.4 0.637698
13.45 0.637875
13.5 0.638898
13.55 0.638226
13.6 0.63924
13.65 0.638572
13.7 0.637909
13.75 0.637251
13.8 0.638254
13.85 0.637599
13.9 0.636128
13.95 0.638761
14 0.639744
14.05 0.637467
14.1 0.636017
14.15 0.636999
14.2 0.634756
14.25 0.634934
14.3 0.63511
14.35 0.63847
14.4 0.638633
14.45 0.637214
14.5 0.635805
14.55 0.63519
14.6 0.63458
14.65 0.634753
14.7 0.632594
14.75 0.634322
14.8 0.632179
14.85 0.63082
14.9 0.632537
14.95 0.631186
e
1 0
1.05 0.17408
1.1 0.34272
1.15 0.447026
1.2 0.6188
1.25 1.00531
1.3 1.29179
1.35 1.39627
1.4 1.53408
1.45 1.72542
1.5 1.95731
1.55 2.1595
1.6 2.27052
1.65 2.41635
1.7 2.50656
1.75 2.59814
1.8 2.86869
1.85 2.90231
1.9 2.94619
1.95 3.03468
2 3.15874
2.05 3.41048
2.1 3.55232
2.15 3.67162
2.2 3.78031
2.25 3.91462
2.3 4.10271
2.35 4.25848
2.4 4.3792
2.45 4.39241
2.5 4.40966
2.55 4.47552
2.6 4.53445
2.65 4.58253
2.7 4.66268
2.75 4.79393
2.8 4.90008
2.85 4.96243
2.9 4.97929
2.95 5.0072
3 5.05322
3.05 5.06027
3.1 5.05604
3.15 5.09184
3.2 5.16222
3.25 5.28316
3.3 5.35543
3.35 5.43919
3.4 5.50704
3.45 5.57293
3.5 5.62714
3.55 5.67338
3.6 5.68661
3.65 5.74643
3.7 5.8108
3.75 5.88564
3.8 5.88937
3.85 5.89597
3.9 5.9229
3.95 5.94048
4 5.9719
4.05 5.98561
4.1 6.0185
4.15 6.08362
4.2 6.09824
4.25 6.11789
4.3 6.09457
4.35 6.06391
4.4 6.08328
4.45 6.08424
4.5 6.04203
4.55 6.0384
4.6 6.0274
4.65 6.02647
4.7 6.06201
4.75 6.06073
4.8 6.06424
4.85 6.09123
4.9 6.13399
4.95 6.17127
5 6.21009
5.05 6.24361
5.1 6.28544
5.15 6.28431
5.2 6.28979
5.25 6.30822
5.3 6.32631
5.35 6.33124
5.4 6.32551
5.45 6.31569
5.5 6.34551
5.55 6.37891
5.6 6.39948
5.65 6.39137
5.7 6.39744
5.75 6.39347
5.8 6.41517
5.85 6.44431
5.9 6.49425
5.95 6.49536
6 6.50406
6.05 6.55417
6.1 6.54539
6.15 6.5349
6.2 6.543
6.25 6.55463
6.3 6.56971
6.35 6.59714
6.4 6.60093
6.45 6.65426
6.5 6.65404
6.55 6.65731
6.6 6.64496
6.65 6.63107
6.7 6.63274
6.75 6.63269
6.8 6.63264
6.85 6.61591
6.9 6.60605
6.95 6.60948
7 6.61123
7.05 6.65671
7.1 6.68867
7.15 6.71859
7.2 6.75285
7.25 6.76774
7.3 6.77459
7.35 6.80933
7.4 6.82661
7.45 6.84213
7.5 6.87877
7.55 6.90736
7.6 6.92054
7.65 6.91861
7.7 6.93007
7.75 6.93105
7.8 6.94374
7.85 6.93735
7.9 6.93827
7.95 6.95499
8 6.98006
8.05 6.99631
8.1 7.018
8.15 7.0198
8.2 7.0383
8.25 7.06072
8.3 7.07875
8.35 7.11161
8.4 7.1536
8.45 7.15994
8.5 7.17562
8.55 7.18309
8.6 7.19978
8.65 7.20835
8.7 7.20894
8.75 7.19647
8.8 7.18673
8.85 7.17453
8.9 7.16246
8.95 7.15053
9 7.13619
9.05 7.12075
9.1 7.1469
9.15 7.15529
9.2 7.16608
9.25 7.17057
9.3 7.17132
9.35 7.18551
9.4 7.1959
9.45 7.20981
9.5 7.20554
9.55 7.2025
9.6 7.20902
9.65 7.1989
9.7 7.19948
9.75 7.19653
9.8 7.21927
9.85 7.2151
9.9 7.22828
9.95 7.24247
10 7.25196
10.05 7.24088
10.1 7.23897
10.15 7.25171
10.2 7.25088
10.25 7.25452
10.3 7.26921
10.35 7.25065
10.4 7.24106
10.45 7.23046
10.5 7.2167
10.55 7.20524
10.6 7.21652
10.65 7.23198
10.7 7.24089
10.75 7.24441
10.8 7.26588
10.85 7.27346
10.9 7.29669
10.95 7.29884
11 7.30097
11.05 7.30826
11.1 7.31959
11.15 7.31853
11.2 7.3236
11.25 7.34284
11.3 7.3609
11.35 7.3788
11.4 7.3785
11.45 7.39517
11.5 7.41368
11.55 7.43796
11.6 7.45022
11.65 7.44178
11.7 7.42755
11.75 7.41928
11.8 7.41205
11.85 7.40005
11.9 7.37856
11.95 7.37445
12 7.36562
12.05 7.36255
12.1 7.36423
12.15 7.35555
12.2 7.37035
12.25 7.3869
12.3 7.40238
12.35 7.41774
12.4 7.43205
12.45 7.43707
12.5 7.45028
12.55 7.45336
12.6 7.44101
12.65 7.43689
12.7 7.45079
12.75 7.44666
12.8 7.46665
12.85 7.45449
12.9 7.44331
12.95 7.43663
13 7.4379
13.05 7.42516
13.1 7.4099
13.15 7.41127
13.2 7.4031
13.25 7.39499
13.3 7.40327
13.35 7.41319
13.4 7.41707
13.45 7.40904
13.5 7.40106
13.55 7.39989
13.6 7.40628
13.65 7.41598
13.7 7.4306
13.75 7.4202
13.8 7.42891
13.85 7.42766
13.9 7.42478
13.95 7.42519
14 7.40928
14.05 7.39836
14.1 7.39562
14.15 7.39936
14.2 7.41031
14.25 7.4244
14.3 7.42081
14.35 7.42441
14.4 7.42084
14.45 7.4252
14.5 7.43269
14.55 7.43777
14.6 7.4436
14.65 7.45328
14.7 7.45125
14.75 7.45155
14.8 7.44953
14.85 7.44214
14.9 7.4532
14.95 7.45426
e
1 0
1.05 0.04352
1.1 0.135011
1.15 0.268216
1.2 0.45696
1.25 0.758554
1.3 0.993009
1.35 1.21856
1.4 1.3872
1.45 1.48906
1.5 1.59174
1.55 1.74677
1.6 1.87068
1.65 1.96631
1.7 2.06304
1.75 2.11507
1.8 2.07536
1.85 2.11807
1.9 2.16455
1.95 2.20864
2 2.37048
2.05 2.50771
2.1 2.64928
2.15 2.77364
2.2 2.87158
2.25 2.97532
2.3 3.00004
2.35 3.09177
2.4 3.17492
2.45 3.21737
2.5 3.24899
2.55 3.30624
2.6 3.32614
2.65 3.41858
2.7 3.46951
2.75 3.56844
2.8 3.6312
2.85 3.65969
2.9 3.65568
2.95 3.64406
3 3.64045
3.05 3.6407
3.1 3.65937
3.15 3.69557
3.2 3.76992
3.25 3.78925
3.3 3.84608
3.35 3.90803
3.4 3.9312
3.45 3.98019
3.5 4.0441
3.55 4.04828
3.6 4.09043
3.65 4.12829
3.7 4.18057
3.75 4.21622
3.8 4.2359
3.85 4.24617
3.9 4.2591
3.95 4.25725
4 4.26401
4.05 4.29035
4.1 4.32162
4.15 4.34112
4.2 4.38464
4.25 4.38413
4.3 4.38894
4.35 4.40415
4.4 4.39305
4.45 4.36936
4.5 4.36905
4.55 4.37878
4.6 4.40321
4.65 4.43693
4.7 4.4821
4.75 4.5215
4.8 4.57436
4.85 4.61671
4.9 4.65586
4.95 4.68499
5 4.72268
5.05 4.72795
5.1 4.74208
5.15 4.74928
5.2 4.77391
5.25 4.78502
5.3 4.81963
5.35 4.82157
5.4 4.83404
5.45 4.8421
5.5 4.87909
5.55 4.89688
5.6 4.90416
5.65 4.90727
5.7 4.91432
5.75 4.93119
5.8 4.94974
5.85 4.97774
5.9 4.98203
5.95 4.9728
6 4.98277
6.05 4.95858
6.1 4.94228
6.15 4.9504
6.2 4.9676
6.25 4.99366
6.3 5.02475
6.35 5.03735
6.4 5.07119
6.45 5.07615
6.5 5.07929
6.55 5.06493
6.6 5.06637
6.65 5.06951
6.7 5.07942
6.75 5.09934
6.8 5.10384
6.85 5.10328
6.9 5.10934
6.95 5.11368
7 5.12448
7.05 5.13189
7.1 5.15206
7.15 5.16557
7.2 5.18047
7.25 5.20147
7.3 5.22218
7.35 5.2395
7.4 5.26122
7.45 5.28724
7.5 5.30074
7.55 5.31254
7.6 5.31817
7.65 5.33419
7.7 5.34109
7.75 5.35528
7.8 5.35903
7.85 5.35982
7.9 5.38229
7.95 5.40736
8 5.41498
8.05 5.42959
8.1 5.43839
8.15 5.45408
8.2 5.46959
8.25 5.4766
8.3 5.49866
8.35 5.51772
8.4 5.52976
8.45 5.54706
8.5 5.55206
8.55 5.5677
8.6 5.57119
8.65 5.56937
8.7 5.56099
8.75 5.55141
8.8 5.53934
8.85 5.52612
8.9 5.51689
8.95 5.50905
9 5.49748
9.05 5.49993
9.1 5.51616
9.15 5.51973
9.2 5.51456
9.25 5.52428
9.3 5.53266
9.35 5.55194
9.4 5.56738
9.45 5.57056
9.5 5.57852
9.55 5.5852
9.6 5.57515
9.65 5.56757
9.7 5.58127
9.75 5.59366
9.8 5.5931
9.85 5.60182
9.9 5.61276
9.95 5.6167
10 5.6286
10.05 5.65175
10.1 5.66563
10.15 5.67598
10.2 5.67728
10.25 5.66185
10.3 5.64434
10.35 5.64136
10.4 5.63621
10.45 5.62673
10.5 5.62714
10.55 5.62754
10.6 5.63225
10.65 5.64228
10.7 5.64901
10.75 5.65355
10.8 5.65911
10.85 5.67094
10.9 5.67427
10.95 5.69426
11 5.70681
11.05 5.72647
11.1 5.74185
11.15 5.74991
11.2 5.76912
11.25 5.77293
11.3 5.78176
11.35 5.78346
11.4 5.79117
11.45 5.8008
11.5 5.80836
11.55 5.80695
11.6 5.80063
11.65 5.79143
11.7 5.78425
11.75 5.77714
11.8 5.76718
11.85 5.75827
11.9 5.75328
11.95 5.75215
12 5.74437
12.05 5.74139
12.1 5.73844
12.15 5.74115
12.2 5.75133
12.25 5.75956
12.3 5.76958
12.35 5.7786
12.4 5.78386
12.45 5.79458
12.5 5.80613
12.55 5.79757
12.6 5.79451
12.65 5.80321
12.7 5.80825
12.75 5.824
12.8 5.82624
12.85 5.82046
12.9 5.81827
12.95 5.8161
13 5.81657
13.05 5.8153
13.1 5.8088
13.15 5.79627
13.2 5.79249
13.25 5.78873
13.3 5.79704
13.35 5.8087
13.4 5.80578
13.45 5.80458
13.5 5.80255
13.55 5.80643
13.6 5.8128
13.65 5.81662
13.7 5.81373
13.75 5.81669
13.8 5.81631
13.85 5.81263
13.9 5.80241
13.95 5.79389
14 5.78952
14.05 5.78518
14.1 5.78654
14.15 5.79919
14.2 5.80452
14.25 5.81061
14.3 5.82145
14.35 5.8139
14.4 5.81989
14.45 5.82505
14.5 5.83569
14.55 5.84234
14.6 5.8505
14.65 5.8586
14.7 5.8581
14.75 5.85838
14.8 5.85943
14.85 5.86201
14.9 5.85768
14.95 5.85107
e
1 0
1.05 0.11968
1.1 0.11424
1.15 0.109273
1.2 0.11424
1.25 0.274176
1.3 0.518474
1.35 0.820836
1.4 1.20768
1.45 1.363
1.5 1.31757
1.55 1.27507
1.6 1.36374
1.65 1.68937
1.7 1.87488
1.75 2.0759
1.8 2.2848
1.85 2.53181
1.9 2.75379
1.95 2.94681
2 3.18158
2.05 3.27674
2.1 3.38368
2.15 3.57598
2.2 3.73357
2.25 3.90955
2.3 3.89409
2.35 3.81124
2.4 3.84608
2.45 3.94944
2.5 3.87045
2.55 3.808
2.6 3.73477
2.65 3.6643
2.7 3.808
2.75 3.85093
2.8 3.8964
2.85 4.01644
2.9 4.1284
2.95 4.27141
3 4.38301
3.05 4.41978
3.1 4.45167
3.15 4.48256
3.2 4.41609
3.25 4.48875
3.3 4.46921
3.35 4.47071
3.4 4.40832
3.45 4.35437
3.5 4.35418
3.55 4.38617
3.6 4.32525
3.65 4.31608
3.7 4.27628
3.75 4.21926
3.8 4.19381
3.85 4.15121
3.9 4.11264
3.95 4.19651
4 4.26686
4.05 4.35522
4.1 4.43586
4.15 4.5173
4.2 4.60496
4.25 4.69325
4.3 4.75823
4.35 4.80071
4.4 4.83703
4.45 4.88536
4.5 4.93771
4.55 4.92111
4.6 4.95206
4.65 5.01182
4.7 4.97795
4.75 4.93036
4.8 4.88376
4.85 4.83341
4.9 4.78409
4.95 4.73577
5 4.69069
5.05 4.64425
5.1 4.59872
5.15 4.55407
5.2 4.51028
5.25 4.46733
5.3 4.47907
5.35 4.43721
5.4 4.39612
5.45 4.36627
5.5 4.32658
5.55 4.33289
5.6 4.2942
5.65 4.26226
5.7 4.22488
5.75 4.18814
5.8 4.15203
5.85 4.1185
5.9 4.17654
5.95 4.17984
6 4.21546
6.05 4.27314
6.1 4.30741
6.15 4.27239
6.2 4.23794
6.25 4.25338
6.3 4.31029
6.35 4.35551
6.4 4.38753
6.45 4.37123
6.5 4.3376
6.55 4.30449
6.6 4.27188
6.65 4.28443
6.7 4.34112
6.75 4.39866
6.8 4.44696
6.85 4.49956
6.9 4.51496
6.95 4.51371
7 4.56307
7.05 4.56474
7.1 4.61143
7.15 4.62552
7.2 4.60927
7.25 4.57748
7.3 4.54613
7.35 4.5152
7.4 4.48469
7.45 4.45459
7.5 4.45384
7.55 4.49394
7.6 4.54104
7.65 4.57856
7.7 4.62746
7.75 4.67131
7.8 4.72192
7.85 4.76315
7.9 4.80531
7.95 4.79664
8 4.83664
8.05 4.88181
8.1 4.92501
8.15 4.94106
8.2 4.91093
8.25 4.88116
8.3 4.85176
8.35 4.82818
8.4 4.85656
8.45 4.86027
8.5 4.86125
8.55 4.83683
8.6 4.81668
8.65 4.82185
8.7 4.83353
8.75 4.81897
8.8 4.8539
8.85 4.88973
8.9 4.90719
8.95 4.94104
9 4.95548
9.05 4.9281
9.1 4.90102
9.15 4.87424
9.2 4.84899
9.25 4.82278
9.3 4.79685
9.35 4.7712
9.4 4.74582
9.45 4.72071
9.5 4.7103
9.55 4.74066
9.6 4.77309
9.65 4.80637
9.7 4.79926
9.75 4.78402
9.8 4.79925
9.85 4.83055
9.9 4.86501
9.95 4.90141
10 4.93174
10.05 4.90948
10.1 4.88517
10.15 4.86111
10.2 4.83728
10.25 4.85492
10.3 4.88681
10.35 4.89963
10.4 4.93209
10.45 4.96753
10.5 4.98522
10.55 4.97567
10.6 4.9522
10.65 4.97078
10.7 4.99667
10.75 5.02443
10.8 5.05618
10.85 5.05604
10.9 5.0339
10.95 5.06099
11 5.06602
11.05 5.04414
11.1 5.02141
11.15 4.9989
11.2 4.97658
11.25 4.95446
11.3 4.95175
11.35 4.93295
11.4 4.91132
11.45 4.89985
11.5 4.9302
11.55 4.95436
11.6 4.98421
11.65 5.01283
11.7 5.04023
11.75 5.0392
11.8 5.02075
11.85 5.03331
11.9 5.01408
11.95 5.01222
12 4.99229
12.05 4.97157
12.1 4.95103
12.15 4.96262
12.2 4.9582
12.25 4.98553
12.3 5.01356
12.35 5.04136
12.4 5.07078
12.45 5.09446
12.5 5.12069
12.55 5.1185
12.6 5.09819
12.65 5.08707
12.7 5.09492
12.75 5.11437
12.8 5.13723
12.85 5.15636
12.9 5.17888
12.95 5.16859
13 5.15047
13.05 5.13161
13.1 5.12249
13.15 5.10301
13.2 5.08714
13.25 5.06795
13.3 5.08755
13.35 5.07448
13.4 5.05555
13.45 5.0427
13.5 5.02656
13.55 5.02403
13.6 5.01144
13.65 5.00647
13.7 5.01655
13.75 4.99831
13.8 4.99593
13.85 4.97789
13.9 4.96081
13.95 4.97906
14 5.00208
14.05 5.02168
14.1 5.04276
14.15 5.04836
14.2 5.06035
14.25 5.08508
14.3 5.10165
14.35 5.12368
14.4 5.14556
14.45 5.16966
14.5 5.18728
14.55 5.20636
14.6 5.22609
14.65 5.22307
14.7 5.2053
14.75 5.18766
14.8 5.19869
14.85 5.21811
14.9 5.24047
14.95 5.26039
e
1 0
1.05 0.07616
1.1 0.405033
1.15 0.854317
1.2 1.19952
1.25 1.16068
1.3 1.11604
1.35 1.0747
1.4 1.03632
1.45 1.12664
1.5 1.44704
1.55 1.75414
1.6 1.92066
1.65 1.86246
1.7 1.89504
1.75 1.86048
1.8 1.84688
1.85 1.79696
1.9 1.74968
1.95 1.71653
2 1.67362
2.05 1.71081
2.1 1.7408
2.15 1.70032
2.2 1.66167
2.25 1.62475
2.3 1.7583
2.35 1.97368
2.4 2.02776
2.45 2.03301
2.5 2.21169
2.55 2.38336
2.6 2.56601
2.65 2.71589
2.7 2.6656
2.75 2.70437
2.8 2.754
2.85 2.70568
2.9 2.65903
2.95 2.61397
3 2.5704
3.05 2.58819
3.1 2.62752
3.15 2.65109
3.2 2.77746
3.25 2.74879
3.3 2.82484
3.35 2.87817
3.4 3.00384
3.45 3.12256
3.5 3.16282
3.55 3.18263
3.6 3.29075
3.65 3.33956
3.7 3.43029
3.75 3.52773
3.8 3.60758
3.85 3.69722
3.9 3.77871
3.95 3.73088
4 3.68995
4.05 3.6444
4.1 3.59995
4.15 3.55658
4.2 3.51424
4.25 3.4729
4.3 3.43251
4.35 3.40882
4.4 3.38566
4.45 3.34762
4.5 3.3155
4.55 3.30919
4.6 3.29309
4.65 3.25768
4.7 3.26921
4.75 3.331
4.8 3.39864
4.85 3.48844
4.9 3.56709
4.95 3.64645
5 3.71508
5.05 3.78236
5.1 3.85728
5.15 3.93074
5.2 4.00499
5.25 4.06912
5.3 4.08031
5.35 4.14467
5.4 4.21207
5.45 4.25937
5.5 4.32035
5.55 4.33289
5.6 4.3962
5.65 4.45233
5.7 4.51549
5.75 4.57953
5.8 4.64248
5.85 4.68677
5.9 4.64899
5.95 4.65792
6 4.63243
6.05 4.59604
6.1 4.57522
6.15 4.62718
6.2 4.68384
6.25 4.67196
6.3 4.63488
6.35 4.60378
6.4 4.5821
6.45 4.61211
6.5 4.66451
6.55 4.71262
6.6 4.76346
6.65 4.75857
6.7 4.72306
6.75 4.68807
6.8 4.6536
6.85 4.61963
6.9 4.60934
6.95 4.62056
7 4.59082
7.05 4.59877
7.1 4.56638
7.15 4.55362
7.2 4.56325
7.25 4.60742
7.3 4.65254
7.35 4.70016
7.4 4.74559
7.45 4.78735
7.5 4.79046
7.55 4.75874
7.6 4.72743
7.65 4.704
7.7 4.67345
7.75 4.6433
7.8 4.61354
7.85 4.58415
7.9 4.55659
7.95 4.57247
8 4.5439
8.05 4.51567
8.1 4.4878
8.15 4.47849
8.2 4.51527
8.25 4.55714
8.3 4.59437
8.35 4.63253
8.4 4.61312
8.45 4.61962
8.5 4.6247
8.55 4.65645
8.6 4.68517
8.65 4.68978
8.7 4.68778
8.75 4.71713
8.8 4.69033
8.85 4.66383
8.9 4.65688
8.95 4.63087
9 4.62418
9.05 4.65796
9.1 4.69137
9.15 4.73191
9.2 4.76331
9.25 4.80302
9.3 4.84107
9.35 4.8775
9.4 4.90989
9.45 4.94315
9.5 4.94599
9.55 4.9201
9.6 4.89447
9.65 4.86911
9.7 4.88641
9.75 4.91056
9.8 4.90183
9.85 4.87695
9.9 4.85232
9.95 4.82793
10 4.80493
10.05 4.82536
10.1 4.8569
10.15 4.88925
10.2 4.92016
10.25 4.90842
10.3 4.88459
10.35 4.87755
10.4 4.8541
10.45 4.83088
10.5 4.81984
10.55 4.8349
10.6 4.86921
10.65 4.85064
10.7 4.83011
10.75 4.80764
10.8 4.78539
10.85 4.7886
10.9 4.81695
10.95 4.79808
11 4.79704
11.05 4.83013
11.1 4.86395
11.15 4.88722
11.2 4.91844
11.25 4.94532
11.3 4.95478
11.35 4.96617
11.4 4.99549
11.45 5.0076
11.5 4.98583
11.55 4.96524
11.6 4.94383
11.65 4.92262
11.7 4.90353
11.75 4.90794
11.8 4.93168
11.85 4.92341
11.9 4.94496
11.95 4.95582
12 4.97991
12.05 5.00665
12.1 5.036
12.15 5.03032
12.2 5.03592
12.25 5.01537
12.3 4.99498
12.35 4.97476
12.4 4.9547
12.45 4.9348
12.5 4.91506
12.55 4.92006
12.6 4.94768
12.65 4.95702
12.7 4.951
12.75 4.93338
12.8 4.9141
12.85 4.89587
12.9 4.8769
12.95 4.87836
13 4.89738
13.05 4.91801
13.1 4.92802
13.15 4.95359
13.2 4.97204
13.25 4.99638
13.3 4.98018
13.35 4.99832
13.4 5.02059
13.45 5.03335
13.5 5.05025
13.55 5.05523
13.6 5.0736
13.65 5.0818
13.7 5.07743
13.75 5.09718
13.8 5.09941
13.85 5.12389
13.9 5.14737
13.95 5.1363
14 5.11958
14.05 5.10218
14.1 5.08409
14.15 5.08065
14.2 5.07403
14.25 5.05622
14.3 5.03854
14.35 5.02099
14.4 5.00355
14.45 4.98624
14.5 4.9722
14.55 4.95511
14.6 4.93892
14.65 4.9361
14.7 4.95506
14.75 4.97622
14.8 4.96712
14.85 4.9504
14.9 4.93379
14.95 4.91729
e
