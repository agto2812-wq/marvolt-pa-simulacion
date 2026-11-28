clc;
clear;
close all;

% ============================================================
% MODELO 03 - RESPUESTA EN FRECUENCIA
% Analisis de sensibilidad del SDOF lineal con parametros provisionales.
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

% Barrido amplio para visualizar la respuesta del modelo matematico.
% No identifica la resonancia fisica mientras c y k no esten calibrados.
omega_values = linspace(0.1, 1.2*omega_n, 180);

% Amplitud estacionaria exacta del SDOF lineal ante fuerza sinusoidal:
% X = F0 / sqrt((k-m*w^2)^2 + (c*w)^2)
denominator = sqrt((k - m.*omega_values.^2).^2 + (c.*omega_values).^2);
amplitudes = F0 ./ denominator;
frequency_hz = omega_values / (2*pi);

fprintf('\nModelo 03 - Barrido matematico de frecuencia\n');
fprintf('Masa seca usada: %.5f kg; masa anadida no incluida.\n', m);
fprintf('omega_n calculada con k provisional: %.3f rad/s (%.3f Hz).\n', omega_n, omega_n/(2*pi));
fprintf('zeta calculada con c provisional: %.3f.\n', zeta);
fprintf('Referencia medida a PWM 1600 us: %.3f rad/s (%.4f Hz).\n', omega_ref, omega_ref/(2*pi));
fprintf('PWM 2000 us: velocidad todavia no medida.\n');
fprintf('No interpretar este barrido como una curva validada del prototipo.\n\n');

figure;
semilogx(frequency_hz, amplitudes, 'LineWidth', 1.5);
hold on;

f_ref = omega_ref / (2*pi);
y_limits = ylim;

semilogx([f_ref, f_ref], y_limits, '--', ...
         'LineWidth', 1.2);

hold off;
grid on;

xlabel('Frecuencia de excitacion (Hz, escala logaritmica)');
ylabel('Amplitud estacionaria calculada (m)');
title('MARVOLT-PA - Respuesta en frecuencia del modelo provisional');

legend('Respuesta matematica', ...
       'Referencia medida PWM 1600 us', ...
       'Location', 'northeast');

print(gcf, ...
 'results/figures/octave/modelo_03_frecuencia.png', ...
 '-dpng', '-r200');
