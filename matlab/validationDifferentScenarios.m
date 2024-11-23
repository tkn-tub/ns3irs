fc = 5.15e9;
c = physconst('lightspeed');
lambda = c/fc;
rng(2024);

Nr = 20;
Nc = 20;
dr = 0.5*lambda;
dc = 0.5*lambda;

% values from ns3
txPower = 16.0206;
noise = -87.5;

% construct surface
ris = helperRISSurface('Size',[Nr Nc],'ElementSpacing',[dr dc],...
    'ReflectorElement',phased.IsotropicAntennaElement,'OperatingFrequency',fc);

pos_ap = [0;0;0];
pos_ue = [50;0;0];
v = zeros(3,1);

% signal
fs = 10e6;
xt = ones(1e4, 1);
% channel
exponent = 2.5;
referencePathloss = 46.6777;
referenceDistance = 1;
chanAPToRIS = LogDistancePL('SampleRate',fs,'PropagationSpeed',c,'MaximumDistanceSource','Property','MaximumDistance',500,'OperatingFrequency',fc, 'Exponent', exponent, 'ReferenceDistance', referenceDistance, 'ReferenceLoss', referencePathloss);
chanRISToUE = LogDistancePL('SampleRate',fs,'PropagationSpeed',c,'MaximumDistanceSource','Property','MaximumDistance',500,'OperatingFrequency',fc, 'Exponent', exponent, 'ReferenceDistance', referenceDistance, 'ReferenceLoss', referencePathloss);
chanAPToUE = LogDistancePL('SampleRate',fs,'PropagationSpeed',c,'MaximumDistanceSource','Property','MaximumDistance',500,'OperatingFrequency',fc, 'Exponent', exponent, 'ReferenceDistance', referenceDistance, 'ReferenceLoss', referencePathloss);
chanRISToRis = LogDistancePL('SampleRate',fs,'PropagationSpeed',c,'MaximumDistanceSource','Property','MaximumDistance',500,'OperatingFrequency',fc, 'Exponent', exponent, 'ReferenceDistance', referenceDistance, 'ReferenceLoss', referencePathloss);

stv = getSteeringVector(ris);

% Scenario 1: LOS
losSNR = txPower + pow2db(bandpower(chanAPToUE(xt, pos_ap, pos_ue, v, v))) - noise;
disp("LOS: " + losSNR);

% Scenario 2: IRS
pos_ris = [0.7; -0.7; 0];
[r_ap_ris,ang_ap_ris,r_ue_ris,ang_ue_ris] = calcangle(pos_ap, pos_ue, pos_ris, [0,1,0]);

g = stv(fc, ang_ap_ris);
hr = stv(fc, ang_ue_ris);
rcoeff_ris = exp(1i*(-angle(hr)-angle(g)));

x_ris_in = chanAPToRIS(xt,pos_ap,pos_ris,v,v);
x_ris_out = ris(x_ris_in,ang_ap_ris,ang_ue_ris,rcoeff_ris);
ylosris = chanRISToUE(x_ris_out,pos_ris,pos_ue,v,v);
risSNR = txPower + pow2db(bandpower(ylosris)) - noise;
disp("IRS: "  + risSNR);

% Scenario 3: IRS Constructive + LOS
pos_ris = [0.7; -0.7; 0];
[r_ap_ris,ang_ap_ris,r_ue_ris,ang_ue_ris] = calcangle(pos_ap, pos_ue, pos_ris, [0,1,0]);

g = stv(fc, ang_ap_ris);
hr = stv(fc, ang_ue_ris);

direct_path_phase = (2 * pi * 50) / lambda;
reflected_path_phase = (2 * pi * (r_ap_ris + r_ue_ris)) / lambda;
required_phase_shift =  reflected_path_phase - direct_path_phase;
rcoeff_ris = exp(1i * (wrapToPi(required_phase_shift) - angle(hr) - angle(g)));

x_ris_in = chanAPToRIS(xt,pos_ap,pos_ris,v,v);
x_ris_out = ris(x_ris_in,ang_ap_ris,ang_ue_ris,rcoeff_ris);
ylosris = chanRISToUE(x_ris_out,pos_ris,pos_ue,v,v) + chanAPToUE(xt, pos_ap, pos_ue, v, v);
risCSNR = txPower + pow2db(bandpower(ylosris)) - noise;
disp("IRS Constructive + LOS: "  + risCSNR);

% Scenario 4: IRS Destructive + LOS
pos_ris = [1.094; -1.2683; 0];
[r_ap_ris,ang_ap_ris,r_ue_ris,ang_ue_ris] = calcangle(pos_ap, pos_ue, pos_ris, [0,1,0]);

g = stv(fc, ang_ap_ris);
hr = stv(fc, ang_ue_ris);

direct_path_phase = (2 * pi * 50) / lambda;
reflected_path_phase = (2 * pi * (r_ap_ris + r_ue_ris)) / lambda;
required_phase_shift =  pi + reflected_path_phase - direct_path_phase;
rcoeff_ris = exp(1i * (wrapToPi(required_phase_shift) - angle(hr) - angle(g)));

x_ris_in = chanAPToRIS(xt,pos_ap,pos_ris,v,v);
x_ris_out = ris(x_ris_in,ang_ap_ris,ang_ue_ris,rcoeff_ris);
ylosris = chanRISToUE(x_ris_out,pos_ris,pos_ue,v,v) + chanAPToUE(xt, pos_ap, pos_ue, v, v);
risCSNR = txPower + pow2db(bandpower(ylosris)) - noise;
disp("IRS Destructive + LOS: "  + risCSNR);

% Scenario 4: Multi-IRS + LOS
pos_ris1 = [0.7; 0.7; 0];
pos_ris2 = [49.3; -0.7; 0];

% Calculate distances and angles for each path
[r_ap_ris1, ang_ap_ris1, r_ris1_ue, ang_ris1_ue] = calcangle(pos_ap, pos_ue, pos_ris1, [0, -1, 0]);
[r_ap_ris2, ang_ap_ris2, r_ris2_ue, ang_ris2_ue] = calcangle(pos_ap, pos_ue, pos_ris2, [0, 1, 0]);
[r_ris1_ris2,ang_ris2_ris1,~,~] = calcangle(pos_ris1, pos_ue, pos_ris2, [0,1,0]);
[~,~,~,ang_ris1_ris2] = calcangle(pos_ap, pos_ris2, pos_ris1, [0,-1,0]);

direct_path_phase = (2 * pi * 50) / lambda;
reflected_path_phase = (2 * pi * (r_ap_ris1 + r_ris1_ris2 + r_ris2_ue)) / lambda;
required_phase_shift =  (reflected_path_phase - direct_path_phase) / 2;

% Steering vector calculations
g = stv(fc, ang_ap_ris1);
hr = stv(fc, ang_ris1_ris2);

% Direct path phase
direct_path_phase = (2 * pi * 50) / lambda;  % Assuming 50 is the distance in meters
rcoeff_ris1 = exp(1i * (wrapToPi(required_phase_shift) - angle(hr) - angle(g)));

% Steering vector for RIS2
g = stv(fc, ang_ris2_ris1);
hr = stv(fc, ang_ris2_ue);

% Reflection coefficient for RIS2
rcoeff_ris2 = exp(1i * (wrapToPi(required_phase_shift) - angle(hr) - angle(g)));

% Path 1: Direct Path (AP -> UE)
path_direct = chanAPToUE(xt, pos_ap, pos_ue, v, v);

% Path 2: AP -> RIS1 -> UE
x_ris_in1 = chanAPToRIS(xt, pos_ap, pos_ris1, v, v);
x_ris_out1 = ris(x_ris_in1, ang_ap_ris1, ang_ris1_ue, rcoeff_ris1);
path_ris1 = chanRISToUE(x_ris_out1, pos_ris1, pos_ue, v, v);

% Path 3: AP -> RIS2 -> UE
x_ris_in2 = chanAPToRIS(xt, pos_ap, pos_ris2, v, v);
x_ris_out2 = ris(x_ris_in2, ang_ap_ris2, ang_ris2_ue, rcoeff_ris2);
path_ris2 = chanRISToUE(x_ris_out2, pos_ris2, pos_ue, v, v);

% Path 4: AP -> RIS1 -> RIS2 -> UE
x_ris_in1_ris2 = chanAPToRIS(xt, pos_ap, pos_ris1, v, v);
x_ris_out1_ris2 = ris(x_ris_in1_ris2, ang_ap_ris1, ang_ris1_ris2, rcoeff_ris1);
x_ris_in2_ris2 = chanRISToRis(x_ris_out1_ris2, pos_ris1, pos_ris2, v, v);
x_ris_out2_ris2 = ris(x_ris_in2_ris2, ang_ris2_ris1, ang_ris2_ue, rcoeff_ris2);
path_ris1_ris2 = chanRISToUE(x_ris_out2_ris2, pos_ris2, pos_ue, v, v);

% Path 5: AP -> RIS2 -> RIS1 -> UE
x_ris_in2_ris1 = chanAPToRIS(xt, pos_ap, pos_ris2, v, v);
x_ris_out2_ris1 = ris(x_ris_in2_ris1, ang_ap_ris2, ang_ris2_ris1, rcoeff_ris2);
x_ris_in2_ris2 = chanRISToRis(x_ris_out2_ris1, pos_ris2, pos_ris1, v, v);
x_ris_out1_ris1 = ris(x_ris_in2_ris2, ang_ris1_ris2, ang_ris1_ue, rcoeff_ris1);
path_ris2_ris1 = chanRISToUE(x_ris_out1_ris1, pos_ris1, pos_ue, v, v);

% Sum all paths
ylosris = path_direct + path_ris1 + path_ris2 + path_ris1_ris2 + path_ris2_ris1;

% debug info
% disp(['Path Direct: ', num2str(txPower + pow2db(bandpower(path_direct))), ' phase: ', num2str(mean(angle(path_direct)))]);
% disp(['Path RIS1: ', num2str(txPower + pow2db(bandpower(path_ris1)))]);
% disp(['Path RIS2: ', num2str(txPower + pow2db(bandpower(path_ris2)))]);
% disp(['Path RIS1 → RIS2: ', num2str(txPower + pow2db(bandpower(path_ris1_ris2))), ' phase: ', num2str(mean(angle(path_ris1_ris2)))]);
% disp(['Path RIS2 → RIS1: ', num2str(txPower + pow2db(bandpower(path_ris2_ris1))), ' phase: ', num2str(mean(angle(path_ris2_ris1)))]);
% disp(['Result: ', num2str(txPower + pow2db(bandpower(ylosris)))]);

risMultiSNR = txPower + pow2db(bandpower(ylosris)) - noise;
disp("MultiIRS: " + risMultiSNR);
