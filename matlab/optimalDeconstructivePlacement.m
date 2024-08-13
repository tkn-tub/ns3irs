function fun = calcDiff(ris)
c = physconst('lightspeed');
fc = 5.21e9;
lambda = c/fc;

ap = [0;0;0];
ue = [40;0;0];

max_power = pow2db(400^2);
txPower = 16.0206;
plLos = 78.7189;

f = @(d1, d2) (abs(abs(txPower - pow2db(((4 * pi * d1) / lambda)^2) + max_power - pow2db(((4 * pi * d2) / lambda)^2)) - plLos));


r_ap_ris = sqrt((ris(1) - ap(1))^2 + (ris(2) - ap(2))^2);
r_ris_ue = sqrt((ris(1) - ue(1))^2 + (ris(2) - ue(2))^2);

fun = f(r_ap_ris, r_ris_ue);
end

ris0 = [8, 0.5];

ris_opt = fminsearch(@(ris) calcDiff(ris), ris0);

disp(['Optimized Point: (', num2str(ris_opt(1)), ', ', num2str(ris_opt(2)), ')']);
