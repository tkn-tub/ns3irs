function ris_table = generateIrsLookupTable(optimal_in_angle, optimal_out_angle, Nr, Nc, fc, r_ap_ue, r_ap_ris, r_ue_ris, phase_shift, file_name, alpha, numIrs)
% generateIrsLookupTable
%   optimal_in_angle, optimal_out_angle - angles the RIS is optimized for
%   Nr, Nc - number of rows/columns of the RIS
%   fc - carrier frequency
%   r_ue_ap, r_ap_ris, r_ue_ris - distances for the respective paths
%   phase_shift - phase shift to create interference with LoS path (pi for destructive interference, 0 for constructive interference)
%   file_name - IRS_[...]_[file_name].csv
%   alpha - gain control
%   numIrs - amount of IRS to split phase shifts

only_in_out = false;
file_name_set = false;

if (nargin == 5)
    % no phase control
    only_in_out = true;
elseif(nargin == 9)
    % phase control
    alpha = 1;
    numIrs = 1;
elseif(nargin == 10)
    % phase control, filename
    file_name_set = true;
    alpha = 1;
    numIrs = 1;

elseif(nargin == 11)
    % phase control, filename, gain control
    file_name_set = true;
    numIrs = 1;
elseif(nargin == 12)
    % phase control, filename, gain control, multi IRS
    file_name_set = true;
else
    error("Wrong amount of arguments");
end

% Set up parameters
c = physconst('lightspeed');
lambda = c/fc;

% Setup
dr = 0.5*lambda; % Row spacing
dc = 0.5*lambda; % Column spacing

% Construct RIS
ris = helperRISSurface('Size',[Nr Nc],'ElementSpacing',[dr dc],...
    'ReflectorElement',phased.IsotropicAntennaElement,'OperatingFrequency',fc);

% Define angle range for lookup table
angles = 0:1:180;

stv = getSteeringVector(ris);

g = stv(fc, optimal_in_angle);
hr = stv(fc, optimal_out_angle);

if (only_in_out)
    rcoeff_ris = exp(1i*(-angle(hr)-angle(g)));
else
    % Calculate the optimal reflection coefficient
    direct_path_phase = (2 * pi * r_ap_ue) / lambda;
    reflected_path_phase = (2 * pi * (r_ap_ris + r_ue_ris)) / lambda;
    required_phase_shift =  (phase_shift + reflected_path_phase - direct_path_phase) / numIrs;
    rcoeff_ris = alpha * exp(1i * (wrapToPi(required_phase_shift) - angle(hr) - angle(g)));

end

% Generate lookup table
[in_angles, out_angles] = meshgrid(angles, angles);
in_angles = in_angles(:);
out_angles = out_angles(:);

% Preallocate results array
num_angles = numel(in_angles);
results = zeros(num_angles, 4);

x_ris_in = ones(1e3, 1);

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
    phase_shift = wrapToPi(phase_in - phase_out);

    % Calculate mean value
    phase_shift = mean(phase_shift);

    % Store results
    results(i, :) = [in_angles(i), out_angles(i), gain_db, phase_shift];
end

% Convert to table
ris_table = array2table(results, 'VariableNames', {'in_angle', 'out_angle', 'gain_dB', 'phase_shift'});

% Export table
if (file_name_set)
    filename = sprintf('IRS_%d_IN%d_OUT%d_FREQ%.2fGHz_%s.csv', Nr*Nc, optimal_in_angle, optimal_out_angle, fc/1e9, file_name);
else
    filename = sprintf('IRS_%d_IN%d_OUT%d_FREQ%.2fGHz.csv', Nr*Nc, optimal_in_angle, optimal_out_angle, fc/1e9);
end
writetable(ris_table, filename, 'Delimiter', ',', 'WriteRowNames', true);

% Display a message to confirm the export
disp(['Table has been exported to ', filename]);
end
