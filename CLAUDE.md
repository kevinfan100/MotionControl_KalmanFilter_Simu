# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is a C++ simulation framework for **optical trap motion control with Kalman filter state estimation**. It simulates 3D particle trapping in optical tweezers using a 6-pole magnetic force coil configuration, implementing multiple control algorithms (PID, Sliding Mode) and Kalman filtering for precision micro-manipulation at the piconewton force scale.

**Key Statistics:**
- ~3,800 lines of C++ code
- Sampling Rate: 1,612 Hz (control loop)
- Simulation Rate: 100,000 Hz (internal physics)
- Force Scale: Piconewtons (10^-12 N)

## Build and Run

### Building
```bash
# Open in Visual Studio 2022
# Select: Release | Win32
# Press F7 to build
```

Executable location: `Release\Jul3_2014_MotionControl_HallSensor_AKF_AR2.exe`

### Running
```bash
cd Release
.\Jul3_2014_MotionControl_HallSensor_AKF_AR2.exe
```

**Expected output:**
1. "Reading Thermal Force: ..." with progress 0-100%
2. Simulation progress 0-99.9999%
3. Generates output file: `TRACKING Ramp (...) Simu WTR Pgain(...).txt`
4. "請按任意鍵繼續 . . ."

## Architecture

### Core Components

**CALCULATION Class** (`CALCULATION.cpp` / `CALCULATION.h`)
- The main simulation engine (3,100+ lines)
- Single class architecture - all functionality encapsulated
- Implements: force models, inverse models, Kalman filters, control algorithms

**Main Program** (`Jul3_2014_MotionControl_HallSensor_AKF_AR2.cpp`)
- Entry point (260 lines)
- Configures and executes simulations
- Current mode: Ramp Tracking with PID control

### Critical Data Flow

```
1. Constructor loads:
   - KI matrices (KI_10Hz.txt, InvKIreal.txt)
   - 6 coefficient files (Feb28_2013 Coeff2nd_0CenterErr_I[1-6]_1891x20.txt)
   - Coordinate transformation matrices

2. Main program loads:
   - Thermal noise data (Thermal Noise XYZ 1612SplRate.txt)
   - Thermal force data (Thermal Force all XYZ Water...100000SplRate.txt)
   - Measurement noise data (Meas. Noise XYZ.txt)

3. Simulation loop (in Tracking_Simu):
   - Update target trajectory
   - Calculate position error
   - PID control → desired force
   - Inverse model: force → current (6 poles)
   - Forward model: current → actual force
   - Physics simulation (100kHz internal loop):
     * Add thermal/measurement noise
     * Calculate drag forces
     * Update position via Newton's laws
```

### Important State Management

**Pointer Initialization Critical:**
The `CALCULATION()` constructor **must initialize all pointer members to NULL**. Uninitialized pointers cause access violations because conditional memory allocation checks `if (ptr == NULL)`.

Currently initialized pointers (lines 202-224 in CALCULATION.cpp):
- `F_Ext_x/y/z_pN_Apply`
- `ThmMotion_X/Y/Z_RD`
- `ThmForce_X/Y/Z_NOa_RD`
- `MeasNoise_X/Y/Z_mOm_RD`
- `DampWtr/Gly_X/Y/Z_Oa_RD`

### Key Constants and Enums

**Simulation Modes:**
- `THEO` (0) / `REAL` (1): Theoretical vs Real inverse model
- `ADAPT_KF` (0) / `CONST_KF` (1): Adaptive vs Constant covariance Kalman Filter
- `WTR` (0) / `GLY` (1): Water vs Glycerol medium

**Control Types** (enum):
- `P_ctrl`, `PID_ctrl`, `PID_DS_ctrl`
- `SM_ctrl`, `SupTwistSM_ctrl`, `DiscrSM_ctrl`

**Key Defines:**
- `SplRate` = 1612 Hz (sampling rate)
- `SimuRate` = 100000 Hz (must be 100k when using Hall sensor)
- `R_norm` = 550.0 μm (normalized workspace center)
- `DELAY_STEP_Meas` = 2 samples (~1.24 ms delay)

## Configuration

### Modifying Simulation Parameters

Edit `Jul3_2014_MotionControl_HallSensor_AKF_AR2.cpp`:

**PID Gains** (line ~97):
```cpp
CALC.Set_PIDgain(30.0, 30.0, 30.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
// Parameters: Kp_x, Kp_y, Kp_z, Ki_x, Ki_y, Ki_z, Kd_x, Kd_y, Kd_z

// IMPORTANT: Tested stable range for P-only control (Positioning mode):
//   Stable: Kp = 10-35 (steady-state error < 20 nm, current < 0.1 A)
//   UNSTABLE: Kp ≥ 40 (causes numerical divergence, position error > 10^9 nm)
```

**Trajectory** (line ~102):
```cpp
CALC.RampTracking_Init(40.0, 0.0, 0, 200, 0);
// Parameters: RangeXY_um, RangeZ_um, UP_or_DOWN, Speed_um_per_s, Angle_deg
```

**Model Selection** (line ~110):
```cpp
CALC.Tracking_Simu(THEO);  // Use THEO or REAL
```

**Enable Kalman Filter Estimation:**
Switch from `Tracking_Simu()` to `Tracking_Simu_WithEsti()` and call:
```cpp
CALC.Estimation_Init(WTR);  // WTR or GLY
CALC.Tracking_Simu_WithEsti(THEO, CONST_KF);  // or ADAPT_KF
```

## Required Data Files

All data files must be in `jul3_2014_motioncontrol_hallsensor_akf_ar2\` directory:

**Essential (program will crash without these):**
- `KI_10Hz.txt` - 6×6 magnetic force coupling matrix
- `InvKIreal.txt` - 6×6 inverse coupling matrix
- `Feb28_2013 Coeff2nd_0CenterErr_I1_1891x20.txt` (through I6) - Polynomial coefficients for 6 poles
- `Thermal Force all XYZ Water DampInc 10sec,10sec,10sec 100000SplRate (SI unit).txt`
- `Thermal Noise XYZ 1612SplRate (SI unit).txt`
- `Meas. Noise XYZ (SI unit).txt`

**File loading is NOT optional** - the main program (lines 45-58) reads thermal data, and `Tracking_Simu()` unconditionally uses these arrays.

## Common Issues and Solutions

### Issue: Access Violation / Program Crash

**Cause:** Uninitialized pointers in constructor
**Solution:** Ensure all pointer members are initialized to NULL in `CALCULATION::CALCULATION()` (see lines 201-224)

### Issue: "Read KI matrix fail!" or "Read InvKIreal fail!"

**Cause:** Missing data files or wrong working directory
**Solution:**
- Ensure all required .txt files are in the same directory as the .exe
- Or run from `jul3_2014_motioncontrol_hallsensor_akf_ar2\` directory

### Issue: Chinese Characters Display as Gibberish

**Cause:** File encoding mismatch (Big5 vs UTF-8)
**Solution:** Already fixed with `/utf-8` compiler flag in project settings. For new Chinese comments:
- Set Visual Studio to save as "UTF-8 with signature"
- Tools → Options → Environment → Documents → Enable UTF-8 encoding
- Existing garbled comments cannot be automatically recovered

## Key Physical Parameters

**Force Model:**
- 6-pole magnetic trap configuration
- Force coupling: F = KI × I (current vector)
- Position-dependent via 1891-point lookup table with polynomial interpolation

**Hydrodynamics:**
- Stokes drag: F_drag = 6πηrv
- Water vs Glycerol damping coefficients
- Temperature: 294K

**Noise Models:**
- Brownian motion: Gaussian white noise from kBT
- Measurement noise: Hall sensor + position detection
- Thermal force: From MATLAB-generated data

**AR2 Kalman Filter State Space:**
- State vector per axis: [position, velocity, force]
- 3×3 transition matrix PHI
- Process noise covariance Qw (3×3)
- Measurement noise covariance R (scalar per axis)

## Debug Log

Detailed debugging history and solutions are in `.claude/debug_log.md`, including:
- 5 resolved issues (macro redefinition, CRT warnings, access violations, pointer initialization, encoding)
- Root cause analysis for each issue
- Step-by-step fixes

## Project-Specific Conventions

- **Coordinate Systems:** Two systems used throughout:
  - `Om` (Measurement coordinate)
  - `Oa` (Actuation coordinate)
  - Transform via `id_m2a` and `id_a2m` matrices

- **Unit Suffixes:**
  - `_um` = micrometers
  - `_pN` = piconewtons
  - `_Oa` = in actuation coordinate
  - `_Om` = in measurement coordinate

- **Naming:**
  - `Real` vs `Meas` positions (real = ground truth, meas = with delay/noise)
  - `KItheo` vs `KIreal` (theoretical vs experimental matrices)

- **Memory Management:** Manual new/delete, no smart pointers
  - All dynamic arrays must be manually deleted in `Simu_Finalize()`
  - Destructor handles coefficient arrays

## Visual Studio Project Settings

**Critical preprocessor definitions** (already configured):
- `_CRT_SECURE_NO_WARNINGS` - Suppresses sprintf warnings
- `/utf-8` compiler flag - Handles UTF-8 source files correctly

**Build configuration:**
- Platform: Win32 (32-bit)
- Toolset: v143 (Visual Studio 2022)
- Character Set: Unicode

## Output Format

Simulation generates space-separated text files with columns:
```
Target_X Target_Y Target_Z Measured_X Measured_Y Measured_Z Real_X Real_Y Real_Z I1 I2 I3 I4 I5 I6 Force_X Force_Y Force_Z ...
```

Each row = one time sample at 1612 Hz
Typical simulation: 30,000-40,000 samples (~20-25 seconds)

## Testing Changes

After modifying control parameters or algorithms:
1. Rebuild (F7)
2. Run and observe progress output (should reach ~99.9%)
3. Check output file is generated
4. Verify file size is reasonable (>1 MB for typical simulation)
5. Plot results in MATLAB/Python to validate trajectory and forces
- 盡量不要態頻繁隨意的生成說明文件，先再跟我說明讓我了解後，如果我有需要再幫我生成，也可以詢問我需不需要
- 不要太頻繁的 git commit 尤其是在對於新生成的腳本或是新的程式碼做修改微調的過程中，到有一定進度解決好了某些內容再commit