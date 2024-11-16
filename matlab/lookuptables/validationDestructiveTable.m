freq = 5.15e9;

Nr = 20;
Nc = 20;

ap = [0;0;0];
ue = [50;0;0];
ris = [1.094;-1.2683;0];
txpower = 16.0206;
[r_ap_ris,a_ap_ris,r_ris_ue,a_ris_ue] = calcangle(ap, ue, ris, [0,1,0])
% alpha = (logDistance(abs(ue(1)-ap(1)), 2, d0, Pr0) - logDistance(r_ap_ris, 2, d0, Pr0) - logDistance(r_ris_ue, 2, d0, Pr0)) / pow2db((Nr*Nc)^2)
% alpha = db2pow(logDistance(abs(ue(1)-ap(1)), 2, d0, Pr0) - logDistance(r_ap_ris, 2, d0, Pr0) - logDistance(r_ris_ue, 2, d0, Pr0)) / (Nr * Nc)^2
ris_table = generateIrsLookupTable(round(a_ap_ris(1)), round(a_ris_ue(1)), Nr, Nc, freq, abs(ue(1)-ap(1)), r_ap_ris, r_ris_ue, pi, "destructive");
plotIrsLookupTable(ris_table, round(a_ap_ris(1)), round(a_ris_ue(1)));
