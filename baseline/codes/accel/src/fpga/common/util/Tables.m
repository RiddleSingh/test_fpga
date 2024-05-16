clear
clc
close all
%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% INPUTS
% Cores
cpu_cores = [1,8,48];
fpga_cores = [1,2,8];
% Prices
cpu_price=[0.119,0.952,5.712];
fpga_price=[1.65,3.3,13.2];
% Time
cpu_time=[37854.52,5303.73,803.63];
fpga_time=[482.30,242.06,62.63];
fpga_single_kernel_time = 1005.08; % 0.84;
cpu_single_economy_time = 37854.5; %31.55;
time_cpu_grids = [5303.73,9502.63,15432.15];    % cpu-8, 100, 200, 300
time_per_grids_fpga=[482.30,242.06,62.63; ...
                  671.28,336.54,86.64; ...
                    1057.53,529.75,134.78
                    ];

cpu_time_linear_search = 97348.3;
cpu_time_binary_search = 49667.3;
cpu_time_jump_search = 37854.5;
% Cost Savings
one_million_economies = 1000000;
% Energy
power_cpu = 8;
power_fpga = 33;
power_fpga_single_kernel = 22;

% Carbon Footprint
summit_power=13; % W
w_gas = 0.37;
w_coal = 0.26;
w_ren = 0.37;
weights = w_gas +w_coal + w_ren;
CO2_per_kWh_gas = 0.91;
CO2_per_kWh_coal = 2.21;
CO2_per_kWh_ren = 0.1;
assert(weights==1)

Tot_core_hour_summit = 150000000;
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% END INPUTS
%% Abstract
display("Abstract")
cpu_seconds = seconds(cpu_time(1));
cpu_seconds.Format = 'hh:mm'; 
fpga_seconds = seconds(fpga_time(1));
fpga_seconds.Format = 'mm:ss'; 
fprintf("Speedup %dx: from %s to %s \n",round(cpu_time(1)/fpga_time(1)),cpu_seconds,fpga_seconds);

%% Table 2
display("Table 2")
Table1_line1 = [cpu_time_linear_search,cpu_time_binary_search,cpu_time_jump_search]
Table1_line2 = [cpu_time_linear_search/cpu_time_binary_search,cpu_time_linear_search/cpu_time_jump_search]
%% Table 3
%%%%%%% SPEEDUP WITH RESPECT TO 1,2,8 FPGAs
speedup_cpu_vs_fpga(:,1)=(cpu_time/fpga_time(1))';
speedup_cpu_vs_fpga(:,2)=(cpu_time/fpga_time(2))';
speedup_cpu_vs_fpga(:,3)=(cpu_time/fpga_time(3))';
speedup_cpu_vs_fpga;
%%%%%%% RELATIVE COSTS WITH RESPECT TO 1,2,8 FPGAs
cpu_price_m = cpu_price'*ones(1,3);
fpga_price_m = ones(3,1)*fpga_price;
cpu_price_m
fpga_price_m
relative_price = fpga_price_m./cpu_price_m;
relative_costs=1./speedup_cpu_vs_fpga.*relative_price*100;
%%%%%%% ENERGY
energy_cpu = cpu_time * power_cpu .* cpu_cores;
energy_fpga = fpga_time * power_fpga .* fpga_cores;
energy_cpu_m = energy_cpu'*ones(1,3);
energy_fpga_m = ones(3,1)*energy_fpga;
energy = energy_fpga_m ./energy_cpu_m*100;
% Table 3
display('Table 3')
Table3 = [speedup_cpu_vs_fpga,relative_costs,energy];
Table3 = round(Table3,2)
%%%%% 1 FPGA vs 1 CPU
cpu_seconds = seconds(cpu_time(1));
cpu_seconds.Format = 'hh:mm'; 
fpga_seconds = seconds(fpga_time(1));
fpga_seconds.Format = 'mm:ss'; 
fprintf("Speedup 1 FPGA vs 1 CPU %dx: from %s to %s \n",round(cpu_time(1)/fpga_time(1)),cpu_seconds,fpga_seconds);
%%%%% 1 FPGA vs 8 CPUs
cpu_seconds = seconds(cpu_time(2));
cpu_seconds.Format = 'hh:mm'; 
fpga_seconds = seconds(fpga_time(1));
fpga_seconds.Format = 'mm:ss'; 
fprintf("Speedup 1 FPGA vs 8 CPUs %dx: from %s to %s \n",round(cpu_time(2)/fpga_time(1)),cpu_seconds,fpga_seconds);
%%%%% Dollar Comparisons
one_million_economies_cpu= one_million_economies*cpu_time(1)/3600/1200*cpu_price(1);
one_million_economies_fpga= one_million_economies*fpga_time(1)/3600/1200*fpga_price(1);
fprintf("Cost of one million economies in cpu %f and FPGA %f\n",round(one_million_economies_cpu),round(one_million_economies_fpga));

%% Table 4
display('Table 4')
speedup_single_kernel = cpu_single_economy_time/fpga_single_kernel_time;
relative_costs_single_kernel = 1/speedup_single_kernel*fpga_price(1)/cpu_price(1)*100;
energy_fpga_single_kernel = fpga_single_kernel_time * power_fpga_single_kernel .* fpga_cores(1);
energy_single_kernel = energy_fpga_single_kernel/energy_cpu(1)*100;
Table4 = [fpga_single_kernel_time,cpu_single_economy_time,speedup_single_kernel,relative_costs_single_kernel,energy_single_kernel];
%Table4(1)
%Table4(2)
display('Table 4 - Speedup')
round(Table4(3),2)
display('Table 4 - Relative Costs')
round(Table4(4),2)
display('Table 4 - Energy')
round(Table4(5),2)
fprintf("Speedup %f x 3 SLR = %fx\n",round(Table4(3),2),round(Table4(3)*3));

%% Table 5
display('Table 5')
time_cpu_grids_m = [time_cpu_grids;time_cpu_grids;time_cpu_grids];
speedup_cpu_vs_fpga(2,:) % 1,2,8 FPGAs vs 8 cores
time_per_fpga_grids = time_per_grids_fpga';
speedup_fpga_grids_cpu = time_cpu_grids_m./time_per_fpga_grids;
round(speedup_fpga_grids_cpu(1,:),2)
round(speedup_fpga_grids_cpu(2,:),2)
round(speedup_fpga_grids_cpu(3,:),2)

%% Table A3
display('Table A3')
TableA3= [cpu_time,fpga_time; ...
          cpu_price.* cpu_time/3600,fpga_price.* fpga_time/3600;
          energy_cpu,energy_fpga];
round(TableA3(1,:),2)
round(TableA3(2,:),2)
round(TableA3(3,:),2)

%% Table C3
display('Table A4')
TableC3 = [time_cpu_grids',time_per_grids_fpga]

%% Carbon Footprint
lbs_CO2_kWh = w_gas * CO2_per_kWh_gas + w_coal * CO2_per_kWh_coal + w_ren * CO2_per_kWh_ren
lbs_per_metric_ton = 2204.62;
joules_kWh = 1/3600*1/1000; % 1J = joules_kWh * kWh, % Joules (watts/second) to kWh (watts * hour / 1000)

%%% Summit on CPU
watts = summit_power;
tot_core_hour_per_year = Tot_core_hour_summit;
% ---
display('Carbon Footprint Summit - CPU')
kWh_core_hour=watts/1000                                        % kWh per core hour
kWh_from_our_analysis = energy_cpu(1) / joules_kWh              % joules one core * kWh/joules
lbs_CO2_core_hour = lbs_CO2_kWh * kWh_core_hour                 % lbs CO2 per coure hour
lbs_CO2_per_year = lbs_CO2_core_hour * tot_core_hour_per_year   % lbs CO2 per year
metric_tons_CO2 = lbs_CO2_per_year / lbs_per_metric_ton         % metric tons of CO2 per year 
metric_tons_CO2_cars = round(metric_tons_CO2/5)                 % metric tons of CO2 per year in cars
% ---
%summit_cpu = [metric_tons_CO2,metric_tons_CO2_cars];

%%% Summit on CPU
watts = power_fpga;
tot_core_hour_per_year = Tot_core_hour_summit/speedup_cpu_vs_fpga(1,1);
% ---
display('Carbon Footprint Summit - FPGA')
kWh_core_hour=watts/1000                                        % kWh per core hour
lbs_CO2_core_hour = lbs_CO2_kWh * kWh_core_hour                 % lbs CO2 per coure hour
tot_core_hour_per_year                                          % xxxxxx COUNTERFACTUAL CORE HOURS
lbs_CO2_per_year = lbs_CO2_core_hour * tot_core_hour_per_year   % lbs CO2 per year
metric_tons_CO2 = lbs_CO2_per_year / lbs_per_metric_ton         % metric tons of CO2 per year 
metric_tons_CO2_cars = floor(metric_tons_CO2/5)                 % metric tons of CO2 per year in cars
% ---




