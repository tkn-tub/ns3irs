Nr = 20;
Nc = 20;
freq = 5.15e9;
ap = [0;0;0];
ue = [50;0;0];
ris = [0.7;-0.7;0];
[r_ap_ris, a_ap_ris, r_ris_ue, a_ris_ue] = calcangle(ap, ue, ris, [0,1,0]);

ris_table = generateIrsLookupTable( ...
    round(a_ap_ris(1)), round(a_ris_ue(1)), Nr, Nc, freq, ...
    abs(ue(1) - ap(1)), r_ap_ris, r_ris_ue, 0, "constructive" ...
);
% plotIrsLookupTable(ris_table);
