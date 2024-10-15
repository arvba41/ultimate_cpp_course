clear; clc; 

%% creating a PMSM object and simulate it 
pmsm = PMSM;
pmsm = pmsm.set_time(0, 1, 10e-6);
pmsm = pmsm.set_input(2, 100, 0.1);

x0 = [0; 0; 0];
t1 = tic;
pmsm = pmsm.simulate(x0);
t2 = toc(t1);
fprintf('Elapsed time is %.2f ms\n', t2 * 1e3);

%% plot the results
figure;

tiledlayout(3, 2);

nexttile;
plot(pmsm.t, pmsm.u(1, :));
title('d-axis voltage');
ylabel('v_d (V)');
xlabel('time (s)');
grid on;

nexttile;
plot(pmsm.t, pmsm.x(1, :));
title('d-axis current');
ylabel('i_d (A)');
xlabel('time (s)');
grid on;

nexttile;
plot(pmsm.t, pmsm.u(2, :));
title('q-axis voltage');
ylabel('v_q (V)');
xlabel('time (s)');
grid on;

nexttile;
plot(pmsm.t, pmsm.x(2, :));
title('q-axis current');
ylabel('i_q (A)');
xlabel('time (s)');
grid on;

nexttile;
plot(pmsm.t, pmsm.u(3, :));
title('load torque');
ylabel('T_l (Nm)');
xlabel('time (s)');
grid on;

nexttile;
plot(pmsm.t, pmsm.x(3, :));
title('speed');
ylabel('\omega (rad/s)');
xlabel('time (s)');
grid on;

%% save the results as a json file

% create a struct to save the results
results = struct;
results.t = pmsm.t;
results.u = pmsm.u;
results.x = pmsm.x;

% save the results as a json file
json = jsonencode(results);
fid = fopen('matlab_results.json', 'w');
fwrite(fid, json, 'char');
fclose(fid);

% copy files to ../results folder
copyfile('matlab_results.json', '../results/matlab_results.json');