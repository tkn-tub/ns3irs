% plot SNR for different IRS sizes with moving UE

function validation = validation_plotChangingSizes(ris_sizes)
fc = 5.21e9;
c = physconst('lightspeed');
lambda = c/fc;
rng(2024);
fs = 10e6;
txPower = -17;

% Define UE distances to simulate (in meters)
ue_distances = 0.1:0.1:10;

% Fixed angles (in degrees)
ap_to_ris_angle = 170;
ris_to_ue_angle = 10;

% Initialize result table
results = table();

% Channel setup
chanAPToRIS = phased.FreeSpace('SampleRate',fs,'PropagationSpeed',c,'MaximumDistanceSource','Property','MaximumDistance',500,'OperatingFrequency',fc);
chanRISToUE = phased.FreeSpace('SampleRate',fs,'PropagationSpeed',c,'MaximumDistanceSource','Property','MaximumDistance',500,'OperatingFrequency',fc);
chanAPToUE = phased.FreeSpace('SampleRate',fs,'PropagationSpeed',c,'MaximumDistanceSource','Property','MaximumDistance',500,'OperatingFrequency',fc);

% Signal
xt = ones(1e4, 1);

% Fixed positions
pos_ris = [0; 0; 0];
ap_distance = 20;
pos_ap = ap_distance * [cosd(ap_to_ris_angle); sind(ap_to_ris_angle); 0];

v = zeros(3,1);

for i = 1:size(ris_sizes, 1)
    Nr = ris_sizes(i, 1);
    Nc = ris_sizes(i, 2);

    % Construct RIS surface
    dr = 0.5*lambda;
    dc = 0.5*lambda;
    ris = helperRISSurface('Size',[Nr Nc],'ElementSpacing',[dr dc],...
        'ReflectorElement',phased.IsotropicAntennaElement,'OperatingFrequency',fc);
    stv = getSteeringVector(ris);

    for d_ris_ue = ue_distances
        % Calculate UE position based on distance and angle
        pos_ue = d_ris_ue * [cosd(ris_to_ue_angle); sind(ris_to_ue_angle); 0];

        % Compute the range and angle of the RIS from the base station and the UE
        [~,ang_ap_ris,~,ang_ue_ris] = calcangle(pos_ap, pos_ue, pos_ris, [0,1,0]);


        g = stv(fc, ang_ap_ris);
        hr = stv(fc, ang_ue_ris);
        rcoeff_ris = exp(1i*(-angle(hr)-angle(g)));

        % RIS path
        x_ris_in = chanAPToRIS(xt, pos_ap, pos_ris, v, v);
        x_ris_out = ris(x_ris_in, ang_ap_ris, ang_ue_ris, rcoeff_ris);

        % RIS + LOS
        ylosris = chanRISToUE(x_ris_out, pos_ris, pos_ue, v, v) + ...
            chanAPToUE(xt, pos_ap, pos_ue, v, v);
        RXpowerRis = pow2db(bandpower(ylosris)) - txPower;

        % Only RIS
        yonlyris = chanRISToUE(x_ris_out, pos_ris, pos_ue, v, v);
        RXpowerOnlyRis = pow2db(bandpower(yonlyris)) - txPower;

        % LOS path
        ylos = chanAPToUE(xt, pos_ap, pos_ue, v, v);
        RXpowerLos = pow2db(bandpower(ylos)) - txPower;

        % Add results to the table
        results = [results; {Nr * Nc, d_ris_ue, RXpowerOnlyRis, RXpowerLos, RXpowerRis}];
    end
end

% Set column names
results.Properties.VariableNames = {'Elements', 'RIS_UE_Distance', 'OnlyRIS', 'LOS', 'RIS_LOS'};

% Extract unique RIS sizes for legend
ris_sizes = unique(results.Elements);

% Create color map
colors = jet(length(ris_sizes));

% sim_results = readtable('simulation_results.csv');

% Create figure with subplots
figure;

% Plot titles
plot_titles = {'Only RIS', 'RIS + LOS'};
y_data = {'OnlyRIS', 'RIS_LOS'};
% sim_y_data_ris = {'IRS_100_IN170_OUT10_FREQ5_21GHz_csv_ris', 'IRS_200_IN170_OUT10_FREQ5_21GHz_csv_ris', 'IRS_300_IN170_OUT10_FREQ5_21GHz_csv_ris', 'IRS_400_IN170_OUT10_FREQ5_21GHz_csv_ris'};
% sim_y_data_ris_los = {'IRS_100_IN170_OUT10_FREQ5_21GHz_csv_ris_los', 'IRS_200_IN170_OUT10_FREQ5_21GHz_csv_ris_los', 'IRS_300_IN170_OUT10_FREQ5_21GHz_csv_ris_los', 'IRS_400_IN170_OUT10_FREQ5_21GHz_csv_ris_los'};


% ris
subplot(1, 2, 1);
hold on;
for i = 1:length(ris_sizes)
    mask = results.Elements == ris_sizes(i);
    plot(results.RIS_UE_Distance(mask), results.(y_data{1})(mask), '-', 'Color', colors(i,:), 'LineWidth', 2);
end
% for i = 1:length(sim_y_data_ris)
%     plot(sim_results.ueDistance, sim_results.(sim_y_data_ris{i}), '--k', 'LineWidth', 2);
% end

xlabel('RIS/UE Distance (m)');
ylabel('Received Power (dB)');
title([plot_titles{1}, ' Received Power vs RIS/UE Distance']);
legend(string(ris_sizes));
grid on;
hold off;

% ris + los
subplot(1, 2, 2);
hold on;
for i = 1:length(ris_sizes)
    mask = results.Elements == ris_sizes(i);
    plot(results.RIS_UE_Distance(mask), results.(y_data{2})(mask), '-', 'Color', colors(i,:), 'LineWidth', 2);
end
% for i = 1:length(sim_y_data_ris_los)
%     plot(sim_results.ueDistance, sim_results.(sim_y_data_ris_los{i}), '--k', 'LineWidth', 2);
% end
xlabel('RIS/UE Distance (m)');
ylabel('Received Power (dB)');
title([plot_titles{2}, ' Received Power vs RIS/UE Distance']);
legend(string(ris_sizes));
grid on;
hold off;


% Adjust subplot spacing
set(gcf, 'Units', 'Normalized', 'OuterPosition', [0, 0.04, 1, 0.96]);
sgtitle('Received Power vs UE Distance for Different Scenarios', 'FontSize', 16);
validation = results;
end

% Define RIS sizes to simulate
ris_sizes = [
    % 5, 10;
    10, 10;
    10, 20;
    15, 20;
    20, 20;
    % 20, 25;
    ];  % [Nr, Nc] pairs
% gen_tables = validation_genTablesChangingSizes(ris_sizes);
validation_plotChangingSizes(ris_sizes);
