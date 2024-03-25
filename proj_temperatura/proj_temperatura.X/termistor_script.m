clear all
close all
clc

% coefficients
A = 1.595167649e-3;
B = 2.985448686e-4;
C = 1.413156023e-7;

R_10 = 580.87;
R_25 = 330;
R_30 = 276.441;

VDD = 3.29;
Rs = 333;

Vo_min = VDD * R_30 / (R_30 + Rs)
Vo_max = VDD * R_10 / (R_10 + Rs)

Vo = Vo_min : 0.01 : Vo_max;

Rs = 333;

T_max = 30;
T_min = 10;

res_ADC = 3.3 / 1023;
res = (Vo_max - Vo_min) / (T_max - T_min)   % em V / ºC

niveis = res / res_ADC  % níveis / ºC

precisao = 1/niveis % precisao décima de grau

T_Celsius = 10:0.1:30;
T = T_Celsius + 273.15;

V_to_R = @(V) ( (VDD ./ V - 1) ./ Rs).^-1;
steinhart = @(R) A + B*log(R) + C.*(log(R).^3);

% Vo = 1.51
Ro = V_to_R(Vo);
To_Kelvin = 1 ./ steinhart(Ro);
To_Celsius = To_Kelvin - 273.15

% RT = [ (VDD / V0 - 1) / R ]^-1 % convert voltage to resistor

%% plot and linear regression

% regressao linear
mdl = fitlm(Vo, To_Celsius, 'linear');
table = table2array(mdl.Coefficients);
m = table(2,1)
b = table(1,1)

r2 = mdl.Rsquared.Ordinary

y_fit = m * Vo + b;

figure()
plot(Vo, To_Celsius, '--b', LineWidth=1.3)
grid on
xlabel('Voltage (V)'); ylabel('Temperature ºC');
eq_txt = sprintf("y = %.4f × t + %.4f", m, b);
legend([eq_txt])
text(1.875, 24,['r^{2} = ' num2str(r2)])

xlim([min(Vo) max(Vo)])
ylim([min(To_Celsius)-0.1 max(To_Celsius)+0.1])