pos_ris = [0; 0; 0];
angles = 1:1:179;
d2_ris = 10;

optimal_in_angle = 170;
optimal_out_angle = 10;
pos_tx_optimal = pos_ris + [d2_ris*cosd(optimal_in_angle); d2_ris*sind(optimal_in_angle); 0]
pos_rx_optimal = pos_ris + [d2_ris*cosd(optimal_out_angle); d2_ris*sind(optimal_out_angle); 0]


% Plot positions
figure;
hold on;
% Plot RIS position
scatter3(pos_ris(1), pos_ris(2), pos_ris(3), 200, 'r', 'filled', 'Marker', 's');
text(pos_ris(1), pos_ris(2), pos_ris(3), '  RIS', 'VerticalAlignment', 'bottom');

% Plot AP and UE positions
for angle = angles
    pos = pos_ris + [d2_ris*cosd(angle); d2_ris*sind(angle); 0];
    scatter3(pos(1), pos(2), pos(3), 50, 'b', 'filled');
end

% Plot optimal AP and UE positions
scatter3(pos_rx_optimal(1), pos_rx_optimal(2), pos_rx_optimal(3), 100, 'g', 'filled');
text(pos_rx_optimal(1), pos_rx_optimal(2), pos_rx_optimal(3), '  Optimal RX', 'VerticalAlignment', 'bottom');
scatter3(pos_tx_optimal(1), pos_tx_optimal(2), pos_tx_optimal(3), 100, 'g', 'filled');
text(pos_tx_optimal(1), pos_tx_optimal(2), pos_tx_optimal(3), '  Optimal TX', 'VerticalAlignment', 'bottom');

% Plot vector [1; 0; 0]
vector = [1; 0; 0];
quiver3(0, 0, 0, vector(1), vector(2), vector(3), 'k', 'LineWidth', 2, 'MaxHeadSize', 0.5);
text(vector(1)/2, vector(2), vector(3), '  [0; 1; 0]', 'VerticalAlignment', 'bottom');

in_vec = pos_ris - pos_tx_optimal;
quiver3(pos_tx_optimal(1), pos_tx_optimal(2), pos_tx_optimal(3), in_vec(1), in_vec(2), in_vec(3), 'k', 'LineWidth', 2, 'MaxHeadSize', 0.25);
text(in_vec(1)/2, in_vec(2), in_vec(3), '', 'VerticalAlignment', 'bottom');

out_vec = pos_rx_optimal - pos_ris;
quiver3(pos_ris(1), pos_ris(2), pos_ris(3), out_vec(1), out_vec(2), out_vec(3), 'k', 'LineWidth', 2, 'MaxHeadSize', 0.25);
text(out_vec(1)/2, out_vec(2), out_vec(3), '', 'VerticalAlignment', 'bottom');


% Set plot properties
xlabel('X');
ylabel('Y');
zlabel('Z');
title('Positions of RIS, RX, and TX');
legend('RIS', 'Possible RX/TX positions', 'Optimal RX/TX', 'Location', 'best');
grid on;
axis equal;
view(3); % 3D view

hold off;