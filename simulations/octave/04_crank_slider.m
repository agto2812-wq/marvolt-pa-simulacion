clc;
clear;
close all;

% ============================================================
% MODELO 04 - CINEMATICA IDEAL DE BIELA-MANIVELA
% r = radio medido del prototipo.
% L = valor heredado del código; verificar físicamente.
% omega = medición de referencia a PWM 1600 us, bajo carga ligera.
% No usar esta velocidad como RPM de PWM 2000 us.
% ============================================================

script_dir = fileparts(mfilename('fullpath'));
addpath(script_dir);
cfg = marvolt_load_config();

r = cfg.mechanical.mechanism.crank_radius_m; % medido: 0.034 m
L = cfg.mechanical.mechanism.connecting_rod_length_m; % provisional: 0.12 m
omega = cfg.experiment.measured_servo_reference.angular_velocity_rad_s; % PWM 1600 us
pwm_reference = cfg.experiment.measured_servo_reference.pwm_us;

if L <= r
  error('La longitud de biela L debe ser mayor que el radio r.');
endif

T_reference = 2*pi/omega;
t = linspace(0, 5*T_reference, 2000);
theta = omega*t;

% Posición ideal del carro para una geometría slider-crank clásica.
x = r.*cos(theta) + sqrt(L^2 - (r.*sin(theta)).^2);
v = gradient(x, t);
a = gradient(v, t);

rpm = omega*60/(2*pi);
stroke_geom = max(x) - min(x);

fprintf('\nModelo 04 - Cinemática ideal del mecanismo\n');
fprintf('Radio medido: %.4f m\n', r);
fprintf('Longitud de biela provisional: %.4f m\n', L);
fprintf('Referencia de velocidad: PWM %d us\n', pwm_reference);
fprintf('Velocidad de referencia: %.3f rad/s = %.2f RPM\n', omega, rpm);
fprintf('Carrera geométrica ideal calculada: %.4f m\n', stroke_geom);
fprintf('La carrera efectiva de la paleta no ha sido medida.\n');
fprintf('Esta simulación no incluye flexibilidad del soporte ni interacción fluido-paleta.\n\n');

figure;
plot(t, x, 'LineWidth', 1.5);
grid on;
xlabel('Tiempo (s)');
ylabel('Posición del carro (m)');
title('MARVOLT-PA - Posición cinemática ideal');

figure;
plot(t, v, 'LineWidth', 1.5);
grid on;
xlabel('Tiempo (s)');
ylabel('Velocidad del carro (m/s)');
title('MARVOLT-PA - Velocidad cinemática ideal');

figure;
plot(t, a, 'LineWidth', 1.5);
grid on;
xlabel('Tiempo (s)');
ylabel('Aceleración del carro (m/s^2)');
title('MARVOLT-PA - Aceleración cinemática ideal');
