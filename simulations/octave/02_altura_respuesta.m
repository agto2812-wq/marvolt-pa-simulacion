clc;
clear;
close all;

% ============================================================
% MODELO 02 - ESCENARIOS DE ALTURA DE OLA VS RESPUESTA
% Las alturas son proyecciones teoricas basadas en S_geom=0.068 m.
% No son alturas medidas por el HC-SR04.
% c, k y alpha continuan siendo provisionales.
% ============================================================

script_dir = fileparts(mfilename('fullpath'));
addpath(script_dir);
cfg = marvolt_load_config();

m = cfg.physical.buoy.moving_assembly_dry_mass_kg; % kg, masa seca
c = cfg.mechanical.buoy_model.damping_N_s_per_m;   % provisional
k = cfg.mechanical.buoy_model.stiffness_N_per_m;   % provisional
alpha = cfg.mechanical.buoy_model.force_height_coefficient_N_per_m; % provisional
omega = cfg.experiment.measured_servo_reference.angular_velocity_rad_s; % PWM 1600 us

H_values = cfg.experiment.wave_height_scenarios_m(:).'; % escenarios teoricos, no medidos
tspan = [0 60];
x0 = [0; 0];
amplitudes = zeros(size(H_values));

for idx = 1:numel(H_values)
  H = H_values(idx);
  F0 = alpha * H;

  f = @(t, x) [ ...
    x(2); ...
    (F0*sin(omega*t) - c*x(2) - k*x(1)) / m ...
  ];

  [t, x] = ode45(f, tspan, x0);
  steady_mask = t >= 0.7 * tspan(2);
  x_steady = x(steady_mask, 1);
  amplitudes(idx) = max(abs(x_steady));
endfor

fprintf('\nModelo 02 - Escenarios teoricos, no mediciones\n');
for idx = 1:numel(H_values)
  fprintf('H = %.4f m -> amplitud simulada = %.5f m\n', H_values(idx), amplitudes(idx));
endfor
fprintf('La fuerza F0=alpha*H usa alpha provisional; no interpretar como prediccion validada.\n\n');

figure;
plot(H_values, amplitudes, '-o', 'LineWidth', 1.5);
grid on;
ylim([0, 1.1 * max(amplitudes)]);
xlabel('Altura de ola H (m) - escenario teorico');
ylabel('Amplitud maxima simulada de la boya (m)');
title('MARVOLT-PA - Sensibilidad preliminar a la altura de ola');

print(gcf, ...
 'results/figures/octave/modelo_02_altura_respuesta.png', ...
 '-dpng', '-r200');
