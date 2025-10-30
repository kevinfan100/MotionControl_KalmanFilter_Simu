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


% ========================================
DATA_FILE = 'Trapping Simu WTR at(0.0,0.0,0.0)um Pgain(50.0,50.0,50.0) (10-30-2025).txt';
% ========================================

% Try multiple possible paths
possiblePaths = {
    '../../jul3_2014_motioncontrol_hallsensor_akf_ar2/',  % From matlab_analysis/positioning/
    '../jul3_2014_motioncontrol_hallsensor_akf_ar2/',     % From matlab_analysis/
    './jul3_2014_motioncontrol_hallsensor_akf_ar2/',      % From project root
    'jul3_2014_motioncontrol_hallsensor_akf_ar2/'         % Direct
};

% Find the data file
filename = '';
for i = 1:length(possiblePaths)
    testPath = fullfile(possiblePaths{i}, DATA_FILE);
    if isfile(testPath)
        filename = testPath;
        fprintf('  Found data: %s\n', testPath);
        break;
    end
end

if isempty(filename)
    fprintf('\nError: Data file not found!\n');
    fprintf('Looking for: %s\n', DATA_FILE);
    fprintf('Searched in:\n');
    for i = 1:length(possiblePaths)
        fprintf('  - %s\n', possiblePaths{i});
    end
    error('Data file not found.');
end

fprintf('  >>> ANALYZING: %s\n', DATA_FILE);

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

fprintf('  Samples: %d (~%.2f ms)\n', numSamples, time(end)*1000);

%% 2. Initial Position Information
fprintf('\n--- Initial Positions ---\n');
fprintf('Target position (nm): (%.1f, %.1f, %.1f)\n', TX_Om(1)*1000, TY_Om(1)*1000, TZ_Om(1)*1000);
fprintf('Real initial position (nm): (%.1f, %.1f, %.1f)\n', RX_Om(1)*1000, RY_Om(1)*1000, RZ_Om(1)*1000);
fprintf('Measured initial position (nm): (%.1f, %.1f, %.1f)\n', MX_Om(1)*1000, MY_Om(1)*1000, MZ_Om(1)*1000);

% Initial offset (Real - Target)
initial_offset_X = RX_Om(1) - TX_Om(1);
initial_offset_Y = RY_Om(1) - TY_Om(1);
initial_offset_Z = RZ_Om(1) - TZ_Om(1);
initial_offset_magnitude = sqrt(initial_offset_X^2 + initial_offset_Y^2 + initial_offset_Z^2);

fprintf('\nInitial offset from target:\n');
fprintf('  X: %.1f nm\n', initial_offset_X*1000);
fprintf('  Y: %.1f nm\n', initial_offset_Y*1000);
fprintf('  Z: %.1f nm\n', initial_offset_Z*1000);
fprintf('  Magnitude: %.1f nm\n', initial_offset_magnitude*1000);

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
        conv_time_ms = time(conv_idx) * 1000;
        fprintf('Convergence time: %.2f ms (error < %d nm)\n', conv_time_ms, THRESHOLD_NM);
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

    % Terminal standard deviation for force (last 10 seconds)
    terminal_samples = min(STEADY_TIME * SAMPLING_RATE, numSamples);
    terminal_idx = (numSamples - terminal_samples + 1):numSamples;
    terminal_force_std = std(force(terminal_idx));
    fprintf('Terminal force std: %.3f pN\n', terminal_force_std);
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

%% 6. Generate Figures (3 separate windows for X, Y, Z)
fprintf('\n--- Generating Figures ---\n');

% Define colors for each axis
axis_colors = {'r', 'g', 'b'}; % X=red, Y=green, Z=blue

% Create 3 separate figure windows
figs = [];
for ax = 1:3
    figs(ax) = figure('Position', [100 + (ax-1)*50, 100 + (ax-1)*50, 1000, 800], 'Color', 'white');

    % Convert position to nm
    T_pos_nm = eval(['T' axes_names{ax} '_Om']) * 1000;
    M_pos_nm = eval(['M' axes_names{ax} '_Om']) * 1000;
    force = forces{ax};

    % Calculate terminal statistics (last 10 seconds)
    terminal_samples = min(STEADY_TIME * SAMPLING_RATE, numSamples);
    terminal_idx = (numSamples - terminal_samples + 1):numSamples;

    % Position statistics
    pos_mean = mean(M_pos_nm(terminal_idx));
    pos_std = std(M_pos_nm(terminal_idx));

    % Force statistics
    force_mean = mean(force(terminal_idx));
    force_std = std(force(terminal_idx));

    % Subplot 1: Position Comparison
    subplot(2, 1, 1);
    hold on; grid on;
    box on;

    % Plot Target with very thick dashed line
    h_target = plot(time, T_pos_nm, 'k--', 'LineWidth', 4, 'DisplayName', 'Target');
    % Plot Measured
    h_measured = plot(time, M_pos_nm, 'Color', axis_colors{ax}, 'LineWidth', 2.5, 'DisplayName', 'Measured');

    % Add text annotation for statistics (left side to avoid legend)
    stats_text = sprintf('Mean: %.2f nm\nStd: %.2f nm', pos_mean, pos_std);
    text(0.02, 0.98, stats_text, ...
        'Units', 'normalized', 'VerticalAlignment', 'top', 'HorizontalAlignment', 'left', ...
        'BackgroundColor', 'white', 'EdgeColor', 'black', 'FontSize', 13, 'FontWeight', 'bold', ...
        'Margin', 5, 'LineWidth', 1.5);

    ylabel([axes_names{ax} ' Position (nm)'], 'FontSize', 16, 'FontWeight', 'bold');
    title([axes_names{ax} ' Position'], 'FontSize', 18, 'FontWeight', 'bold');
    xlabel('Time (s)', 'FontSize', 16, 'FontWeight', 'bold');
    legend([h_target, h_measured], 'Location', 'northeast', 'FontSize', 12, 'FontWeight', 'bold');

    % Increase tick font size and linewidth
    set(gca, 'FontSize', 14, 'FontWeight', 'bold', 'LineWidth', 2);

    % Subplot 2: Control Force
    subplot(2, 1, 2);
    hold on; grid on;
    box on;

    plot(time, force, 'Color', axis_colors{ax}, 'LineWidth', 2);

    % Add text annotation for statistics
    stats_text = sprintf('Mean: %.3f pN\nStd: %.3f pN', force_mean, force_std);
    text(0.98, 0.98, stats_text, ...
        'Units', 'normalized', 'VerticalAlignment', 'top', 'HorizontalAlignment', 'right', ...
        'BackgroundColor', 'white', 'EdgeColor', 'black', 'FontSize', 13, 'FontWeight', 'bold', ...
        'Margin', 5, 'LineWidth', 1.5);

    ylabel([axes_names{ax} ' Force (pN)'], 'FontSize', 16, 'FontWeight', 'bold');
    xlabel('Time (s)', 'FontSize', 16, 'FontWeight', 'bold');
    title([axes_names{ax} ' Force'], 'FontSize', 18, 'FontWeight', 'bold');

    % Increase tick font size and linewidth
    set(gca, 'FontSize', 14, 'FontWeight', 'bold', 'LineWidth', 2);
end

%% 7. Save Outputs
outputDir = 'results';
if ~exist(outputDir, 'dir')
    mkdir(outputDir);
end
if ~exist(fullfile(outputDir, 'figures'), 'dir')
    mkdir(fullfile(outputDir, 'figures'));
end
if ~exist(fullfile(outputDir, 'reports'), 'dir')
    mkdir(fullfile(outputDir, 'reports'));
end

% Extract Pgain from filename for unique naming
% Example: 'Trapping Simu WTR at(0.0,0.0,0.0)um Pgain(50.0,50.0,50.0) (10-30-2025).txt'
pgainMatch = regexp(DATA_FILE, 'Pgain\((\d+\.?\d*)', 'tokens');
if ~isempty(pgainMatch)
    pgainStr = sprintf('Pgain%.0f', str2double(pgainMatch{1}{1}));
else
    % Fallback for old format: 'with PID gain(30.0,30.0,30.0)'
    pidMatch = regexp(DATA_FILE, 'PID gain\((\d+\.?\d*)', 'tokens');
    if ~isempty(pidMatch)
        pgainStr = sprintf('PIDgain%.0f', str2double(pidMatch{1}{1}));
    else
        pgainStr = 'unknown';
    end
end

% Save all 3 figures with Pgain in filename
dateStr = datestr(now, 'yyyy-mm-dd');
for ax = 1:3
    figFile = fullfile(outputDir, 'figures', sprintf('%s_axis_%s_%s.png', axes_names{ax}, pgainStr, dateStr));
    saveas(figs(ax), figFile);
    fprintf('Figure saved: %s\n', figFile);
end

% Save report with Pgain in filename
reportFile = fullfile(outputDir, 'reports', sprintf('analysis_%s_%s.txt', pgainStr, dateStr));
fid = fopen(reportFile, 'w');
fprintf(fid, '========== Positioning Only Analysis Report ==========\n');
fprintf(fid, 'Date: %s\n', datestr(now));
fprintf(fid, 'Input file: %s\n', DATA_FILE);
fprintf(fid, 'Simulation duration: %.2f ms\n', time(end)*1000);
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
    fprintf(fid, 'Maximum force: %.3f pN\n', max(abs(force)));

    % Terminal standard deviation for force (last 10 seconds)
    terminal_samples = min(STEADY_TIME * SAMPLING_RATE, numSamples);
    terminal_idx = (numSamples - terminal_samples + 1):numSamples;
    fprintf(fid, 'Terminal force std: %.3f pN\n\n', std(force(terminal_idx)));
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
