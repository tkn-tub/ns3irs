% Generate the lookup table
Nr = 20;
Nc = 20;
freq = 5.15e9;

ap = [0;0;0];
ue = [50;0;0];
ris1 = [0.7;0.7;0];
ris2 = [49.3;-0.7;0];

[r_ap_ris1,a_ap_ris1,~,a_ris1_ris2] = calcangle(ap, ris2, ris1, [0,-1,0])
[r_ris1_ris2,a_ris2_ris1,r_ris2_ue,a_ris2_ue] = calcangle(ris1, ue, ris2, [0,1,0])

ris_table1 = generateIrsLookupTable(round(a_ap_ris1(1)), round(a_ris1_ris2(1)), Nr, Nc, freq, abs(ue(1)-ap(1)), r_ap_ris1 + r_ris1_ris2 + r_ris2_ue, 0, 0, "multiIrs1", 1, 2);
ris_table2 = generateIrsLookupTable(round(a_ris2_ris1(1)), round(a_ris2_ue(1)), Nr, Nc, freq, abs(ue(1)-ap(1)), r_ap_ris1 + r_ris1_ris2 + r_ris2_ue, 0, 0, "multiIrs2", 1, 2);
