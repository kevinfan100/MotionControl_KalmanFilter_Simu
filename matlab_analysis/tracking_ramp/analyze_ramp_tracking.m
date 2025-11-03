%% Ramp Tracking Simulation Analysis Script
% Date: 2025-11-02
% Purpose: Analyze ramp tracking control performance
%
% This script analyzes the output from RampTracking_Init() and Tracking_Simu()
% and generates trajectory tracking performance metrics and figures.

clear; clc; close all;

%% Configuration
SAMPLING_RATE = 1612; % Hz

% ========================================
% Manual file specification (modify this line after each simulation)
DATA_FILE = 'TRACKING_Ramp_40um_20ums_0deg_WTR_THEO_P35_I0_D0_20251102.txt';
% ========================================

%% 1. Load Simulation Data
fprintf('========== Ramp Tracking Analysis ==========\n\n');
fprintf('Loading simulation data...\n');

% Try multiple possible paths
possiblePaths = {
    '../../jul3_2014_motioncontrol_hallsensor_akf_ar2/',  % From matlab_analysis/tracking_ramp/
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
    error('Data file not found. Please check DATA_FILE name.');
end

fprintf('  >>> ANALYZING: %s\n', DATA_FILE);

% Read data (skip 4 header lines)
data = readmatrix(filename, 'NumHeaderLines', 4);

% Extract columns (based on CALCULATION.cpp output format)
TX_Om = data(:,1);   TY_Om = data(:,2);   TZ_Om = data(:,3);   % Target (Om coordinate)
MX_Om = data(:,4);   MY_Om = data(:,5);   MZ_Om = data(:,6);   % Measured (Om coordinate)
RX_Om = data(:,13);  RY_Om = data(:,14);  RZ_Om = data(:,15);  % Real (Om coordinate)
I1 = data(:,19); I2 = data(:,20); I3 = data(:,21);             % Currents
I4 = data(:,22); I5 = data(:,23); I6 = data(:,24);
Fx_dsir_Om = data(:,25); Fy_dsir_Om = data(:,26); Fz_dsir_Om = data(:,27); % Desired force

numSamples = length(TX_Om);
time = (0:numSamples-1) / SAMPLING_RATE; % Time vector in seconds

fprintf('  Samples: %d (%.2f s)\n', numSamples, time(end));

%% 2. Auto-detect parameters from filename
fprintf('\n--- Simulation Parameters ---\n');

% Parse filename (new format)
tokens = regexp(DATA_FILE, 'TRACKING_Ramp_(\d+)um_(\d+)ums_(\d+)deg_.*_P(\d+)_I(\d+)_D(\d+)', 'tokens');
if ~isempty(tokens)
    RangeXY = str2double(tokens{1}{1});
    Speed = str2double(tokens{1}{2});
    Angle = str2double(tokens{1}{3});
    Pgain = str2double(tokens{1}{4});
    Igain = str2double(tokens{1}{5});
    Dgain = str2double(tokens{1}{6});

    fprintf('RangeXY: %d um\n', RangeXY);
    fprintf('Speed: %d um/s\n', Speed);
    fprintf('Angle: %d deg\n', Angle);
    fprintf('PID gains: P=%d, I=%d, D=%d\n', Pgain, Igain, Dgain);

    % Create parameter string for titles
    param_str = sprintf('Ramp Tracking (%dum, %dum/s, %ddeg, P=%d)', ...
                        RangeXY, Speed, Angle, Pgain);
else
    fprintf('Warning: Could not parse parameters from filename\n');
    param_str = 'Ramp Tracking';
    RangeXY = 0; Speed = 0; Angle = 0; Pgain = 0; Igain = 0; Dgain = 0;
end

%% 3. Calculate Tracking Errors (Target - Measured)
error_X = (TX_Om - MX_Om) * 1000;  % Convert to nm
error_Y = (TY_Om - MY_Om) * 1000;
error_Z = (TZ_Om - MZ_Om) * 1000;

% 3D tracking error magnitude
error_3D = sqrt((TX_Om - MX_Om).^2 + (TY_Om - MY_Om).^2 + (TZ_Om - MZ_Om).^2) * 1000; % nm

%% 4. Calculate Statistics
fprintf('\n--- Trajectory Tracking Performance ---\n');

% 3D error statistics
Std_3D = std(error_3D);
Peak_3D = max(error_3D);
Mean_3D = mean(error_3D);

fprintf('\n3D tracking error:\n');
fprintf('  Std:        %.2f nm\n', Std_3D);
fprintf('  Peak error: %.2f nm\n', Peak_3D);
fprintf('  Mean error: %.2f nm\n', Mean_3D);

% Per-axis analysis
axes_names = {'X', 'Y', 'Z'};
errors = {error_X, error_Y, error_Z};
forces = {Fx_dsir_Om, Fy_dsir_Om, Fz_dsir_Om};

fprintf('\n--- Per-Axis Analysis ---\n\n');

for ax = 1:3
    fprintf('--- %s Axis ---\n', axes_names{ax});

    err = errors{ax};
    force = forces{ax};

    % Error statistics
    Std_err = std(err);
    Peak_err = max(abs(err));
    Mean_err = mean(err);

    fprintf('Position tracking:\n');
    fprintf('  Std:        %.2f nm\n', Std_err);
    fprintf('  Peak error: %.2f nm\n', Peak_err);
    fprintf('  Mean error: %.2f nm\n', Mean_err);

    % Force statistics
    Avg_force = mean(abs(force));
    Max_force = max(abs(force));
    Std_force = std(force);

    fprintf('Control force:\n');
    fprintf('  Average: %.3f pN\n', Avg_force);
    fprintf('  Maximum: %.3f pN\n', Max_force);
    fprintf('  Std:     %.3f pN\n', Std_force);
    fprintf('\n');
end

%% 5. Phase-wise Analysis (by velocity)
fprintf('\n========================================\n');
fprintf('Phase-wise Standard Deviation Analysis\n');
fprintf('========================================\n\n');

% Calculate velocity from target trajectory
vel_X = [0; diff(TX_Om)] * SAMPLING_RATE;
vel_Y = [0; diff(TY_Om)] * SAMPLING_RATE;
vel_Z = [0; diff(TZ_Om)] * SAMPLING_RATE;
vel_3D = sqrt(vel_X.^2 + vel_Y.^2 + vel_Z.^2);
vel_3D_smooth = movmean(vel_3D, 100);  % Smooth to reduce noise

% Define velocity thresholds for phase classification
SLOW_THRESHOLD = 5;    % um/s (below this = slow/stay)
FAST_THRESHOLD = 50;   % um/s (above this = fast motion)

% Classify each sample
slow_idx = find(vel_3D_smooth < SLOW_THRESHOLD);
medium_idx = find(vel_3D_smooth >= SLOW_THRESHOLD & vel_3D_smooth < FAST_THRESHOLD);
fast_idx = find(vel_3D_smooth >= FAST_THRESHOLD);

fprintf('Motion Classification:\n');
fprintf('  Slow/Stay  (< %.0f um/s):   %d samples (%.2f s)\n', ...
        SLOW_THRESHOLD, length(slow_idx), length(slow_idx)/SAMPLING_RATE);
fprintf('  Transition (%.0f-%.0f um/s): %d samples (%.2f s)\n', ...
        SLOW_THRESHOLD, FAST_THRESHOLD, length(medium_idx), length(medium_idx)/SAMPLING_RATE);
fprintf('  Fast Motion (>= %.0f um/s):  %d samples (%.2f s)\n\n', ...
        FAST_THRESHOLD, length(fast_idx), length(fast_idx)/SAMPLING_RATE);

% Calculate statistics for each phase
if ~isempty(slow_idx)
    slow_std_X = std(error_X(slow_idx));
    slow_std_Y = std(error_Y(slow_idx));
    slow_std_Z = std(error_Z(slow_idx));
    slow_std_3D = std(error_3D(slow_idx));
    slow_mean_3D = mean(error_3D(slow_idx));
else
    slow_std_X = NaN; slow_std_Y = NaN; slow_std_Z = NaN;
    slow_std_3D = NaN; slow_mean_3D = NaN;
end

if ~isempty(medium_idx)
    medium_std_X = std(error_X(medium_idx));
    medium_std_Y = std(error_Y(medium_idx));
    medium_std_Z = std(error_Z(medium_idx));
    medium_std_3D = std(error_3D(medium_idx));
    medium_mean_3D = mean(error_3D(medium_idx));
else
    medium_std_X = NaN; medium_std_Y = NaN; medium_std_Z = NaN;
    medium_std_3D = NaN; medium_mean_3D = NaN;
end

if ~isempty(fast_idx)
    fast_std_X = std(error_X(fast_idx));
    fast_std_Y = std(error_Y(fast_idx));
    fast_std_Z = std(error_Z(fast_idx));
    fast_std_3D = std(error_3D(fast_idx));
    fast_mean_3D = mean(error_3D(fast_idx));
else
    fast_std_X = NaN; fast_std_Y = NaN; fast_std_Z = NaN;
    fast_std_3D = NaN; fast_mean_3D = NaN;
end

% Print results table
fprintf('%-20s | %10s | %10s | %10s | %10s | %10s\n', ...
        'Phase', 'Mean (nm)', 'Std (nm)', 'X Std', 'Y Std', 'Z Std');
fprintf('%s\n', repmat('-', 1, 85));

fprintf('%-20s | %10.2f | %10.2f | %10.2f | %10.2f | %10.2f\n', ...
        'Slow/Stay', slow_mean_3D, slow_std_3D, slow_std_X, slow_std_Y, slow_std_Z);
fprintf('%-20s | %10.2f | %10.2f | %10.2f | %10.2f | %10.2f\n', ...
        'Transition', medium_mean_3D, medium_std_3D, medium_std_X, medium_std_Y, medium_std_Z);
fprintf('%-20s | %10.2f | %10.2f | %10.2f | %10.2f | %10.2f\n', ...
        'Fast Motion', fast_mean_3D, fast_std_3D, fast_std_X, fast_std_Y, fast_std_Z);

% Performance assessment
fprintf('\n--- Performance Assessment ---\n\n');
if ~isnan(slow_std_3D) && ~isnan(fast_std_3D)
    ratio = fast_std_3D / slow_std_3D;
    fprintf('Fast/Slow Std Ratio: %.2f\n\n', ratio);

    if ratio < 1.5
        fprintf('✓ EXCELLENT: Error barely increases during fast motion\n');
        fprintf('  → P-only control handles acceleration well\n');
        fprintf('  → D control is optional\n');
    elseif ratio < 2.5
        fprintf('✓ GOOD: Moderate error increase during fast motion\n');
        fprintf('  → P control works acceptably\n');
        fprintf('  → D control may provide improvement\n');
    elseif ratio < 4.0
        fprintf('⚠ ACCEPTABLE: Noticeable error increase\n');
        fprintf('  → Consider adding D control (Kd = 5-10)\n');
        fprintf('  → Or reduce acceleration rate\n');
    else
        fprintf('✗ NEEDS IMPROVEMENT: Significant error during fast motion\n');
        fprintf('  → Strongly recommend D control (Kd = 10-15)\n');
        fprintf('  → Or increase RAMP_ACCE_L in C++ code\n');
    end
end

fprintf('\n========================================\n\n');

%% 6. Current Statistics
fprintf('--- Current Statistics ---\n');
currents = [I1, I2, I3, I4, I5, I6];
fprintf('       Avg (A)    Max (A)\n');
for i = 1:6
    avg_curr = mean(abs(currents(:,i)));
    max_curr = max(abs(currents(:,i)));
    fprintf('I%d:    %.3f      %.3f\n', i, avg_curr, max_curr);
end

total_avg = mean(mean(abs(currents)));
total_max = max(max(abs(currents)));
fprintf('\nTotal average current: %.3f A\n', total_avg);
fprintf('Total maximum current: %.3f A\n', total_max);

%% 6. Generate Figures

fprintf('\n--- Generating Figures ---\n');

% Define colors for each axis
axis_colors = {'r', 'g', 'b'}; % X=red, Y=green, Z=blue

%% Figure 0: 3D Trajectory
fig0 = figure('Position', [50, 50, 1000, 800], 'Color', 'white');

% Use um for position display (no conversion needed, data is already in um)
TX_um = TX_Om;
TY_um = TY_Om;
TZ_um = TZ_Om;
MX_um = MX_Om;
MY_um = MY_Om;
MZ_um = MZ_Om;

hold on; grid on; box on;

% Plot trajectories
h_target = plot3(TX_um, TY_um, TZ_um, 'k--', 'LineWidth', 4, 'DisplayName', 'Target');
h_measured = plot3(MX_um, MY_um, MZ_um, 'Color', [0.8 0.2 0.2], 'LineWidth', 2.5, 'DisplayName', 'Measured');

% Mark start point (show in legend)
scatter3(TX_um(1), TY_um(1), TZ_um(1), 150, 'g', 'filled', 'MarkerEdgeColor', 'k', 'LineWidth', 2, 'DisplayName', 'Start');

% Find and mark edge points (do NOT show in legend by using HandleVisibility='off')
% For ramp tracking, we want to mark the two extreme positions

% Edge A (Red): maximum positive distance from origin
distances = sqrt(TX_um.^2 + TY_um.^2 + TZ_um.^2);
[max_dist, edge_a_idx] = max(distances);
scatter3(TX_um(edge_a_idx), TY_um(edge_a_idx), TZ_um(edge_a_idx), 120, 'r', 's', 'filled', 'MarkerEdgeColor', 'k', 'LineWidth', 2, 'HandleVisibility', 'off');

% Edge B (Blue): Find the second local maximum in distance
% This typically corresponds to the opposite edge in ramp tracking
if edge_a_idx < length(TX_um) - 100  % Make sure there's enough data after Edge A
    % Look for second peak after passing through minimum
    second_half = distances(edge_a_idx:end);
    [~, min_idx] = min(second_half);  % Find minimum after first peak
    if min_idx < length(second_half) - 10
        [second_max, edge_b_idx_relative] = max(second_half(min_idx:end));
        edge_b_idx = edge_a_idx + min_idx + edge_b_idx_relative - 2;
        % Only plot if it's a significant peak (> 30% of max distance)
        if second_max > max_dist * 0.3
            scatter3(TX_um(edge_b_idx), TY_um(edge_b_idx), TZ_um(edge_b_idx), 120, 'b', 's', 'filled', 'MarkerEdgeColor', 'k', 'LineWidth', 2, 'HandleVisibility', 'off');
        end
    end
end

xlabel('X Position (\mum)', 'FontSize', 16, 'FontWeight', 'bold');
ylabel('Y Position (\mum)', 'FontSize', 16, 'FontWeight', 'bold');
zlabel('Z Position (\mum)', 'FontSize', 16, 'FontWeight', 'bold');
title(['3D Trajectory: ' param_str], 'FontSize', 18, 'FontWeight', 'bold');
legend('Location', 'best', 'FontSize', 12, 'FontWeight', 'bold');

% Set all axis limits to match XY scale (to prevent Z noise from distorting the plot)
xy_range = max(max(abs(TX_um)), max(abs(TY_um)));
xlim([-xy_range, xy_range]);
ylim([-xy_range, xy_range]);
zlim([-xy_range, xy_range]);
axis equal;  % Equal aspect ratio for all axes

set(gca, 'FontSize', 14, 'FontWeight', 'bold', 'LineWidth', 2);
view(45, 30);

%% Figures 1-3: Per-axis analysis (3 separate windows)
figs = [];
for ax = 1:3
    figs(ax) = figure('Position', [100 + (ax-1)*50, 100 + (ax-1)*50, 1000, 800], 'Color', 'white');

    % Get position in um (no conversion needed, data already in um)
    T_pos_um = eval(['T' axes_names{ax} '_Om']);
    M_pos_um = eval(['M' axes_names{ax} '_Om']);
    err = errors{ax};  % Already in nm
    force = forces{ax};

    % Calculate statistics (error already in nm)
    Std_err = std(err);
    Peak_err = max(abs(err));
    Mean_err = mean(err);

    % Subplot 1: Position Tracking
    subplot(2, 1, 1);
    hold on; grid on; box on;

    % Plot Target with thick dashed line
    h_target = plot(time, T_pos_um, 'k--', 'LineWidth', 4, 'DisplayName', 'Target');
    % Plot Measured
    h_measured = plot(time, M_pos_um, 'Color', axis_colors{ax}, 'LineWidth', 2.5, 'DisplayName', 'Measured');

    % Add statistics text box (only Std, removed Peak per user request)
    stats_text = sprintf('Std: %.1f nm', Std_err);
    text(0.02, 0.98, stats_text, ...
        'Units', 'normalized', 'VerticalAlignment', 'top', 'HorizontalAlignment', 'left', ...
        'BackgroundColor', 'white', 'EdgeColor', 'black', 'FontSize', 13, 'FontWeight', 'bold', ...
        'Margin', 5, 'LineWidth', 1.5);

    ylabel([axes_names{ax} ' Position (\mum)'], 'FontSize', 16, 'FontWeight', 'bold');
    title([axes_names{ax} ' Axis: ' param_str], 'FontSize', 18, 'FontWeight', 'bold');
    legend([h_target, h_measured], 'Location', 'northeast', 'FontSize', 12, 'FontWeight', 'bold');
    set(gca, 'FontSize', 14, 'FontWeight', 'bold', 'LineWidth', 2);

    % Subplot 2: Tracking Error
    subplot(2, 1, 2);
    hold on; grid on; box on;

    plot(time, err, 'Color', axis_colors{ax}, 'LineWidth', 2);

    % Add reference lines at ±100nm
    yline(100, 'k--', 'LineWidth', 1, 'Alpha', 0.5);
    yline(-100, 'k--', 'LineWidth', 1, 'Alpha', 0.5);
    yline(0, 'k:', 'LineWidth', 1, 'Alpha', 0.3);

    % Add statistics text box (right side)
    stats_text = sprintf('Mean: %.1f nm\nStd: %.1f nm', Mean_err, Std_err);
    text(0.98, 0.98, stats_text, ...
        'Units', 'normalized', 'VerticalAlignment', 'top', 'HorizontalAlignment', 'right', ...
        'BackgroundColor', 'white', 'EdgeColor', 'black', 'FontSize', 13, 'FontWeight', 'bold', ...
        'Margin', 5, 'LineWidth', 1.5);

    ylabel([axes_names{ax} ' Tracking Error (nm)'], 'FontSize', 16, 'FontWeight', 'bold');
    xlabel('Time (s)', 'FontSize', 16, 'FontWeight', 'bold');
    title([axes_names{ax} ' Tracking Error'], 'FontSize', 18, 'FontWeight', 'bold');
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

% Generate base filename from parameters
if RangeXY > 0
    baseFilename = sprintf('Ramp_%dum_%dums_%ddeg_P%d_I%d_D%d', ...
                          RangeXY, Speed, Angle, Pgain, Igain, Dgain);
else
    baseFilename = 'Ramp_analysis';
end

dateStr = datestr(now, 'yyyy-mm-dd');

% Save 3D trajectory figure
figFile = fullfile(outputDir, 'figures', sprintf('%s_3D_%s.png', baseFilename, dateStr));
saveas(fig0, figFile);
fprintf('Figure saved: %s\n', figFile);

% Save per-axis figures
for ax = 1:3
    figFile = fullfile(outputDir, 'figures', sprintf('%s_%s_axis_%s.png', baseFilename, axes_names{ax}, dateStr));
    try
        saveas(figs(ax), figFile);
        fprintf('Figure saved: %s\n', figFile);
    catch ME
        fprintf('Warning: Could not save figure %d: %s\n', ax, ME.message);
    end
end

% Save text report
reportFile = fullfile(outputDir, 'reports', sprintf('%s_report_%s.txt', baseFilename, dateStr));
fid = fopen(reportFile, 'w');
fprintf(fid, '========== Ramp Tracking Analysis Report ==========\n');
fprintf(fid, 'Date: %s\n', datestr(now));
fprintf(fid, 'Input file: %s\n', DATA_FILE);
fprintf(fid, 'Simulation duration: %.2f s\n', time(end));
fprintf(fid, 'Samples: %d\n\n', numSamples);

fprintf(fid, '--- Simulation Parameters ---\n');
fprintf(fid, 'RangeXY: %d um\n', RangeXY);
fprintf(fid, 'Speed: %d um/s\n', Speed);
fprintf(fid, 'Angle: %d deg\n', Angle);
fprintf(fid, 'PID gains: P=%d, I=%d, D=%d\n\n', Pgain, Igain, Dgain);

fprintf(fid, '--- Trajectory Tracking Performance ---\n');
fprintf(fid, '3D tracking error:\n');
fprintf(fid, '  Std:        %.2f nm\n', Std_3D);
fprintf(fid, '  Peak error: %.2f nm\n', Peak_3D);
fprintf(fid, '  Mean error: %.2f nm\n\n', Mean_3D);

for ax = 1:3
    fprintf(fid, '--- %s Axis ---\n', axes_names{ax});
    err = errors{ax};
    force = forces{ax};

    fprintf(fid, 'Position tracking:\n');
    fprintf(fid, '  Std:        %.2f nm\n', std(err));
    fprintf(fid, '  Peak error: %.2f nm\n', max(abs(err)));
    fprintf(fid, '  Mean error: %.2f nm\n', mean(err));

    fprintf(fid, 'Control force:\n');
    fprintf(fid, '  Average: %.3f pN\n', mean(abs(force)));
    fprintf(fid, '  Maximum: %.3f pN\n', max(abs(force)));
    fprintf(fid, '  Std:     %.3f pN\n\n', std(force));
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
