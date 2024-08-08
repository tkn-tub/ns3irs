function ris_table = generateIrsLookupTableConstructive(optimal_in_angle, optimal_out_angle, Nr, Nc, fc, r_ap_ue, r_ap_ris, r_ue_ris)
% generateIrsLookupTableConstructive
%   optimal_in_angle, optimal_out_angle - angles the RIS is optimized for
%   Nr, Nc - number of rows/columns of the RIS
%   fc - carrier frequency
%   r_ue_ap, r_ap_ris, r_ue_ris - distances for the respective paths

    % Set up parameters
    c = physconst('lightspeed');
    lambda = c/fc;
    fs = 10e6; % Sample rate

    % Setup
    dr = 0.5*lambda; % Row spacing
    dc = 0.5*lambda; % Column spacing

    % Construct RIS
    ris = helperRISSurface('Size',[Nr Nc],'ElementSpacing',[dr dc],...
        'ReflectorElement',phased.IsotropicAntennaElement,'OperatingFrequency',fc);

    % Create channel models
    chanAPToRIS = phased.FreeSpace('SampleRate',fs,'PropagationSpeed',c,...
        'MaximumDistanceSource','Property','MaximumDistance',500, 'OperatingFrequency', fc);

    % Define angle range for lookup table
    angles = 1:1:179;

    % Preallocate results array
    num_angles = length(angles);
    results = zeros(num_angles^2, 4);

    % RIS position
    d2_ris = 10;
    pos_ris = [0; 0; 0];
    v = [0; 0; 0]; % Static scenario

    stv = getSteeringVector(ris);
    % Calculate steering vectors for input and output angles
    g = stv(fc, optimal_in_angle);
    hr = stv(fc, optimal_out_angle);
    
    % Calculate the direct path phase
    direct_path_phase = 2 * pi * r_ap_ue / lambda;
    % Calculate the reflected path phase
    reflected_path_phase = 2 * pi * (r_ap_ris + r_ue_ris) / lambda;
    % Calculate the required phase shift for constructive interference
    required_phase_shift = (2 * pi) - (reflected_path_phase - direct_path_phase);
    % Calculate the optimal reflection coefficient
    rcoeff_ris_optimal = exp(1i * (required_phase_shift - angle(hr) - angle(g)));
    
    pos_ap = [d2_ris*cosd(optimal_in_angle); d2_ris*sind(optimal_in_angle); 0];
    x_ris_in = chanAPToRIS(ones(1e3, 1), pos_ap, pos_ris, v, v); % e6
    
    % Generate lookup table
    idx = 1;
    for in_angle = angles
        for out_angle = angles
            % Set positions based on angles
            pos_ap = [d2_ris*cosd(in_angle); d2_ris*sind(in_angle); 0];

            % Apply RIS phase control (optimized for optimal constellation) and simulate paths
            x_ris_out = ris(x_ris_in, in_angle, out_angle, rcoeff_ris_optimal);

            % Calculate gain
            in_power = pow2db(bandpower(x_ris_in));
            out_power = pow2db(bandpower(x_ris_out));
            gain_db = out_power-in_power;

            % Calculate phase shift
            phase_in = angle(x_ris_in);
            phase_out = angle(x_ris_out);
            phase_shift = wrapToPi(phase_out - phase_in);

            % Calculate mean value
            phase_shift = mean(phase_shift);

            % Store results
            results(idx, :) = [in_angle, out_angle, gain_db, phase_shift];
            idx = idx + 1;
            if mod(idx, 2000) == 0
                fprintf('Processed %d out of %d rows (%.2f%% complete)\n', idx, 32041, (idx/32041)*100);
            end
        end
    end

    % Convert to table
    ris_table = array2table(results, 'VariableNames', {'in_angle', 'out_angle', 'gain_dB', 'phase_shift'});

    filename = sprintf('IRS_%d_IN%d_OUT%d_FREQ%.2fGHz_constructive.csv', Nr*Nc, optimal_in_angle, optimal_out_angle, fc/1e9);
    
    writetable(ris_table, filename, 'Delimiter', ',', 'WriteRowNames', true);
    
    % Display a message to confirm the export
    disp(['Table has been exported to ', filename]);
end