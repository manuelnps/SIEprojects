% clear all
% close all
% 
% load dados.mat time temp;
% 
% temp_filtered=movmean(temp,5);
% %time
% data=[time,temp];
% plot(time,temp_filtered);
% title("Plot resposta ao degrau filtrado")
% Ts=5;
% deadTime = 1;
% sys= tfest(data,1,0,deadTime,'Ts',Ts);
% 
% wc=0.05
% controller = pidtune(sys,'PI',wc)
% closed_loop_sys = feedback(sys, controller, 1)
% 
% timeToShow=0:Ts:500;
% 
% ref = [ (zeros(1, round(length(timeToShow)/10))) (40*ones(1, length(timeToShow) - round(length(timeToShow)/10))) ]';  
% lsim(closed_loop_sys, ref, timeToShow)

% 
% 

clear all
close all

readtable('dadosPI3.csv');
time = ans.Var1;
temp = ans.Var2;

temp_filtered=movmean(temp,5);
%time
data=[time,temp];
plot(time,temp_filtered);
title("Plot resposta ao degrau filtrado")
Ts=5;
deadTime = 1;
sys= tfest(data,1,0,deadTime,'Ts',Ts);

wc=0.01
controller = pidtune(sys,'PI',wc)
closed_loop_sys = feedback(sys, controller, 1)

timeToShow=0:Ts:500;

ref = [ (zeros(1, round(length(timeToShow)/10))) (40*ones(1, length(timeToShow) - round(length(timeToShow)/10))) ]';  
lsim(closed_loop_sys, ref, timeToShow)
