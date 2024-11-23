set terminal pdf
set output "hidden_node_problem.pdf"
set xlabel "Time (s)" font "Helvetica,12" offset 0,0.5
set ylabel "Throughput (Mbps)" font "Helvetica,12" offset 1,0
set key right center box 3
set key font "Helvetica,12"
set tics font "Helvetica,10"
set key width -1
set key height 1
set xrange [1:12]
set grid
set key offset -0.5,-4
set key spacing 1.2
plot "-"  smooth cspline title "Node A - base case" with lines lw 2, \
	 "-"  smooth cspline title "Node C - base case" with lines lw 2, \
	 "-"  smooth cspline title "Node A - with IRS" with lines lw 2, \
	 "-"  smooth cspline title "Node C - with IRS" with lines lw 2, \
	 "-"  smooth cspline title "Node A - RTS/CTS" with lines lw 2, \
	 "-"  smooth cspline title "Node C - RTS/CTS" with lines lw 2
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
1.25 1.03273
1.3 1.30937
1.35 1.39627
1.4 1.51776
1.45 1.85148
1.5 2.05632
1.55 2.42484
1.6 2.69178
1.65 2.76945
1.7 2.76864
1.75 2.78746
1.8 2.82427
1.85 2.91466
1.9 3.01834
1.95 3.1167
2 3.30154
2.05 3.38262
2.1 3.47616
2.15 3.62911
2.2 3.7128
2.25 3.7623
2.3 3.77489
2.35 3.83069
2.4 3.82228
2.45 3.86085
2.5 3.85217
2.55 3.9424
2.6 3.90174
2.65 3.9402
2.7 4.02802
2.75 4.07525
2.8 4.1412
2.85 4.30905
2.9 4.31354
2.95 4.39921
3 4.51248
3.05 4.58833
3.1 4.63962
3.15 4.6784
3.2 4.72668
3.25 4.72426
3.3 4.76346
3.35 4.8049
3.4 4.81824
3.45 4.8908
3.5 4.89926
3.55 4.89462
3.6 4.89328
3.65 4.87946
3.7 4.87218
3.75 4.97782
3.8 5.08368
3.85 5.12151
3.9 5.16716
3.95 5.22612
4 5.26361
4.05 5.33402
4.1 5.35535
4.15 5.38167
4.2 5.4128
4.25 5.40557
4.3 5.38522
4.35 5.37847
4.4 5.34591
4.45 5.35516
4.5 5.34135
4.55 5.37807
4.6 5.40653
4.65 5.41964
4.7 5.46651
4.75 5.49555
4.8 5.5573
4.85 5.59187
4.9 5.58144
4.95 5.58045
5 5.57948
5.05 5.56043
5.1 5.57088
5.15 5.59887
5.2 5.61753
5.25 5.65107
5.3 5.68829
5.35 5.71627
5.4 5.73527
5.45 5.78117
5.5 5.81585
5.55 5.85197
5.6 5.88336
5.65 5.90611
5.7 5.92244
5.75 5.9524
5.8 5.96018
5.85 5.95415
5.9 5.96178
5.95 5.94624
6 5.96142
6.05 6.00657
6.1 6.01914
6.15 6.04079
6.2 6.04735
6.25 6.08488
6.3 6.11637
6.35 6.14377
6.4 6.18681
6.45 6.19021
6.5 6.17072
6.55 6.1585
6.6 6.16896
6.65 6.15522
6.7 6.16896
6.75 6.14696
6.8 6.13872
6.85 6.12393
6.9 6.10439
6.95 6.09335
7 6.09552
7.05 6.08308
7.1 6.05955
7.15 6.03475
7.2 6.05789
7.25 6.07127
7.3 6.07506
7.35 6.10679
7.4 6.11184
7.45 6.12296
7.5 6.14916
7.55 6.18712
7.6 6.17648
7.65 6.19435
7.7 6.2194
7.75 6.21908
7.8 6.21583
7.85 6.20971
7.9 6.21957
7.95 6.22788
8 6.25464
8.05 6.28249
8.1 6.31564
8.15 6.35259
8.2 6.35982
8.25 6.40298
8.3 6.4112
8.35 6.41523
8.4 6.392
8.45 6.39068
8.5 6.37594
8.55 6.37339
8.6 6.36822
8.65 6.33405
8.7 6.30815
8.75 6.31388
8.8 6.30397
8.85 6.29675
8.9 6.28833
8.95 6.30298
9 6.30097
9.05 6.28888
9.1 6.2832
9.15 6.28258
9.2 6.27078
9.25 6.27023
9.3 6.2918
9.35 6.31069
9.4 6.30508
9.45 6.29831
9.5 6.29282
9.55 6.28499
9.6 6.27963
9.65 6.27195
9.7 6.26789
9.75 6.27324
9.8 6.28203
9.85 6.30814
9.9 6.31436
9.95 6.32511
10 6.32547
10.05 6.3406
10.1 6.34315
10.15 6.35017
10.2 6.34256
10.25 6.36958
10.3 6.37082
10.35 6.38199
10.4 6.40513
10.45 6.4193
10.5 6.43443
10.55 6.44184
10.6 6.46534
10.65 6.48433
10.7 6.47965
10.75 6.51115
10.8 6.52014
10.85 6.55011
10.9 6.57561
10.95 6.59045
11 6.61138
11.05 6.62282
11.1 6.62386
11.15 6.61465
11.2 6.61572
11.25 6.62694
11.3 6.63906
11.35 6.65209
11.4 6.66901
11.45 6.69875
11.5 6.70241
11.55 6.70999
11.6 6.7116
11.65 6.72986
11.7 6.73918
11.75 6.76495
11.8 6.78179
11.85 6.77342
11.9 6.76704
11.95 6.76454
12 6.77062
12.05 6.76718
12.1 6.75338
12.15 6.75567
12.2 6.76732
12.25 6.78166
12.3 6.79124
12.35 6.8063
12.4 6.81571
12.45 6.83421
12.5 6.8279
12.55 6.83437
12.6 6.82629
12.65 6.83905
12.7 6.84001
12.75 6.83469
12.8 6.83744
12.85 6.8464
12.9 6.85971
12.95 6.86146
13 6.86934
13.05 6.87541
13.1 6.87446
13.15 6.88828
13.2 6.89854
13.25 6.90872
13.3 6.9214
13.35 6.9297
13.4 6.94818
13.45 6.95972
13.5 6.9551
13.55 6.95473
13.6 6.9804
13.65 6.98329
13.7 6.98115
13.75 6.97321
13.8 6.96119
13.85 6.95008
13.9 6.95056
13.95 6.96168
14 6.97027
14.05 6.97962
14.1 6.98727
14.15 6.99246
14.2 7.00726
14.25 7.02275
14.3 7.03095
14.35 7.04148
14.4 7.04718
14.45 7.05363
14.5 7.0687
14.55 7.09073
14.6 7.10009
14.65 7.10705
14.7 7.11008
14.75 7.11696
14.8 7.11298
14.85 7.10981
14.9 7.10971
14.95 7.11192
e
1 0
1.05 0.04352
1.1 0.135011
1.15 0.268216
1.2 0.45696
1.25 0.767693
1.3 1.06331
1.35 1.2101
1.4 1.3872
1.45 1.5836
1.5 1.78214
1.55 1.85732
1.6 1.90638
1.65 1.95247
1.7 1.99584
1.75 2.06285
1.8 2.10075
1.85 2.19835
1.9 2.2848
1.95 2.39025
2 2.45045
2.05 2.56901
2.1 2.65472
2.15 2.76833
2.2 2.80927
2.25 2.85854
2.3 2.9007
2.35 2.89246
2.4 2.9036
2.45 2.91429
2.5 2.97024
2.55 3.00608
2.6 3.03615
2.65 3.0737
2.7 3.10987
2.75 3.18626
2.8 3.28848
2.85 3.331
2.9 3.39962
2.95 3.45044
3 3.52621
3.05 3.54706
3.1 3.58566
3.15 3.60491
3.2 3.63783
3.25 3.69786
3.3 3.71453
3.35 3.71706
3.4 3.7464
3.45 3.7484
3.5 3.75686
3.55 3.749
3.6 3.74136
3.65 3.74332
3.7 3.81315
3.75 3.87197
3.8 3.89318
3.85 3.94647
3.9 3.97497
3.95 4.03455
4 4.10978
4.05 4.12956
4.1 4.14886
4.15 4.17595
4.2 4.1616
4.25 4.16102
4.3 4.16046
4.35 4.15203
4.4 4.15418
4.45 4.13574
4.5 4.15072
4.55 4.16537
4.6 4.19459
4.65 4.23057
4.7 4.2682
4.75 4.31947
4.8 4.32446
4.85 4.31285
4.9 4.29915
4.95 4.30189
5 4.30685
5.05 4.30493
5.1 4.3232
5.15 4.32559
5.2 4.34112
5.25 4.36941
5.3 4.42303
5.35 4.45002
5.4 4.48921
5.45 4.52977
5.5 4.54883
5.55 4.57783
5.6 4.59816
5.65 4.62621
5.7 4.63975
5.75 4.67689
5.8 4.68187
5.85 4.69263
5.9 4.67997
5.95 4.68096
6 4.70098
6.05 4.71122
6.1 4.72691
6.15 4.72935
6.2 4.74649
6.25 4.75238
6.3 4.76
6.35 4.79088
6.4 4.7838
6.45 4.77151
6.5 4.7682
6.55 4.78413
6.6 4.78943
6.65 4.78605
6.7 4.77932
6.75 4.77439
6.8 4.76616
6.85 4.75472
6.9 4.74841
6.95 4.74055
7 4.7279
7.05 4.71868
7.1 4.71119
7.15 4.72139
7.2 4.7362
7.25 4.74451
7.3 4.76052
7.35 4.77787
7.4 4.78573
7.45 4.79655
7.5 4.80113
7.55 4.8238
7.6 4.8537
7.65 4.87125
7.7 4.86633
7.75 4.86441
7.8 4.85374
7.85 4.84902
7.9 4.85737
7.95 4.86993
8 4.87948
8.05 4.89174
8.1 4.90527
8.15 4.92003
8.2 4.95133
8.25 4.95317
8.3 4.96187
8.35 4.96636
8.4 4.97488
8.45 4.96707
8.5 4.96742
8.55 4.9651
8.6 4.95616
8.65 4.9526
8.7 4.9609
8.75 4.95867
8.8 4.95127
8.85 4.94911
8.9 4.96751
8.95 4.96402
9 4.96055
9.05 4.95587
9.1 4.95249
9.15 4.95165
9.2 4.95578
9.25 4.96728
9.3 4.97005
9.35 4.96913
9.4 4.97066
9.45 4.96128
9.5 4.952
9.55 4.95
9.6 4.9504
9.65 4.95079
9.7 4.95119
9.75 4.95274
9.8 4.97061
9.85 4.97901
9.9 4.9931
9.95 5.00475
10 5.02656
10.05 5.03111
10.1 5.02769
10.15 5.01418
10.2 5.01648
10.25 5.0065
10.3 5.01103
10.35 5.03208
10.4 5.04414
10.45 5.06482
10.5 5.06899
10.55 5.0872
10.6 5.09338
10.65 5.10379
10.7 5.12265
10.75 5.1137
10.8 5.12916
10.85 5.12659
10.9 5.13766
10.95 5.14236
11 5.1408
11.05 5.12891
11.1 5.11816
11.15 5.1116
11.2 5.11122
11.25 5.11795
11.3 5.12867
11.35 5.14432
11.4 5.15383
11.45 5.15327
11.5 5.16762
11.55 5.16503
11.6 5.16936
11.65 5.17757
11.7 5.18376
11.75 5.18407
11.8 5.18146
11.85 5.17406
11.9 5.16864
11.95 5.17474
12 5.17412
12.05 5.17256
12.1 5.17196
12.15 5.17794
12.2 5.18762
12.25 5.20282
12.3 5.20767
12.35 5.21526
12.4 5.22464
12.45 5.21925
12.5 5.21666
12.55 5.20953
12.6 5.21152
12.65 5.21079
12.7 5.21546
12.75 5.22816
12.8 5.24344
12.85 5.24971
12.9 5.25681
12.95 5.26915
13 5.27349
13.05 5.28043
13.1 5.28818
13.15 5.30195
13.2 5.31822
13.25 5.32574
13.3 5.33149
13.35 5.34318
13.4 5.34882
13.45 5.35272
13.5 5.35405
13.55 5.35958
13.6 5.36172
13.65 5.35882
13.7 5.35594
13.75 5.35308
13.8 5.34776
13.85 5.3499
13.9 5.35284
13.95 5.35659
14 5.36438
14.05 5.37619
14.1 5.38873
14.15 5.39471
14.2 5.40468
14.25 5.41377
14.3 5.4268
14.35 5.43018
14.4 5.43671
14.45 5.43925
14.5 5.44728
14.55 5.4474
14.6 5.45692
14.65 5.47338
14.7 5.4773
14.75 5.47965
14.8 5.47657
14.85 5.4766
14.9 5.48045
14.95 5.47435
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
