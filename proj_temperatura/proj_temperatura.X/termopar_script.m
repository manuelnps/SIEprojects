close all
clear all
clc

%% Thermocouple Linearity
thermocouple_arr = [0.000, 0.198, 0.397,  0.597, 0.798,  1.000,  1.203, 1.407, 1.612, 1.817, 2.023, 2.230,  2.436, 2.644,  2.851, 3.059, 3.267, 3.474, 3.682, 3.889, 4.096] * 1e-3
temp_arr = 0 : 5 : 100;

% instrumentation amplifier
Vref = 0.499;
RG = 181;
G = (100e3 / RG) + 1

Vo_In_Amp = G * thermocouple_arr + Vref;

% difference amplifier
G_diff = 2.437;         % V/V
V_off_diff = 1.005;

Vo = G_diff * (Vo_In_Amp - V_off_diff);

%% Linear Regression
mdl = fitlm(thermocouple_arr, temp_arr, 'linear');

table = table2array(mdl.Coefficients);
m = table(2,1)
b = table(1,1)

r2 = mdl.Rsquared.Ordinary

y_fit = m * temp_arr + b;

figure()
plot(temp_arr, thermocouple_arr, LineStyle="none", Marker="o", MarkerSize=7, MarkerEdgeColor='r')
hold on
plot(temp_arr, thermocouple_arr, LineStyle="--", Color='b', LineWidth=1.2)
grid on
xlabel('Temperature Difference °C'), ylabel('Thermocouple Voltage (V)')
eq_txt = sprintf("T = %.4f × V + %.4f", m, b);
legend(["Reference Data" eq_txt])
text(61, 0.00373,['r^{2} = ' num2str(r2)])

mdl = fitlm(temp_arr, Vo, 'linear');
table = table2array(mdl.Coefficients);
m2 = table(2,1)
b2 = table(1,1)

r2 = mdl.Rsquared.Ordinary

y_fit = m2 * temp_arr + b2;

figure()
plot(temp_arr, Vo, LineStyle="--", Color='b', LineWidth=1.4)
grid on
xlabel('Temperature Difference °C'), ylabel('Conditioning Circuit Output Voltage (V)')
ylim([0 3.6])

eq_txt = sprintf("T = %.5f × V - %.5f", m2, abs(b2));
legend([eq_txt])
text(55, 3.15,['r^{2} = ' num2str(r2)])