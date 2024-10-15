%% creating Runge-Kutta 4th order solver class
classdef RunkeKutta4
    properties
        func = @(x, u) [0; 0; 0]; % function to be solved 
    end

    methods
        function obj = set_func(obj, func)
            obj.func = func;
        end

        function x = integrare(obj, t, x, u)
            
            for i = 1:length(t) - 1
                % Runge-Kutta 4th order solver
                h = t(i + 1) - t(i);
                k1 = h * obj.func(x(:, i), u(:, i));
                k2 = h * obj.func(x(:, i) + 0.5 * k1, u(:, i));
                k3 = h * obj.func(x(:, i) + 0.5 * k2, u(:, i));
                k4 = h * obj.func(x(:, i) + k3, u(:, i));
                x(:, i + 1) = x(:, i) + (k1 + 2 * k2 + 2 * k3 + k4) / 6;
            end 
        end
    end
end 