d_ap_ue = 15;
pos_ap = [0;0;0];
pos_ue = [d_ap_ue;0;0];
step_size = 0.5;

x_ris_range = 0:step_size:d_ap_ue;

Nr = 20;
Nc = 20;
freq = 5.21e9;

c_array = strings(1, length(x_ris_range));
for i = 1:length(x_ris_range)
    pos_ris = [x_ris_range(i); -1; 0];
    [r_ap_ris,a_ap_ris,r_ris_ue,a_ris_ue] = calcangle(pos_ap, pos_ue, pos_ris, [0,1,0]);

    ris_table = generateIrsLookupTable(round(a_ap_ris(1)), round(a_ris_ue(1)), Nr, Nc, freq, abs(pos_ue(1)-pos_ap(1)), r_ap_ris, r_ris_ue, 0, "constructive_changeRisPos");
    % c_array(i) = sprintf('IRS_%d_IN%d_OUT%d_FREQ%.2fGHz_%s.csv', Nr*Nc, round(a_ap_ris(1)), round(a_ris_ue(1)), freq/1e9, "constructive_changeRisPos");
end
% fprintf('const char* irs_lookup_table[%d] = {\n', length(c_array));
% for i = 1:length(c_array)
%     fprintf('    %s', c_array(i));
%     if i < length(c_array)
%         fprintf(',\n');
%     else
%         fprintf('\n');
%     end
% end
% fprintf('};\n');