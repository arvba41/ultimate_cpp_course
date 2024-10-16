clear; clc; 

%% 
% % motor parameters
% obj.Ld = 2.85e-3; % d-axis inductance
% obj.Lq = 2.85e-3; % q-axis inductance
% obj.Rs = 0.054; % stator resistance 
% obj.psi_r = 0.8603; % rotor flux linkage
% obj.np = 3; % number of pole pairs
% obj.J = 0.25; % rotor inertia
% 
% t = set_time(0, 1, 10e-6);
% u = set_input(2, 100, 0.1, t);
% 
% x0 = [0; 0; 0];
% 
% x = zeros(3, length(t));
% x(:, 1) = x0; 
% t1 = tic;
% for i = 1:length(t) - 1
%     % Runge-Kutta 4th order solver
%     h = t(i + 1) - t(i);
%     k1 = h * fx(obj, x(:, i), u(:, i));
%     k2 = h * fx(obj, x(:, i) + 0.5 * k1, u(:, i));
%     k3 = h * fx(obj, x(:, i) + 0.5 * k2, u(:, i));
%     k4 = h * fx(obj, x(:, i) + k3, u(:, i));
%     x(:, i + 1) = x(:, i) + (k1 + 2 * k2 + 2 * k3 + k4) / 6;
% end 
% t2 = toc(t1);
% fprintf('Elapsed time is %.2f ms\n', t2 * 1e3);
% 
% out.t = t;
% out.u = u; 
% out.x = x; 

out = pmsm();

%%
figure;

tiledlayout(3, 2);

nexttile;
plot(out.t, out.u(1, :));
title('d-axis voltage');
ylabel('v_d (V)');
xlabel('time (s)');
grid on;

nexttile;
plot(out.t, out.x(1, :));
title('d-axis current');
ylabel('i_d (A)');
xlabel('time (s)');
grid on;

nexttile;
plot(out.t, out.u(2, :));
title('q-axis voltage');
ylabel('v_q (V)');
xlabel('time (s)');
grid on;

nexttile;
plot(out.t, out.x(2, :));
title('q-axis current');
ylabel('i_q (A)');
xlabel('time (s)');
grid on;

nexttile;
plot(out.t, out.u(3, :));
title('load torque');
ylabel('T_l (Nm)');
xlabel('time (s)');
grid on;

nexttile;
plot(out.t, out.x(3, :));
title('speed');
ylabel('\omega (rad/s)');
xlabel('time (s)');
grid on;

