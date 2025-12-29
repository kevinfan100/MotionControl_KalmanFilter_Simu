#include <iostream>
using namespace std;
#include <time.h>
#include <stdlib.h>

#include "CALCULATION.h"
#include "cmath"

int main()
{
	CALCULATION CALC;
	CALC.OutputParameter();

	srand((int)time(0));
	int THEO_or_REAL = THEO;
	//int THEO_or_REAL = REAL;


	//#############################################  //test inverse model
	//****** Force Calculation test (Debug); ******
	//#############################################
    double PosTest_umOa[3] = {0.0, 0.0, 0.0};
    double FdTest_pNOa[3]  = {5.0, 0.0, 0.0};

	// === Output Input Values ===
	cout << "\n=== Inverse Model Test ===" << endl;
	cout << "Input Position (um): " << PosTest_umOa[X] << ", " << PosTest_umOa[Y] << ", " << PosTest_umOa[Z] << endl;
	cout << "Input Force    (pN): " << FdTest_pNOa[X] << ", " << FdTest_pNOa[Y] << ", " << FdTest_pNOa[Z] << endl;

	// Save original force values (will be modified by function)
	double FdTest_original[3] = {FdTest_pNOa[X], FdTest_pNOa[Y], FdTest_pNOa[Z]};

    //**** Theo: ****
    CALC.OptInvMdl_Curr_FromLS_KItheo (FdTest_pNOa, PosTest_umOa);
    CALC.Calc_FNor_FromSixCurr_KItheo (PosTest_umOa, CALC.I1toI6_KItheo);
    double FXcalc_pN_Oa = CALC.FNor_Curr_KItheo[X]*CALC.FGain_x;
    double FYcalc_pN_Oa = CALC.FNor_Curr_KItheo[Y]*CALC.FGain_y;
    double FZcalc_pN_Oa = CALC.FNor_Curr_KItheo[Z]*CALC.FGain_z;
    ////**** Real: ****
    //CALC.OptInvMdl_Curr_FromLS_KIreal (FdTest_pNOa, PosTest_umOa);
    //CALC.Calc_FNor_FromSixCurr_KIreal (PosTest_umOa, CALC.I1toI6_KIreal);
    //double FXcalc_pN_Oa = CALC.FNor_Curr_KIreal[X]*CALC.FGain_x;
    //double FYcalc_pN_Oa = CALC.FNor_Curr_KIreal[Y]*CALC.FGain_y;
    //double FZcalc_pN_Oa = CALC.FNor_Curr_KIreal[Z]*CALC.FGain_z;

	// === Calculate Error ===
	double err_x = FXcalc_pN_Oa - FdTest_original[X];
	double err_y = FYcalc_pN_Oa - FdTest_original[Y];
	double err_z = FZcalc_pN_Oa - FdTest_original[Z];

	// === Output Results ===
	cout << "Output Force   (pN): " << FXcalc_pN_Oa << ", " << FYcalc_pN_Oa << ", " << FZcalc_pN_Oa << endl;
	cout << "Error          (pN): " << err_x << ", " << err_y << ", " << err_z << endl;
	//##########################################
	//****** (end) Force Calculation test ******
	//##########################################


	//=====================================================
	// Hall Sensor-based Force Model Verification
	//=====================================================
	cout << "\n=== Hall Sensor-based Verification ===" << endl;

	// 0. Print 6 currents (I1 to I6 from KItheo)
	cout << "Current I1-I6: ";
	for (int i = 0; i < 6; i++) {
		cout << CALC.I1toI6_KItheo[i] << " ";
	}
	cout << endl;

	// 1. Current -> Hall Voltage: V_m = D_H^-1 * K_I * I
	double V_m[6];
	CALC.Calc_HallVoltage_FromCurrent(CALC.I1toI6_KItheo, V_m);

	cout << "Hall Voltage V_m: ";
	for (int i = 0; i < 6; i++) {
		cout << V_m[i] << " ";
	}
	cout << endl;

	// 2. Hall Voltage -> Force: F = g_I * Phi^T * L * Phi
	CALC.Calc_FNor_FromHallVoltage(PosTest_umOa, V_m);
	double FX_Hall_pN = CALC.FNor_Hall[X] * CALC.FGain_x;
	double FY_Hall_pN = CALC.FNor_Hall[Y] * CALC.FGain_y;
	double FZ_Hall_pN = CALC.FNor_Hall[Z] * CALC.FGain_z;

	cout << "Hall Force (pN): " << FX_Hall_pN << ", " << FY_Hall_pN << ", " << FZ_Hall_pN << endl;

	// 3. Compare with Current-based result
	double diff_x = FX_Hall_pN - FXcalc_pN_Oa;
	double diff_y = FY_Hall_pN - FYcalc_pN_Oa;
	double diff_z = FZ_Hall_pN - FZcalc_pN_Oa;

	cout << "Difference (pN): " << diff_x << ", " << diff_y << ", " << diff_z << endl;
	cout << "==========================================" << endl;

	// 4. Print Nx_KItheo matrix (6x6)
	cout << "\n=== Nx_KItheo Matrix (6x6) ===" << endl;
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 6; j++) {
			cout << CALC.Nx_KItheo[i][j] << "\t";
		}
		cout << endl;
	}
	cout << "==============================" << endl;
	//=====================================================
	// (end) Hall Sensor-based Force Model Verification
	//=====================================================



	//====================================================================
	// All simulation sections below are commented out for Force Calculation Test
	//====================================================================

	//####################################################################
	//****** Read Thermal Motion, Measurement Noise, Thermal Force: ******  //100k
	//####################################################################
	//char fname_ThmMotionXYZ[200],
	//	 fname_MeasNoiseXYZ[200],
	//	 fname_ThmForceXYZ[200];
	//sprintf(fname_ThmMotionXYZ, "Thermal Noise XYZ 1612SplRate (SI unit).txt");
	//sprintf(fname_MeasNoiseXYZ, "Meas. Noise XYZ (SI unit).txt");
	//sprintf(fname_ThmForceXYZ,  "Thermal Force all XYZ Water DampInc 10sec,10sec,10sec 100000SplRate (SI unit).txt");
	//CALC.Read_ThmMotion_XYZ(fname_ThmMotionXYZ);
	//CALC.Read_ThmForce_XYZ (fname_ThmForceXYZ);
	//CALC.Read_MeasNoise_XYZ(fname_MeasNoiseXYZ);
	//#########################################################################
	//****** (end) Read Thermal Motion, Measurement Noise, Thermal Force ******
	//#########################################################################


	//###############################
	//****** Positioning Only: ******
	//###############################
	//int NUM_Simu_Meas = 40000;
	//CALC.Trap_Target_umOm[X] = 0.0;
	//CALC.Trap_Target_umOm[Y] = 0.0;
	//CALC.Trap_Target_umOm[Z] = 0.0;
	//CALC.Positioning_Simu_Init( NUM_Simu_Meas );
	//CALC.Estimation_Init( WTR );
	//double Kp = 30.0;
	//CALC.Set_PIDgain( Kp,Kp,Kp,  0.0,0.0,0.0,  0.0,0.0,0.0 );
	//CALC.Calc_Ctrl_Coeff_Force();
	//CALC.Positioning_Simu( THEO );
	//char fname_TrapRst [200];
	//sprintf( fname_TrapRst,"Trapping Simu WTR at(%3.1f,%3.1f,%3.1f)um Pgain(%3.1f,%3.1f,%3.1f) (10-30-2025).txt",
	//	CALC.Trap_Target_umOm[X], CALC.Trap_Target_umOm[Y], CALC.Trap_Target_umOm[Z],
	//	CALC.Pgain[X], CALC.Pgain[Y], CALC.Pgain[Z]);
	//CALC.Write_SimuRst_Target_Meas_Real_Curr_Force(fname_TrapRst);
	//CALC.Simu_Finalize();
	//####################################
	//****** (end) Positioning Only ******
	//####################################


	//############################
	//****** Tracking Only: ******
	//############################
	//double RangeXY_um = 40.0;
	//double RangeZ_um = 20.0;
	//int UP_or_DOWN = 0;
	//int Speed_um_per_s = 200;
	//double deg = 45.0;
	//CALC.RampTracking_Init(RangeXY_um, RangeZ_um, UP_or_DOWN, Speed_um_per_s, deg);
	//CALC.ThmF_Counter = 0;
	//CALC.Index = 0;
	//double Kp = 35.0;
	//double Ki = 0.0;
	//double Kd = 1.0;
	//CALC.Set_PIDgain( Kp,Kp,Kp,  Ki,Ki,Ki,  Kd,Kd,Kd );
	//CALC.Calc_Ctrl_Coeff_Force();
	//CALC.Tracking_Simu( THEO );
	//char fname_TrapRst [300];
	//sprintf( fname_TrapRst,"TRACKING_Ramp_%dum_%dums_%ddeg_WTR_THEO_P%d_I%d_D%d_20251102.txt",
	//	(int)RangeXY_um, Speed_um_per_s, (int)deg,
	//	(int)CALC.Pgain[X], (int)CALC.Igain[X], (int)CALC.Dgain[X]);
	//CALC.Write_SimuRst_Target_Meas_Real_Curr_Force(fname_TrapRst);
	//CALC.Simu_Finalize();
	//#################################
	//****** (end) Tracking Only ******
	//#################################


	//################################################
	//****** Positioning & Const Kalman Filter: ******
	//################################################
	// (already commented out)
	//#####################################################
	//****** (end) Positioning & Const Kalman Filter ******
	//#####################################################


	//#########################################
	//#### Tracking & Const Kalman Filter: ####
	//#########################################
	// (already commented out)
	//#################################################
	//#### (end) Tracking & Const Kalman Filter ####
	//#################################################


	//#########################################
	//#### Tracking & Adapt Kalman Filter: ####
	//#########################################
	// (already commented out)
	//#################################################
	//#### (end) Tracking & Adapt Kalman Filter ####
	//#################################################


	system("pause");
	return 0;
}