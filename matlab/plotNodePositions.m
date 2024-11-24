pos_ris = [0; 0; 0];
angles = 0:2:180;
d2_ris = 10;
d_tx = 10;
d_rx = 10;
optimal_in_angle = 135;
optimal_out_angle = 45;
pos_tx_optimal = pos_ris + [d_tx*cosd(optimal_in_angle); d_tx*sind(optimal_in_angle); 0];
pos_rx_optimal = pos_ris + [d_rx*cosd(optimal_out_angle); d_rx*sind(optimal_out_angle); 0];

% Plot positions
figure;
hold on;

% Plot RIS as a thicker surface with 20x20 elements
ris_width = 2;
ris_height = 2;
ris_depth = 0.1;
num_elements = 20;
[X, Z] = meshgrid(linspace(-ris_width/2, ris_width/2, num_elements), linspace(-ris_height/2, ris_height/2, num_elements));
Y_front = zeros(size(X));
Y_back = ones(size(X)) * ris_depth;

% Create the front and back surfaces of the RIS
surf(X, Y_front, Z, 'FaceColor', 'r', 'EdgeColor', 'k', 'FaceAlpha', 0.5);
surf(X, Y_back, Z, 'FaceColor', 'r', 'EdgeColor', 'k', 'FaceAlpha', 0.5);

% Create the side surfaces to make it look solid
for i = 1:size(X, 1)
    patch([X(i,1) X(i,end) X(i,end) X(i,1)], [Y_front(i,1) Y_front(i,1) Y_back(i,1) Y_back(i,1)], [Z(i,1) Z(i,end) Z(i,end) Z(i,1)], 'r', 'FaceAlpha', 0.5, 'EdgeColor', 'k');
end
for j = 1:size(X, 2)
    patch([X(1,j) X(end,j) X(end,j) X(1,j)], [Y_front(1,j) Y_front(1,j) Y_back(1,j) Y_back(1,j)], [Z(1,j) Z(end,j) Z(end,j) Z(1,j)], 'r', 'FaceAlpha', 0.5, 'EdgeColor', 'k');
end

text(pos_ris(1), pos_ris(2)+0.5, pos_ris(3), ' IRS', 'VerticalAlignment', 'bottom');


% Plot AP and UE positions
for a = angles
    pos = pos_ris + [d2_ris*cosd(a); d2_ris*sind(a); 0];
    scatter3(pos(1), pos(2), pos(3), 40, 'b', 'filled');
end

% Plot optimal AP (Tx) and UE (Rx) positions with triangle symbols
scatter3(pos_rx_optimal(1), pos_rx_optimal(2), pos_rx_optimal(3), 200, 'g', 'filled', '^'); % Triangle for Rx
text(pos_rx_optimal(1)-1, pos_rx_optimal(2)-1, pos_rx_optimal(3), ' RX', 'VerticalAlignment', 'bottom');
scatter3(pos_tx_optimal(1), pos_tx_optimal(2), pos_tx_optimal(3), 200, 'm', 'filled', '^'); % Triangle for Tx
text(pos_tx_optimal(1)-1.5, pos_tx_optimal(2), pos_tx_optimal(3), ' TX', 'VerticalAlignment', 'bottom');

% Set plot properties
xlabel('X (m)');
ylabel('Y (m)');
zlabel('Z (m)');
grid on;
axis equal;
view(2);
hold off;

% save as file
set(gcf,'Units','inches');
screenposition = get(gcf,'Position');
set(gcf,...
    'PaperPosition',[0 0 screenposition(3:4)],...
    'PaperSize',[screenposition(3:4)]);
print -dpdf -painters nodePositions
