clear; clc; 

%% loading the simulink model and running it
simulation_mdl = 'simulink_main'; % the name of the simulink model
load_system(simulation_mdl); % load the model
simIN = Simulink.SimulationInput(simulation_mdl); % load the model
set_param(simulation_mdl, 'SimulationMode', 'normal'); % set the simulation mode to normal
tstart = tic; % start the timer
out = sim(simIN); % run the simulation
tstop = toc(tstart); % stop the timer
fprintf('Elapsed time is %.2f ms\n', tstop * 1e3);

%% saving the results as a json file
results = struct;
results.t = out.tout;
results.u = out.u;
results.x = out.x;

% save the results as a json file
json_file = 'simulink_nominal_results.json';
json_str = jsonencode(results);
fid = fopen(json_file, 'w');
fprintf(fid, json_str);
fclose(fid);

%% using the rapid accelerator to speed up the simulation
% the rapid accelerator is a feature in simulink that speeds up the simulation

% set the rapid accelerator
set_param(simulation_mdl, 'SimulationMode', 'rapid');
tstart = tic; % start the timer
out = sim(simIN); % run the simulation
tstop = toc(tstart); % stop the timer
fprintf('Elapsed time is %.2f ms\n', tstop * 1e3);

% save the results as a json file
results = struct;
results.t = out.tout;
results.u = out.u;
results.x = out.x;

% save the results as a json file
json_file = 'simulink_rapid_results.json';
json_str = jsonencode(results);
fid = fopen(json_file, 'w');
fprintf(fid, json_str);
fclose(fid);

% copy the results to the results folder
copyfile('simulink_nominal_results.json', '../results/simulink_nominal_results.json');
copyfile('simulink_rapid_results.json', '../results/simulink_rapid_results.json');