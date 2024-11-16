function plt = plotIrsLookupTable(ris_table, opt_in_angle, opt_out_angle)
% plotIrsLookupTable
% ris_table
% opt_in_angle, opt_out_angle - angles the RIS is optimized for

% Create matrices of the gain and phase shift
out_angles = unique(ris_table.out_angle);
in_angles = unique(ris_table.in_angle);
gain_matrix = reshape(ris_table.gain_dB, length(out_angles), length(in_angles));
phase_matrix = reshape(ris_table.phase_shift, length(out_angles), length(in_angles));

% Clear lowest 6% for better plot
threshold = 0.06; % in percent
cleaned_gain_matrix = clear_low_gain_values(gain_matrix, threshold);

% Define font size
fontSize = 16;

% Create first figure for gain
fig1 = figure('Units', 'inches', 'Position', [0, 0, 8, 6]);
h = imagesc(out_angles, in_angles, cleaned_gain_matrix');
cb = colorbar;
xlabel('Out Angle (degrees)', 'FontSize', fontSize);
ylabel('In Angle (degrees)', 'FontSize', fontSize);
% title('Gain (dB)', 'FontSize', fontSize);

% Adjust margins without cropping
ax = gca;
outerpos = ax.OuterPosition;
ti = ax.TightInset; 
left = outerpos(1) + ti(1);
bottom = outerpos(2) + ti(2);
ax_width = outerpos(3) - ti(1) - ti(3);
ax_height = outerpos(4) - ti(2) - ti(4);
ax.Position = [left bottom ax_width ax_height];

% Set background color if desired
set(gcf, 'Color', 'white');
set(gca, 'Color', 'white');

% Export first figure as PDF
set(gcf, 'PaperUnits', 'inches');
set(gcf, 'PaperPosition', [0 0 8 6]);
set(gcf, 'PaperSize', [8 6]);
print('-dpdf', '-painters', 'gain_plot.pdf');

% Create second figure for phase shift
fig2 = figure('Units', 'inches', 'Position', [9, 0, 8, 6]);
h = imagesc(out_angles, in_angles, phase_matrix');
cb = colorbar;
xlabel('Out Angle (degrees)', 'FontSize', fontSize);
ylabel('In Angle (degrees)', 'FontSize', fontSize);
% title('Phase Shift (radians)', 'FontSize', fontSize);

% Adjust margins without cropping
ax = gca;
outerpos = ax.OuterPosition;
ti = ax.TightInset; 
left = outerpos(1) + ti(1);
bottom = outerpos(2) + ti(2);
ax_width = outerpos(3) - ti(1) - ti(3);
ax_height = outerpos(4) - ti(2) - ti(4);
ax.Position = [left bottom ax_width ax_height];

% Set background color if desired
set(gcf, 'Color', 'white');
set(gca, 'Color', 'white');

% Export second figure as PDF
set(gcf, 'PaperUnits', 'inches');
set(gcf, 'PaperPosition', [0 0 8 6]);
set(gcf, 'PaperSize', [8 6]);
print('-dpdf', '-painters', 'phase_plot.pdf');

end