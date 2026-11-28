clc;
clear;
close all;

% ============================================================
% MODELO 04 - CINEMATICA IDEAL DE BIELA-MANIVELA
% r = radio medido del prototipo.
% L = valor heredado del codigo; verificar fisicamente.
% omega = medicion de referencia a PWM 1600 us, bajo carga ligera.
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

% Posicion ideal del carro para una geometria slider-crank clasica.
q = sqrt(L^2 - (r .* sin(theta)).^2);

% Posicion del mecanismo
x = r .* cos(theta) + q;

% Primera derivada respecto al angulo
dx_dtheta = -r .* sin(theta) ...
    - (r^2 .* sin(theta) .* cos(theta)) ./ q;

% Segunda derivada respecto al angulo
d2x_dtheta2 = -r .* cos(theta) ...
    - (r^2 .* cos(2 .* theta)) ./ q ...
    - (r^4 .* sin(theta).^2 .* cos(theta).^2) ./ (q.^3);

% Velocidad y aceleracion lineales
v = omega .* dx_dtheta;
a = (omega^2) .* d2x_dtheta2;

rpm = omega*60/(2*pi);
stroke_geom = max(x) - min(x);

fprintf('\nModelo 04 - Cinematica ideal del mecanismo\n');
fprintf('Radio medido: %.4f m\n', r);
fprintf('Longitud de biela provisional: %.4f m\n', L);
fprintf('Referencia de velocidad: PWM %d us\n', pwm_reference);
fprintf('Velocidad de referencia: %.3f rad/s = %.2f RPM\n', omega, rpm);
fprintf('Carrera geometrica ideal calculada: %.4f m\n', stroke_geom);
fprintf('La carrera efectiva de la paleta no ha sido medida.\n');
fprintf('Esta simulacion no incluye flexibilidad del soporte ni interaccion fluido-paleta.\n\n');

figure;
plot(t, x, 'LineWidth', 1.5);
grid on;
xlabel('Tiempo (s)');
ylabel('Posicion del carro (m)');
title('MARVOLT-PA - Posicion cinematica ideal');

print(gcf, ...
 'results/figures/octave/modelo_04_posicion.png', ...
 '-dpng', '-r200');

figure;
plot(t, v, 'LineWidth', 1.5);
grid on;
xlabel('Tiempo (s)');
ylabel('Velocidad del carro (m/s)');
title('MARVOLT-PA - Velocidad cinematica ideal');

print(gcf, ...
 'results/figures/octave/modelo_04_velocidad.png', ...
 '-dpng', '-r200');

figure;
plot(t, a, 'LineWidth', 1.5);
grid on;
xlabel('Tiempo (s)');
ylabel('Aceleracion del carro (m/s^2)');
title('MARVOLT-PA - Aceleracion cinematica ideal');

print(gcf, ...
 'results/figures/octave/modelo_04_aceleracion.png', ...
 '-dpng', '-r200');

