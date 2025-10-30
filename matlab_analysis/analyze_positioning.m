%% Positioning Only Simulation Analysis Script
% Date: 2025-10-30
% Purpose: Analyze positioning control performance with noise
%
% This script analyzes the output from Positioning_Simu() function
% and generates key performance metrics and figures.

clear; clc; close all;

%% Configuration
SAMPLING_RATE = 1612; % Hz
THRESHOLD_NM = 100;   % Convergence threshold in nm
STEADY_TIME = 10;     % Last 10 seconds for steady-state analysis

%% 1. Load Simulation Data
fprintf('========== Positioning Only Analysis ==========\n\n');
fprintf('Loading simulation data...\n');

% Find the most recent simulation file
% Try multiple possible paths
possiblePaths = {
    '../jul3_2014_motioncontrol_hallsensor_akf_ar2/',  % From matlab_analysis/
    './jul3_2014_motioncontrol_hallsensor_akf_ar2/',   % From project root
    'jul3_2014_motioncontrol_hallsensor_akf_ar2/'      % Direct
};

filePattern = 'Trapping Simu WTR at*.txt';
files = [];
dataPath = '';

for i = 1:length(possiblePaths)
    tempFiles = dir(fullfile(possiblePaths{i}, filePattern));
    if ~isempty(tempFiles)
        files = tempFiles;
        dataPath = possiblePaths{i};
        break;
    end
end

if isempty(files)
    fprintf('\nError: No simulation output file found!\n');
    fprintf('Searched in:\n');
    for i = 1:length(possiblePaths)
        fprintf('  - %s\n', possiblePaths{i});
    end
    fprintf('\nPlease:\n');
    fprintf('  1. Run the C++ simulation first\n');
    fprintf('  2. Make sure you are in the matlab_analysis/ directory\n');
    fprintf('  3. Or run from project root directory\n');
    error('Simulation output file not found.');
end

fprintf('  Found data in: %s\n', dataPath);

% Get the most recent file
[~, idx] = max([files.datenum]);
filename = fullfile(dataPath, files(idx).name);
fprintf('  File: %s\n', files(idx).name);

% Read data (skip 4 header lines)
data = readmatrix(filename, 'NumHeaderLines', 4);

% Extract columns (based on CALCULATION.cpp:3002-3017)
TX_Om = data(:,1);   TY_Om = data(:,2);   TZ_Om = data(:,3);   % Target (Om)
MX_Om = data(:,4);   MY_Om = data(:,5);   MZ_Om = data(:,6);   % Measured (Om)
RX_Om = data(:,13);  RY_Om = data(:,14);  RZ_Om = data(:,15);  % Real (Om)
I1 = data(:,19); I2 = data(:,20); I3 = data(:,21);             % Currents
I4 = data(:,22); I5 = data(:,23); I6 = data(:,24);
Fx_dsir_Om = data(:,25); Fy_dsir_Om = data(:,26); Fz_dsir_Om = data(:,27); % Desired force

numSamples = length(TX_Om);
time = (0:numSamples-1) / SAMPLING_RATE; % Time vector in seconds

fprintf('  Samples: %d (~%.2f seconds)\n', numSamples, time(end));

%% 2. Initial Position Information
fprintf('\n--- Initial Positions ---\n');
fprintf('Target position (um): (%.3f, %.3f, %.3f)\n', TX_Om(1), TY_Om(1), TZ_Om(1));
fprintf('Real initial position (um): (%.3f, %.3f, %.3f)\n', RX_Om(1), RY_Om(1), RZ_Om(1));
fprintf('Measured initial position (um): (%.3f, %.3f, %.3f)\n', MX_Om(1), MY_Om(1), MZ_Om(1));

% Initial offset (Real - Target)
initial_offset_X = RX_Om(1) - TX_Om(1);
initial_offset_Y = RY_Om(1) - TY_Om(1);
initial_offset_Z = RZ_Om(1) - TZ_Om(1);
initial_offset_magnitude = sqrt(initial_offset_X^2 + initial_offset_Y^2 + initial_offset_Z^2);

fprintf('\nInitial offset from target:\n');
fprintf('  X: %.3f um (%.1f nm)\n', initial_offset_X, initial_offset_X*1000);
fprintf('  Y: %.3f um (%.1f nm)\n', initial_offset_Y, initial_offset_Y*1000);
fprintf('  Z: %.3f um (%.1f nm)\n', initial_offset_Z, initial_offset_Z*1000);
fprintf('  Magnitude: %.3f um (%.1f nm)\n', initial_offset_magnitude, initial_offset_magnitude*1000);

%% 3. Calculate Errors (Target - Measured)
error_X = TX_Om - MX_Om;  % in um
error_Y = TY_Om - MY_Om;
error_Z = TZ_Om - MZ_Om;

%% 4. Calculate Metrics for Each Axis
fprintf('\n--- Analysis Results ---\n\n');

axes_names = {'X', 'Y', 'Z'};
errors = {error_X, error_Y, error_Z};
forces = {Fx_dsir_Om, Fy_dsir_Om, Fz_dsir_Om};

for ax = 1:3
    fprintf('--- %s Axis ---\n', axes_names{ax});

    err = errors{ax} * 1000; % Convert to nm
    force = forces{ax};

    % Initial error
    initial_error = abs(err(1));
    fprintf('Initial error: %.2f nm\n', initial_error);

    % Convergence time (first time error < threshold)
    conv_idx = find(abs(err) < THRESHOLD_NM, 1, 'first');
    if ~isempty(conv_idx)
        conv_time = time(conv_idx);
        fprintf('Convergence time: %.3f s (error < %d nm)\n', conv_time, THRESHOLD_NM);
    else
        fprintf('Convergence time: NOT CONVERGED (error never < %d nm)\n', THRESHOLD_NM);
    end

    % Steady-state analysis (last 10 seconds)
    steady_samples = STEADY_TIME * SAMPLING_RATE;
    if numSamples > steady_samples
        steady_idx = (numSamples - steady_samples + 1):numSamples;
        steady_mean = mean(err(steady_idx));
        steady_std = std(err(steady_idx));
        fprintf('Steady-state error mean: %.2f nm\n', steady_mean);
        fprintf('Steady-state error std: %.2f nm\n', steady_std);
    else
        fprintf('Steady-state analysis: N/A (simulation too short)\n');
    end

    % Control force statistics
    avg_force = mean(abs(force));
    max_force = max(abs(force));
    fprintf('Average control force: %.3f pN\n', avg_force);
    fprintf('Maximum control force: %.3f pN\n', max_force);
    fprintf('\n');
end

%% 5. Current Statistics
fprintf('\n--- Current Statistics ---\n');
currents = [I1, I2, I3, I4, I5, I6];
fprintf('       Avg (A)    Max (A)\n');
for i = 1:6
    avg_curr = mean(abs(currents(:,i)));
    max_curr = max(abs(currents(:,i)));
    fprintf('I%d:    %.3f      %.3f\n', i, avg_curr, max_curr);
end

total_avg = mean(mean(abs(currents)));
total_max = max(max(abs(currents)));
fprintf('Total average current: %.3f A\n', total_avg);
fprintf('Total maximum current: %.3f A\n', total_max);

%% 6. Generate Figures
fprintf('\n--- Generating Figures ---\n');

fig = figure('Position', [100, 100, 1400, 1000], 'Color', 'white');

% Figure 1: Position Comparison (3 subplots for X, Y, Z)
for ax = 1:3
    subplot(4, 3, ax);
    hold on; grid on;
    plot(time, eval(['T' axes_names{ax} '_Om']), 'k--', 'LineWidth', 1.5, 'DisplayName', 'Target');
    plot(time, eval(['M' axes_names{ax} '_Om']), 'b-', 'LineWidth', 1, 'DisplayName', 'Measured');
    ylabel([axes_names{ax} ' Position (\mum)']);
    if ax == 1
        title('Position Comparison');
    end
    if ax == 3
        xlabel('Time (s)');
    end
    legend('Location', 'best');
end

% Figure 2: Error Convergence (3 subplots for X, Y, Z)
for ax = 1:3
    subplot(4, 3, 3 + ax);
    hold on; grid on;
    err_nm = errors{ax} * 1000; % Convert to nm
    plot(time, err_nm, 'r-', 'LineWidth', 1);
    yline(0, 'k--', 'LineWidth', 0.5);
    ylabel([axes_names{ax} ' Error (nm)']);
    if ax == 1
        title('Error Convergence');
    end
    if ax == 3
        xlabel('Time (s)');
    end
end

% Figure 3: Control Force (3 subplots for X, Y, Z)
for ax = 1:3
    subplot(4, 3, 6 + ax);
    hold on; grid on;
    force = forces{ax};
    plot(time, force, 'g-', 'LineWidth', 1);

    % Add average and max lines
    avg_val = mean(abs(force));
    max_val = max(abs(force));
    yline(avg_val, 'b--', sprintf('Avg: %.2f pN', avg_val), 'LineWidth', 1.5);
    yline(-avg_val, 'b--', 'LineWidth', 1.5);
    yline(max_val, 'r--', sprintf('Max: %.2f pN', max_val), 'LineWidth', 1.5);
    yline(-max_val, 'r--', 'LineWidth', 1.5);

    ylabel([axes_names{ax} ' Force (pN)']);
    if ax == 1
        title('Control Force');
    end
    if ax == 3
        xlabel('Time (s)');
    end
end

% Figure 4: 6 Currents (single plot)
subplot(4, 3, [10, 11, 12]);
hold on; grid on;
plot(time, I1, 'LineWidth', 1, 'DisplayName', 'I1');
plot(time, I2, 'LineWidth', 1, 'DisplayName', 'I2');
plot(time, I3, 'LineWidth', 1, 'DisplayName', 'I3');
plot(time, I4, 'LineWidth', 1, 'DisplayName', 'I4');
plot(time, I5, 'LineWidth', 1, 'DisplayName', 'I5');
plot(time, I6, 'LineWidth', 1, 'DisplayName', 'I6');
ylabel('Current (A)');
xlabel('Time (s)');
title('Coil Currents');
legend('Location', 'best');

%% 7. Save Outputs
outputDir = 'output';
if ~exist(outputDir, 'dir')
    mkdir(outputDir);
end
if ~exist(fullfile(outputDir, 'figures'), 'dir')
    mkdir(fullfile(outputDir, 'figures'));
end
if ~exist(fullfile(outputDir, 'reports'), 'dir')
    mkdir(fullfile(outputDir, 'reports'));
end

% Save figure
dateStr = datestr(now, 'yyyy-mm-dd');
figFile = fullfile(outputDir, 'figures', sprintf('positioning_analysis_%s.png', dateStr));
saveas(fig, figFile);
fprintf('Figure saved: %s\n', figFile);

% Save report
reportFile = fullfile(outputDir, 'reports', sprintf('positioning_analysis_%s.txt', dateStr));
fid = fopen(reportFile, 'w');
fprintf(fid, '========== Positioning Only Analysis Report ==========\n');
fprintf(fid, 'Date: %s\n', datestr(now));
fprintf(fid, 'Input file: %s\n', files(idx).name);
fprintf(fid, 'Simulation duration: %.2f seconds\n', time(end));
fprintf(fid, 'Samples: %d\n\n', numSamples);

for ax = 1:3
    fprintf(fid, '--- %s Axis ---\n', axes_names{ax});
    err = errors{ax} * 1000;
    force = forces{ax};

    fprintf(fid, 'Initial error: %.2f nm\n', abs(err(1)));

    conv_idx = find(abs(err) < THRESHOLD_NM, 1, 'first');
    if ~isempty(conv_idx)
        fprintf(fid, 'Convergence time: %.3f s\n', time(conv_idx));
    else
        fprintf(fid, 'Convergence time: NOT CONVERGED\n');
    end

    if numSamples > STEADY_TIME * SAMPLING_RATE
        steady_idx = (numSamples - STEADY_TIME * SAMPLING_RATE + 1):numSamples;
        fprintf(fid, 'Steady-state mean: %.2f nm\n', mean(err(steady_idx)));
        fprintf(fid, 'Steady-state std: %.2f nm\n', std(err(steady_idx)));
    end

    fprintf(fid, 'Average force: %.3f pN\n', mean(abs(force)));
    fprintf(fid, 'Maximum force: %.3f pN\n\n', max(abs(force)));
end

fprintf(fid, '--- Current Statistics ---\n');
fprintf(fid, '       Avg (A)    Max (A)\n');
for i = 1:6
    fprintf(fid, 'I%d:    %.3f      %.3f\n', i, mean(abs(currents(:,i))), max(abs(currents(:,i))));
end
fprintf(fid, '\nTotal average: %.3f A\n', total_avg);
fprintf(fid, 'Total maximum: %.3f A\n', total_max);

fclose(fid);
fprintf('Report saved: %s\n', reportFile);

fprintf('\n========================================\n');
fprintf('Analysis complete!\n');
fprintf('========================================\n');
