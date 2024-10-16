function out = pmsm()
    %#codegen

    % motor parameters
    obj.Ld = 2.85e-3; % d-axis inductance
    obj.Lq = 2.85e-3; % q-axis inductance
    obj.Rs = 0.054; % stator resistance 
    obj.psi_r = 0.8603; % rotor flux linkage
    obj.np = 3; % number of pole pairs
    obj.J = 0.25; % rotor inertia
    
    t = set_time(0, 1, 10e-6);
    u = set_input(2, 100, 0.1, t);
    
    x0 = [0; 0; 0];
               
    x = zeros(3, length(t));
    x(:, 1) = x0; 
    t1 = tic;
    for i = 1:length(t) - 1
        % Runge-Kutta 4th order solver
        h = t(i + 1) - t(i);
        k1 = h * fx(obj, x(:, i), u(:, i));
        k2 = h * fx(obj, x(:, i) + 0.5 * k1, u(:, i));
        k3 = h * fx(obj, x(:, i) + 0.5 * k2, u(:, i));
        k4 = h * fx(obj, x(:, i) + k3, u(:, i));
        x(:, i + 1) = x(:, i) + (k1 + 2 * k2 + 2 * k3 + k4) / 6;
    end 
    t2 = toc(t1);
    fprintf('Elapsed time is %.2f ms\n', t2 * 1e3);
    
    out.t = t;
    out.u = u; 
    out.x = x; 
end

%% Functions

function t = set_time(start, finish, step)
    t = start:step:finish;
end

function u = set_input(idx, val, t0, t)
    u = zeros(3, length(t));
    u(idx, t >= t0) = val;
end

function xdot = fx(obj, x, u)
    % state-space model of PMSM
    % x = [id; iq; omega]
    % u = [v_d; v_q]
    id = x(1);
    iq = x(2);
    omega = x(3);
    vd = u(1);
    vq = u(2);
    Tl = u(3);
    
    % d-axis voltage equation
    id_dot = (1 / obj.Ld) * (vd - obj.Rs * id + obj.np * omega * obj.Lq * iq);
    iq_dot = (1 / obj.Lq) * (vq - obj.Rs * iq - obj.np * omega * (obj.Ld * id + obj.psi_r));
    omega_dot = (1 / obj.J) * ( 3 * obj.np / 2 * (obj.psi_r * iq + (obj.Ld - obj.Lq) * id * iq) - Tl); 

    xdot = [id_dot; iq_dot; omega_dot];
end
