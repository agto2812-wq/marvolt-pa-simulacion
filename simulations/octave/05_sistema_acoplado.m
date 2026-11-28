clc;
clear;
close all;

% ============================================================
% MODELO 05 - ACOPLAMIENTO PRELIMINAR OLA -> BOYA -> ROTOR
%
% Correccion conceptual:
% El mecanismo biela-manivela del servomotor acciona la PALETA
% para generar las olas. No esta conectado en serie entre la boya
% y el generador electrico.
%
% La boya esta conectada directamente al rotor portaimanes.
% Por tanto, en esta aproximacion x_rotor = x_boya.
%
% La entrada de oleaje sigue siendo F0=alpha*H, con alpha provisional.
% La frecuencia usada es solo una referencia basada en PWM 1600 us;
% no representa la prueba actual a PWM 2000 us.
% No se calcula FEM ni potencia: faltan parametros electromagneticos.
% ============================================================

script_dir = fileparts(mfilename('fullpath'));
addpath(script_dir);
cfg = marvolt_load_config();

% Parametros fisicos/experimentales
m = cfg.physical.buoy.moving_assembly_dry_mass_kg; % masa seca, kg
H = cfg.experiment.wave_height_scenarios_m(2);       % escenario teorico medio, m
omega_wave = cfg.experiment.measured_servo_reference.angular_velocity_rad_s; % referencia PWM 1600

g = cfg.physical.constants.gravity_m_s2;

% Profundidad operativa del tanque [m]
h = cfg.physical.tank.operating_water_depth_m;

% Parametros provisionales del modelo de boya
c = cfg.mechanical.buoy_model.damping_N_s_per_m;
k = cfg.mechanical.buoy_model.stiffness_N_per_m;
alpha = cfg.mechanical.buoy_model.force_height_coefficient_N_per_m;
F0 = alpha*H;

% Supuesto pendiente de validar: un ciclo de ola por revolucion del actuador.
T_reference = 2*pi/omega_wave;
c_wave = sqrt(g*h);                  % aguas someras, aproximacion teorica
lambda_reference = c_wave*T_reference;

tspan = [0 60];
x0 = [0; 0];

f = @(t, state) [ ...
  state(2); ...
  (F0*sin(omega_wave*t) - c*state(2) - k*state(1)) / m ...
];

[t, state] = ode45(f, tspan, x0);
x_boya = state(:,1);
v_boya = state(:,2);

% El rotor portaimanes esta acoplado directamente a la boya.
x_rotor = x_boya;
v_rotor = v_boya;

fprintf('\nModelo 05 - Acoplamiento preliminar boya-rotor\n');
fprintf('Masa seca modelada: %.5f kg (masa anadida no incluida).\n', m);
fprintf('Altura de ola usada: %.4f m (escenario teorico, no medido).\n', H);
fprintf('Frecuencia angular usada: %.3f rad/s (referencia PWM 1600 us).\n', omega_wave);
fprintf('Periodo de referencia: %.3f s.\n', T_reference);
fprintf('Este resultado no representa la prueba PWM 2000 us.\n');
fprintf('No se calcula longitud de onda de la prueba PWM 2000, FEM ni potencia electrica.\n\n');

figure;
plot(t, x_boya, 'LineWidth', 1.5);
hold on;
plot(t, x_rotor, '--', 'LineWidth', 1.2);
hold off;
grid on;
xlabel('Tiempo (s)');
ylabel('Desplazamiento vertical (m)');
title('MARVOLT-PA - Movimiento preliminar de boya y rotor');
legend('Boya', 'Rotor portaimanes', 'Location', 'best');

print(gcf, ...
 'results/figures/octave/modelo_05_boya_rotor.png', ...
 '-dpng', '-r200');

figure;
plot(t, v_boya, 'LineWidth', 1.5);
grid on;
xlabel('Tiempo (s)');
ylabel('Velocidad vertical (m/s)');
title('MARVOLT-PA - Velocidad del conjunto boya-rotor');

print(gcf, ...
 'results/figures/octave/modelo_05_velocidad.png', ...
 '-dpng', '-r200');
