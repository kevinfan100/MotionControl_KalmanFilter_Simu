# Motion Control Kalman Filter Simulation

A C++ simulation framework for optical trap motion control with integrated Kalman filter state estimation.

## Overview

This project simulates 3D particle trapping in an optical tweezers system using a 6-pole magnetic force coil configuration. It implements multiple control algorithms (PID, Sliding Mode) and state estimation techniques (Kalman filtering) for precision micro-manipulation at the piconewton force scale.

## Features

- **Multiple Control Schemes**: PID, Sliding Mode, Super-Twisting Sliding Mode
- **State Estimation**: Constant and Adaptive Kalman Filters with AR2 model
- **High-Fidelity Physics**: Includes thermal noise, drag forces, and measurement noise
- **Flexible Simulation**: Supports positioning and tracking (ramp/circular) trajectories
- **Experimental Validation**: Uses real calibration data from optical trap experiments

## Requirements

- Visual Studio 2022
- Windows SDK
- Required data files (see below)

## Building

1. Open `Jul3_2014_MotionControl_HallSensor_AKF_AR2.sln` in Visual Studio 2022
2. Select **Release** configuration and **Win32** platform
3. Build the solution (F7)

The executable will be created at: `Release\Jul3_2014_MotionControl_HallSensor_AKF_AR2.exe`

## Required Data Files

Place these files in the `jul3_2014_motioncontrol_hallsensor_akf_ar2\` directory:

- `KI_10Hz.txt` - Magnetic force coupling matrix (6×6)
- `InvKIreal.txt` - Inverse coupling matrix (6×6)
- `Feb28_2013 Coeff2nd_0CenterErr_I[1-6]_1891x20.txt` - Inverse model coefficients (6 files)

Optional (for noise simulation):
- `Thermal Noise XYZ 1612SplRate (SI unit).txt`
- `Meas. Noise XYZ (SI unit).txt`
- `Thermal Force *.txt`

## Running

```bash
cd Release
.\Jul3_2014_MotionControl_HallSensor_AKF_AR2.exe
```

The program will:
1. Load coefficient matrices
2. Run the simulation (default: ramp tracking with PID control)
3. Generate output file with results
4. Pause for user input

## Configuration

Edit `Jul3_2014_MotionControl_HallSensor_AKF_AR2.cpp` to configure:

**PID Gains** (lines 107):
```cpp
CALC.Set_PIDgain(50, 50, 50, 200, 200, 200, 0, 0, 0);
// Kp_xyz, Ki_xyz, Kd_xyz
```

**Trajectory** (line 102):
```cpp
CALC.RampTracking_Init(40.0, 0.0, 0, 200, 0);
// RangeXY(μm), RangeZ(μm), Direction, Speed(μm/s), Angle(deg)
```

**Inverse Model** (line 110):
```cpp
CALC.Tracking_Simu(THEO);  // THEO or REAL
```

## Output

Simulation results are saved as space-separated text files containing:
- Target position (X, Y, Z) in μm
- Measured position (with sensor delay)
- Real position (ground truth)
- Current values (I1-I6) for 6 poles
- Force values in piconewtons

Example filename: `TRACKING Ramp (0.0 degree) Simu WTR Pgain(50,50,50) Igain(200,200,200) Dgain(0,0,0) (9-21-2016).txt`

## Project Structure

```
MotionControl_KalmanFilter_Simu/
├── Jul3_2014_MotionControl_HallSensor_AKF_AR2.sln    # VS Solution
├── jul3_2014_motioncontrol_hallsensor_akf_ar2/
│   ├── Jul3_2014_MotionControl_HallSensor_AKF_AR2.cpp  # Main entry (260 lines)
│   ├── CALCULATION.cpp                                  # Core engine (3,100 lines)
│   ├── CALCULATION.h                                    # Class definition (444 lines)
│   ├── Jul3_2014_MotionControl_HallSensor_AKF_AR2.vcxproj
│   └── [Data files: KI_10Hz.txt, InvKIreal.txt, etc.]
├── Release/                                             # Compiled executables
├── .claude/                                             # Claude Code config
│   ├── claude_code.json
│   ├── project_overview.md
│   └── commands/
└── README.md
```

## Key Parameters

| Parameter | Value | Description |
|-----------|-------|-------------|
| Sampling Rate | 1,612 Hz | Control loop frequency |
| Simulation Rate | 100,000 Hz | Internal physics rate |
| Workspace | 550 μm | Normalized radius |
| Temperature | 294 K | For thermal noise |
| Control Delay | 2 samples | ~1.24 ms |

## Recent Updates (2025-10-29)

### Bug Fixes
1. **Fixed macro redefinition error** in `CALCULATION.h`
   - Removed duplicate `N2pN` definition (conflicting values: 1e12 vs 1e-12)

2. **Suppressed CRT security warnings**
   - Added `_CRT_SECURE_NO_WARNINGS` to preprocessor definitions
   - Enables compilation without modifying `sprintf` calls

### Claude Code Integration
- Added `.claude/` directory with project documentation
- Created custom commands: `/build`, `/explain-structure`, `/run-simulation`
- Project metadata in `claude_code.json`

## Claude Code Commands

If using Claude Code, you can use these slash commands:

- `/build` - Instructions for building the project
- `/explain-structure` - Detailed architecture explanation
- `/run-simulation` - Guide for running simulations

## License

Academic research project (license not specified).

## Authors

Original development: Jul 3, 2014
Last updated: Oct 29, 2025

## References

This simulation implements algorithms from research on:
- Optical tweezers force control
- Kalman filtering for micro-manipulation
- Magnetic force actuation in optical traps

For more details, see `.claude/project_overview.md`.
