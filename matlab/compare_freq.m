function ris_lookup_table = generate_ris_lookup_table(optimal_in_angle, optimal_out_angle, fc)
% Set up parameters
c = physconst('lightspeed');
lambda = c/fc;
fs = 10e6; % Sample rate

% Setup RIS
Nr = 10; % Number of rows
Nc = 20; % Number of columns
dr = 0.5*lambda; % Row spacing
dc = 0.5*lambda; % Column spacing
x = 2*randi(2,[100 1])-3;
tx = phased.Transmitter('PeakPower',50e-3,'Gain',0);
xt = tx(x);

% Construct RIS
ris = helperRISSurface('Size',[Nr Nc],'ElementSpacing',[dr dc],...
    'ReflectorElement',phased.IsotropicAntennaElement,'OperatingFrequency',fc);

% Create channel models
chanAPToRIS = phased.FreeSpace('SampleRate',fs,'PropagationSpeed',c,...
    'MaximumDistanceSource','Property','MaximumDistance',500,'OperatingFrequency',fc);

% Define angle range for lookup table
% angles = -89:1:89;
angles = 0:1:180;

% Preallocate results array
num_angles = length(angles);
results = zeros(num_angles^2, 4);

% RIS position
d2_ris = 10;
pos_ris = [0; 0; 0];
v = [0; 0; 0]; % Static scenario

% Optimize RIS for the optimal constellation point
pos_ap_optimal = [d2_ris*cosd(optimal_in_angle); d2_ris*sind(optimal_in_angle); 0];
pos_ue_optimal = [d2_ris*cosd(optimal_out_angle); d2_ris*sind(optimal_out_angle); 0];

[~,ang_ap_ris_optimal,~,ang_ue_ris_optimal] = calcangle(ap, ue, ris, [0,1,0]);


stv = getSteeringVector(ris);
g_optimal = stv(fc, ang_ap_ris_optimal);
hr_optimal = stv(fc, ang_ue_ris_optimal);
rcoeff_ris_optimal = exp(1i*(-angle(hr_optimal)-angle(g_optimal)));

% Generate lookup table
idx = 1;
for in_angle = angles
    for out_angle = angles
        % Set positions based on angles
        pos_ap = [d2_ris*cosd(in_angle); d2_ris*sind(in_angle); 0];
        pos_ue = [d2_ris*cosd(out_angle); d2_ris*sind(out_angle); 0];

        % Apply RIS phase control (optimized for optimal constellation) and simulate paths
        x_ris_in = chanAPToRIS(xt, pos_ap, pos_ris, v, v);
        x_ris_out = ris(x_ris_in, in_angle, out_angle, rcoeff_ris_optimal);

        % Calculate gain
        in_power = pow2db(bandpower(x_ris_in));
        out_power = pow2db(bandpower(x_ris_out));
        gain_db = out_power-in_power;

        % Calculate phase shift
        phase_in = angle(x_ris_in);
        phase_out = angle(x_ris_out);
        phase_shift = wrapToPi(phase_out - phase_in);

        % Calculate average phase shift
        avg_phase_shift = mean(phase_shift);

        % Store results
        results(idx, :) = [in_angle, out_angle, gain_db, avg_phase_shift];
        idx = idx + 1;
    end
end

% Convert to table
ris_lookup_table = array2table(results, 'VariableNames', {'in_angle', 'out_angle', 'gain_db', 'phase_shift'});
end
% Generate the lookup table
in = 135;
out = 15;

% table1 = generate_ris_lookup_table(in, out, 5.21e9);
% table2 = generate_ris_lookup_table(in, out, 28e9);

table1 = ris_table;
table2 = ris_tableCopy;

% Compute differences
diff_gain = table1.gain_dB - table2.gain_dB;
diff_phase = table1.phase_shift - table2.phase_shift;

% Create a new table with differences
diff_table = table(table1.in_angle, table1.out_angle, diff_gain, diff_phase, ...
    'VariableNames', {'in_angle', 'out_angle', 'diff_gain_db', 'diff_phase_shift'});

% Display the difference table
disp(diff_table);

figure;

% Plot gain differences
subplot(2,1,1);
plot(table1.in_angle, table1.gain_dB - table2.gain_dB);
xlabel('Input Angle');
ylabel('Gain Difference (dB)');
title('Gain Difference (Table 1 - Table 2)');
grid on;

% Plot phase shift differences
subplot(2,1,2);
plot(table1.in_angle, table1.phase_shift - table2.phase_shift);
xlabel('Input Angle');
ylabel('Phase Shift Difference');
title('Phase Shift Difference (Table 1 - Table 2)');
grid on;



