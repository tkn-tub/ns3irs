function ris_lookup_table = generate_ris_lookup_table(optimal_in_angle, optimal_out_angle)
    % Set up parameters
    fc = 28e9; % Carrier frequency
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
        'MaximumDistanceSource','Property','MaximumDistance',500);

    % Define angle range for lookup table
    % angles = -89:1:89;
    angles = 1:1:179;

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

    [~, ang_ap_ris_optimal] = rangeangle(pos_ap_optimal, pos_ris);
    [~, ang_ue_ris_optimal] = rangeangle(pos_ue_optimal, pos_ris);

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

            % Compute angles for RIS - Debug purpose
            % [~, ang_ap_ris] = rangeangle(pos_ap, pos_ris);
            % [~, ang_ue_ris] = rangeangle(pos_ue, pos_ris);

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
            phase_shift = wrapTo180(rad2deg(phase_out - phase_in));
            
            % Calculate average phase shift
            avg_phase_shift = mean(phase_shift);

            % Store results
            results(idx, :) = [in_angle, out_angle, gain_db, avg_phase_shift];
            idx = idx + 1;
        end
    end

    % Convert to table
    ris_lookup_table = array2table(results, 'VariableNames', {'in_angle', 'out_angle', 'gain_dB', 'phase_shift'});
end
% Generate the lookup table
in = 170;
out = 10;
ris_table = generate_ris_lookup_table(in, out);

% Create matrices of the gain and phase shift
out_angles = unique(ris_table.out_angle);
in_angles = unique(ris_table.in_angle);
gain_matrix = reshape(ris_table.gain_dB, length(out_angles), length(in_angles));
phase_matrix = reshape(ris_table.phase_shift, length(out_angles), length(in_angles));

threshold = -60;
cleaned_gain_matrix = clear_low_gain_values(gain_matrix, threshold);

% Plot gain heatmap
figure;
subplot(1,2,1);
imagesc(out_angles, in_angles, cleaned_gain_matrix');
colorbar;
xlabel('Outgoing Angle (degrees)');
ylabel('Ingoing Angle (degrees)');
title('RIS Gain (dB)');

% Plot phase shift heatmap
subplot(1,2,2);
imagesc(out_angles, in_angles, phase_matrix');
colorbar;
xlabel('Outgoing Angle (degrees)');
ylabel('Ingoing Angle (degrees)');
title('Phase Shift (degrees)');

sgtitle(sprintf('RIS Performance - Optimized for (%d°, %d°)', in, out));

filename = 'lookuptable.csv';

writetable(ris_table, filename, 'Delimiter', ',', 'WriteRowNames', true);

% Display a message to confirm the export
disp(['Table has been exported to ', filename]);
