clc;
clear;
close all;

% ============================================================
% MODELO 01
% Sistema masa-resorte-amortiguador
% ============================================================

% Parametros provisionales
m = 100;        % kg
c = 50;         % N*s/m
k = 2000;       % N/m

omega = 2;      % rad/s
H = 1.0;        % m

% Relacion provisional fuerza-altura
alpha = 1000;
F0 = alpha * H;

% Tiempo
tspan = [0 60];

% Condiciones iniciales
x0 = [0; 0];

% Sistema diferencial
f = @(t, x) [
    x(2);
    (F0*sin(omega*t) - c*x(2) - k*x(1)) / m
];

% Resolver
[t, x] = ode45(f, tspan, x0);

% Resultados
desplazamiento = x(:,1);
velocidad = x(:,2);

% ============================================================
% GRAFICA
% ============================================================

figure;

plot(t, desplazamiento, 'LineWidth', 1.5);

grid on;

xlabel('Tiempo (s)');
ylabel('Desplazamiento (m)');
title('MARVOLT-PA — Modelo masa-resorte-amortiguador');

legend('Boya');