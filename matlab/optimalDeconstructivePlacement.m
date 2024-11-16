% gradient descent to find optimal IRS pos for destructive interference
% where amplitude of LoS and IRS signal are equal

function pl = logDistance(d, n)
d0 = 1; % reference distance
Pr0 = 46.6777; % reference loss at distance d0
pl = -Pr0 - (10 * n * log10(d / d0));
pl = -pl;
end

function fun = calcDiff(ris)
ap = [0;0;0];
ue = [50;0;0];
max_power = pow2db(400^2);

f = @(d1, d2) (abs(abs(-logDistance(d1, 2.5) + max_power - logDistance(d2, 2.5)) - logDistance(ue(1)-ap(1), 2.5)));

r_ap_ris = sqrt((ris(1) - ap(1))^2 + (ris(2) - ap(2))^2);
r_ris_ue = sqrt((ris(1) - ue(1))^2 + (ris(2) - ue(2))^2);

fun = f(r_ap_ris, r_ris_ue);
end

ris0 = [1, 1];

ris_opt = fminsearch(@(ris) calcDiff(ris), ris0);

disp(['Optimized Point: (', num2str(ris_opt(1)), ', ', num2str(ris_opt(2)), ')']);
