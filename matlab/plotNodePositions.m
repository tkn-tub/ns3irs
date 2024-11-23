pos_ris = [0; 0; 0];
angles = 0:2:180;
d2_ris = 10;
d_tx = 10;
d_rx = 10;
optimal_in_angle = 135;
optimal_out_angle = 45;
pos_tx_optimal = pos_ris + [d_tx*sind(optimal_in_angle); d_tx*cosd(optimal_in_angle); 0];
pos_rx_optimal = pos_ris + [d_rx*sind(optimal_out_angle); d_rx*cosd(optimal_out_angle); 0];

% Plot positions
figure;
hold on;

% Plot RIS as a thicker surface with 20x20 elements
ris_width = 2;
ris_height = 2;
ris_depth = 0.1;
num_elements = 20;
[Y, Z] = meshgrid(linspace(-ris_width/2, ris_width/2, num_elements), linspace(-ris_height/2, ris_height/2, num_elements));
X_front = zeros(size(Y));
X_back = ones(size(Y)) * ris_depth;

% Create the front and back surfaces of the RIS
surf(X_front, Y, Z, 'FaceColor', 'r', 'EdgeColor', 'k', 'FaceAlpha', 0.5);
surf(X_back, Y, Z, 'FaceColor', 'r', 'EdgeColor', 'k', 'FaceAlpha', 0.5);

% Create the side surfaces to make it look solid
for i = 1:size(Y, 1)
    patch([X_front(i,1) X_front(i,1) X_back(i,1) X_back(i,1)], [Y(i,1) Y(i,end) Y(i,end) Y(i,1)], [Z(i,1) Z(i,end) Z(i,end) Z(i,1)], 'r', 'FaceAlpha', 0.5, 'EdgeColor', 'k');
end
for j = 1:size(Y, 2)
    patch([X_front(1,j) X_front(1,j) X_back(1,j) X_back(1,j)], [Y(1,j) Y(end,j) Y(end,j) Y(1,j)], [Z(1,j) Z(end,j) Z(end,j) Z(1,j)], 'r', 'FaceAlpha', 0.5, 'EdgeColor', 'k');
end

text(pos_ris(1), pos_ris(2)+0.5, pos_ris(3), ' IRS', 'VerticalAlignment', 'bottom');


% Plot AP and UE positions
for a = angles
    pos = pos_ris + [d2_ris*sind(a); d2_ris*cosd(a); 0];
    scatter3(pos(1), pos(2), pos(3), 50, 'b', 'filled');
end

% Plot optimal AP (Tx) and UE (Rx) positions with triangle symbols
scatter3(pos_rx_optimal(1), pos_rx_optimal(2), pos_rx_optimal(3), 200, 'g', 'filled', '^'); % Triangle for Rx
text(pos_rx_optimal(1)-1, pos_rx_optimal(2)-1, pos_rx_optimal(3), ' RX', 'VerticalAlignment', 'bottom');
scatter3(pos_tx_optimal(1), pos_tx_optimal(2), pos_tx_optimal(3), 200, 'm', 'filled', '^'); % Triangle for Tx
text(pos_tx_optimal(1)-1, pos_tx_optimal(2), pos_tx_optimal(3), ' TX', 'VerticalAlignment', 'bottom');

% Set plot properties
xlabel('X');
ylabel('Y');
zlabel('Z');
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
