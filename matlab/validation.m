fc = 5.21e9;
c = physconst('lightspeed');
lambda = c/fc;
rng(2024);
fs = 10e6;

% Setup surface
Nr = 10;
Nc = 20;
dr = 0.5*lambda;
dc = 0.5*lambda;

% construct surface
ris = helperRISSurface('Size',[Nr Nc],'ElementSpacing',[dr dc],...
    'ReflectorElement',phased.IsotropicAntennaElement,'OperatingFrequency',fc);

% scene
pos_ap = [-7.0711;7.0711;0];
pos_ris = [0;0;0];
% pos_ue = [0.3536;0.3536;0];
d_rx = 2.5;
pos_ue = pos_ris + [d_rx*cosd(45); d_rx*sind(45); 0]

v = zeros(3,1);

% compute the range and angle of the RIS from the base station and the UE
[r_ap_ris,ang_ap_ris] = rangeangle(pos_ap,pos_ris);
[r_ue_ris,ang_ue_ris] = rangeangle(pos_ue,pos_ris);
% ang_ap_ris = 135;
% ang_ue_ris = 15;

% signal
fs = 10e6;
% x = 2*randi(2,[100 1])-3;
% tx = phased.Transmitter('PeakPower',50e-3,'Gain',0);
% xt = tx(x);
xt = ones(1e6, 1);

% channel
chanAPToRIS = phased.FreeSpace('SampleRate',fs,'PropagationSpeed',c,'MaximumDistanceSource','Property','MaximumDistance',500,'OperatingFrequency',fc);
chanRISToUE = phased.FreeSpace('SampleRate',fs,'PropagationSpeed',c,'MaximumDistanceSource','Property','MaximumDistance',500,'OperatingFrequency',fc);
chanAPToUE = phased.FreeSpace('SampleRate',fs,'PropagationSpeed',c,'MaximumDistanceSource','Property','MaximumDistance',500,'OperatingFrequency',fc);

txPower = -17;
noise = -93.966;

optimal_in_angle = 135;
optimal_out_angle = 45;

stv = getSteeringVector(ris);
g = stv(fc, optimal_in_angle);
hr = stv(fc, optimal_out_angle);
rcoeff_ris = exp(1i*(-angle(hr)-angle(g)));

% rcoeff_ris = ones(Nr*Nc,1);

% distanceApRisUe = r_ap_ris + r_ue_ris;
% posC = [pos_ue(1) + distanceApRisUe, pos_ue(2), pos_ue(3)]';

x_ris_in = chanAPToRIS(xt,pos_ap,pos_ris,v,v);
x_ris_out = ris(x_ris_in,ang_ap_ris,ang_ue_ris,rcoeff_ris);
ylosris = chanRISToUE(x_ris_out,pos_ris,pos_ue,v,v) + chanAPToUE(xt,pos_ap,pos_ue,v,v);
RXpowerRis = pow2db(bandpower(ylosris)) - noise - txPower

x_ris_in = chanAPToRIS(xt,pos_ap,pos_ris,v,v);
x_ris_out = ris(x_ris_in,ang_ap_ris,ang_ue_ris,rcoeff_ris);
ylosris = chanRISToUE(x_ris_out,pos_ris,pos_ue,v,v);
RXpowerOnlyRis = pow2db(bandpower(ylosris)) - noise - txPower

% LOS path propagation
yref = chanAPToUE(xt,pos_ap,pos_ue,v,v);
RXpowerLos = pow2db(bandpower(yref)) - noise - txPower
