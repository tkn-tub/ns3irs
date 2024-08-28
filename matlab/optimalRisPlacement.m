% Static AP/UE moving RIS on x axes + calculating optimal phase shift to
% find optimal placement

fc = 5.21e9;
c = physconst('lightspeed');
lambda = c/fc;
rng(2024);

% Setup surface
Nr = 20;
Nc = 20;
dr = 0.5*lambda;
dc = 0.5*lambda;
element_size = (lambda^2)/(4*pi);

txPower = -17;
noise = -94;

% construct surface
ris = helperRISSurface('Size',[Nr Nc],'ElementSpacing',[dr dc],...
    'ReflectorElement',phased.IsotropicAntennaElement,'OperatingFrequency',fc);

d_ap_ue = 15;
pos_ap = [0;0;0];
pos_ue = [d_ap_ue;0;0];

v = zeros(3,1);

step_size = 0.5;

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
% LOS path propagation

yref = chanAPToUE(xt,pos_ap,pos_ue,v,v);
RXpowerLos = pow2db(bandpower(yref)) - txPower;
% Loop through different d_rx values
for i = 1:length(x_ris_range)
    pos_ris = [x_ris_range(i); -1; 0];

    % compute the range and angle of the RIS from the base station and the UE
    [r_ap_ris,ang_ap_ris,r_ue_ris,ang_ue_ris] = calcangle(pos_ap, pos_ue, pos_ris, [0,1,0]);

    % Calculate steering vectors for input and output angles
    g = stv(fc, ang_ap_ris);
    hr = stv(fc, ang_ue_ris);

    direct_path_phase = (2 * pi * d_ap_ue) / lambda;
    reflected_path_phase = (2 * pi * (r_ap_ris + r_ue_ris)) / lambda;
    required_phase_shift =  reflected_path_phase - direct_path_phase;
    rcoeff_ris = exp(1i * (wrapToPi(required_phase_shift) - angle(hr) - angle(g)));

    x_ris_in = chanAPToRIS(xt,pos_ap,pos_ris,v,v);
    x_ris_out = ris(x_ris_in,ang_ap_ris,ang_ue_ris,rcoeff_ris);
    ylosris = chanRISToUE(x_ris_out,pos_ris,pos_ue,v,v) + yref;
    RXpowerRis = pow2db(bandpower(ylosris)) - txPower;

    ylosris = chanRISToUE(x_ris_out,pos_ris,pos_ue,v,v);
    RXpowerOnlyRis = pow2db(bandpower(ylosris)) - txPower;

    risA = 1;
    RXPowerRisETSI = -pow2db(((4*pi*r_ap_ris*r_ue_ris)/(prod([Nr, Nc])*element_size*risA)).^2);
    RXPowerRisETSI = RXPowerRisETSI - txPower;
    ris_etsi_array(i) = RXPowerRisETSI;

    onlyris_array(i) = RXpowerOnlyRis;
    ris_array(i) = RXpowerRis;
    los_array(i) = RXpowerLos;

end
% % Plot the results
% figure;
% 
% hold on;
% plot(x_ris_range, ris_array-noise, 'r', 'LineWidth', 2.5);
% plot(x_ris_range, los_array-noise, 'k--', 'LineWidth', 2.5);
% plot(x_ris_range, onlyris_array-noise, 'b', 'LineWidth', 2.5);
% plot(x_ris_range, ris_etsi_array-noise, 'c--', 'LineWidth', 2.5);
% plot(optimalrisplacementns3.ris_x, optimalrisplacementns3.only_irs-noise, 'm-o', 'LineWidth', 2.5, 'MarkerSize', 8);
% plot(optimalrisplacementns3.ris_x, optimalrisplacementns3.irs_los-noise, 'g-o', 'LineWidth', 2.5, 'MarkerSize', 8);
% 
% % Increase font size for labels and title
% xlabel('x pos [m]', 'FontSize', 20, 'FontWeight', 'bold');
% ylabel('SNR [dBm]', 'FontSize', 20, 'FontWeight', 'bold');
% title(sprintf('SNR vs. Position of RIS for N = %d (%d x %d)', Nr*Nc, Nr, Nc), 'FontSize', 22, 'FontWeight', 'bold');
% 
% % Enhance legend and grid
% legend('RIS + LOS', 'LOS', 'RIS', 'RIS ETSI', 'RIS (Ns3)', 'RIS + LOS (Ns3)', 'FontSize', 22, 'Location', 'Best');
% grid on;
% 
% % Improve overall figure appearance
% set(gca, 'LineWidth', 1.5, 'FontSize', 18);
% set(gcf, 'Color', 'w');

% Ensure all arrays are column vectors
x_ris_range = x_ris_range(:);
ris_array = ris_array(:);
los_array = los_array(:);
onlyris_array = onlyris_array(:);
ris_etsi_array = ris_etsi_array(:);
optimalrisplacementns3.ris_x = optimalrisplacementns3.ris_x(:);
optimalrisplacementns3.only_irs = optimalrisplacementns3.only_irs(:);
optimalrisplacementns3.irs_los = optimalrisplacementns3.irs_los(:);


% Combine all relevant data into a matrix
data = [x_ris_range, ris_array - noise, los_array - noise, onlyris_array - noise, ris_etsi_array - noise, ...
        optimalrisplacementns3.ris_x, optimalrisplacementns3.only_irs - noise, optimalrisplacementns3.irs_los - noise];

% Write the data to a CSV file or a text file
csvwrite('plot_data.csv', data);
