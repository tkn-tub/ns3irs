fc = 5.21e9;
c = physconst('lightspeed');
lambda = c/fc;
rng(2024);
fs = 10e6;

% Setup surface
Nr = 25;
Nc = 20;
dr = 0.5*lambda;
dc = 0.5*lambda;

txPower = -17;
noise = -93.966;

% construct surface
ris = helperRISSurface('Size',[Nr Nc],'ElementSpacing',[dr dc],...
    'ReflectorElement',phased.IsotropicAntennaElement,'OperatingFrequency',fc);

d_ap_ue = 30;
pos_ap = [0;0;0];
pos_ue = [d_ap_ue;0;0];

v = zeros(3,1);

step_size = lambda

x_ris_range = 0:step_size:d_ap_ue;

% Initialize arrays to store the results
onlyris_array = zeros(size(x_ris_range));
ris_array = zeros(size(x_ris_range));
los_array = zeros(size(x_ris_range));
ris_etsi_array = zeros(size(x_ris_range));

% signal
fs = 10e6;
xt = ones(1e4, 1);

% channel
chanAPToRIS = phased.FreeSpace('SampleRate',fs,'PropagationSpeed',c,'MaximumDistanceSource','Property','MaximumDistance',500,'OperatingFrequency',fc);
chanRISToUE = phased.FreeSpace('SampleRate',fs,'PropagationSpeed',c,'MaximumDistanceSource','Property','MaximumDistance',500,'OperatingFrequency',fc);
chanAPToUE = phased.FreeSpace('SampleRate',fs,'PropagationSpeed',c,'MaximumDistanceSource','Property','MaximumDistance',500,'OperatingFrequency',fc);


stv = getSteeringVector(ris);
% Loop through different d_rx values
for i = 1:length(x_ris_range) 
    pos_ris = [x_ris_range(i); -1; 0];
    
    % compute the range and angle of the RIS from the base station and the UE
    [r_ap_ris,ang_ap_ris] = rangeangle(pos_ap,pos_ris);
    [r_ue_ris,ang_ue_ris] = rangeangle(pos_ue,pos_ris);
 
    g = stv(fc, ang_ap_ris);
    hr = stv(fc, ang_ue_ris);
    rcoeff_ris = exp(1i*(-angle(hr)-angle(g)));

    x_ris_in = chanAPToRIS(xt,pos_ap,pos_ris,v,v);
    x_ris_out = ris(x_ris_in,ang_ap_ris,ang_ue_ris,rcoeff_ris);
    ylosris = chanRISToUE(x_ris_out,pos_ris,pos_ue,v,v) + chanAPToUE(xt,pos_ap,pos_ue,v,v);
    RXpowerRis = pow2db(bandpower(ylosris)) - txPower;
    
    ylosris = chanRISToUE(x_ris_out,pos_ris,pos_ue,v,v);
    RXpowerOnlyRis = pow2db(bandpower(ylosris)) - txPower;
    
    % LOS path propagation
    yref = chanAPToUE(xt,pos_ap,pos_ue,v,v);
    RXpowerLos = pow2db(bandpower(yref)) - txPower;

    risA = 1;
    RXPowerRisETSI = -pow2db(((4*pi*r_ap_ris*r_ue_ris)/(prod([Nr, Nc])*dr*dc*risA)).^2);
    RXPowerRisETSI = RXPowerRisETSI - txPower;
    ris_etsi_array(i) = RXPowerRisETSI;

    onlyris_array(i) = RXpowerOnlyRis;
    ris_array(i) = RXpowerRis;
    los_array(i) = RXpowerLos;

end
difference = onlyris_array - ris_etsi_array;
mean_diff = mean(difference) % -9.942999727847097

onlyris_2_array = onlyris_array+9.9429;

% Plot the results
figure;

hold on;
plot(x_ris_range, ris_array, 'r', 'LineWidth', 2);
plot(x_ris_range, los_array, 'g--', 'LineWidth', 2);
plot(x_ris_range, onlyris_array, 'b', 'LineWidth', 2);
plot(x_ris_range, ris_etsi_array, 'c', 'LineWidth', 2);
plot(x_ris_range, onlyris_2_array, 'm--', 'LineWidth', 2);
plot(x_ris_range, ris_array-mean_diff, 'y', 'LineWidth', 2);

difference = onlyris_array - onlyris_2_array;
max_diff = max(difference)
mean_diff = mean(difference)
xlabel('x pos [m]');
ylabel('Received Power [dB]');
title('Rx Power vs. Position of RIS');
legend('RIS + LOS', 'LOS', 'RIS', 'RIS ETSI', 'RIS + 9.94 dB', 'RIS + LOS + 9.94 dB');
grid on;