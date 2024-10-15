%% creating a PMSM class
classdef PMSM
    properties
        % motor parameters
        Ld = 2.85e-3; % d-axis inductance
        Lq = 2.85e-3; % q-axis inductance
        Rs = 0.054; % stator resistance 
        psi_r = 0.8603; % rotor flux linkage
        np = 3; % number of pole pairs
        J = 0.25; % rotor inertia
        
        % t; % time vector
        t = 0; % time vector
        u = 0; % input vector
        x = 0; % state vector
        integrator = 0; % solver object
        % u; % input vector
        % x; % state vector
        % integrator; % solver object
    end
    methods
        function obj = PMSM(Ld, Lq, Rs, psi_r, np, J)
            % constructor
            if nargin > 0
                obj.Ld = Ld;
                obj.Lq = Lq;
                obj.Rs = Rs;
                obj.psi_r = psi_r;
                obj.np = np;
                obj.J = J;
            end
        end

        function obj = set_time(obj, start, finish, step)
            obj.t = start:step:finish;
        end

        function obj = set_input(obj, idx, val, t0)
            obj.u = zeros(3, length(obj.t));
            obj.u(idx, obj.t >= t0) = val;
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

        function obj = simulate(obj, x0)
            % initial conditions
            obj.x = zeros(3, length(obj.t));
            obj.x(:, 1) = x0;

            obj.integrator = RunkeKutta4();
            obj.integrator = obj.integrator.set_func(@(x, u) obj.fx(x, u));
            obj.x = obj.integrator.integrare(obj.t, obj.x, obj.u);
        end 
    end
end