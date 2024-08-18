function ris_table = generateIrsLookupTableConstructive(optimal_in_angle, optimal_out_angle, Nr, Nc, fc, r_ap_ue, r_ap_ris, r_ue_ris)
% generateIrsLookupTableConstructive
%   optimal_in_angle, optimal_out_angle - angles the RIS is optimized for
%   Nr, Nc - number of rows/columns of the RIS
%   fc - carrier frequency
%   r_ue_ap, r_ap_ris, r_ue_ris - distances for the respective paths

% Set up parameters
c = physconst('lightspeed');
lambda = c/fc;

% Setup
dr = 0.5*lambda; % Row spacing
dc = 0.5*lambda; % Column spacing

% Construct RIS
ris = helperRISSurface('Size',[Nr Nc],'ElementSpacing',[dr dc],...
    'ReflectorElement',phased.IsotropicAntennaElement,'OperatingFrequency',fc);

% Create channel
chanAPToRIS = phased.FreeSpace('SampleRate',10e6,'PropagationSpeed',c,...
    'MaximumDistanceSource','Property','MaximumDistance',500, 'OperatingFrequency', fc);

% Define angle range for lookup table
angles = 0:1:180;

% RIS position
d2_ris = 10;
pos_ris = zeros(3,1);
v = zeros(3,1);

stv = getSteeringVector(ris);

% Calculate steering vectors for input and output angles
g = stv(fc, optimal_in_angle);
hr = stv(fc, optimal_out_angle);

% Calculate the optimal reflection coefficient
direct_path_phase = 2 * pi * r_ap_ue / lambda;
reflected_path_phase = 2 * pi * (r_ap_ris + r_ue_ris) / lambda;
required_phase_shift = (2 * pi) - (reflected_path_phase - direct_path_phase);
rcoeff_ris = exp(1i * (required_phase_shift - angle(hr) - angle(g)));

% Generate lookup table
[in_angles, out_angles] = meshgrid(angles, angles);
in_angles = in_angles(:);
out_angles = out_angles(:);

% Position calculation
pos_ap = [d2_ris*cosd(in_angles)'; d2_ris*sind(in_angles)'; zeros(1, numel(in_angles))];

% Preallocate results array
num_angles = numel(in_angles);
results = zeros(num_angles, 4);

% input signal
x_ris_in = chanAPToRIS(ones(1e3, 1), pos_ap(:,1), pos_ris, v, v);

% Use parfor for parallel processing
parfor i = 1:num_angles
    % Apply RIS phase control and simulate paths
    x_ris_out = ris(x_ris_in, in_angles(i), out_angles(i), rcoeff_ris);

    % Calculate gain
    in_power = pow2db(bandpower(x_ris_in));
    out_power = pow2db(bandpower(x_ris_out));
    gain_db = out_power - in_power;

    % Calculate phase shift
    phase_in = angle(x_ris_in);
    phase_out = angle(x_ris_out);
    phase_shift = wrapToPi(phase_out - phase_in);

    % Calculate mean value
    phase_shift = mean(phase_shift);

    % Store results
    results(i, :) = [in_angles(i), out_angles(i), gain_db, phase_shift];
end

% Convert to table
ris_table = array2table(results, 'VariableNames', {'in_angle', 'out_angle', 'gain_dB', 'phase_shift'});

% Export table
filename = sprintf('IRS_%d_IN%d_OUT%d_FREQ%.2fGHz_constructive.csv', Nr*Nc, optimal_in_angle, optimal_out_angle, fc/1e9);
writetable(ris_table, filename, 'Delimiter', ',', 'WriteRowNames', true);

% Display a message to confirm the export
disp(['Table has been exported to ', filename]);
end