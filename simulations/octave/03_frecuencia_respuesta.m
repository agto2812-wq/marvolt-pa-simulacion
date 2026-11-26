clc;
clear;
close all;

% ============================================================
% MODELO 03 - RESPUESTA EN FRECUENCIA
% Análisis de sensibilidad del SDOF lineal con parámetros provisionales.
% La frecuencia de PWM 1600 us se muestra como referencia medida.
% No corresponde al ensayo actual de PWM 2000 us.
% ============================================================

script_dir = fileparts(mfilename('fullpath'));
addpath(script_dir);
cfg = marvolt_load_config();

m = cfg.physical.buoy.moving_assembly_dry_mass_kg; % kg, masa seca
c = cfg.mechanical.buoy_model.damping_N_s_per_m;   % provisional
k = cfg.mechanical.buoy_model.stiffness_N_per_m;   % provisional
alpha = cfg.mechanical.buoy_model.force_height_coefficient_N_per_m; % provisional
H = cfg.experiment.wave_height_scenarios_m(2); % escenario medio, no medido
F0 = alpha * H;

omega_ref = cfg.experiment.measured_servo_reference.angular_velocity_rad_s;
omega_n = sqrt(k/m);
zeta = c / (2*sqrt(k*m));

% Barrido amplio para visualizar la respuesta del modelo matemático.
% No identifica la resonancia física mientras c y k no estén calibrados.
omega_values = linspace(0.1, 1.2*omega_n, 180);

% Amplitud estacionaria exacta del SDOF lineal ante fuerza sinusoidal:
% X = F0 / sqrt((k-m*w^2)^2 + (c*w)^2)
denominator = sqrt((k - m.*omega_values.^2).^2 + (c.*omega_values).^2);
amplitudes = F0 ./ denominator;
frequency_hz = omega_values / (2*pi);

fprintf('\nModelo 03 - Barrido matemático de frecuencia\n');
fprintf('Masa seca usada: %.5f kg; masa añadida no incluida.\n', m);
fprintf('omega_n calculada con k provisional: %.3f rad/s (%.3f Hz).\n', omega_n, omega_n/(2*pi));
fprintf('zeta calculada con c provisional: %.3f.\n', zeta);
fprintf('Referencia medida a PWM 1600 us: %.3f rad/s (%.4f Hz).\n', omega_ref, omega_ref/(2*pi));
fprintf('PWM 2000 us: velocidad todavía no medida.\n');
fprintf('No interpretar este barrido como una curva validada del prototipo.\n\n');

figure;
plot(frequency_hz, amplitudes, 'LineWidth', 1.5);
hold on;
y_limits = ylim;
plot([omega_ref/(2*pi), omega_ref/(2*pi)], y_limits, '--');
hold off;
grid on;
xlabel('Frecuencia de excitación (Hz)');
ylabel('Amplitud estacionaria calculada (m)');
title('MARVOLT-PA - Respuesta en frecuencia del modelo provisional');
legend('Respuesta matemática', 'Referencia medida PWM 1600 us', 'Location', 'best');
