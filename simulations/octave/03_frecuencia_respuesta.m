clc;
clear;
close all;

% ============================================================
% MODELO 03
% Barrido de frecuencia
% ============================================================

m = 100;
c = 50;
k = 2000;

alpha = 1000;

H = 1.0;

F0 = alpha * H;

tspan = [0 60];
x0 = [0; 0];

% Frecuencia angular [rad/s]
omega_values = linspace(0.5, 5.0, 40);

amplitudes = zeros(size(omega_values));

for i = 1:length(omega_values)

    omega = omega_values(i);

    f = @(t, x) [
        x(2);
        (F0*sin(omega*t) - c*x(2) - k*x(1)) / m
    ];

    [t, x] = ode45(f, tspan, x0);

    inicio_estacionario = round(length(t) * 0.7);

    x_ss = x(inicio_estacionario:end, 1);

    amplitudes(i) = max(abs(x_ss));

end

% Convertir frecuencia angular a Hz
frecuencia_hz = omega_values / (2*pi);

% ============================================================
% GRAFICA
% ============================================================

figure;

plot(frecuencia_hz, amplitudes, '-o', 'LineWidth', 1.5);

grid on;

xlabel('Frecuencia de excitacion (Hz)');
ylabel('Amplitud maxima (m)');
title('MARVOLT-PA — Respuesta en frecuencia');