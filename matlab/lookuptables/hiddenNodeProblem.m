% Generate the lookup table for the Hidden Node Problem
Nr = 25;
Nc = 25;
freq = 5.15e9;

ap = [0;0;0];
ue = [100;0;0];
ris = [0.7;-0.7;0];

[r_ap_ris,a_ap_ris,r_ris_ue,a_ris_ue] = calcangle(ap, ue, ris, [0,1,0])
[r_ap_ris,a_ap_ris,r_ris_ueB,a_ris_ueB] = calcangle(ap, [50;0;0], ris, [0,1,0])



ris_table = generateIrsLookupTable(round(a_ap_ris(1)), round(a_ris_ue(1)), Nr, Nc, freq, 50, r_ap_ris, r_ris_ueB, 0, "hidden_node");
% plotIrsLookupTable(ris_table, round(a_ap_ris(1)), round(a_ris_ue(1)));
