amplitudes = [];

for i = 1:length(H_values)
    
    H = H_values(i);
    F0 = 100 * H;
    
    f = @(t, x) [
        x(2);
        (1/m)*(F0*sin(omega*t) - c*x(2) - k*x(1))
    ];
    
    [t, x] = ode45(f, tspan, x0);
    
    % Tomar amplitud máxima en régimen estacionario
    x_ss = x(round(end*0.7):end, 1);
    amp = max(abs(x_ss));
    
    amplitudes = [amplitudes amp];
end

figure;
plot(H_values, amplitudes, '-o');
xlabel('Altura de ola (m)');
ylabel('Amplitud de la boya (m)');
title('Altura de ola vs desplazamiento de la boya');
grid on;
omega_values = linspace(0.5, 5, 20);
