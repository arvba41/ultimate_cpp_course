function [oplsol,elps] = rl_prtcl_obsavoid_elp(mdl,obst,cons,optops)
% Friction-limited Particle with Obstacle Avoidance using CasADi:
% ----------------------
% An optimal control problem (OCP),
% solved with direct multiple-shooting.
%
% For more information see: http://labs.casadi.org/OCP

Xfin = mdl.Xfin;    % finishing position x
mass = mdl.mass;    % mass of the car
mu = mdl.mu;        % friction
g = mdl.g;          % acceleration due to gravity

% obstacle parameters
Xa = obst.Xa;       % center of the elipse
R1 = obst.R1;       % radius of elipse in x direction
R2 = obst.R2;       % radius of elipse in x direction
pow = obst.pow;     % curvature of elipse 

Ymax = cons.Ymax;   % upper limit for the y direction
delta_max = cons.delta_max; % max force in y 
ddelta_max = cons.ddelta_max; % max jerk in y
vxinit = cons.vxinit; % initial speed in the x


N = optops.N;        % number of control intervals for the optimizatio

opti = casadi.Opti(); % Optimization problem

% ---- decision variables ---------
X = opti.variable(6,N+1); % state trajectory
xpos = X(1,:);
ypos = X(2,:);
xspeed = X(3,:);
yspeed = X(4,:);
delta = X(5,:); 
ddelta = X(6,:); 
U = opti.variable(2,N);   % control trajectory (throttle)
T = opti.variable();      % final time

% ---- objective          ---------
opti.minimize(T);         % minimize time

% ---- dynamic constraints --------
% x(1) --> x
% x(2) --> y
% x(3) --> vx
% x(4) --> vy
% x(5) --> delta 
% x(6) --> delta'
f = @(x,u) [x(3);... x' = vx
            x(4);... y' = vy
            u(1)*cos(x(5))/mass;... vx' = u1*cos(delta)/m
            u(1)*sin(x(5))/mass;... vy' = u1*sin(delta)/m
            u(2);... % delta' = u2/m
            x(5)]; % delta'' = delta'
% dx/dt = f(x,u)

dt = T/N; % length of a control interval
for k=1:N % loop over control intervals
   % Runge-Kutta 4 integration
   k1 = f(X(:,k),         U(:,k));
   k2 = f(X(:,k)+dt/2*k1, U(:,k));
   k3 = f(X(:,k)+dt/2*k2, U(:,k));
   k4 = f(X(:,k)+dt*k3,   U(:,k));
   x_next = X(:,k) + dt/6*(k1+2*k2+2*k3+k4); 
   opti.subject_to(X(:,k+1)==x_next); % close the gaps
end

% ---- path constraints ------------
% y1curve = @(x) sin(2*pi*x/(2*Xfin))*2 + 5; % bottom road limits
% yucurve = @(x) sin(2*pi*x/(2*Xfin))*2 + 7; % upper road limits
elps = @(x,y) -((x-Xa)/R1).^pow -(y/R2).^pow + 1; % obstacle 

yobst = @(x) (1 -((x-Xa)/R1).^pow).^(1/pow)*R2; %% y value 
% opti.subject_to(ypos>y1curve(xpos)); % be above the lower limit
% opti.subject_to(ypos<yucurve(xpos)); % be below the upper limit

opti.subject_to(elps(xpos,ypos)<=0); % ensure that we are far from the elipse

% ---- force constraints -----------
Flim = U(1,:); % Force limit equation
opti.subject_to(-mass*mu*g<=Flim<=mass*mu*g); % Force is limited

opti.subject_to(-delta_max<=delta<=delta_max); % limit aceleration in y direction
opti.subject_to(-ddelta_max<=ddelta<=ddelta_max); % limit jerk in y direction

% ---- additional constrains ------
opti.subject_to(xspeed>0); % be above zero
% opti.subject_to(ypos>0); % be above zero

% ---- boundary conditions --------
% define all the initial values here 
opti.subject_to(xpos(1)==0);   % start at position x 0 ...
opti.subject_to(ypos(1)==1);   % start at position 0 ... 
opti.subject_to(xpos(N+1)==2*Xfin);  % finish line at position x 1
opti.subject_to(ypos(N+1)==ypos(1));  % finish line at position x 1
opti.subject_to(xspeed(1)==vxinit);   % start with speed x 0 ...
opti.subject_to(yspeed(1)==0);   % start with speed y 0 ...
opti.subject_to(delta(1)==0);   % start with acceleration x 0 ...
opti.subject_to(ddelta(1)==0);   % start with acceleration y 0 ...

% ---- misc. constraints  ----------
opti.subject_to(T>=0); % Time must be positive

% ---- initial values for solver ---
% define all the initial values here 
opti.set_initial(xpos, 0);
opti.set_initial(ypos, 1);
opti.set_initial(xspeed, vxinit);
opti.set_initial(yspeed, 0);
opti.set_initial(delta, 0);
opti.set_initial(ddelta, 0);
opti.set_initial(T, 1);

% ---- solve NLP              ------
opti.solver('ipopt'); % set numerical backend
sol = opti.solve();   % actual solve

%% converting to readable values
oplsol.x = sol.value(xpos); 
oplsol.y = sol.value(ypos);
oplsol.vx = sol.value(xspeed); 
oplsol.vy = sol.value(yspeed);
oplsol.delta = sol.value(delta); 
oplsol.ddelta = sol.value(ddelta);
oplsol.ux = sol.value(U(1,:)).*cos(sol.value(delta(2:end))); 
oplsol.uy = sol.value(U(1,:)).*sin(sol.value(delta(2:end)));
oplsol.u = sol.value(U(1,:));
oplsol.topt = sol.value(T); 
oplsol.tvec = linspace(0,sol.value(T),N+1);

oplsol.cons.Flim = mass*mu*g;
%% plotting
%%%% debugging
% oplsol.x = opti.debug.value(xpos); oplsol.y = opti.debug.value(ypos);
% oplsol.vx = opti.debug.value(xspeed); oplsol.vy = opti.debug.value(yspeed);
% oplsol.ay = opti.debug.value(delta); oplsol.jy = opti.debug.value(ddelta);
% oplsol.ux = opti.debug.value(U(1,:)); oplsol.uy = opti.debug.value(U(2,:));
% oplsol.topt = opti.debug.value(T); oplsol.tvec = linspace(0,opti.debug.value(T),N+1);
% 
% oplsol.cons.Flim = mass*mu*g; 
% oplsol.cons.obst = obst(oplsol.x,oplsol.y);
% oplsol.cons.yobst = yobst(oplsol.x);
% prob5.cons.obst = obst(linspace(0,2*Xfin,100),linspace(0,Yulim,100)); 

% prob5.cons.yllim = y1curve(prob5.x); 
% prob5.cons.yulim = yucurve(prob5.x);
% 
% f = figure(5); clf;
% optim_plot_prob5(oplsol);

