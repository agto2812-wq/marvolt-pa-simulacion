clc;
clear;
close all;

m = 50;
c = 20;
k = 50;   % reducido

omega = 2;

tspan = [0 50];
x0 = [0; 0];

H_values = [0.5, 1, 1.5, 2];

figure;
hold on;
grid on;

for i = 1:length(H_values)
    
    H = H_values(i);
    F0 = 500 * H;  % aumentado
    
    f = @(t, x) [
        x(2);
        (1/m)*(F0*sin(omega*t) - c*x(2) - k*x(1))
    ];
    
    [t, x] = ode45(f, tspan, x0);
    
    disp(max(abs(x(:,1)))) % diagnóstico
    
    plot(t, x(:,1), 'LineWidth', 2, ...
        'DisplayName', ["H = ", num2str(H), " m"]);
end

xlabel('Tiempo (s)');
ylabel('Desplazamiento de la boya (m)');
title('Respuesta de la boya');
legend;