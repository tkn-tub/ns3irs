% Parameters
Nx = 20; % Number of elements in x-direction
Ny = 20; % Number of elements in y-direction
az = 135; % Azimuth angle (in degrees)
el = 10; % Elevation angle (in degrees)
fc = 5.21e9; % Carrier frequency in Hz
c = physconst('LightSpeed'); % Speed of light in m/s
lambda = c / fc; % Wavelength in meters
dx = 0.5 * lambda; % Spacing in x-direction (in wavelengths)
dy = 0.5 * lambda; % Spacing in y-direction (in wavelengths)

x = zeros(1, 100);
y = zeros(1, 100);
z = zeros(1, 100);
count = 1;
for i = 1:Nx
    for j = 1:Ny
        x(count) = 0;
        y(count) = (i - ((Nx-1)/2)-1) * dx;
        z(count) = ((Ny-1)/2+1 - j) * dy;
        count = count + 1;
    end
end
posSelf = [x; y; z];

% wave vector
k = (2*pi/lambda) * [cosd(el)*cosd(az); cosd(el)*sind(az); sind(el)];
k = (2*pi/lambda) * [cosd(el)*cosd(az); cosd(el)*sind(az); sind(el)];

% steering vector calc
stvSelf = zeros(Nx*Ny, 1);
for i = 1:Nx*Ny
    stvSelf(i,:) = exp(-1j * (k' * posSelf(:,i)))';
end

% comparison steering vector
ura = phased.URA('Size', [Nx, Ny], 'ElementSpacing', [dx, dy], 'Element', phased.IsotropicAntennaElement);
stv = phased.SteeringVector('SensorArray', ura);
stvURA = stv(fc, [az; el]);

posURA = getElementPosition(ura);

disp(isequal(posSelf, posURA));
postions = posSelf == posURA;
disp(isequal(stvSelf, stvURA));
steering = stvSelf == stvURA;
% unequallity seems to be floating point errors

