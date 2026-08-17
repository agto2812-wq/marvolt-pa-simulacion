clc;
clear;
close all;

% ============================================================
% MODELO 02
% Altura de ola vs respuesta de la boya
% ============================================================

m = 100;
c = 50;
k = 2000;

omega = 2;

alpha = 1000;

tspan = [0 60];
x0 = [0; 0];

H_values = linspace(0.5, 2.0, 10);

amplitudes = zeros(size(H_values));

for i = 1:length(H_values)

    H = H_values(i);

    F0 = alpha * H;

    f = @(t, x) [
        x(2);
        (F0*sin(omega*t) - c*x(2) - k*x(1)) / m
    ];

    [t, x] = ode45(f, tspan, x0);

    % Último 30 % de la simulación
    inicio_estacionario = round(length(t) * 0.7);

    x_ss = x(inicio_estacionario:end, 1);

    amplitudes(i) = max(abs(x_ss));

end

% ============================================================
% GRAFICA
% ============================================================

figure;

plot(H_values, amplitudes, '-o', 'LineWidth', 1.5);

grid on;

xlabel('Altura de ola H (m)');
ylabel('Amplitud maxima de la boya (m)');
title('MARVOLT-PA — Altura de ola vs respuesta');