clc;
clear;
close all;

% ============================================================
% MODELO 04
% Cinematica de biela-manivela
% ============================================================

% Geometria
r = 0.04;       % radio de manivela [m]
L = 0.12;       % longitud de biela [m]

% Velocidad angular de prueba
omega = 2.0;    % rad/s

% Tiempo
t = linspace(0, 10, 2000);

% Angulo de la manivela
theta = omega * t;

% Posicion del carro
x = r .* cos(theta) + ...
    sqrt(L^2 - (r .* sin(theta)).^2);

% Velocidad numerica
v = gradient(x, t);

% Aceleracion numerica
a = gradient(v, t);

% ============================================================
% RPM
% ============================================================

rpm = omega * 60 / (2*pi);

fprintf('Velocidad angular: %.3f rad/s\n', omega);
fprintf('Velocidad de giro: %.3f RPM\n', rpm);

% ============================================================
% GRAFICA DE POSICION
% ============================================================

figure;

plot(t, x, 'LineWidth', 1.5);

grid on;

xlabel('Tiempo (s)');
ylabel('Posicion del carro (m)');
title('MARVOLT-PA — Cinematica de biela-manivela');

% ============================================================
% GRAFICA DE VELOCIDAD
% ============================================================

figure;

plot(t, v, 'LineWidth', 1.5);

grid on;

xlabel('Tiempo (s)');
ylabel('Velocidad (m/s)');
title('Velocidad del carro');

% ============================================================
% GRAFICA DE ACELERACION
% ============================================================

figure;

plot(t, a, 'LineWidth', 1.5);

grid on;

xlabel('Tiempo (s)');
ylabel('Aceleracion (m/s^2)');
title('Aceleracion del carro');