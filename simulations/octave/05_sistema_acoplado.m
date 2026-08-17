clc;
clear;
close all;

% ============================================================
% MODELO 05
% Sistema preliminar acoplado:
%
% Ola → Boya → Biela-manivela
% ============================================================

% ============================================================
% 1. PARAMETROS DE LA BOYA
% ============================================================

m = 100;
c = 50;
k = 2000;

H = 1.0;
omega_wave = 2.0;

alpha = 1000;
F0 = alpha * H;

tspan = [0 60];
x0 = [0; 0];

% ============================================================
% 2. MODELO DE LA BOYA
% ============================================================

f = @(t, x) [
    x(2);
    (F0*sin(omega_wave*t) - c*x(2) - k*x(1)) / m
];

[t, estado] = ode45(f, tspan, x0);

x_boya = estado(:,1);
v_boya = estado(:,2);

% ============================================================
% 3. GEOMETRIA DE BIELA-MANIVELA
% ============================================================

r = 0.04;       % radio de manivela [m]
L = 0.12;       % longitud de biela [m]

% Posicion angular nominal
theta0 = pi/3;

% Posicion correspondiente al angulo nominal
s0 = r*cos(theta0) + ...
     sqrt(L^2 - (r*sin(theta0)).^2);

% ============================================================
% 4. POSICION DEL CARRO
% ============================================================

s = s0 + x_boya;

% ============================================================
% 5. INVERSION GEOMETRICA
%
% s = r*cos(theta) + sqrt(L^2-r^2*sin(theta)^2)
%
% cos(theta) =
% (s^2 - L^2 + r^2)/(2*s*r)
% ============================================================

cos_theta = ...
    (s.^2 - L^2 + r^2) ./ ...
    (2 .* s .* r);

% Evitar errores numericos
cos_theta = max(-1, min(1, cos_theta));

theta = acos(cos_theta);

% ============================================================
% 6. VELOCIDAD ANGULAR
% ============================================================

omega_mech = gradient(theta, t);

rpm = omega_mech * 60 / (2*pi);

% ============================================================
% 7. RESULTADOS
% ============================================================

fprintf('RPM maxima: %.3f\n', max(abs(rpm)));
fprintf('RPM minima: %.3f\n', min(rpm));

% ============================================================
% 8. GRAFICA BOYA
% ============================================================

figure;

plot(t, x_boya, 'LineWidth', 1.5);

grid on;

xlabel('Tiempo (s)');
ylabel('Desplazamiento de la boya (m)');
title('Sistema acoplado — Movimiento de la boya');

% ============================================================
% 9. GRAFICA ANGULO
% ============================================================

figure;

plot(t, theta * 180/pi, 'LineWidth', 1.5);

grid on;

xlabel('Tiempo (s)');
ylabel('Angulo de manivela (°)');
title('Sistema acoplado — Angulo de manivela');

% ============================================================
% 10. GRAFICA RPM
% ============================================================

figure;

plot(t, rpm, 'LineWidth', 1.5);

grid on;

xlabel('Tiempo (s)');
ylabel('RPM');
title('Sistema acoplado — Velocidad de giro');