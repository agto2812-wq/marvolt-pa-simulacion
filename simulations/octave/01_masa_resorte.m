clc;
clear;
close all;

% ============================================================
% MODELO 01 - RESPUESTA TEMPORAL DE LA BOYA
% Primera aproximación: se usa masa seca del conjunto móvil.
% La masa añadida del agua aún no está caracterizada.
% c, k y alpha continúan siendo valores provisionales.
% Caso de referencia de velocidad: PWM 1600 us (NO PWM 2000 us).
% ============================================================

script_dir = fileparts(mfilename('fullpath'));
addpath(script_dir);
cfg = marvolt_load_config();

% Masa seca experimental del conjunto móvil [kg]
m = cfg.physical.buoy.moving_assembly_dry_mass_kg;

% Parámetros aún provisionales
c = cfg.mechanical.buoy_model.damping_N_s_per_m;
k = cfg.mechanical.buoy_model.stiffness_N_per_m;
alpha = cfg.mechanical.buoy_model.force_height_coefficient_N_per_m;

% Escenario medio de altura de ola teórica [m], no medida
H = cfg.experiment.wave_height_scenarios_m(2);
F0 = alpha * H;  % Fuerza provisional [N]

% Frecuencia angular de referencia medida a PWM 1600 us [rad/s]
omega = cfg.experiment.measured_servo_reference.angular_velocity_rad_s;


% Diagnóstico del modelo lineal con los parámetros actuales
omega_n = sqrt(k / m);
f_n = omega_n / (2*pi);
c_crit = 2 * sqrt(k*m);
zeta = c / c_crit;

fprintf('\nMARVOLT-PA - Modelo 01: respuesta temporal preliminar\n');
fprintf('Masa seca usada: %.5f kg (masa añadida NO incluida)\n', m);
fprintf('Nota: la velocidad PWM 1600 us es una referencia de otro ensayo; no se combina como dato de la prueba PWM 2000 us.\n');
fprintf('Altura de ola teórica usada: %.4f m (no medida)\n', H);
fprintf('Velocidad de referencia: %.3f rad/s (PWM 1600 us)\n', omega);
fprintf('Frecuencia natural calculada con c y k provisionales: %.3f rad/s (%.3f Hz)\n', omega_n, f_n);
fprintf('Razón de amortiguamiento calculada con c y k provisionales: %.3f\n', zeta);
fprintf('ADVERTENCIA: resultado exploratorio; no es una calibración experimental.\n\n');

% Tiempo y condiciones iniciales
tspan = [0 60];
x0 = [0; 0];

% Sistema diferencial: m*x'' + c*x' + k*x = F0*sin(omega*t)
f = @(t, x) [ ...
  x(2); ...
  (F0*sin(omega*t) - c*x(2) - k*x(1)) / m ...
];

[t, x] = ode45(f, tspan, x0);
desplazamiento = x(:,1);
velocidad = x(:,2);

figure;
plot(t, desplazamiento, 'LineWidth', 1.5);
grid on;
xlabel('Tiempo (s)');
ylabel('Desplazamiento vertical de la boya (m)');
title('MARVOLT-PA - Respuesta temporal preliminar (PWM 1600 us de referencia)');
legend('Boya');

figure;
plot(t, velocidad, 'LineWidth', 1.5);
grid on;
xlabel('Tiempo (s)');
ylabel('Velocidad vertical (m/s)');
title('MARVOLT-PA - Velocidad vertical preliminar de la boya');
