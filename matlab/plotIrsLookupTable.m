function plt = plotIrsLookupTable(ris_table, opt_in_angle, opt_out_angle)
    % Create matrices of the gain and phase shift
    out_angles = unique(ris_table.out_angle);
    in_angles = unique(ris_table.in_angle);
    gain_matrix = reshape(ris_table.gain_dB, length(out_angles), length(in_angles));
    phase_matrix = reshape(ris_table.phase_shift, length(out_angles), length(in_angles));
    
    threshold = 0.06; % in percent
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
    title('Phase Shift (radians)');
    
    sgtitle(sprintf('RIS Performance - Optimized for (%d, %d)', opt_in_angle, opt_out_angle));
end