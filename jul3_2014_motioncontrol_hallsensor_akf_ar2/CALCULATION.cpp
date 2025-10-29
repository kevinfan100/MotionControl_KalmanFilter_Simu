#include "CALCULATION.h"
#include <math.h>
#include <windows.h>
#include <iostream>
#include <cstdlib>
#include <iomanip>
#include <stdlib.h>
#include <time.h>


//constrctor:
CALCULATION::CALCULATION()
{
	Simu_StepNum = SimuRate/SplRate; //Simulation Step Number

	for (int i=0; i<18; i++)
	{
		Bias_umOa_P1toP6[i] = 0.0; //****//Default Bias: 0.0
	}

	//########################
	//****** KI matrix: ******
	//########################
	KItheo[0][0] =  5.0/6.0;  KItheo[0][1] = -1.0/6.0;  KItheo[0][2] = -1.0/6.0;  KItheo[0][3] = -1.0/6.0;  KItheo[0][4] = -1.0/6.0;  KItheo[0][5] = -1.0/6.0;
	KItheo[1][0] = -1.0/6.0;  KItheo[1][1] =  5.0/6.0;  KItheo[1][2] = -1.0/6.0;  KItheo[1][3] = -1.0/6.0;  KItheo[1][4] = -1.0/6.0;  KItheo[1][5] = -1.0/6.0;
	KItheo[2][0] = -1.0/6.0;  KItheo[2][1] = -1.0/6.0;  KItheo[2][2] =  5.0/6.0;  KItheo[2][3] = -1.0/6.0;  KItheo[2][4] = -1.0/6.0;  KItheo[2][5] = -1.0/6.0;
	KItheo[3][0] = -1.0/6.0;  KItheo[3][1] = -1.0/6.0;  KItheo[3][2] = -1.0/6.0;  KItheo[3][3] =  5.0/6.0;  KItheo[3][4] = -1.0/6.0;  KItheo[3][5] = -1.0/6.0;
	KItheo[4][0] = -1.0/6.0;  KItheo[4][1] = -1.0/6.0;  KItheo[4][2] = -1.0/6.0;  KItheo[4][3] = -1.0/6.0;  KItheo[4][4] =  5.0/6.0;  KItheo[4][5] = -1.0/6.0;
	KItheo[5][0] = -1.0/6.0;  KItheo[5][1] = -1.0/6.0;  KItheo[5][2] = -1.0/6.0;  KItheo[5][3] = -1.0/6.0;  KItheo[5][4] = -1.0/6.0;  KItheo[5][5] =  5.0/6.0;
	for (int i=0; i<6; i++)
		for (int j=0; j<6; j++)
		{
			KItheo_T[i][j] = KItheo[j][i];
		}


	TipCoeff_KIreal = 1.2;

	ifstream str_KIreal;
	str_KIreal.open("KI_10Hz.txt");
	if ( str_KIreal.fail() )
	{
		cout<<"Read KI matrix fail!"<<endl;
	    exit(1);
	}
	else
	{
		for (int i=0; i<6; i++)
			for (int j=0; j<6; j++)
			{
				str_KIreal >> KIreal[i][j];
				KIreal_T[j][i] = KIreal[i][j];

				if (i==0 || i==2 || i==5) // pole 1,3 or 6
				{ 
					KIreal[i][j] *= TipCoeff_KIreal; 
				}

				KIreal_T[j][i] = KIreal[i][j];
			}
		str_KIreal.close();
	}

	//******//
	ifstream str_InvKIreal;
	str_InvKIreal.open("InvKIreal.txt"); // (This inverse already considered the TipCoeff_KIreal)
	if ( str_InvKIreal.fail() )
	{
		cout<<"Read InvKIreal fail!"<<endl;
		exit(1);
	}
	else
	{
		for (int i=0; i<6; i++)
			for (int j=0; j<6; j++)
			{
				str_InvKIreal >> InvKIreal[i][j];
				InvKIreal_T[j][i] = InvKIreal[i][j];
			}
		str_InvKIreal.close();
	}
	//#############################
	//****** (end) KI matrix ******
	//#############################


	//****** Coordinate Transform: ******
	id_a2m[0][0] = -1.0/sqrt(6.0);  id_a2m[0][1] = -1.0/sqrt(6.0);  id_a2m[0][2] = -sqrt(2.0/3.0);
	id_a2m[1][0] =  1.0/sqrt(2.0);  id_a2m[1][1] = -1.0/sqrt(2.0);  id_a2m[1][2] = 0.0;
	id_a2m[2][0] = -1.0/sqrt(3.0);  id_a2m[2][1] = -1.0/sqrt(3.0);  id_a2m[2][2] = 1.0/sqrt(3.0);
	CalcInverseMat_3x3(id_a2m,id_m2a);

	FGain_x = 8.0;
	FGain_y = 8.0;
	FGain_z = 8.0;


   //***********************************
   //****** Reading Coeff I1toI6: ******
   //***********************************
   OptInvMdl_RowNum = 1891;
   OptInvMdl_ColNum = 10;

   InvMdl_Coeff_I1 = new double* [OptInvMdl_RowNum];
   InvMdl_Coeff_I2 = new double* [OptInvMdl_RowNum];
   InvMdl_Coeff_I3 = new double* [OptInvMdl_RowNum];
   InvMdl_Coeff_I4 = new double* [OptInvMdl_RowNum];
   InvMdl_Coeff_I5 = new double* [OptInvMdl_RowNum];
   InvMdl_Coeff_I6 = new double* [OptInvMdl_RowNum];
   for (int i=0; i<OptInvMdl_RowNum; i++)
   {
      InvMdl_Coeff_I1[i] = new double [OptInvMdl_ColNum];
      InvMdl_Coeff_I2[i] = new double [OptInvMdl_ColNum];
      InvMdl_Coeff_I3[i] = new double [OptInvMdl_ColNum];
      InvMdl_Coeff_I4[i] = new double [OptInvMdl_ColNum];
      InvMdl_Coeff_I5[i] = new double [OptInvMdl_ColNum];
      InvMdl_Coeff_I6[i] = new double [OptInvMdl_ColNum];
   }

   ifstream InvMdl_Stream1, 
	        InvMdl_Stream2, 
			InvMdl_Stream3, 
			InvMdl_Stream4, 
			InvMdl_Stream5, 
			InvMdl_Stream6;

   InvMdl_Stream1.open("Feb28_2013 Coeff2nd_0CenterErr_I1_1891x20.txt");
   InvMdl_Stream2.open("Feb28_2013 Coeff2nd_0CenterErr_I2_1891x20.txt");
   InvMdl_Stream3.open("Feb28_2013 Coeff2nd_0CenterErr_I3_1891x20.txt");
   InvMdl_Stream4.open("Feb28_2013 Coeff2nd_0CenterErr_I4_1891x20.txt");
   InvMdl_Stream5.open("Feb28_2013 Coeff2nd_0CenterErr_I5_1891x20.txt");
   InvMdl_Stream6.open("Feb28_2013 Coeff2nd_0CenterErr_I6_1891x20.txt");


   if ( InvMdl_Stream1.fail() || InvMdl_Stream2.fail() || InvMdl_Stream3.fail() || InvMdl_Stream4.fail() || InvMdl_Stream5.fail() || InvMdl_Stream6.fail() )
   {
	   MessageBox(NULL,L"opening file fail",L"Coeff",MB_OK);
	   exit(1);       
   }
   else
   {
       for (int i=0; i<OptInvMdl_RowNum; i++)
		   for (int j=0; j<OptInvMdl_ColNum; j++)
		   {
		       InvMdl_Stream1 >> InvMdl_Coeff_I1[i][j];
               InvMdl_Stream2 >> InvMdl_Coeff_I2[i][j];
               InvMdl_Stream3 >> InvMdl_Coeff_I3[i][j];
               InvMdl_Stream4 >> InvMdl_Coeff_I4[i][j];
               InvMdl_Stream5 >> InvMdl_Coeff_I5[i][j];
               InvMdl_Stream6 >> InvMdl_Coeff_I6[i][j];
		   }
   }
   InvMdl_Stream1.close();
   InvMdl_Stream2.close();
   InvMdl_Stream3.close();
   InvMdl_Stream4.close();
   InvMdl_Stream5.close();
   InvMdl_Stream6.close();
   //****************************************
   //****** (end) Reading Coeff I1toI6 ******
   //****************************************


   // ****** For control scheme: ******
   PosCtrl_Counter = 0;

   Pgain[X] = 25.0; Pgain[Y] = 25.0; Pgain[Z] = 25.0;
   Igain[X] = 0.0;  Igain[Y] = 0.0;  Igain[Z] = 0.0;
   Dgain[X] = 0.0;  Dgain[Y] = 0.0;  Dgain[Z] = 0.0; //(Default) PID Control Gain

   SM_Mag = 20.0;  //(Default) sliding mode magnitute

   SupTwist_M = 2.0;
   SupTwist_a = 2.0;

   Coeff_ErrPos_pNOm [X] = 0.0; Coeff_ErrPos_pNOm [Y] = 0.0; Coeff_ErrPos_pNOm [Z] = 0.0;
   Coeff_ErrPos1_pNOm[X] = 0.0; Coeff_ErrPos1_pNOm[Y] = 0.0; Coeff_ErrPos1_pNOm[Z] = 0.0;
   Coeff_ErrPos2_pNOm[X] = 0.0; Coeff_ErrPos2_pNOm[Y] = 0.0; Coeff_ErrPos2_pNOm[Z] = 0.0;

   Fd_pNOa [X] = 0.0; Fd_pNOa [Y] = 0.0; Fd_pNOa [Z] = 0.0;
   Fd1_pNOa[X] = 0.0; Fd1_pNOa[Y] = 0.0; Fd1_pNOa[Z] = 0.0;
   Fd2_pNOa[X] = 0.0; Fd2_pNOa[Y] = 0.0; Fd2_pNOa[Z] = 0.0; //from controller

   Fd_pNOm [X] = 0.0; Fd_pNOm [Y] = 0.0; Fd_pNOm [Z] = 0.0;
   Fd1_pNOm[X] = 0.0; Fd1_pNOm[Y] = 0.0; Fd1_pNOm[Z] = 0.0;
   Fd2_pNOm[X] = 0.0; Fd2_pNOm[Y] = 0.0; Fd2_pNOm[Z] = 0.0; //from controller

   ERRpos_umOm [X] = 0.0; ERRpos_umOm[Y]  = 0.0; ERRpos_umOm [Z] = 0.0;
   ERRpos1_umOm[X] = 0.0; ERRpos1_umOm[Y] = 0.0; ERRpos1_umOm[Z] = 0.0;
   ERRpos2_umOm[X] = 0.0; ERRpos2_umOm[Y] = 0.0; ERRpos2_umOm[Z] = 0.0;

   Vsuptwist_pNOm [X] = 0.0; Vsuptwist_pNOm [Y] = 0.0; Vsuptwist_pNOm [Z] = 0.0;
   V1suptwist_pNOm[X] = 0.0; V1suptwist_pNOm[Y] = 0.0; V1suptwist_pNOm[Z] = 0.0;

   ThmF_Counter = 0;
   //
   Px_umOa_k1 = 0;
   Py_umOa_k1 = 0;
   Pz_umOa_k1 = 0;

   // Initialize all pointers to NULL to prevent access violation
   F_Ext_x_pN_Apply = NULL;
   F_Ext_y_pN_Apply = NULL;
   F_Ext_z_pN_Apply = NULL;

   ThmMotion_X_RD = NULL;
   ThmMotion_Y_RD = NULL;
   ThmMotion_Z_RD = NULL;

   ThmForce_X_NOa_RD = NULL;
   ThmForce_Y_NOa_RD = NULL;
   ThmForce_Z_NOa_RD = NULL;

   MeasNoise_X_mOm_RD = NULL;
   MeasNoise_Y_mOm_RD = NULL;
   MeasNoise_Z_mOm_RD = NULL;

   DampWtr_X_Oa_RD = NULL;
   DampWtr_Y_Oa_RD = NULL;
   DampWtr_Z_Oa_RD = NULL;

   DampGly_X_Oa_RD = NULL;
   DampGly_Y_Oa_RD = NULL;
   DampGly_Z_Oa_RD = NULL;
}
//constrctor ends here


// destructor starts here:
CALCULATION::~CALCULATION()
{
	for (int i=0; i<OptInvMdl_RowNum; i++)
	{
		delete [] InvMdl_Coeff_I1[i];
		delete [] InvMdl_Coeff_I2[i];
		delete [] InvMdl_Coeff_I3[i];
		delete [] InvMdl_Coeff_I4[i];
		delete [] InvMdl_Coeff_I5[i];
		delete [] InvMdl_Coeff_I6[i];
	}
	delete [] InvMdl_Coeff_I1;
	delete [] InvMdl_Coeff_I2;
	delete [] InvMdl_Coeff_I3;
	delete [] InvMdl_Coeff_I4;
	delete [] InvMdl_Coeff_I5;
	delete [] InvMdl_Coeff_I6;
}
// destructor ends here


void CALCULATION::Simu_Finalize()
{
   delete [] Simu_TX_umOm;  Simu_TX_umOm = NULL;
   delete [] Simu_TY_umOm;  Simu_TY_umOm = NULL;  
   delete [] Simu_TZ_umOm;  Simu_TZ_umOm = NULL;  
   delete [] Simu_TX_umOa;  Simu_TX_umOa = NULL;
   delete [] Simu_TY_umOa;  Simu_TY_umOa = NULL;
   delete [] Simu_TZ_umOa;  Simu_TZ_umOa = NULL;

   delete [] Simu_RX_umOm;  Simu_RX_umOm = NULL;  
   delete [] Simu_RY_umOm;  Simu_RY_umOm = NULL;  
   delete [] Simu_RZ_umOm;  Simu_RZ_umOm = NULL;
   delete [] Simu_RX_umOa;  Simu_RX_umOa = NULL;
   delete [] Simu_RY_umOa;  Simu_RY_umOa = NULL;
   delete [] Simu_RZ_umOa;  Simu_RZ_umOa = NULL;

   delete [] Simu_MX_umOm;  Simu_MX_umOm = NULL;
   delete [] Simu_MY_umOm;  Simu_MY_umOm = NULL;
   delete [] Simu_MZ_umOm;  Simu_MZ_umOm = NULL;
   delete [] Simu_MX_umOa;  Simu_MX_umOa = NULL;
   delete [] Simu_MY_umOa;  Simu_MY_umOa = NULL;
   delete [] Simu_MZ_umOa;  Simu_MZ_umOa = NULL;

   delete [] TRK_Vx_Om;  TRK_Vx_Om = NULL;
   delete [] TRK_Vy_Om;  TRK_Vy_Om = NULL;
   delete [] TRK_Vz_Om;  TRK_Vz_Om = NULL;
   delete [] Trk_Vx_Oa;  Trk_Vx_Oa = NULL;
   delete [] Trk_Vy_Oa;  Trk_Vy_Oa = NULL;
   delete [] Trk_Vz_Oa;  Trk_Vz_Oa = NULL;

   delete [] Simu_I1;  Simu_I1 = NULL;
   delete [] Simu_I3;  Simu_I3 = NULL;
   delete [] Simu_I5;  Simu_I5 = NULL; 
   delete [] Simu_I2;  Simu_I2 = NULL;
   delete [] Simu_I4;  Simu_I4 = NULL;
   delete [] Simu_I6;  Simu_I6 = NULL;

   delete [] Simu_FdX_pNOm;  Simu_FdX_pNOm = NULL;
   delete [] Simu_FdY_pNOm;  Simu_FdY_pNOm = NULL;
   delete [] Simu_FdZ_pNOm;  Simu_FdZ_pNOm = NULL;

   delete [] Simu_FdX_pNOa;  Simu_FdX_pNOa = NULL;
   delete [] Simu_FdY_pNOa;	 Simu_FdY_pNOa = NULL;
   delete [] Simu_FdZ_pNOa;	 Simu_FdZ_pNOa = NULL;

   delete [] Simu_FdX_norOm;  Simu_FdX_norOm = NULL;
   delete [] Simu_FdY_norOm;  Simu_FdY_norOm = NULL;
   delete [] Simu_FdZ_norOm;  Simu_FdZ_norOm = NULL;

   delete [] Simu_FdX_norOa;  Simu_FdX_norOa = NULL;
   delete [] Simu_FdY_norOa;  Simu_FdY_norOa = NULL;
   delete [] Simu_FdZ_norOa;  Simu_FdZ_norOa = NULL;

   delete [] Simu_FXcalc_pNOa;  Simu_FXcalc_pNOa = NULL;
   delete [] Simu_FYcalc_pNOa;	Simu_FYcalc_pNOa = NULL;
   delete [] Simu_FZcalc_pNOa;	Simu_FZcalc_pNOa = NULL;
}



void CALCULATION::Read_ThmMotion_XYZ( char* fname_ThmMotionXYZ )  //The White Gaussian Noise is generated by MATLAB
{
	ifstream  istr_ThmMotionXYZ;
	istr_ThmMotionXYZ.open( fname_ThmMotionXYZ );

	if ( istr_ThmMotionXYZ.fail() )
	{
		MessageBox(NULL,L"Reading Thermal Motion Fail !",L"Thermal Motion",MB_OK);
		exit(1);
	}
	else
	{
		istr_ThmMotionXYZ >> LEN_ThmMotion;
		ThmMotion_X_RD = new double [LEN_ThmMotion];
		ThmMotion_Y_RD = new double [LEN_ThmMotion];
		ThmMotion_Z_RD = new double [LEN_ThmMotion];
		
		for (int i=0; i<LEN_ThmMotion; i++)
		{
			istr_ThmMotionXYZ >> ThmMotion_X_RD[i] >> 
				                 ThmMotion_Y_RD[i] >> 
								 ThmMotion_Z_RD[i];
		}
		istr_ThmMotionXYZ.close();
	}
}


// This will be used in position simulation:
void CALCULATION::Read_ThmForce_XYZ (char* fname_ThmForceXYZ)
{
	ifstream istr_ThmForceXYZ;
	istr_ThmForceXYZ.open( fname_ThmForceXYZ );

	if ( istr_ThmForceXYZ.fail() )
	{
		MessageBox(NULL,L"Reading Thermal Force Fail !",L"Thermal Motion",MB_OK);
		exit(1);
	}
	else
	{
		istr_ThmForceXYZ >> LEN_ThmForce;
		DampWtr_X_Oa_RD   = new double [LEN_ThmForce];
		ThmForce_X_NOa_RD = new double [LEN_ThmForce];
		//
		DampWtr_Y_Oa_RD   = new double [LEN_ThmForce];
		ThmForce_Y_NOa_RD = new double [LEN_ThmForce];
		//
		DampWtr_Z_Oa_RD   = new double [LEN_ThmForce];
		ThmForce_Z_NOa_RD = new double [LEN_ThmForce];
		
		cout<<"Reading Thermal Force: ...\n";
		for (int i=0; i<LEN_ThmForce; i++)
		{
			if (i%10000==0)
			{
				cout<<"% "<<(double)i/LEN_ThmForce * 100 <<endl;
			}

			istr_ThmForceXYZ >> DampWtr_X_Oa_RD[i]>>
				                ThmForce_X_NOa_RD[i]>> 
								DampWtr_Y_Oa_RD[i]>>
				                ThmForce_Y_NOa_RD[i]>>
								DampWtr_Z_Oa_RD[i]>>
								ThmForce_Z_NOa_RD[i];
		}
		istr_ThmForceXYZ.close();
	}
}


void CALCULATION::Read_MeasNoise_XYZ( char* fname_MeasNoiseXYZ )  //The White Gaussian Noise is generated by MATLAB
{
	ifstream istr_MeasNoiseXYZ;
	istr_MeasNoiseXYZ.open( fname_MeasNoiseXYZ );

	if ( istr_MeasNoiseXYZ.fail() )
	{
		MessageBox(NULL,L"Reading Measurement Noise Fail !",L"Thermal Motion",MB_OK);
		exit(1);	
	}
	else
	{
		istr_MeasNoiseXYZ >> LEN_MeasNoise;
		MeasNoise_X_mOm_RD = new double [LEN_MeasNoise];
		MeasNoise_Y_mOm_RD = new double [LEN_MeasNoise];
		MeasNoise_Z_mOm_RD = new double [LEN_MeasNoise];

		for (int i=0; i<LEN_MeasNoise; i++)
		{
			istr_MeasNoiseXYZ >> MeasNoise_X_mOm_RD[i] >> 
				                 MeasNoise_Y_mOm_RD[i] >> 
								 MeasNoise_Z_mOm_RD[i];
		}
		istr_MeasNoiseXYZ.close();
	}
}


void CALCULATION::Positioning_Simu_Init(int LEN_Spl) //Dynamic Array Initialization
{
	LEN_Simu_Meas = LEN_Spl;
	Simu_Real_LEN = LEN_Simu_Meas + DELAY_STEP_Meas + 1;

	Simu_TX_umOm = new double [LEN_Simu_Meas];  Simu_TX_umOa = new double [LEN_Simu_Meas];
	Simu_TY_umOm = new double [LEN_Simu_Meas];  Simu_TY_umOa = new double [LEN_Simu_Meas];
	Simu_TZ_umOm = new double [LEN_Simu_Meas];  Simu_TZ_umOa = new double [LEN_Simu_Meas];

	Simu_RX_umOm = new double [Simu_Real_LEN];  Simu_RX_umOa = new double [Simu_Real_LEN];
	Simu_RY_umOm = new double [Simu_Real_LEN];  Simu_RY_umOa = new double [Simu_Real_LEN];
	Simu_RZ_umOm = new double [Simu_Real_LEN];  Simu_RZ_umOa = new double [Simu_Real_LEN]; //Real Position, larger size than Meas.

	Simu_MX_umOm = new double [LEN_Simu_Meas];  Simu_MX_umOa = new double [LEN_Simu_Meas];
	Simu_MY_umOm = new double [LEN_Simu_Meas];  Simu_MY_umOa = new double [LEN_Simu_Meas];
	Simu_MZ_umOm = new double [LEN_Simu_Meas];  Simu_MZ_umOa = new double [LEN_Simu_Meas]; //Mea. Position

	Simu_I1 = new double [LEN_Simu_Meas];
	Simu_I2 = new double [LEN_Simu_Meas];
	Simu_I3 = new double [LEN_Simu_Meas];
	Simu_I4 = new double [LEN_Simu_Meas];
	Simu_I5 = new double [LEN_Simu_Meas];
	Simu_I6 = new double [LEN_Simu_Meas];

	Simu_FdX_pNOm = new double [LEN_Simu_Meas];
	Simu_FdY_pNOm = new double [LEN_Simu_Meas];
	Simu_FdZ_pNOm = new double [LEN_Simu_Meas]; //Record Fdsire from controller
	//
	Simu_FdX_pNOa = new double [LEN_Simu_Meas];
	Simu_FdY_pNOa = new double [LEN_Simu_Meas];
	Simu_FdZ_pNOa = new double [LEN_Simu_Meas]; //Record Fdsire from controller
	//
	Simu_FdX_norOm = new double [LEN_Simu_Meas];
	Simu_FdY_norOm = new double [LEN_Simu_Meas];
	Simu_FdZ_norOm = new double [LEN_Simu_Meas]; //For Recording
	//
	Simu_FdX_norOa = new double [LEN_Simu_Meas];
	Simu_FdY_norOa = new double [LEN_Simu_Meas];
	Simu_FdZ_norOa = new double [LEN_Simu_Meas]; //For Recording
	//
	Simu_FXcalc_pNOa = new double [LEN_Simu_Meas];
	Simu_FYcalc_pNOa = new double [LEN_Simu_Meas];
	Simu_FZcalc_pNOa = new double [LEN_Simu_Meas]; //Check the performance of the InvMdl


	for (int i=0; i<LEN_Spl; i++)
	{
		Simu_TX_umOm[i] = Trap_Target_umOm[X];  //Specify Target
		Simu_TY_umOm[i] = Trap_Target_umOm[Y];  //Specify Target
		Simu_TZ_umOm[i] = Trap_Target_umOm[Z];  //Specify Target
		CoordTrans_Elem_m2a( Simu_TX_umOm[i], Simu_TY_umOm[i], Simu_TZ_umOm[i],  
			                &Simu_TX_umOa[i],&Simu_TY_umOa[i],&Simu_TZ_umOa[i] ); //All the Target Position is Initialized in Positioning

		if ( i<DELAY_STEP_Meas + 1 ) // (DELAY_STEP_Meas + 1) is already loaded 
		{
			if (i==0)
			{
				Simu_RX_umOm[i] = Simu_TX_umOm[i] + ThmMotion_X_RD[i]*m2um;
				Simu_RY_umOm[i] = Simu_TY_umOm[i] + ThmMotion_Y_RD[i]*m2um;
				Simu_RZ_umOm[i] = Simu_TZ_umOm[i] + ThmMotion_Z_RD[i]*m2um; //Real Position
				CoordTrans_Elem_m2a( Simu_RX_umOm[i], Simu_RY_umOm[i], Simu_RZ_umOm[i],  
					                &Simu_RX_umOa[i],&Simu_RY_umOa[i],&Simu_RZ_umOa[i] );
			}
			else
			{
				Simu_RX_umOm[i] = Simu_RX_umOm[i-1] + ThmMotion_X_RD[i]*m2um;
				Simu_RY_umOm[i] = Simu_RY_umOm[i-1] + ThmMotion_Y_RD[i]*m2um;
				Simu_RZ_umOm[i] = Simu_RZ_umOm[i-1] + ThmMotion_Z_RD[i]*m2um; //Real Position
				CoordTrans_Elem_m2a( Simu_RX_umOm[i], Simu_RY_umOm[i], Simu_RZ_umOm[i],  
					                &Simu_RX_umOa[i],&Simu_RY_umOa[i],&Simu_RZ_umOa[i]);
			}
		}
	}
}


void CALCULATION::Estimation_Init(int LIQUID_TYPE)
{
	lambda = 0.99;      //forgetting factor
	kB = 1.3806503e-23; //Bolzmann
	T = 294;

	F_Ext_x_pN_Apply = new double [LEN_Simu_Meas];
	F_Ext_y_pN_Apply = new double [LEN_Simu_Meas];
	F_Ext_z_pN_Apply = new double [LEN_Simu_Meas]; //applied force in pN

	OBSRV_x = new double [LEN_Simu_Meas];
	INNOV_x = new double [LEN_Simu_Meas];
	Damp_x_ESTI      = new double [LEN_Simu_Meas];
	pN_O_Damp_x_ESTI = new double[LEN_Simu_Meas];
	M_Ext_x_ESTI     = new double [LEN_Simu_Meas];
	F_Ext_x_ESTI     = new double [LEN_Simu_Meas];
	Vari_Tot_x_ESTI  = new double [LEN_Simu_Meas];
	Vari_Meas_x_ESTI = new double [LEN_Simu_Meas];
	Fx_pNOa_Curr = new double [LEN_Simu_Meas];
	//
	OBSRV_y = new double [LEN_Simu_Meas];
	INNOV_y = new double [LEN_Simu_Meas];
	Damp_y_ESTI      = new double [LEN_Simu_Meas];
	pN_O_Damp_y_ESTI = new double[LEN_Simu_Meas];
	M_Ext_y_ESTI     = new double [LEN_Simu_Meas];
	F_Ext_y_ESTI     = new double [LEN_Simu_Meas];
	Vari_Tot_y_ESTI  = new double [LEN_Simu_Meas];
	Vari_Meas_y_ESTI = new double [LEN_Simu_Meas];
	Fy_pNOa_Curr = new double [LEN_Simu_Meas];
	//
	OBSRV_z = new double [LEN_Simu_Meas];
	INNOV_z = new double [LEN_Simu_Meas];
	Damp_z_ESTI      = new double [LEN_Simu_Meas];
	pN_O_Damp_z_ESTI = new double[LEN_Simu_Meas];
	M_Ext_z_ESTI     = new double [LEN_Simu_Meas];
	F_Ext_z_ESTI     = new double [LEN_Simu_Meas];
	Vari_Tot_z_ESTI  = new double [LEN_Simu_Meas];
	Vari_Meas_z_ESTI = new double [LEN_Simu_Meas];
	Fz_pNOa_Curr = new double [LEN_Simu_Meas];



	////if ( LIQUID_TYPE==WTR )
	////{
	////	Damp_x_ESTI[0] = 3.0e-8;
	////	Damp_y_ESTI[0] = 3.0e-8;
	////	Damp_z_ESTI[0] = 3.0e-8;
	////	//
	////	pN_O_Damp_x_ESTI[0] = N2pN/Damp_x_ESTI[0];
	////	pN_O_Damp_y_ESTI[0] = N2pN/Damp_y_ESTI[0];
	////	pN_O_Damp_z_ESTI[0] = N2pN/Damp_z_ESTI[0];
	////}
	////else if ( LIQUID_TYPE==GLY )
	////{
	////	Damp_x_ESTI[0] = 3.0e-6;
	////	Damp_y_ESTI[0] = 3.0e-6;
	////	Damp_z_ESTI[0] = 3.0e-6;
	////	//
	////	pN_O_Damp_x_ESTI[0] = N2pN/Damp_x_ESTI[0];
	////	pN_O_Damp_y_ESTI[0] = N2pN/Damp_y_ESTI[0];
	////	pN_O_Damp_z_ESTI[0] = N2pN/Damp_z_ESTI[0];
	////}



	beta = 0.95;
	PHI[0][0] = 1.0+beta;  PHI[0][1] = -beta; PHI[0][2] = 0.0; 
	PHI[1][0] = 1.0;       PHI[1][1] = 0.0;   PHI[1][2] = 0.0;
	PHI[2][0] = 0.0;       PHI[2][1] = 0.0;   PHI[2][2] = 1.0;
	//
	for (int i=0; i<3; i++)
		for (int j=0; j<3; j++)
		{
			PHI_T[i][j] = PHI[j][i]; //Transpose
		}


	//****** Initialize Dynamic matrix in x direction: ******
	if ( LIQUID_TYPE==WTR )
	{
		Damp_x_ESTI[0] = 3.0e-8;
	}
	else if ( LIQUID_TYPE==GLY )
	{
		Damp_x_ESTI[0] = 3e-6;
	}
	//
	pN_O_Damp_x_ESTI[0] = N2pN/Damp_x_ESTI[0]; //pN/Damp
	Vari_Tot_x_ESTI[0]  = 2*kB*T*SplIntv/Damp_x_ESTI[0] + pow(1.5e-9,2.0); //var(Thermal + Meas.)
	//
	STATE_km1_km1_x[0] = 0;
	STATE_km1_km1_x[1] = 0;
	STATE_km1_km1_x[2] = pN_O_Damp_x_ESTI[0];
	//
	M_Ext_x_ESTI[0] = STATE_km1_km1_x[0];
	F_Ext_x_ESTI[0] = Damp_x_ESTI[0]*M_Ext_x_ESTI[0]/SplIntv;
	//
	if ( LIQUID_TYPE==WTR )
	{
		Qw1_x = 1e-8*Vari_Tot_x_ESTI[0]; //(thermal variance + meas. noise)
		Qw2_x = 1e-14;                   //(N2pN/Damp_x_ESTI)
	}
	else if ( LIQUID_TYPE==GLY )
	{
		Qw1_x = 1e-8*Vari_Tot_x_ESTI[0]; //(thermal variance + meas. noise)
		Qw2_x = 1e-17;                   //(N2pN/Damp_x_ESTI)
	}
	Qw_x[0][0] = Qw1_x; Qw_x[0][1] = 0;  Qw_x[0][2] = 0;
	Qw_x[1][0] = 0;     Qw_x[1][1] = 0;  Qw_x[1][2] = 0;
	Qw_x[2][0] = 0;     Qw_x[2][1] = 0;  Qw_x[2][2] = Qw2_x;
	//
	OBSRV_x[0] = 0.0;
	Hx_k[0] = 1.0;  Hx_k[1] = 0.0;  Hx_k[2] = 0.0;  // Hx_k[2] will be updated in real-time by observation
	//
	for (int i=0; i<3; i++)
		for (int j=0; j<3; j++)
		{
		   P_km1_km1_x[i][j] = Qw_x[i][j];

		   P_k_km1_x[i][j] = 0.0;
		   P_k_k_x[i][j] = 0.0;
		}
	//
	Kx_k[0] = 0.0;  Kx_k[1] = 0.0;  Kx_k[2] = 0.0;
	STATE_k_k_x[0] = 0.0;    STATE_k_k_x[1] = 0.0;    STATE_k_k_x[2] = 0.0;
	STATE_k_km1_x[0] = 0.0;  STATE_k_km1_x[1] = 0.0;  STATE_k_km1_x[2] = 0.0;
	INNOV_x[0] = 0.0;
	//****************************************************



	//****** Initialize Dynamic matrix in y direction: ******
	if ( LIQUID_TYPE==WTR )
	{
		Damp_y_ESTI[0] = 3.0e-8;
	}
	else if ( LIQUID_TYPE==GLY )
	{
		Damp_y_ESTI[0] = 3e-6;
	}
	//
	pN_O_Damp_y_ESTI[0] = N2pN/Damp_y_ESTI[0]; //N2pN/Damp
	Vari_Tot_y_ESTI[0]  = 2*kB*T*SplIntv/Damp_y_ESTI[0] + pow(1.5e-9,2.0); //var(Thermal + Meas.)
	//
	STATE_km1_km1_y[0] = 0;
	STATE_km1_km1_y[1] = 0;
	STATE_km1_km1_y[2] = pN_O_Damp_y_ESTI[0];
	//
	M_Ext_y_ESTI[0] = STATE_km1_km1_y[0];
	F_Ext_y_ESTI[0] = Damp_y_ESTI[0]*M_Ext_y_ESTI[0]/SplIntv;
	//
	if ( LIQUID_TYPE==WTR )
	{
		Qw1_y = 1e-8*Vari_Tot_y_ESTI[0]; //(thermal variance + meas. noise)
		Qw2_y = 1e-14;                   //(N2pN/Damp_y_ESTI)
	}
	else if ( LIQUID_TYPE==GLY )
	{
		Qw1_y = 1e-8*Vari_Tot_y_ESTI[0]; //(thermal variance + meas. noise)
		Qw2_y = 1e-17;                   //(N2pN/Damp_y_ESTI)
	}
	Qw_y[0][0] = Qw1_y; Qw_y[0][1] = 0;  Qw_y[0][2] = 0;
	Qw_y[1][0] = 0;     Qw_y[1][1] = 0;  Qw_y[1][2] = 0;
	Qw_y[2][0] = 0;     Qw_y[2][1] = 0;  Qw_y[2][2] = Qw2_y;
	//
	OBSRV_y[0] = 0.0;
	Hy_k[0] = 1.0;  Hy_k[1] = 0.0;  Hy_k[2] = 0.0; // Hy_k[2] will be updated by observation
	//
	for (int i=0; i<3; i++)
		for (int j=0; j<3; j++)
		{
		   P_km1_km1_y[i][j] = Qw_y[i][j];

		   P_k_km1_y[i][j] = 0.0;
		   P_k_k_y[i][j] = 0.0;
		}
	//
	Ky_k[0] = 0.0;  Ky_k[1] = 0.0;  Ky_k[2] = 0.0;
	STATE_k_k_y[0] = 0.0;    STATE_k_k_y[1] = 0.0;    STATE_k_k_y[2] = 0.0;
	STATE_k_km1_y[0] = 0.0;  STATE_k_km1_y[1] = 0.0;  STATE_k_km1_y[2] = 0.0;
	INNOV_y[0] = 0.0;
	//****************************************************



	//****** Initialize Dynamic matrix in z direction: ******
	if ( LIQUID_TYPE==WTR )
	{
		Damp_z_ESTI[0] = 3.0e-8;
	}
	else if ( LIQUID_TYPE==GLY )
	{
		Damp_z_ESTI[0] = 5e-6;
	}
	//
	pN_O_Damp_z_ESTI[0] = N2pN/Damp_z_ESTI[0]; //N2pN/Damp
	Vari_Tot_z_ESTI[0]  = 2*kB*T*SplIntv/Damp_z_ESTI[0] + pow(1.5e-9,2.0); //var(Thermal + Meas.)
	//
	STATE_km1_km1_z[0] = 0;
	STATE_km1_km1_z[1] = 0;
	STATE_km1_km1_z[2] = pN_O_Damp_z_ESTI[0];
	//
	M_Ext_z_ESTI[0] = STATE_km1_km1_z[0];
	F_Ext_z_ESTI[0] = Damp_z_ESTI[0]*M_Ext_z_ESTI[0]/SplIntv;
	//
	if ( LIQUID_TYPE==WTR )
	{
		Qw1_z = 1e-8*Vari_Tot_z_ESTI[0]; //(thermal variance + meas. noise)
		Qw2_z = 1e-14;                   //(N2pN/Damp_z_ESTI)
	}
	else if ( LIQUID_TYPE==GLY )
	{
		Qw1_z = 1e-8*Vari_Tot_z_ESTI[0]; //(thermal variance + meas. noise)
		Qw2_z = 1e-17;                   //(N2pN/Damp_z_ESTI)
	}
	Qw_z[0][0] = Qw1_z; Qw_z[0][1] = 0;  Qw_z[0][2] = 0;
	Qw_z[1][0] = 0;     Qw_z[1][1] = 0;  Qw_z[1][2] = 0;
	Qw_z[2][0] = 0;     Qw_z[2][1] = 0;  Qw_z[2][2] = Qw2_z;
	//
	OBSRV_z[0] = 0.0;
	Hz_k[0] = 1.0;  Hz_k[1] = 0.0;  Hz_k[2] = 0.0; // Hz_k[2] will be updated by observation
	//
	for (int i=0; i<3; i++)
		for (int j=0; j<3; j++)
		{
		   P_km1_km1_z[i][j] = Qw_z[i][j];

		   P_k_km1_z[i][j] = 0.0;
		   P_k_k_z[i][j] = 0.0;
		}
	//
	Kz_k[0] = 0.0;  Kz_k[1] = 0.0;  Kz_k[2] = 0.0;
	STATE_k_k_z[0] = 0.0;    STATE_k_k_z[1] = 0.0;    STATE_k_k_z[2] = 0.0;
	STATE_k_km1_z[0] = 0.0;  STATE_k_km1_z[1] = 0.0;  STATE_k_km1_z[2] = 0.0;
	INNOV_z[0] = 0.0;
	//****************************************************


	EYE[0][0] = 1.0; EYE[0][1] = 0.0; EYE[0][2] = 0.0;
	EYE[1][0] = 0.0; EYE[1][1] = 1.0; EYE[1][2] = 0.0;
	EYE[2][0] = 0.0; EYE[2][1] = 0.0; EYE[2][2] = 1.0;

	Vari_Meas_x_ESTI[0] = pow( (1.5e-9),2.0);
	Vari_Meas_y_ESTI[0] = pow( (1.5e-9),2.0);
	Vari_Meas_z_ESTI[0] = pow( (1.5e-9),2.0);
}


void CALCULATION::Get_STATE_k_km1() //Time Update
{
	CalcMat_3x3_3x1( PHI, STATE_km1_km1_x, PHI_M_STATE_km1_km1_x );
	CalcMat_3x3_3x1( PHI, STATE_km1_km1_y, PHI_M_STATE_km1_km1_y );
	CalcMat_3x3_3x1( PHI, STATE_km1_km1_z, PHI_M_STATE_km1_km1_z ); //Time Update

	for (int i=0; i<3; i++)
	{
		STATE_k_km1_x[i] = PHI_M_STATE_km1_km1_x[i];
		STATE_k_km1_y[i] = PHI_M_STATE_km1_km1_y[i];
		STATE_k_km1_z[i] = PHI_M_STATE_km1_km1_z[i];
	}
}

void CALCULATION::Get_EstiCov_P_k_km1() //time update
{
	CalcMat_3x3_3x3_3x3( PHI, P_km1_km1_x, PHI_T, PHI_M_P_km1_km1_M_PHIT_x );  //PHI_M_P_km1_km1_M_PHIT_x obtained
	CalcMat_3x3_3x3_3x3( PHI, P_km1_km1_y, PHI_T, PHI_M_P_km1_km1_M_PHIT_y );
	CalcMat_3x3_3x3_3x3( PHI, P_km1_km1_z, PHI_T, PHI_M_P_km1_km1_M_PHIT_z );

	CalcMatSum_3x3( PHI_M_P_km1_km1_M_PHIT_x, Qw_x, P_k_km1_x );
	CalcMatSum_3x3( PHI_M_P_km1_km1_M_PHIT_y, Qw_y, P_k_km1_y );
	CalcMatSum_3x3( PHI_M_P_km1_km1_M_PHIT_z, Qw_z, P_k_km1_z ); //TIME UPDATE of P
}


void CALCULATION::Get_InnovCov()  //meas. update
{
	Sx_k = CalcMat_1x3_3x3_3x1( Hx_k, P_k_km1_x, Hx_k ) + Vari_Tot_x_ESTI[Index-1];
	Sy_k = CalcMat_1x3_3x3_3x1( Hy_k, P_k_km1_y, Hy_k ) + Vari_Tot_y_ESTI[Index-1];
	Sz_k = CalcMat_1x3_3x3_3x1( Hz_k, P_k_km1_z, Hz_k ) + Vari_Tot_z_ESTI[Index-1]; //constant measurement covariance
}


void CALCULATION::Get_KalmanGain_K() //meas. update
{
	CalcMat_3x3_3x1( P_k_km1_x, Hx_k, Kx_k );
	CalcMat_3x3_3x1( P_k_km1_y, Hy_k, Ky_k );
	CalcMat_3x3_3x1( P_k_km1_z, Hz_k, Kz_k ); //here, Kmain is not the final Kalman gain

	CalcVectDev_3x1( Kx_k, Sx_k );
	CalcVectDev_3x1( Ky_k, Sy_k );
	CalcVectDev_3x1( Kz_k, Sz_k ); //here, Kmain is the final Kalman gain
}


void CALCULATION::Get_STATE_k()  //MEAS. UPDATE
{
	for (int i=0; i<3; i++)
	{
		STATE_k_k_x[i] = STATE_k_km1_x[i] + Kx_k[i] * INNOV_x[Index];
		STATE_k_k_y[i] = STATE_k_km1_y[i] + Ky_k[i] * INNOV_y[Index];
		STATE_k_k_z[i] = STATE_k_km1_z[i] + Kz_k[i] * INNOV_z[Index];
	}
}


void CALCULATION::Get_EstiCov_P_k_k()//meas. update
{
	double EYE_m_KkHk_x[3][3],
		   EYE_m_KkHk_y[3][3],
		   EYE_m_KkHk_z[3][3];

	for (int i=0; i<3; i++)
		for (int j=0; j<3; j++)
		{
			EYE_m_KkHk_x[i][j] = EYE[i][j] - Kx_k[i] * Hx_k[j];
			EYE_m_KkHk_y[i][j] = EYE[i][j] - Ky_k[i] * Hy_k[j];
			EYE_m_KkHk_z[i][j] = EYE[i][j] - Kz_k[i] * Hz_k[j];
		}

    CalcMat_3x3_3x3( EYE_m_KkHk_x, P_k_km1_x, P_k_k_x );
    CalcMat_3x3_3x3( EYE_m_KkHk_y, P_k_km1_y, P_k_k_y );
    CalcMat_3x3_3x3( EYE_m_KkHk_z, P_k_km1_z, P_k_k_z );
}


void CALCULATION::Esti_Update_Parameter()
{
    Index++; //increase index

	OBSRV_x[Index] = (Simu_MX_umOa[Index] - Simu_MX_umOa[Index - 1])*um2m; //(The positions are from Simulaiton!)
	OBSRV_y[Index] = (Simu_MY_umOa[Index] - Simu_MY_umOa[Index - 1])*um2m;
	OBSRV_z[Index] = (Simu_MZ_umOa[Index] - Simu_MZ_umOa[Index - 1])*um2m; //observations
	//
	if( Index >= DELAY_STEP_Esti + 1)
	{
		P_XYZ_umOa_forEsti[X] = Simu_MX_umOa[ Index - 1 - DELAY_STEP_Esti ];
		P_XYZ_umOa_forEsti[Y] = Simu_MY_umOa[ Index - 1 - DELAY_STEP_Esti ];
		P_XYZ_umOa_forEsti[Z] = Simu_MZ_umOa[ Index - 1 - DELAY_STEP_Esti ];
		In_P1toP6_forEsti[0] = Simu_I1[ Index - 1 - DELAY_STEP_Esti ];
		In_P1toP6_forEsti[1] = Simu_I2[ Index - 1 - DELAY_STEP_Esti ];
		In_P1toP6_forEsti[2] = Simu_I3[ Index - 1 - DELAY_STEP_Esti ];
		In_P1toP6_forEsti[3] = Simu_I4[ Index - 1 - DELAY_STEP_Esti ];
		In_P1toP6_forEsti[4] = Simu_I5[ Index - 1 - DELAY_STEP_Esti ];
		In_P1toP6_forEsti[5] = Simu_I6[ Index - 1 - DELAY_STEP_Esti ];

		//Calc_Lxyz( P_XYZ_umOa_forEsti );
		Calc_FNor_FromSixCurr_KItheo( P_XYZ_umOa_forEsti, In_P1toP6_forEsti ); //fx_nor_Oa, fy_nor_Oa, fz_nor_Oa
		Fx_pNOa_Curr[Index] = FGain_x * FNor_Curr_KItheo[X];
		Fy_pNOa_Curr[Index] = FGain_y * FNor_Curr_KItheo[Y];
		Fz_pNOa_Curr[Index] = FGain_z * FNor_Curr_KItheo[Z];
	}
	else
	{
		Fx_pNOa_Curr[Index] = 0.0;
		Fy_pNOa_Curr[Index] = 0.0;
		Fz_pNOa_Curr[Index] = 0.0;
	}
	Hx_k[2] = Fx_pNOa_Curr[Index] * SplIntv; //Hd[k] matrix
	Hy_k[2] = Fy_pNOa_Curr[Index] * SplIntv; //Hd[k] matrix
	Hz_k[2] = Fz_pNOa_Curr[Index] * SplIntv; //Hd[k] matrix


	//#########################################
	//****** Time Update & Meas. Update: ******
	//#########################################
	Get_STATE_k_km1();     //STATE (k-1|k-1)  ->  STATE (k|k-1) ;
	Get_EstiCov_P_k_km1(); //P (k-1|k-1)  ->  P (k|k-1) ; use CovQ[Index-1]

	INNOV_x[Index] = OBSRV_x[Index] - CalcMat_1x3_3x1( Hx_k, STATE_k_km1_x ) ;
	INNOV_y[Index] = OBSRV_y[Index] - CalcMat_1x3_3x1( Hy_k, STATE_k_km1_y ) ;
	INNOV_z[Index] = OBSRV_z[Index] - CalcMat_1x3_3x1( Hz_k, STATE_k_km1_z ) ; //innovations

	Get_InnovCov();      //Innovation Covariance, CovR_x[Index-1],
	Get_KalmanGain_K();  //get Kalman filter gain
	Get_STATE_k();       //meas. update
	Get_EstiCov_P_k_k(); //Estimation Covariance
	//##############################################
	//****** (end) Time Update & Meas. Update ******
	//##############################################	
	// x direction:
	pN_O_Damp_x_ESTI [Index] = STATE_k_k_x[2];
	Damp_x_ESTI[Index]       = N2pN/pN_O_Damp_x_ESTI[Index];
	M_Ext_x_ESTI[Index] = STATE_k_k_x[0];
	F_Ext_x_ESTI[Index] = Damp_x_ESTI[Index]*M_Ext_x_ESTI[Index]/SplIntv;

	// y direction:
	pN_O_Damp_y_ESTI[Index] = STATE_k_k_y[2];
	Damp_y_ESTI[Index]      = N2pN/pN_O_Damp_y_ESTI[Index];
	M_Ext_y_ESTI[Index] = STATE_k_k_y[0];
	F_Ext_y_ESTI[Index] = Damp_y_ESTI[Index]*M_Ext_y_ESTI[Index]/SplIntv;

	// z direction:
	pN_O_Damp_z_ESTI [Index] = STATE_k_k_z[2];
	Damp_z_ESTI[Index]       = N2pN/pN_O_Damp_z_ESTI[Index];
	M_Ext_z_ESTI[Index] = STATE_k_k_z[0];
	F_Ext_z_ESTI[Index] = Damp_z_ESTI[Index]*M_Ext_z_ESTI[Index]/SplIntv;


	Vari_Tot_x_ESTI[Index] = lambda * Vari_Tot_x_ESTI[Index-1] + (1-lambda)*pow( INNOV_x[Index], 2.0 );
	Vari_Tot_y_ESTI[Index] = lambda * Vari_Tot_y_ESTI[Index-1] + (1-lambda)*pow( INNOV_y[Index], 2.0 );
	Vari_Tot_z_ESTI[Index] = lambda * Vari_Tot_z_ESTI[Index-1] + (1-lambda)*pow( INNOV_z[Index], 2.0 );
	//
    Qw1_x = 1e-8*Vari_Tot_x_ESTI[Index]; //(thermal variance + meas. noise)
	Qw_x[0][0] = Qw1_x;
	//
    Qw1_y = 1e-8*Vari_Tot_y_ESTI[Index]; //(thermal variance + meas. noise)
	Qw_y[0][0] = Qw1_y;
	//
    Qw1_z = 1e-8*Vari_Tot_z_ESTI[Index]; //(thermal variance + meas. noise)
	Qw_z[0][0] = Qw1_z;


	Vari_thm_x = 2*kB*T*SplIntv/Damp_x_ESTI[Index];
	Vari_thm_y = 2*kB*T*SplIntv/Damp_y_ESTI[Index];
	Vari_thm_z = 2*kB*T*SplIntv/Damp_z_ESTI[Index];
	Vari_Meas_x_ESTI[Index] = Vari_Tot_x_ESTI[Index] - Vari_thm_x;
	Vari_Meas_y_ESTI[Index] = Vari_Tot_y_ESTI[Index] - Vari_thm_y;
	Vari_Meas_z_ESTI[Index] = Vari_Tot_z_ESTI[Index] - Vari_thm_z;


	//#############################
	//****** History Update: ******
	//#############################
	for (int i=0; i<3; i++)
	{
		STATE_km1_km1_x[i] = STATE_k_k_x[i];
		STATE_km1_km1_y[i] = STATE_k_k_y[i];
		STATE_km1_km1_z[i] = STATE_k_k_z[i];
		for (int j=0; j<3; j++)
		{
			P_km1_km1_x[i][j] = P_k_k_x[i][j];
			P_km1_km1_y[i][j] = P_k_k_y[i][j];
			P_km1_km1_z[i][j] = P_k_k_z[i][j];
		}
	}
	//##################################
	//****** (end) History Update ******
	//##################################
}



void CALCULATION::Positioning_Simu( int THEO_or_REAL )
{
	double PosAft_umOa [3],
		   //
		   PosMeas_umOm[3],
		   PosMeas_umOa[3], //measurement position
		   //
		   PosReal_umOm[3],
		   PosReal_umOa[3]; //real position

	for ( Index=0; Index<LEN_Simu_Meas; Index++ )
	{
		if ( PosCtrl_Counter%1000 == 0)
		{
			cout<<((double)(PosCtrl_Counter))/LEN_Simu_Meas*100<<" %"<<endl;
		}

		Simu_MX_umOm[Index] = Simu_RX_umOm[Index] + MeasNoise_X_mOm_RD[Index]*m2um;
		Simu_MY_umOm[Index] = Simu_RY_umOm[Index] + MeasNoise_Y_mOm_RD[Index]*m2um;
		Simu_MZ_umOm[Index] = Simu_RZ_umOm[Index] + MeasNoise_Z_mOm_RD[Index]*m2um;  //Measurement in Om (With Delay & meas. noise)

		CoordTrans_Elem_m2a( Simu_MX_umOm[Index], Simu_MY_umOm[Index], Simu_MZ_umOm[Index],  
			                &Simu_MX_umOa[Index],&Simu_MY_umOa[Index],&Simu_MZ_umOa[Index] ); //Measurement in Om -> Oa

		ERRpos_umOm[X] = Simu_TX_umOm[Index] - Simu_MX_umOm[Index];
		ERRpos_umOm[Y] = Simu_TY_umOm[Index] - Simu_MY_umOm[Index];
		ERRpos_umOm[Z] = Simu_TZ_umOm[Index] - Simu_MZ_umOm[Index]; //Error in Om

		PosMeas_umOm[X] = Simu_MX_umOm[Index];
		PosMeas_umOm[Y] = Simu_MY_umOm[Index];
		PosMeas_umOm[Z] = Simu_MZ_umOm[Index];
		CoordTrans_Vect_m2a ( PosMeas_umOm, PosMeas_umOa ); //PosMeas in Om -> Oa

		Calc_Ctrl_Effort_Force ( PID_ctrl ); //(will use ERRpos_umOm[3], return Fd_pNOm, PosCtrl_Counter ++)
		CoordTrans_Vect_m2a ( Fd_pNOm, Fd_pNOa );

		Simu_FdX_pNOm[Index] = Fd_pNOm[X];
		Simu_FdY_pNOm[Index] = Fd_pNOm[Y];
		Simu_FdZ_pNOm[Index] = Fd_pNOm[Z]; //data  Recording, Fd in Om
		//
		Simu_FdX_pNOa[Index] = Fd_pNOa[X];
		Simu_FdY_pNOa[Index] = Fd_pNOa[Y];
		Simu_FdZ_pNOa[Index] = Fd_pNOa[Z]; //data  Recording, Fd in Oa

		Simu_FdX_norOa[Index] = Simu_FdX_pNOa[Index] / FGain_x; //****//
		Simu_FdY_norOa[Index] = Simu_FdY_pNOa[Index] / FGain_y; //****//
		Simu_FdZ_norOa[Index] = Simu_FdZ_pNOa[Index] / FGain_z; //****//Fnor is better to be in Oa
		CoordTrans_Elem_a2m( Simu_FdX_norOa[Index], Simu_FdY_norOa[Index], Simu_FdZ_norOa[Index],  
			                &Simu_FdX_norOm[Index],&Simu_FdY_norOm[Index],&Simu_FdZ_norOm[Index]); //Measurement in Oa -> Om

		if ( THEO_or_REAL == THEO )
		{
			OptInvMdl_Curr_FromLS_KItheo( Fd_pNOa, PosMeas_umOa); //Obtained: Vhall_Dsir_P1toP6
			Calc_FNor_FromSixCurr_KItheo( PosMeas_umOa, I1toI6_KItheo ); //check if the force is ok
			Simu_FXcalc_pNOa[Index] = FNor_Curr_KItheo[X] * FGain_x; //force check
			Simu_FYcalc_pNOa[Index] = FNor_Curr_KItheo[Y] * FGain_y; //force check
			Simu_FZcalc_pNOa[Index] = FNor_Curr_KItheo[Z] * FGain_z; //force check
		}
		else
		{
			OptInvMdl_Curr_FromLS_KIreal( Fd_pNOa, PosMeas_umOa ); //Obtained: Vhall_Dsir_P1toP6[6]
			Calc_FNor_FromSixCurr_KIreal( PosMeas_umOa, I1toI6_KIreal ); //check if the force is ok
			Simu_FXcalc_pNOa[Index] = FNor_Curr_KIreal[X] * FGain_x; //force check
			Simu_FYcalc_pNOa[Index] = FNor_Curr_KIreal[Y] * FGain_y; //force check
			Simu_FZcalc_pNOa[Index] = FNor_Curr_KIreal[Z] * FGain_z; //force check
		}


		PosReal_umOm[X] = Simu_RX_umOm[ Index + DELAY_STEP_Meas ];
		PosReal_umOm[Y] = Simu_RY_umOm[ Index + DELAY_STEP_Meas ];
		PosReal_umOm[Z] = Simu_RZ_umOm[ Index + DELAY_STEP_Meas ]; //Real Position (with delay)
		CoordTrans_Vect_m2a ( PosReal_umOm, PosReal_umOa ); //PosReal in Om -> Oa

		Calc_NextBeadPos( PosReal_umOa, PosAft_umOa, THEO_or_REAL );  //Calc Next Real Pos (Note: This is in Oa, so Damping need to be in Oa)

		Simu_RX_umOa[ Index+DELAY_STEP_Meas+1 ] = PosAft_umOa[X];
		Simu_RY_umOa[ Index+DELAY_STEP_Meas+1 ] = PosAft_umOa[Y];
		Simu_RZ_umOa[ Index+DELAY_STEP_Meas+1 ] = PosAft_umOa[Z]; //With Delay
        CoordTrans_Elem_a2m( Simu_RX_umOa[ Index+DELAY_STEP_Meas+1 ],  Simu_RY_umOa[ Index+DELAY_STEP_Meas+1],  Simu_RZ_umOa[ Index+DELAY_STEP_Meas+1 ],
                            &Simu_RX_umOm[ Index+DELAY_STEP_Meas+1 ], &Simu_RY_umOm[ Index+DELAY_STEP_Meas+1], &Simu_RZ_umOm[ Index+DELAY_STEP_Meas+1 ]); //Real motion in Om -> Oa

		Simu_I1[Index] = I1toI6_KItheo[0];
		Simu_I2[Index] = I1toI6_KItheo[1];
		Simu_I3[Index] = I1toI6_KItheo[2];
		Simu_I4[Index] = I1toI6_KItheo[3];
		Simu_I5[Index] = I1toI6_KItheo[4];
		Simu_I6[Index] = I1toI6_KItheo[5];
	}
}


void CALCULATION::Positioning_Simu_WithEsti(int THEO_or_REAL, int ADAPT_or_CONST_KF)
{
	double PosAft_umOa [3],
		   //
		   PosMeas_umOm[3],
		   PosMeas_umOa[3], //measurement position
		   //
		   PosReal_umOm[3],
		   PosReal_umOa[3]; //real position

	for ( Index=0; Index<LEN_Simu_Meas; Index++)
	{
		if ( PosCtrl_Counter%1000 == 0)
		{
			cout<<((double)(PosCtrl_Counter))/LEN_Simu_Meas*100<<" %"<<endl;
		}

		Simu_MX_umOm[Index] = Simu_RX_umOm[Index] + MeasNoise_X_mOm_RD[Index]*m2um;
		Simu_MY_umOm[Index] = Simu_RY_umOm[Index] + MeasNoise_Y_mOm_RD[Index]*m2um;
		Simu_MZ_umOm[Index] = Simu_RZ_umOm[Index] + MeasNoise_Z_mOm_RD[Index]*m2um;  //Measurement in Om (With Delay & meas. noise)
		CoordTrans_Elem_m2a( Simu_MX_umOm[Index], Simu_MY_umOm[Index], Simu_MZ_umOm[Index],  
			                &Simu_MX_umOa[Index],&Simu_MY_umOa[Index],&Simu_MZ_umOa[Index] ); //Measurement in Om -> Oa

		ERRpos_umOm[X] = Simu_TX_umOm[Index] - Simu_MX_umOm[Index];
		ERRpos_umOm[Y] = Simu_TY_umOm[Index] - Simu_MY_umOm[Index];
		ERRpos_umOm[Z] = Simu_TZ_umOm[Index] - Simu_MZ_umOm[Index]; //Error in Om

		PosMeas_umOm[X] = Simu_MX_umOm[Index];
		PosMeas_umOm[Y] = Simu_MY_umOm[Index];
		PosMeas_umOm[Z] = Simu_MZ_umOm[Index];
		CoordTrans_Vect_m2a ( PosMeas_umOm, PosMeas_umOa ); //PosMeas in Om -> Oa

		Calc_Ctrl_Effort_Force ( PID_ctrl ); //(will use ERRpos_umOm[3], return Fd_pNOm, PosCtrl_Counter ++)
		CoordTrans_Vect_m2a ( Fd_pNOm, Fd_pNOa );

		Simu_FdX_pNOm[Index] = Fd_pNOm[X];
		Simu_FdY_pNOm[Index] = Fd_pNOm[Y];
		Simu_FdZ_pNOm[Index] = Fd_pNOm[Z]; //data  Recording, Fd in Om
		//
		Simu_FdX_pNOa[Index] = Fd_pNOa[X];
		Simu_FdY_pNOa[Index] = Fd_pNOa[Y];
		Simu_FdZ_pNOa[Index] = Fd_pNOa[Z]; //data  Recording, Fd in Oa

		Simu_FdX_norOa[Index] = Simu_FdX_pNOa[Index] / FGain_x; //****//
		Simu_FdY_norOa[Index] = Simu_FdY_pNOa[Index] / FGain_y; //****//
		Simu_FdZ_norOa[Index] = Simu_FdZ_pNOa[Index] / FGain_z; //****//Fnor is better to be in Oa
		CoordTrans_Elem_a2m( Simu_FdX_norOa[Index], Simu_FdY_norOa[Index], Simu_FdZ_norOa[Index],  
			                &Simu_FdX_norOm[Index],&Simu_FdY_norOm[Index],&Simu_FdZ_norOm[Index]); //Measurement in Oa -> Om

		if ( THEO_or_REAL == THEO )
		{
			OptInvMdl_Curr_FromLS_KItheo( Fd_pNOa, PosMeas_umOa); //Obtained: Vhall_Dsir_P1toP6
			Calc_FNor_FromSixCurr_KItheo( PosMeas_umOa, I1toI6_KItheo); //check if the force is ok
			Simu_FXcalc_pNOa[Index] = FNor_Curr_KItheo[X] * FGain_x; //force check
			Simu_FYcalc_pNOa[Index] = FNor_Curr_KItheo[Y] * FGain_y; //force check
			Simu_FZcalc_pNOa[Index] = FNor_Curr_KItheo[Z] * FGain_z; //force check
		}
		else
		{
			OptInvMdl_Curr_FromLS_KIreal( Fd_pNOa, PosMeas_umOa ); //Obtained: Vhall_Dsir_P1toP6[6]
			Calc_FNor_FromSixCurr_KIreal( PosMeas_umOa, I1toI6_KIreal ); //check if the force is ok
			Simu_FXcalc_pNOa[Index] = FNor_Curr_KIreal[X] * FGain_x; //force check
			Simu_FYcalc_pNOa[Index] = FNor_Curr_KIreal[Y] * FGain_y; //force check
			Simu_FZcalc_pNOa[Index] = FNor_Curr_KIreal[Z] * FGain_z; //force check
		}

		PosReal_umOm[X] = Simu_RX_umOm[ Index + DELAY_STEP_Meas ];
		PosReal_umOm[Y] = Simu_RY_umOm[ Index + DELAY_STEP_Meas ];
		PosReal_umOm[Z] = Simu_RZ_umOm[ Index + DELAY_STEP_Meas ]; //Real Position (with delay)
		CoordTrans_Vect_m2a ( PosReal_umOm, PosReal_umOa );      //PosReal in Om -> Oa

		Calc_NextBeadPos( PosReal_umOa, PosAft_umOa, THEO_or_REAL );  //Calc Next Real Pos (Note: This is in Oa, so Damping need to be in Oa)

		Simu_RX_umOa[ Index + DELAY_STEP_Meas + 1 ] = PosAft_umOa[X];
		Simu_RY_umOa[ Index + DELAY_STEP_Meas + 1 ] = PosAft_umOa[Y];
		Simu_RZ_umOa[ Index + DELAY_STEP_Meas + 1 ] = PosAft_umOa[Z]; //With Delay
        CoordTrans_Elem_a2m( Simu_RX_umOa[ Index+DELAY_STEP_Meas+1 ],  Simu_RY_umOa[ Index+DELAY_STEP_Meas+1],  Simu_RZ_umOa[ Index+DELAY_STEP_Meas+1 ],
                            &Simu_RX_umOm[ Index+DELAY_STEP_Meas+1 ], &Simu_RY_umOm[ Index+DELAY_STEP_Meas+1], &Simu_RZ_umOm[ Index+DELAY_STEP_Meas+1 ]); //Real motion in Om -> Oa

		Simu_I1[Index] = I1toI6_KItheo[0];
		Simu_I2[Index] = I1toI6_KItheo[1];
		Simu_I3[Index] = I1toI6_KItheo[2];
		Simu_I4[Index] = I1toI6_KItheo[3];
		Simu_I5[Index] = I1toI6_KItheo[4];
		Simu_I6[Index] = I1toI6_KItheo[5];


		//#########################
		//#########################
		//###### Estimation: ######
		//#########################
		//#########################
		if (Index>0)
		{
			if (Index>0)
			{
				OBSRV_x[Index] = (Simu_MX_umOa[Index] - Simu_MX_umOa[Index-1]) * um2m;
				OBSRV_y[Index] = (Simu_MY_umOa[Index] - Simu_MY_umOa[Index-1]) * um2m;
				OBSRV_z[Index] = (Simu_MZ_umOa[Index] - Simu_MZ_umOa[Index-1]) * um2m; //Oa
			}

			if ( Index>= DELAY_STEP_Esti + 1 )
			{
				P_XYZ_umOa_forEsti[X] = Simu_MX_umOa[ Index - 1 - DELAY_STEP_Esti ];
				P_XYZ_umOa_forEsti[Y] = Simu_MY_umOa[ Index - 1 - DELAY_STEP_Esti ];
				P_XYZ_umOa_forEsti[Z] = Simu_MZ_umOa[ Index - 1 - DELAY_STEP_Esti ];
				In_P1toP6_forEsti[0] = Simu_I1[ Index - 1 - DELAY_STEP_Esti ];
				In_P1toP6_forEsti[1] = Simu_I2[ Index - 1 - DELAY_STEP_Esti ];
				In_P1toP6_forEsti[2] = Simu_I3[ Index - 1 - DELAY_STEP_Esti ];
				In_P1toP6_forEsti[3] = Simu_I4[ Index - 1 - DELAY_STEP_Esti ];
				In_P1toP6_forEsti[4] = Simu_I5[ Index - 1 - DELAY_STEP_Esti ];
				In_P1toP6_forEsti[5] = Simu_I6[ Index - 1 - DELAY_STEP_Esti ];

				//Calc_Lxyz( P_XYZ_umOa_forEsti );
				Calc_FNor_FromSixCurr_KItheo( P_XYZ_umOa_forEsti, In_P1toP6_forEsti ); //fx_nor_Oa, fy_nor_Oa, fz_nor_Oa
				Fx_pNOa_Curr[Index] = FGain_x * FNor_Curr_KItheo[X] ;
				Fy_pNOa_Curr[Index] = FGain_y * FNor_Curr_KItheo[Y];
				Fz_pNOa_Curr[Index] = FGain_z * FNor_Curr_KItheo[Z];
			}
			else
			{
				Fx_pNOa_Curr[Index] = 0.0;
				Fy_pNOa_Curr[Index] = 0.0;
				Fz_pNOa_Curr[Index] = 0.0;
			}
			Hx_k[2] = Fx_pNOa_Curr[Index] * SplIntv;
			Hy_k[2] = Fy_pNOa_Curr[Index] * SplIntv;
			Hz_k[2] = Fz_pNOa_Curr[Index] * SplIntv;


			//#########################################
			//****** Time Update & Meas. Update: ******
			//#########################################
			Get_STATE_k_km1();
			Get_EstiCov_P_k_km1();

			INNOV_x[Index] = OBSRV_x[Index] - CalcMat_1x3_3x1( Hx_k, STATE_k_km1_x ) ;
			INNOV_y[Index] = OBSRV_y[Index] - CalcMat_1x3_3x1( Hy_k, STATE_k_km1_y ) ;
			INNOV_z[Index] = OBSRV_z[Index] - CalcMat_1x3_3x1( Hz_k, STATE_k_km1_z ) ; //innovations

			Get_InnovCov();      //Innovation Covariance, CovR_x[Index-1],
			Get_KalmanGain_K();  //get Kalman filter gain
			Get_STATE_k();       //meas. update
			Get_EstiCov_P_k_k(); //Estimation Covariance
			//##############################################
			//****** (end) Time Update & Meas. Update ******
			//##############################################

			// x direction:
			pN_O_Damp_x_ESTI[Index] = STATE_k_k_x[2];
			Damp_x_ESTI[Index]      = N2pN/pN_O_Damp_x_ESTI[Index];
			M_Ext_x_ESTI[Index] = STATE_k_k_x[0];
			F_Ext_x_ESTI[Index] = Damp_x_ESTI[Index]*M_Ext_x_ESTI[Index]/SplIntv;

			// y direction:
			pN_O_Damp_y_ESTI[Index] = STATE_k_k_y[2];
			Damp_y_ESTI[Index]      = N2pN/pN_O_Damp_y_ESTI[Index];
			M_Ext_y_ESTI[Index] = STATE_k_k_y[0];
			F_Ext_y_ESTI[Index] = Damp_y_ESTI[Index]*M_Ext_y_ESTI[Index]/SplIntv;

			// z direction:
			pN_O_Damp_z_ESTI[Index] = STATE_k_k_z[2];
			Damp_z_ESTI[Index]      = N2pN/pN_O_Damp_z_ESTI[Index];
			M_Ext_z_ESTI[Index] = STATE_k_k_z[0];
			F_Ext_z_ESTI[Index] = Damp_z_ESTI[Index]*M_Ext_z_ESTI[Index]/SplIntv;

			Vari_Tot_x_ESTI[Index] = lambda * Vari_Tot_x_ESTI[Index-1] + (1-lambda)*pow( INNOV_x[Index], 2.0 );
			Vari_Tot_y_ESTI[Index] = lambda * Vari_Tot_y_ESTI[Index-1] + (1-lambda)*pow( INNOV_y[Index], 2.0 );
			Vari_Tot_z_ESTI[Index] = lambda * Vari_Tot_z_ESTI[Index-1] + (1-lambda)*pow( INNOV_z[Index], 2.0 );
			//
			Qw1_x = 1e-8*Vari_Tot_x_ESTI[Index]; //(thermal variance + meas. noise)
			Qw_x[0][0] = Qw1_x;
			//
			Qw1_y = 1e-8*Vari_Tot_y_ESTI[Index]; //(thermal variance + meas. noise)
			Qw_y[0][0] = Qw1_y;
			//
			Qw1_z = 1e-8*Vari_Tot_z_ESTI[Index]; //(thermal variance + meas. noise)
			Qw_z[0][0] = Qw1_z;

			Vari_thm_x = 2*kB*T*SplIntv/Damp_x_ESTI[Index];
			Vari_thm_y = 2*kB*T*SplIntv/Damp_y_ESTI[Index];
			Vari_thm_z = 2*kB*T*SplIntv/Damp_z_ESTI[Index];
			Vari_Meas_x_ESTI[Index] = Vari_Tot_x_ESTI[Index] - Vari_thm_x;
			Vari_Meas_y_ESTI[Index] = Vari_Tot_y_ESTI[Index] - Vari_thm_y;
			Vari_Meas_z_ESTI[Index] = Vari_Tot_z_ESTI[Index] - Vari_thm_z;



			//////#########################################
			//////****** Time Update & Meas. Update: ****** (using Thermal Variance)
			//////#########################################
			////Get_STATE_k_km1();
			////Get_EstiCov_P_k_km1();

			////INNOV_x[Index] = OBSRV_x[Index] - CalcMat_1x3_3x1( Hx_k, STATE_k_km1_x ) ;
			////INNOV_y[Index] = OBSRV_y[Index] - CalcMat_1x3_3x1( Hy_k, STATE_k_km1_y ) ;
			////INNOV_z[Index] = OBSRV_z[Index] - CalcMat_1x3_3x1( Hz_k, STATE_k_km1_z ) ; //innovations

			////Get_InnovCov();      //Innovation Covariance, CovR_x[Index-1],
			////Get_KalmanGain_K();  //get Kalman filter gain
			////Get_STATE_k();       //meas. update
			////Get_EstiCov_P_k_k(); //Estimation Covariance
			//////##############################################
			//////****** (end) Time Update & Meas. Update ******
			//////##############################################

			////Vari_Tot_x_ESTI[Index] = lambda * Vari_Tot_x_ESTI[Index-1] + (1-lambda)*pow( INNOV_x[Index], 2.0 );
			////Vari_Tot_y_ESTI[Index] = lambda * Vari_Tot_y_ESTI[Index-1] + (1-lambda)*pow( INNOV_y[Index], 2.0 );
			////Vari_Tot_z_ESTI[Index] = lambda * Vari_Tot_z_ESTI[Index-1] + (1-lambda)*pow( INNOV_z[Index], 2.0 );

			////// x direction:
			////pN_O_Damp_x_ESTI[Index] = STATE_k_k_x[2];
			////Damp_x_ESTI[Index]      = 2*kB*T*SplIntv/Vari_Tot_x_ESTI[Index];
			////M_Ext_x_ESTI[Index] = STATE_k_k_x[0];
			//////F_Ext_x_ESTI[Index] = Damp_x_ESTI[Index]*M_Ext_x_ESTI[Index]/SplIntv;
			////F_Ext_x_ESTI[Index] = Damp_x_ESTI[Index]*OBSRV_x[Index]/SplIntv;
			////// y direction:
			////pN_O_Damp_y_ESTI[Index] = STATE_k_k_y[2];
			////Damp_y_ESTI[Index]      = 2*kB*T*SplIntv/Vari_Tot_y_ESTI[Index];
			////M_Ext_y_ESTI[Index] = STATE_k_k_y[0];
			////F_Ext_y_ESTI[Index] = Damp_y_ESTI[Index]*M_Ext_y_ESTI[Index]/SplIntv;

			////// z direction:
			////pN_O_Damp_z_ESTI[Index] = STATE_k_k_z[2];
			////Damp_z_ESTI[Index]      = 2*kB*T*SplIntv/Vari_Tot_z_ESTI[Index];
			////M_Ext_z_ESTI[Index] = STATE_k_k_z[0];
			////F_Ext_z_ESTI[Index] = Damp_z_ESTI[Index]*M_Ext_z_ESTI[Index]/SplIntv;

			//////
			////Qw1_x = 1e-8*Vari_Tot_x_ESTI[Index]; //(thermal variance + meas. noise)
			////Qw_x[0][0] = Qw1_x;
			//////
			////Qw1_y = 1e-8*Vari_Tot_y_ESTI[Index]; //(thermal variance + meas. noise)
			////Qw_y[0][0] = Qw1_y;
			//////
			////Qw1_z = 1e-8*Vari_Tot_z_ESTI[Index]; //(thermal variance + meas. noise)
			////Qw_z[0][0] = Qw1_z;

			////Vari_thm_x = 2*kB*T*SplIntv/Damp_x_ESTI[Index];
			////Vari_thm_y = 2*kB*T*SplIntv/Damp_y_ESTI[Index];
			////Vari_thm_z = 2*kB*T*SplIntv/Damp_z_ESTI[Index];
			////Vari_Meas_x_ESTI[Index] = Vari_Tot_x_ESTI[Index] - Vari_thm_x;
			////Vari_Meas_y_ESTI[Index] = Vari_Tot_y_ESTI[Index] - Vari_thm_y;
			////Vari_Meas_z_ESTI[Index] = Vari_Tot_z_ESTI[Index] - Vari_thm_z;



			//*****************************
			//****** History Update: ******
			//*****************************
			for (int i=0; i<3; i++)
			{
				STATE_km1_km1_x[i] = STATE_k_k_x[i];
				STATE_km1_km1_y[i] = STATE_k_k_y[i];
				STATE_km1_km1_z[i] = STATE_k_k_z[i];
				for (int j=0; j<3; j++)
				{
					P_km1_km1_x[i][j] = P_k_k_x[i][j];
					P_km1_km1_y[i][j] = P_k_k_y[i][j];
					P_km1_km1_z[i][j] = P_k_k_z[i][j];
				}
			}
			//**********************************
			//****** (end) History Update ******
			//**********************************
		}
		//##############################
		//##############################
		//###### (end) Estimation ######
		//##############################
		//##############################
	}
}



void CALCULATION::CircTracking_Init(double CirD_um, double CirSpd_ums, int CirNum)
{
	Set_CircTrk_Dia_Spd( CirD_um, CirSpd_ums, CirNum );

	Simu_FdX_pNOm = new double [LEN_Simu_Meas];
	Simu_FdY_pNOm = new double [LEN_Simu_Meas];
	Simu_FdZ_pNOm = new double [LEN_Simu_Meas]; //For Recording

	Simu_FdX_pNOa = new double [LEN_Simu_Meas];
	Simu_FdY_pNOa = new double [LEN_Simu_Meas];
	Simu_FdZ_pNOa = new double [LEN_Simu_Meas]; //For Recording

	Simu_FdX_norOm = new double [LEN_Simu_Meas];
	Simu_FdY_norOm = new double [LEN_Simu_Meas];
	Simu_FdZ_norOm = new double [LEN_Simu_Meas]; //For Recording

	Simu_FdX_norOa = new double [LEN_Simu_Meas];
	Simu_FdY_norOa = new double [LEN_Simu_Meas];
	Simu_FdZ_norOa = new double [LEN_Simu_Meas]; //For Recording

	Simu_FXcalc_pNOa = new double [LEN_Simu_Meas];
	Simu_FYcalc_pNOa = new double [LEN_Simu_Meas];
	Simu_FZcalc_pNOa = new double [LEN_Simu_Meas]; //For Checking

	for (int i=0; i<LEN_Simu_Meas; i++)
	{
		CoordTrans_Elem_m2a( Simu_TX_umOm[i], Simu_TY_umOm[i], Simu_TZ_umOm[i],  
			                &Simu_TX_umOa[i],&Simu_TY_umOa[i],&Simu_TZ_umOa[i] );

		if (i<DELAY_STEP_Meas+1)
		{
			if (i==0)
			{
				Simu_RX_umOm[i] = Simu_TX_umOm[i] + ThmMotion_X_RD[i]*m2um;
				Simu_RY_umOm[i] = Simu_TY_umOm[i] + ThmMotion_Y_RD[i]*m2um;
				Simu_RZ_umOm[i] = Simu_TZ_umOm[i] + ThmMotion_Z_RD[i]*m2um; //Real Position
				CoordTrans_Elem_m2a( Simu_RX_umOm[i], Simu_RY_umOm[i], Simu_RZ_umOm[i],  
					                &Simu_RX_umOa[i],&Simu_RY_umOa[i],&Simu_RZ_umOa[i] );
			}
			else
			{
				Simu_RX_umOm[i] = Simu_RX_umOm[i-1] + ThmMotion_X_RD[i]*m2um;
				Simu_RY_umOm[i] = Simu_RY_umOm[i-1] + ThmMotion_Y_RD[i]*m2um;
				Simu_RZ_umOm[i] = Simu_RZ_umOm[i-1] + ThmMotion_Z_RD[i]*m2um; //Real Position
				CoordTrans_Elem_m2a( Simu_RX_umOm[i], Simu_RY_umOm[i], Simu_RZ_umOm[i],  
					                &Simu_RX_umOa[i],&Simu_RY_umOa[i],&Simu_RZ_umOa[i]);
			}
		}
	}
}


void CALCULATION::RampTracking_Init(double RangeXY_um, double RangeZ_um, int UP_or_DOWN, double RampSpd_ums, double RampAngle_deg)
{
	Set_RampTrk_Dia_Spd( RangeXY_um, RangeZ_um, UP_or_DOWN, RampSpd_ums, RampAngle_deg );


	if (F_Ext_x_pN_Apply == NULL)
	{
		F_Ext_x_pN_Apply = new double[LEN_Simu_Meas];
		F_Ext_y_pN_Apply = new double[LEN_Simu_Meas];
		F_Ext_z_pN_Apply = new double[LEN_Simu_Meas]; //applied force in pN
	}
	Simu_FdX_pNOm = new double [LEN_Simu_Meas];
	Simu_FdY_pNOm = new double [LEN_Simu_Meas];
	Simu_FdZ_pNOm = new double [LEN_Simu_Meas]; //For data recording

	Simu_FdX_pNOa = new double [LEN_Simu_Meas];
	Simu_FdY_pNOa = new double [LEN_Simu_Meas];
	Simu_FdZ_pNOa = new double [LEN_Simu_Meas]; //For data recording

	Simu_FdX_norOm = new double [LEN_Simu_Meas];
	Simu_FdY_norOm = new double [LEN_Simu_Meas];
	Simu_FdZ_norOm = new double [LEN_Simu_Meas]; //For Recording

	Simu_FdX_norOa = new double [LEN_Simu_Meas];
	Simu_FdY_norOa = new double [LEN_Simu_Meas];
	Simu_FdZ_norOa = new double [LEN_Simu_Meas]; //For Recording

	Simu_FXcalc_pNOa = new double [LEN_Simu_Meas];
	Simu_FYcalc_pNOa = new double [LEN_Simu_Meas];
	Simu_FZcalc_pNOa = new double [LEN_Simu_Meas]; //For data checking

	for (int i=0; i<LEN_Simu_Meas; i++)
	{
		CoordTrans_Elem_m2a( Simu_TX_umOm[i], Simu_TY_umOm[i], Simu_TZ_umOm[i],  
			                &Simu_TX_umOa[i],&Simu_TY_umOa[i],&Simu_TZ_umOa[i] );

		if (i<DELAY_STEP_Meas+1)
		{
			if (i==0)
			{
				//Simu_RX_umOm[i] = Simu_TX_umOm[i] + ThmMotion_X_RD[i]*m2um;
				//Simu_RY_umOm[i] = Simu_TY_umOm[i] + ThmMotion_Y_RD[i]*m2um;
				//Simu_RZ_umOm[i] = Simu_TZ_umOm[i] + ThmMotion_Z_RD[i]*m2um; //Real Position
				Simu_RX_umOm[i] = Simu_TX_umOm[i];
				Simu_RY_umOm[i] = Simu_TY_umOm[i];
				Simu_RZ_umOm[i] = Simu_TZ_umOm[i]; //Real Position
				CoordTrans_Elem_m2a( Simu_RX_umOm[i], Simu_RY_umOm[i], Simu_RZ_umOm[i],
					                &Simu_RX_umOa[i],&Simu_RY_umOa[i],&Simu_RZ_umOa[i] );
			}
			else
			{
				//Simu_RX_umOm[i] = Simu_RX_umOm[i-1] + ThmMotion_X_RD[i]*m2um;
				//Simu_RY_umOm[i] = Simu_RY_umOm[i-1] + ThmMotion_Y_RD[i]*m2um;
				//Simu_RZ_umOm[i] = Simu_RZ_umOm[i-1] + ThmMotion_Z_RD[i]*m2um; //Real Position
				Simu_RX_umOm[i] = Simu_RX_umOm[i - 1];
				Simu_RY_umOm[i] = Simu_RY_umOm[i - 1];
				Simu_RZ_umOm[i] = Simu_RZ_umOm[i - 1]; //Real Position
				CoordTrans_Elem_m2a( Simu_RX_umOm[i], Simu_RY_umOm[i], Simu_RZ_umOm[i],  
					                &Simu_RX_umOa[i],&Simu_RY_umOa[i],&Simu_RZ_umOa[i]);
			}
		}
	}
}


void CALCULATION::Tracking_Simu( int THEO_or_REAL )
{
	double PosAft_umOa [3],
		   //
		   PosMeas_umOm[3],
		   PosMeas_umOa[3], //measurement position
		   //
		   PosReal_umOm[3],
		   PosReal_umOa[3]; //real position

	for ( Index=0; Index<LEN_Simu_Meas; Index++)
	{
		if ( PosCtrl_Counter%1000 == 0)
		{
			cout<<((double)(PosCtrl_Counter))/LEN_Simu_Meas*100<<" %"<<endl;
		}

		//Simu_MX_umOm[Index] = Simu_RX_umOm[Index] + MeasNoise_X_mOm_RD[Index]*m2um;  //m2um = 10^-6
		//Simu_MY_umOm[Index] = Simu_RY_umOm[Index] + MeasNoise_Y_mOm_RD[Index]*m2um;
		//Simu_MZ_umOm[Index] = Simu_RZ_umOm[Index] + MeasNoise_Z_mOm_RD[Index]*m2um;  //Measurement in Om (With Delay & meas. noise)
		Simu_MX_umOm[Index] = 0.0;  //m2um = 10^-6
		Simu_MY_umOm[Index] = 0.0;
		Simu_MZ_umOm[Index] = 0.0;  //Measurement in Om (With Delay & meas. noise)
		CoordTrans_Elem_m2a( Simu_MX_umOm[Index], Simu_MY_umOm[Index], Simu_MZ_umOm[Index],  
			                &Simu_MX_umOa[Index],&Simu_MY_umOa[Index],&Simu_MZ_umOa[Index] ); //Measurement in Om -> Oa

		ERRpos_umOm[X] = Simu_TX_umOm[Index] - Simu_MX_umOm[Index];
		ERRpos_umOm[Y] = Simu_TY_umOm[Index] - Simu_MY_umOm[Index];
		ERRpos_umOm[Z] = Simu_TZ_umOm[Index] - Simu_MZ_umOm[Index]; //Error in Om

		PosMeas_umOm[X] = Simu_MX_umOm[Index];
		PosMeas_umOm[Y] = Simu_MY_umOm[Index];
		PosMeas_umOm[Z] = Simu_MZ_umOm[Index];
		CoordTrans_Vect_m2a ( PosMeas_umOm, PosMeas_umOa ); //PosMeas in Om -> Oa

		Calc_Ctrl_Effort_Force ( PID_ctrl ); //(will use ERRpos_umOm[3], return Fd_pNOm, PosCtrl_Counter ++)
		Fd_pNOm[X] = 1.0;
		Fd_pNOm[Y] = 1.0;
		Fd_pNOm[Z] = 1.0;
		CoordTrans_Vect_m2a ( Fd_pNOm, Fd_pNOa );

		Simu_FdX_pNOm[Index] = Fd_pNOm[X];
		Simu_FdY_pNOm[Index] = Fd_pNOm[Y];
		Simu_FdZ_pNOm[Index] = Fd_pNOm[Z]; //data  Recording, Fd in Om
		//
		Simu_FdX_pNOa[Index] = Fd_pNOa[X];
		Simu_FdY_pNOa[Index] = Fd_pNOa[Y];
		Simu_FdZ_pNOa[Index] = Fd_pNOa[Z]; //data  Recording, Fd in Oa

		Simu_FdX_norOa[Index] = Simu_FdX_pNOa[Index] / FGain_x; //****//
		Simu_FdY_norOa[Index] = Simu_FdY_pNOa[Index] / FGain_y; //****//
		Simu_FdZ_norOa[Index] = Simu_FdZ_pNOa[Index] / FGain_z; //****//Fnor is better to be in Oa
		CoordTrans_Elem_a2m( Simu_FdX_norOa[Index], Simu_FdY_norOa[Index], Simu_FdZ_norOa[Index],  
			                &Simu_FdX_norOm[Index],&Simu_FdY_norOm[Index],&Simu_FdZ_norOm[Index]); //Measurement in Oa -> Om

		if ( THEO_or_REAL == THEO )
		{
			OptInvMdl_Curr_FromLS_KItheo( Fd_pNOa, PosMeas_umOa); //Obtained: Vhall_Dsir_P1toP6
			Calc_FNor_FromSixCurr_KItheo( PosMeas_umOa, I1toI6_KItheo); //check if the force is ok
			Simu_FXcalc_pNOa[Index] = FNor_Curr_KItheo[X] * FGain_x; //force check
			Simu_FYcalc_pNOa[Index] = FNor_Curr_KItheo[Y] * FGain_y; //force check
			Simu_FZcalc_pNOa[Index] = FNor_Curr_KItheo[Z] * FGain_z; //force check
		}
		else
		{
			OptInvMdl_Curr_FromLS_KIreal( Fd_pNOa, PosMeas_umOa ); //Obtained: Vhall_Dsir_P1toP6[6]
			Calc_FNor_FromSixCurr_KIreal( PosMeas_umOa, I1toI6_KIreal ); //check if the force is ok
			Simu_FXcalc_pNOa[Index] = FNor_Curr_KIreal[X] * FGain_x; //force check
			Simu_FYcalc_pNOa[Index] = FNor_Curr_KIreal[Y] * FGain_y; //force check
			Simu_FZcalc_pNOa[Index] = FNor_Curr_KIreal[Z] * FGain_z; //force check
		}


		PosReal_umOm[X] = Simu_RX_umOm[ Index + DELAY_STEP_Meas ];
		PosReal_umOm[Y] = Simu_RY_umOm[ Index + DELAY_STEP_Meas ];
		PosReal_umOm[Z] = Simu_RZ_umOm[ Index + DELAY_STEP_Meas ]; //Real Position (with delay)
		CoordTrans_Vect_m2a ( PosReal_umOm, PosReal_umOa );     //PosReal in Om -> Oa

		Calc_NextBeadPos( PosReal_umOa, PosAft_umOa, THEO_or_REAL );  //Calc Next Real Pos (Note: This is in Oa, so Damping need to be in Oa)

		Simu_RX_umOa[ Index + DELAY_STEP_Meas + 1 ] = PosAft_umOa[X];
		Simu_RY_umOa[ Index + DELAY_STEP_Meas + 1 ] = PosAft_umOa[Y];
		Simu_RZ_umOa[ Index + DELAY_STEP_Meas + 1 ] = PosAft_umOa[Z]; //With Delay
        CoordTrans_Elem_a2m( Simu_RX_umOa[ Index+DELAY_STEP_Meas+1 ],  Simu_RY_umOa[ Index+DELAY_STEP_Meas+1],  Simu_RZ_umOa[ Index+DELAY_STEP_Meas+1 ],
                            &Simu_RX_umOm[ Index+DELAY_STEP_Meas+1 ], &Simu_RY_umOm[ Index+DELAY_STEP_Meas+1], &Simu_RZ_umOm[ Index+DELAY_STEP_Meas+1 ]); //Real motion in Om -> Oa

		Simu_I1[Index] = I1toI6_KItheo[0];
		Simu_I2[Index] = I1toI6_KItheo[1];
		Simu_I3[Index] = I1toI6_KItheo[2];
		Simu_I4[Index] = I1toI6_KItheo[3];
		Simu_I5[Index] = I1toI6_KItheo[4];
		Simu_I6[Index] = I1toI6_KItheo[5];
	}
}


void CALCULATION::Tracking_Simu_WithEsti(int THEO_or_REAL, int ADAPT_or_CONST_KF)
{
	double PosAft_umOa [3],
		   //
		   PosMeas_umOm[3],
		   PosMeas_umOa[3], //measurement position
		   //
		   PosReal_umOm[3],
		   PosReal_umOa[3]; //real position

	for ( Index=0; Index<LEN_Simu_Meas; Index++)
	{
		if ( PosCtrl_Counter%1000 == 0)
		{
			cout<<((double)(PosCtrl_Counter))/LEN_Simu_Meas*100<<" %"<<endl;
		}

		Simu_MX_umOm[Index] = Simu_RX_umOm[Index] + MeasNoise_X_mOm_RD[Index]*m2um;
		Simu_MY_umOm[Index] = Simu_RY_umOm[Index] + MeasNoise_Y_mOm_RD[Index]*m2um;
		Simu_MZ_umOm[Index] = Simu_RZ_umOm[Index] + MeasNoise_Z_mOm_RD[Index]*m2um;  //Measurement in Om (With Delay & meas. noise)
		CoordTrans_Elem_m2a( Simu_MX_umOm[Index], Simu_MY_umOm[Index], Simu_MZ_umOm[Index],  
			                &Simu_MX_umOa[Index],&Simu_MY_umOa[Index],&Simu_MZ_umOa[Index] ); //Measurement in Om -> Oa

		ERRpos_umOm[X] = Simu_TX_umOm[Index] - Simu_MX_umOm[Index];
		ERRpos_umOm[Y] = Simu_TY_umOm[Index] - Simu_MY_umOm[Index];
		ERRpos_umOm[Z] = Simu_TZ_umOm[Index] - Simu_MZ_umOm[Index]; //Error in Om

		PosMeas_umOm[X] = Simu_MX_umOm[Index];
		PosMeas_umOm[Y] = Simu_MY_umOm[Index];
		PosMeas_umOm[Z] = Simu_MZ_umOm[Index]; //
		CoordTrans_Vect_m2a ( PosMeas_umOm, PosMeas_umOa ); //PosMeas in Om -> Oa

		Calc_Ctrl_Effort_Force ( PID_ctrl ); //(will use ERRpos_umOm[3], return Fd_pNOm, PosCtrl_Counter ++)
		CoordTrans_Vect_m2a ( Fd_pNOm, Fd_pNOa );

		Simu_FdX_pNOm[Index] = Fd_pNOm[X];
		Simu_FdY_pNOm[Index] = Fd_pNOm[Y];
		Simu_FdZ_pNOm[Index] = Fd_pNOm[Z]; //data  Recording, Fd in Om
		//
		Simu_FdX_pNOa[Index] = Fd_pNOa[X];
		Simu_FdY_pNOa[Index] = Fd_pNOa[Y];
		Simu_FdZ_pNOa[Index] = Fd_pNOa[Z]; //data  Recording, Fd in Oa

		Simu_FdX_norOa[Index] = Simu_FdX_pNOa[Index] / FGain_x; //****//
		Simu_FdY_norOa[Index] = Simu_FdY_pNOa[Index] / FGain_y; //****//
		Simu_FdZ_norOa[Index] = Simu_FdZ_pNOa[Index] / FGain_z; //****//Fnor is better to be in Oa
		CoordTrans_Elem_a2m( Simu_FdX_norOa[Index], Simu_FdY_norOa[Index], Simu_FdZ_norOa[Index],  
			                &Simu_FdX_norOm[Index],&Simu_FdY_norOm[Index],&Simu_FdZ_norOm[Index]); //Measurement in Oa -> Om

		if ( THEO_or_REAL == THEO )
		{
			OptInvMdl_Curr_FromLS_KItheo( Fd_pNOa, PosMeas_umOa); //Obtained: Vhall_Dsir_P1toP6[6]
			Calc_FNor_FromSixCurr_KItheo( PosMeas_umOa, I1toI6_KItheo ); //check if the force is ok //z
			Simu_FXcalc_pNOa[Index] = FNor_Curr_KItheo[X] * FGain_x; //force check
			Simu_FYcalc_pNOa[Index] = FNor_Curr_KItheo[Y] * FGain_y; //force check
			Simu_FZcalc_pNOa[Index] = FNor_Curr_KItheo[Z] * FGain_z; //force check
		}
		else
		{
			OptInvMdl_Curr_FromLS_KIreal( Fd_pNOa, PosMeas_umOa); //Obtained: Vhall_Dsir_P1toP6[6]
			Calc_FNor_FromSixCurr_KIreal( PosMeas_umOa, I1toI6_KIreal); //check if the force is ok
			Simu_FXcalc_pNOa[Index] = FNor_Curr_KIreal[X] * FGain_x; //force check
			Simu_FYcalc_pNOa[Index] = FNor_Curr_KIreal[Y] * FGain_y; //force check
			Simu_FZcalc_pNOa[Index] = FNor_Curr_KIreal[Z] * FGain_z; //force check
		}


		PosReal_umOm[X] = Simu_RX_umOm[ Index+DELAY_STEP_Meas ];
		PosReal_umOm[Y] = Simu_RY_umOm[ Index+DELAY_STEP_Meas ];
		PosReal_umOm[Z] = Simu_RZ_umOm[ Index+DELAY_STEP_Meas ]; //Real Position (with delay)
		CoordTrans_Vect_m2a ( PosReal_umOm, PosReal_umOa );     //PosReal in Om -> Oa

		Calc_NextBeadPos( PosReal_umOa, PosAft_umOa, THEO_or_REAL );  //Calc Next Real Pos (Note: This is in Oa, so Damping need to be in Oa)

		Simu_RX_umOa[ Index+DELAY_STEP_Meas+1 ] = PosAft_umOa[X];
		Simu_RY_umOa[ Index+DELAY_STEP_Meas+1 ] = PosAft_umOa[Y];
		Simu_RZ_umOa[ Index+DELAY_STEP_Meas+1 ] = PosAft_umOa[Z]; //With Delay
        CoordTrans_Elem_a2m( Simu_RX_umOa[ Index+DELAY_STEP_Meas+1 ],  Simu_RY_umOa[ Index+DELAY_STEP_Meas+1],  Simu_RZ_umOa[ Index+DELAY_STEP_Meas+1 ],
                            &Simu_RX_umOm[ Index+DELAY_STEP_Meas+1 ], &Simu_RY_umOm[ Index+DELAY_STEP_Meas+1], &Simu_RZ_umOm[ Index+DELAY_STEP_Meas+1 ]); //Real motion in Om -> Oa

		//#####################
		//#### Estimation: ####
		//#####################
		if (Index>0)
		{
			if (Index>0)
			{
				OBSRV_x[Index] = (Simu_MX_umOa[Index] - Simu_MX_umOa[Index-1])*um2m;
				OBSRV_y[Index] = (Simu_MY_umOa[Index] - Simu_MY_umOa[Index-1])*um2m;
				OBSRV_z[Index] = (Simu_MZ_umOa[Index] - Simu_MZ_umOa[Index-1])*um2m;
			}

			if ( Index>= DELAY_STEP_Esti + 1 )
			{
				P_XYZ_umOa_forEsti[0] = Simu_MX_umOa[ Index - 1 - DELAY_STEP_Esti ];
				P_XYZ_umOa_forEsti[1] = Simu_MY_umOa[ Index - 1 - DELAY_STEP_Esti ];
				P_XYZ_umOa_forEsti[2] = Simu_MZ_umOa[ Index - 1 - DELAY_STEP_Esti ];
				In_P1toP6_forEsti[0] = Simu_I1[ Index - 1 - DELAY_STEP_Esti ];
				In_P1toP6_forEsti[1] = Simu_I2[ Index - 1 - DELAY_STEP_Esti ];
				In_P1toP6_forEsti[2] = Simu_I3[ Index - 1 - DELAY_STEP_Esti ];
				In_P1toP6_forEsti[3] = Simu_I4[ Index - 1 - DELAY_STEP_Esti ];
				In_P1toP6_forEsti[4] = Simu_I5[ Index - 1 - DELAY_STEP_Esti ];
				In_P1toP6_forEsti[5] = Simu_I6[ Index - 1 - DELAY_STEP_Esti ];

				Calc_Lxyz( P_XYZ_umOa_forEsti );
				Calc_FNor_FromSixCurr_KItheo( P_XYZ_umOa_forEsti, In_P1toP6_forEsti ); //fx_nor_Oa, fy_nor_Oa, fz_nor_Oa
				Fx_pNOa_Curr[Index] = FGain_x * FNor_Curr_KItheo[0] ;
				Fy_pNOa_Curr[Index] = FGain_y * FNor_Curr_KItheo[1];
				Fz_pNOa_Curr[Index] = FGain_z * FNor_Curr_KItheo[2];
			}
			else
			{
				Fx_pNOa_Curr[Index] = 0.0;
				Fy_pNOa_Curr[Index] = 0.0;
				Fz_pNOa_Curr[Index] = 0.0;
			}
			Hx_k[2] = Fx_pNOa_Curr[Index] * SplIntv * pN2N;
			Hy_k[2] = Fy_pNOa_Curr[Index] * SplIntv * pN2N;
			Hz_k[2] = Fz_pNOa_Curr[Index] * SplIntv * pN2N;


			//#########################################
			//****** Time Update & Meas. Update: ******
			//#########################################
			Get_STATE_k_km1();
			Get_EstiCov_P_k_km1();

			INNOV_x[Index] = OBSRV_x[Index] - CalcMat_1x3_3x1( Hx_k, STATE_k_km1_x ) ;
			INNOV_y[Index] = OBSRV_y[Index] - CalcMat_1x3_3x1( Hy_k, STATE_k_km1_y ) ;
			INNOV_z[Index] = OBSRV_z[Index] - CalcMat_1x3_3x1( Hz_k, STATE_k_km1_z ) ; //innovations

			Get_InnovCov();      //Innovation Covariance, CovR_x[Index-1],
			Get_KalmanGain_K();  //get Kalman filter gain
			Get_STATE_k();       //meas. update
			Get_EstiCov_P_k_k(); //Estimation Covariance
			//##############################################
			//****** (end) Time Update & Meas. Update ******
			//##############################################
			// x direction:
			M_Ext_x_ESTI   [Index] = STATE_k_k_x[0];
			pN_O_Damp_x_ESTI [Index] = STATE_k_k_x[2];
			Damp_x_ESTI[Index]  = 1/pN_O_Damp_x_ESTI[Index];
			F_Ext_x_ESTI[Index] = Damp_x_ESTI[Index]*M_Ext_x_ESTI[Index]/SplIntv;

			// y direction:
			M_Ext_y_ESTI  [Index] = STATE_k_k_y[0];
			pN_O_Damp_y_ESTI[Index] = STATE_k_k_y[2];
			Damp_y_ESTI[Index]  = 1/pN_O_Damp_y_ESTI[Index];
			F_Ext_y_ESTI[Index] = Damp_y_ESTI[Index]*M_Ext_y_ESTI[Index]/SplIntv;

			// z direction:
			M_Ext_z_ESTI   [Index] = STATE_k_k_z[0];
			pN_O_Damp_z_ESTI [Index] = STATE_k_k_z[2];
			Damp_z_ESTI[Index]  = 1/pN_O_Damp_z_ESTI[Index];
			F_Ext_z_ESTI[Index] = Damp_z_ESTI[Index]*M_Ext_z_ESTI[Index]/SplIntv;


			Vari_Tot_x_ESTI[Index] = lambda * Vari_Tot_x_ESTI[Index-1] + (1-lambda)*pow( INNOV_x[Index], 2.0 );
			Vari_Tot_y_ESTI[Index] = lambda * Vari_Tot_y_ESTI[Index-1] + (1-lambda)*pow( INNOV_y[Index], 2.0 );
			Vari_Tot_z_ESTI[Index] = lambda * Vari_Tot_z_ESTI[Index-1] + (1-lambda)*pow( INNOV_z[Index], 2.0 );
			//
			Qw1_x = 1e-7*Vari_Tot_x_ESTI[Index]; //(thermal variance + meas. noise)
			Qw_x[0][0] = Qw1_x;
			//
			Qw1_y = 1e-7*Vari_Tot_y_ESTI[Index]; //(thermal variance + meas. noise)
			Qw_y[0][0] = Qw1_y;
			//
			Qw1_z = 1e-7*Vari_Tot_z_ESTI[Index]; //(thermal variance + meas. noise)
			Qw_z[0][0] = Qw1_z;


			double Vari_thm_x, 
				   Vari_thm_y, 
				   Vari_thm_z;
			Vari_thm_x = 2*kB*T*SplIntv/Damp_x_ESTI[Index];
			Vari_thm_y = 2*kB*T*SplIntv/Damp_y_ESTI[Index];
			Vari_thm_z = 2*kB*T*SplIntv/Damp_z_ESTI[Index];
			Vari_Meas_x_ESTI[Index] = Vari_Tot_x_ESTI[Index] - Vari_thm_x;
			Vari_Meas_y_ESTI[Index] = Vari_Tot_y_ESTI[Index] - Vari_thm_y;
			Vari_Meas_z_ESTI[Index] = Vari_Tot_z_ESTI[Index] - Vari_thm_z;

			//****** History Update: ******
			for (int i=0; i<3; i++)
			{
				STATE_km1_km1_x[i] = STATE_k_k_x[i];
				STATE_km1_km1_y[i] = STATE_k_k_y[i];
				STATE_km1_km1_z[i] = STATE_k_k_z[i];
				for (int j=0; j<3; j++)
				{
					P_km1_km1_x[i][j] = P_k_k_x[i][j];
					P_km1_km1_y[i][j] = P_k_k_y[i][j];
					P_km1_km1_z[i][j] = P_k_k_z[i][j];
				}
			}
			//*****************************
		}
		//##########################
		//#### (end) Estimation ####
		//##########################
	}
}



void CALCULATION::Calc_Pre_L(double Pos_umOa[3])        //Pos_um[3]: in um, to be used for calculating Lx,y,z
{
      x = Pos_umOa[X]/R_norm;                                 //norlizing
	  y = Pos_umOa[Y]/R_norm;                                 //norlizing
	  z = Pos_umOa[Z]/R_norm;                                 //norlizing

	  //Dx1_x = - x;
	  //Dx2_x = - x;
	  //Dx3_x = - x;
	  //Dx4_x = - x;
	  //Dx5_x = - x;
	  //Dx6_x = - x;

	  //Dy1_y = - y;
	  //Dy2_y = - y;
	  //Dy3_y = - y;
	  //Dy4_y = - y;
	  //Dy5_y = - y;
	  //Dy6_y = - y;

	  //Dz1_z = - z;
	  //Dz2_z = - z;
	  //Dz3_z = - z;
	  //Dz4_z = - z; 
	  //Dz5_z = - z; 
	  //Dz6_z = - z;

	  //Dx1_xPlus1 = - x + 1;
	  //x_Dx2Plus1 = x + 1;
	  //Dy3_yPlus1 = - y + 1;
	  //y_Dy4Plus1 = y + 1;
	  //Dz5_zPlus1 = -z + 1;
	  //z_Dz6Plus1 = z + 1;

	  Dx1_x = Bias_umOa_P1toP6[0]/R_norm  - x;
	  Dx2_x = Bias_umOa_P1toP6[3]/R_norm  - x;
	  Dx3_x = Bias_umOa_P1toP6[6]/R_norm  - x;
	  Dx4_x = Bias_umOa_P1toP6[9]/R_norm  - x;
	  Dx5_x = Bias_umOa_P1toP6[12]/R_norm - x;
	  Dx6_x = Bias_umOa_P1toP6[15]/R_norm - x;

	  Dy1_y = Bias_umOa_P1toP6[1]/R_norm  - y;
	  Dy2_y = Bias_umOa_P1toP6[4]/R_norm  - y;
	  Dy3_y = Bias_umOa_P1toP6[7]/R_norm  - y;
	  Dy4_y = Bias_umOa_P1toP6[10]/R_norm - y;
	  Dy5_y = Bias_umOa_P1toP6[13]/R_norm - y;
	  Dy6_y = Bias_umOa_P1toP6[16]/R_norm - y;

	  Dz1_z = Bias_umOa_P1toP6[2]/R_norm  - z;
	  Dz2_z = Bias_umOa_P1toP6[5]/R_norm  - z;
	  Dz3_z = Bias_umOa_P1toP6[8]/R_norm  - z;
	  Dz4_z = Bias_umOa_P1toP6[11]/R_norm - z; 
	  Dz5_z = Bias_umOa_P1toP6[14]/R_norm - z; 
	  Dz6_z = Bias_umOa_P1toP6[17]/R_norm - z;

	  Dx1_xPlus1 = Bias_umOa_P1toP6[0]/R_norm - x + 1;
	  x_Dx2Plus1 = x - Bias_umOa_P1toP6[3]/R_norm + 1;
	  Dy3_yPlus1 = Bias_umOa_P1toP6[7]/R_norm - y + 1;
	  y_Dy4Plus1 = y - Bias_umOa_P1toP6[10]/R_norm + 1;
	  Dz5_zPlus1 = Bias_umOa_P1toP6[14]/R_norm - z + 1;
	  z_Dz6Plus1 = z - Bias_umOa_P1toP6[17]/R_norm + 1;

	  Sq_r1 = (pow(Dx1_xPlus1,2) + pow(Dy1_y,2) + pow(Dz1_z,2));
	  Sq_r2 = (pow(x_Dx2Plus1,2) + pow(Dy2_y,2) + pow(Dz2_z,2));
	  Sq_r3 = (pow(Dx3_x,2) + pow(Dy3_yPlus1,2) + pow(Dz3_z,2));
	  Sq_r4 = (pow(Dx4_x,2) + pow(y_Dy4Plus1,2) + pow(Dz4_z,2));
	  Sq_r5 = (pow(Dx5_x,2) + pow(Dy5_y,2) + pow(Dz5_zPlus1,2));
	  Sq_r6 = (pow(Dx6_x,2) + pow(Dy6_y,2) + pow(z_Dz6Plus1,2));  
}


void CALCULATION::Calc_Lx()       //After running func: Calc_Pre_L(double Pos_um[3])
{
	  Lx[0][0] = 4*Dx1_xPlus1/pow(Sq_r1,3);
	  Lx[0][1] = (((3*Dy1_y*Dy2_y - 3*Dx1_xPlus1*x_Dx2Plus1 + 3*Dz1_z*Dz2_z)/Sq_r1 - 1)*Dx1_xPlus1 - ((3*Dy1_y*Dy2_y - 3*Dx1_xPlus1*x_Dx2Plus1 + 3*Dz1_z*Dz2_z)/Sq_r2 - 1)*x_Dx2Plus1)/(pow(Sq_r1,1.5)*pow(Sq_r2,1.5));
      Lx[0][2] = (((3*Dx3_x*Dx1_xPlus1 + 3*Dy1_y*Dy3_yPlus1 + 3*Dz1_z*Dz3_z)/Sq_r1 - 1)*Dx1_xPlus1 + Dx3_x*((3*Dx3_x*Dx1_xPlus1 + 3*Dy1_y*Dy3_yPlus1 + 3*Dz1_z*Dz3_z)/Sq_r3 - 1))/(pow(Sq_r1,1.5)*pow(Sq_r3,1.5));
	  Lx[0][3] = (((3*Dx4_x*Dx1_xPlus1 - 3*Dy1_y*y_Dy4Plus1 + 3*Dz1_z*Dz4_z)/Sq_r1 - 1)*Dx1_xPlus1 + Dx4_x*((3*Dx4_x*Dx1_xPlus1 - 3*Dy1_y*y_Dy4Plus1 + 3*Dz1_z*Dz4_z)/Sq_r4 - 1))/(pow(Sq_r1,1.5)*pow(Sq_r4,1.5));
	  Lx[0][4] = (((3*Dx5_x*Dx1_xPlus1 + 3*Dy1_y*Dy5_y + 3*Dz1_z*Dz5_zPlus1)/Sq_r1 - 1)*Dx1_xPlus1 + Dx5_x*((3*Dx5_x*Dx1_xPlus1 + 3*Dy1_y*Dy5_y + 3*Dz1_z*Dz5_zPlus1)/Sq_r5 - 1))/(pow(Sq_r1,1.5)*pow(Sq_r5,1.5));
	  Lx[0][5] = (((3*Dx6_x*Dx1_xPlus1 + 3*Dy1_y*Dy6_y - 3*Dz1_z*z_Dz6Plus1)/Sq_r1 - 1)*Dx1_xPlus1 + Dx6_x*((3*Dx6_x*Dx1_xPlus1 + 3*Dy1_y*Dy6_y - 3*Dz1_z*z_Dz6Plus1)/Sq_r6 - 1))/(pow(Sq_r1,1.5)*pow(Sq_r6,1.5));

	  Lx[1][0] =  Lx[0][1];
	  Lx[1][1] = -4*x_Dx2Plus1/pow(Sq_r2,3);
	  Lx[1][2] = -(((3*Dy2_y*Dy3_yPlus1 - 3*Dx3_x*x_Dx2Plus1 + 3*Dz2_z*Dz3_z)/Sq_r2 - 1)*x_Dx2Plus1 - Dx3_x*((3*Dy2_y*Dy3_yPlus1 - 3*Dx3_x*x_Dx2Plus1 + 3*Dz2_z*Dz3_z)/Sq_r3 - 1))/(pow(Sq_r2,1.5)*pow(Sq_r3,1.5));
	  Lx[1][3] =  (((3*Dx4_x*x_Dx2Plus1 + 3*Dy2_y*y_Dy4Plus1 - 3*Dz2_z*Dz4_z)/Sq_r2 + 1)*x_Dx2Plus1 - Dx4_x*((3*Dx4_x*x_Dx2Plus1 + 3*Dy2_y*y_Dy4Plus1 - 3*Dz2_z*Dz4_z)/Sq_r4 + 1))/(pow(Sq_r2,1.5)*pow(Sq_r4,1.5));
	  Lx[1][4] = -(((3*Dy2_y*Dy5_y - 3*Dx5_x*x_Dx2Plus1 + 3*Dz2_z*Dz5_zPlus1)/Sq_r2 - 1)*x_Dx2Plus1 - Dx5_x*((3*Dy2_y*Dy5_y - 3*Dx5_x*x_Dx2Plus1 + 3*Dz2_z*Dz5_zPlus1)/Sq_r5 - 1))/(pow(Sq_r2,1.5)*pow(Sq_r5,1.5));
	  Lx[1][5] =  (((3*Dx6_x*x_Dx2Plus1 - 3*Dy2_y*Dy6_y + 3*Dz2_z*z_Dz6Plus1)/Sq_r2 + 1)*x_Dx2Plus1 - Dx6_x*((3*Dx6_x*x_Dx2Plus1 - 3*Dy2_y*Dy6_y + 3*Dz2_z*z_Dz6Plus1)/Sq_r6 + 1))/(pow(Sq_r2,1.5)*pow(Sq_r6,1.5));

	  Lx[2][0] = Lx[0][2];
	  Lx[2][1] = Lx[1][2];
	  Lx[2][2] = 4*Dx3_x/pow(Sq_r3,3);
	  Lx[2][3] = (Dx3_x*((3*Dx3_x*Dx4_x - 3*Dy3_yPlus1*y_Dy4Plus1 + 3*Dz3_z*Dz4_z)/Sq_r3 - 1) + Dx4_x*((3*Dx3_x*Dx4_x - 3*Dy3_yPlus1*y_Dy4Plus1 + 3*Dz3_z*Dz4_z)/Sq_r4 - 1))/(pow(Sq_r3,1.5)*pow(Sq_r4,1.5));
	  Lx[2][4] = (Dx3_x*((3*Dx3_x*Dx5_x + 3*Dy5_y*Dy3_yPlus1 + 3*Dz3_z*Dz5_zPlus1)/Sq_r3 - 1) + Dx5_x*((3*Dx3_x*Dx5_x + 3*Dy5_y*Dy3_yPlus1 + 3*Dz3_z*Dz5_zPlus1)/Sq_r5 - 1))/(pow(Sq_r3,1.5)*pow(Sq_r5,1.5));
	  Lx[2][5] = (Dx3_x*((3*Dx3_x*Dx6_x + 3*Dy6_y*Dy3_yPlus1 - 3*Dz3_z*z_Dz6Plus1)/Sq_r3 - 1) + Dx6_x*((3*Dx3_x*Dx6_x + 3*Dy6_y*Dy3_yPlus1 - 3*Dz3_z*z_Dz6Plus1)/Sq_r6 - 1))/(pow(Sq_r3,1.5)*pow(Sq_r6,1.5));

	  Lx[3][0] =  Lx[0][3];
	  Lx[3][1] =  Lx[1][3];
	  Lx[3][2] =  Lx[2][3];
	  Lx[3][3] =  4*Dx4_x/pow(Sq_r4,3);
	  Lx[3][4] =  (Dx4_x*((3*Dx4_x*Dx5_x - 3*Dy5_y*y_Dy4Plus1 + 3*Dz4_z*Dz5_zPlus1)/Sq_r4 - 1) + Dx5_x*((3*Dx4_x*Dx5_x - 3*Dy5_y*y_Dy4Plus1 + 3*Dz4_z*Dz5_zPlus1)/Sq_r5 - 1))/(pow(Sq_r4,1.5)*pow(Sq_r5,1.5));
	  Lx[3][5] = -(Dx4_x*((3*Dy6_y*y_Dy4Plus1 - 3*Dx4_x*Dx6_x + 3*Dz4_z*z_Dz6Plus1)/Sq_r4 + 1) + Dx6_x*((3*Dy6_y*y_Dy4Plus1 - 3*Dx4_x*Dx6_x + 3*Dz4_z*z_Dz6Plus1)/Sq_r6 + 1))/(pow(Sq_r4,1.5)*pow(Sq_r6,1.5));

	  Lx[4][0] = Lx[0][4];
	  Lx[4][1] = Lx[1][4];
	  Lx[4][2] = Lx[2][4];
	  Lx[4][3] = Lx[3][4];
	  Lx[4][4] = 4*Dx5_x/pow(Sq_r5,3);
	  Lx[4][5] = (Dx5_x*((3*Dx5_x*Dx6_x + 3*Dy5_y*Dy6_y - 3*Dz5_zPlus1*z_Dz6Plus1)/Sq_r5 - 1) + Dx6_x*((3*Dx5_x*Dx6_x + 3*Dy5_y*Dy6_y - 3*Dz5_zPlus1*z_Dz6Plus1)/Sq_r6 - 1))/(pow(Sq_r5,1.5)*pow(Sq_r6,1.5));

	  Lx[5][0] = Lx[0][5];
	  Lx[5][1] = Lx[1][5];
	  Lx[5][2] = Lx[2][5];
	  Lx[5][3] = Lx[3][5];
	  Lx[5][4] = Lx[4][5];
	  Lx[5][5] = 4*Dx6_x/pow(Sq_r6,3);
}



void CALCULATION::Calc_Ly()           //After running func: Calc_Pre_L(double Pos_um[3])
{
	  Ly[0][0] = 4*Dy1_y/pow(Sq_r1,3);
	  Ly[0][1] = (Dy1_y*((3*Dy1_y*Dy2_y - 3*Dx1_xPlus1*x_Dx2Plus1 + 3*Dz1_z*Dz2_z)/Sq_r1 - 1) + Dy2_y*((3*Dy1_y*Dy2_y - 3*Dx1_xPlus1*x_Dx2Plus1 + 3*Dz1_z*Dz2_z)/Sq_r2 - 1))/(pow(Sq_r1,1.5)*pow(Sq_r2,1.5));
      Ly[0][2] = (Dy1_y*((3*Dx3_x*Dx1_xPlus1 + 3*Dy1_y*Dy3_yPlus1 + 3*Dz1_z*Dz3_z)/Sq_r1 - 1) + ((3*Dx3_x*Dx1_xPlus1 + 3*Dy1_y*Dy3_yPlus1 + 3*Dz1_z*Dz3_z)/Sq_r3 - 1)*Dy3_yPlus1)/(pow(Sq_r1,1.5)*pow(Sq_r3,1.5));
	  Ly[0][3] = (Dy1_y*((3*Dx4_x*Dx1_xPlus1 - 3*Dy1_y*y_Dy4Plus1 + 3*Dz1_z*Dz4_z)/Sq_r1 - 1) - ((3*Dx4_x*Dx1_xPlus1 - 3*Dy1_y*y_Dy4Plus1 + 3*Dz1_z*Dz4_z)/Sq_r4 - 1)*y_Dy4Plus1)/(pow(Sq_r1,1.5)*pow(Sq_r4,1.5));
	  Ly[0][4] = (Dy1_y*((3*Dx5_x*Dx1_xPlus1 + 3*Dy1_y*Dy5_y + 3*Dz1_z*Dz5_zPlus1)/Sq_r1 - 1) + Dy5_y*((3*Dx5_x*Dx1_xPlus1 + 3*Dy1_y*Dy5_y + 3*Dz1_z*Dz5_zPlus1)/Sq_r5 - 1))/(pow(Sq_r1,1.5)*pow(Sq_r5,1.5));
	  Ly[0][5] = (Dy1_y*((3*Dx6_x*Dx1_xPlus1 + 3*Dy1_y*Dy6_y - 3*Dz1_z*z_Dz6Plus1)/Sq_r1 - 1) + Dy6_y*((3*Dx6_x*Dx1_xPlus1 + 3*Dy1_y*Dy6_y - 3*Dz1_z*z_Dz6Plus1)/Sq_r6 - 1))/(pow(Sq_r1,1.5)*pow(Sq_r6,1.5));

	  Ly[1][0] =  Ly[0][1];
	  Ly[1][1] =  4*Dy2_y/pow(Sq_r2,3);
	  Ly[1][2] =  (Dy2_y*((3*Dy2_y*Dy3_yPlus1 - 3*Dx3_x*x_Dx2Plus1 + 3*Dz2_z*Dz3_z)/Sq_r2 - 1) + ((3*Dy2_y*Dy3_yPlus1 - 3*Dx3_x*x_Dx2Plus1 + 3*Dz2_z*Dz3_z)/Sq_r3 - 1)*Dy3_yPlus1)/(pow(Sq_r2,1.5)*pow(Sq_r3,1.5));
	  Ly[1][3] = -(Dy2_y*((3*Dx4_x*x_Dx2Plus1 + 3*Dy2_y*y_Dy4Plus1 - 3*Dz2_z*Dz4_z)/Sq_r2 + 1) - ((3*Dx4_x*x_Dx2Plus1 + 3*Dy2_y*y_Dy4Plus1 - 3*Dz2_z*Dz4_z)/Sq_r4 + 1)*y_Dy4Plus1)/(pow(Sq_r2,1.5)*pow(Sq_r4,1.5));
	  Ly[1][4] =  (Dy2_y*((3*Dy2_y*Dy5_y - 3*Dx5_x*x_Dx2Plus1 + 3*Dz2_z*Dz5_zPlus1)/Sq_r2 - 1) + Dy5_y*((3*Dy2_y*Dy5_y - 3*Dx5_x*x_Dx2Plus1 + 3*Dz2_z*Dz5_zPlus1)/Sq_r5 - 1))/(pow(Sq_r2,1.5)*pow(Sq_r5,1.5));
	  Ly[1][5] = -(Dy2_y*((3*Dx6_x*x_Dx2Plus1 - 3*Dy2_y*Dy6_y + 3*Dz2_z*z_Dz6Plus1)/Sq_r2 + 1) + Dy6_y*((3*Dx6_x*x_Dx2Plus1 - 3*Dy2_y*Dy6_y + 3*Dz2_z*z_Dz6Plus1)/Sq_r6 + 1))/(pow(Sq_r2,1.5)*pow(Sq_r6,1.5));

	  Ly[2][0] = Ly[0][2];
	  Ly[2][1] = Ly[1][2];
	  Ly[2][2] = 4*Dy3_yPlus1/pow(Sq_r3,3);
	  Ly[2][3] = (((3*Dx3_x*Dx4_x - 3*Dy3_yPlus1*y_Dy4Plus1 + 3*Dz3_z*Dz4_z)/Sq_r3 - 1)*Dy3_yPlus1 - ((3*Dx3_x*Dx4_x - 3*Dy3_yPlus1*y_Dy4Plus1 + 3*Dz3_z*Dz4_z)/Sq_r4 - 1)*y_Dy4Plus1)/(pow(Sq_r3,1.5)*pow(Sq_r4,1.5));
	  Ly[2][4] = (((3*Dx3_x*Dx5_x + 3*Dy5_y*Dy3_yPlus1 + 3*Dz3_z*Dz5_zPlus1)/Sq_r3 - 1)*Dy3_yPlus1 + Dy5_y*((3*Dx3_x*Dx5_x + 3*Dy5_y*Dy3_yPlus1 + 3*Dz3_z*Dz5_zPlus1)/Sq_r5 - 1))/(pow(Sq_r3,1.5)*pow(Sq_r5,1.5));
	  Ly[2][5] = (((3*Dx3_x*Dx6_x + 3*Dy6_y*Dy3_yPlus1 - 3*Dz3_z*z_Dz6Plus1)/Sq_r3 - 1)*Dy3_yPlus1 + Dy6_y*((3*Dx3_x*Dx6_x + 3*Dy6_y*Dy3_yPlus1 - 3*Dz3_z*z_Dz6Plus1)/Sq_r6 - 1))/(pow(Sq_r3,1.5)*pow(Sq_r6,1.5));

	  Ly[3][0] =  Ly[0][3];
	  Ly[3][1] =  Ly[1][3];
	  Ly[3][2] =  Ly[2][3];
	  Ly[3][3] = -4*y_Dy4Plus1/pow(Sq_r4,3);
	  Ly[3][4] = -(((3*Dx4_x*Dx5_x - 3*Dy5_y*y_Dy4Plus1 + 3*Dz4_z*Dz5_zPlus1)/Sq_r4 - 1)*y_Dy4Plus1 - Dy5_y*((3*Dx4_x*Dx5_x - 3*Dy5_y*y_Dy4Plus1 + 3*Dz4_z*Dz5_zPlus1)/Sq_r5 - 1))/(pow(Sq_r4,1.5)*pow(Sq_r5,1.5));
	  Ly[3][5] =  (((3*Dy6_y*y_Dy4Plus1 - 3*Dx4_x*Dx6_x + 3*Dz4_z*z_Dz6Plus1)/Sq_r4 + 1)*y_Dy4Plus1 - Dy6_y*((3*Dy6_y*y_Dy4Plus1 - 3*Dx4_x*Dx6_x + 3*Dz4_z*z_Dz6Plus1)/Sq_r6 + 1))/(pow(Sq_r4,1.5)*pow(Sq_r6,1.5));

	  Ly[4][0] = Ly[0][4];
	  Ly[4][1] = Ly[1][4];
	  Ly[4][2] = Ly[2][4];
	  Ly[4][3] = Ly[3][4];
	  Ly[4][4] = 4*Dy5_y/pow(Sq_r5,3);
	  Ly[4][5] = (Dy5_y*((3*Dx5_x*Dx6_x + 3*Dy5_y*Dy6_y - 3*Dz5_zPlus1*z_Dz6Plus1)/Sq_r5 - 1) + Dy6_y*((3*Dx5_x*Dx6_x + 3*Dy5_y*Dy6_y - 3*Dz5_zPlus1*z_Dz6Plus1)/Sq_r6 - 1))/(pow(Sq_r5,1.5)*pow(Sq_r6,1.5));
	  
	  Ly[5][0] = Ly[0][5];
	  Ly[5][1] = Ly[1][5];
	  Ly[5][2] = Ly[2][5];
	  Ly[5][3] = Ly[3][5];
	  Ly[5][4] = Ly[4][5];
	  Ly[5][5] = 4*Dy6_y/pow(Sq_r6,3);
}



void CALCULATION::Calc_Lz()           //After running func: Calc_Pre_L(double Pos_um[3])
{
	  Lz[0][0] = 4*Dz1_z/pow(Sq_r1,3);
	  Lz[0][1] = (Dz1_z*((3*Dy1_y*Dy2_y - 3*Dx1_xPlus1*x_Dx2Plus1 + 3*Dz1_z*Dz2_z)/Sq_r1 - 1) + Dz2_z*((3*Dy1_y*Dy2_y - 3*Dx1_xPlus1*x_Dx2Plus1 + 3*Dz1_z*Dz2_z)/Sq_r2 - 1))/(pow(Sq_r1,1.5)*pow(Sq_r2,1.5));
      Lz[0][2] = (Dz1_z*((3*Dx3_x*Dx1_xPlus1 + 3*Dy1_y*Dy3_yPlus1 + 3*Dz1_z*Dz3_z)/Sq_r1 - 1) + Dz3_z*((3*Dx3_x*Dx1_xPlus1 + 3*Dy1_y*Dy3_yPlus1 + 3*Dz1_z*Dz3_z)/Sq_r3 - 1))/(pow(Sq_r1,1.5)*pow(Sq_r3,1.5));
	  Lz[0][3] = (Dz1_z*((3*Dx4_x*Dx1_xPlus1 - 3*Dy1_y*y_Dy4Plus1 + 3*Dz1_z*Dz4_z)/Sq_r1 - 1) + Dz4_z*((3*Dx4_x*Dx1_xPlus1 - 3*Dy1_y*y_Dy4Plus1 + 3*Dz1_z*Dz4_z)/Sq_r4 - 1))/(pow(Sq_r1,1.5)*pow(Sq_r4,1.5));
	  Lz[0][4] = (Dz1_z*((3*Dx5_x*Dx1_xPlus1 + 3*Dy1_y*Dy5_y + 3*Dz1_z*Dz5_zPlus1)/Sq_r1 - 1) + ((3*Dx5_x*Dx1_xPlus1 + 3*Dy1_y*Dy5_y + 3*Dz1_z*Dz5_zPlus1)/Sq_r5 - 1)*Dz5_zPlus1)/(pow(Sq_r1,1.5)*pow(Sq_r5,1.5));
	  Lz[0][5] = (Dz1_z*((3*Dx6_x*Dx1_xPlus1 + 3*Dy1_y*Dy6_y - 3*Dz1_z*z_Dz6Plus1)/Sq_r1 - 1) - ((3*Dx6_x*Dx1_xPlus1 + 3*Dy1_y*Dy6_y - 3*Dz1_z*z_Dz6Plus1)/Sq_r6 - 1)*z_Dz6Plus1)/(pow(Sq_r1,1.5)*pow(Sq_r6,1.5));

	  Lz[1][0] =  Lz[0][1];
	  Lz[1][1] =  4*Dz2_z/pow(Sq_r2,3);
	  Lz[1][2] =  (Dz2_z*((3*Dy2_y*Dy3_yPlus1 - 3*Dx3_x*x_Dx2Plus1 + 3*Dz2_z*Dz3_z)/Sq_r2 - 1) + Dz3_z*((3*Dy2_y*Dy3_yPlus1 - 3*Dx3_x*x_Dx2Plus1 + 3*Dz2_z*Dz3_z)/Sq_r3 - 1))/(pow(Sq_r2,1.5)*pow(Sq_r3,1.5));
	  Lz[1][3] = -(Dz2_z*((3*Dx4_x*x_Dx2Plus1 + 3*Dy2_y*y_Dy4Plus1 - 3*Dz2_z*Dz4_z)/Sq_r2 + 1) + Dz4_z*((3*Dx4_x*x_Dx2Plus1 + 3*Dy2_y*y_Dy4Plus1 - 3*Dz2_z*Dz4_z)/Sq_r4 + 1))/(pow(Sq_r2,1.5)*pow(Sq_r4,1.5));
	  Lz[1][4] =  (Dz2_z*((3*Dy2_y*Dy5_y - 3*Dx5_x*x_Dx2Plus1 + 3*Dz2_z*Dz5_zPlus1)/Sq_r2 - 1) + ((3*Dy2_y*Dy5_y - 3*Dx5_x*x_Dx2Plus1 + 3*Dz2_z*Dz5_zPlus1)/Sq_r5 - 1)*Dz5_zPlus1)/(pow(Sq_r2,1.5)*pow(Sq_r5,1.5));
	  Lz[1][5] = -(Dz2_z*((3*Dx6_x*x_Dx2Plus1 - 3*Dy2_y*Dy6_y + 3*Dz2_z*z_Dz6Plus1)/Sq_r2 + 1) - ((3*Dx6_x*x_Dx2Plus1 - 3*Dy2_y*Dy6_y + 3*Dz2_z*z_Dz6Plus1)/Sq_r6 + 1)*z_Dz6Plus1)/(pow(Sq_r2,1.5)*pow(Sq_r6,1.5));

	  Lz[2][0] = Lz[0][2];
	  Lz[2][1] = Lz[1][2];
	  Lz[2][2] = 4*Dz3_z/pow(Sq_r3,3);
	  Lz[2][3] = (Dz3_z*((3*Dx3_x*Dx4_x - 3*Dy3_yPlus1*y_Dy4Plus1 + 3*Dz3_z*Dz4_z)/Sq_r3 - 1) + Dz4_z*((3*Dx3_x*Dx4_x - 3*Dy3_yPlus1*y_Dy4Plus1 + 3*Dz3_z*Dz4_z)/Sq_r4 - 1))/(pow(Sq_r3,1.5)*pow(Sq_r4,1.5));
	  Lz[2][4] = (Dz3_z*((3*Dx3_x*Dx5_x + 3*Dy5_y*Dy3_yPlus1 + 3*Dz3_z*Dz5_zPlus1)/Sq_r3 - 1) + ((3*Dx3_x*Dx5_x + 3*Dy5_y*Dy3_yPlus1 + 3*Dz3_z*Dz5_zPlus1)/Sq_r5 - 1)*Dz5_zPlus1)/(pow(Sq_r3,1.5)*pow(Sq_r5,1.5));
	  Lz[2][5] = (Dz3_z*((3*Dx3_x*Dx6_x + 3*Dy6_y*Dy3_yPlus1 - 3*Dz3_z*z_Dz6Plus1)/Sq_r3 - 1) - ((3*Dx3_x*Dx6_x + 3*Dy6_y*Dy3_yPlus1 - 3*Dz3_z*z_Dz6Plus1)/Sq_r6 - 1)*z_Dz6Plus1)/(pow(Sq_r3,1.5)*pow(Sq_r6,1.5));

	  Lz[3][0] =  Lz[0][3];
	  Lz[3][1] =  Lz[1][3];
	  Lz[3][2] =  Lz[2][3];
	  Lz[3][3] =  4*Dz4_z/pow(Sq_r4,3);
	  Lz[3][4] =  (Dz4_z*((3*Dx4_x*Dx5_x - 3*Dy5_y*y_Dy4Plus1 + 3*Dz4_z*Dz5_zPlus1)/Sq_r4 - 1) + ((3*Dx4_x*Dx5_x - 3*Dy5_y*y_Dy4Plus1 + 3*Dz4_z*Dz5_zPlus1)/Sq_r5 - 1)*Dz5_zPlus1)/(pow(Sq_r4,1.5)*pow(Sq_r5,1.5));
	  Lz[3][5] = -(Dz4_z*((3*Dy6_y*y_Dy4Plus1 - 3*Dx4_x*Dx6_x + 3*Dz4_z*z_Dz6Plus1)/Sq_r4 + 1) - ((3*Dy6_y*y_Dy4Plus1 - 3*Dx4_x*Dx6_x + 3*Dz4_z*z_Dz6Plus1)/Sq_r6 + 1)*z_Dz6Plus1)/(pow(Sq_r4,1.5)*pow(Sq_r6,1.5));

	  Lz[4][0] = Lz[0][4];
	  Lz[4][1] = Lz[1][4];
	  Lz[4][2] = Lz[2][4];
	  Lz[4][3] = Lz[3][4];
	  Lz[4][4] = 4*Dz5_zPlus1/pow(Sq_r5,3);
	  Lz[4][5] = (((3*Dx5_x*Dx6_x + 3*Dy5_y*Dy6_y - 3*Dz5_zPlus1*z_Dz6Plus1)/Sq_r5 - 1)*Dz5_zPlus1 - ((3*Dx5_x*Dx6_x + 3*Dy5_y*Dy6_y - 3*Dz5_zPlus1*z_Dz6Plus1)/Sq_r6 - 1)*z_Dz6Plus1)/(pow(Sq_r5,1.5)*pow(Sq_r6,1.5));
	  
	  Lz[5][0] =  Lz[0][5];
	  Lz[5][1] =  Lz[1][5];
	  Lz[5][2] =  Lz[2][5];
	  Lz[5][3] =  Lz[3][5];
	  Lz[5][4] =  Lz[4][5];
	  Lz[5][5] = -4*z_Dz6Plus1/pow(Sq_r6,3);
}


void CALCULATION::Calc_Lxyz(double Pos_umOa[3])
{
	Calc_Pre_L(Pos_umOa);  //for CALC Lx,y,z  //****//(Bias is introduced)
	Calc_Lx(); //****//(Bias is introduced)
	Calc_Ly(); //****//(Bias is introduced)
	Calc_Lz(); //****//(Bias is introduced)
}


void CALCULATION::Calc_Nxyz_KItheo(double Pos_umOa[3])
{
	Calc_Pre_L(Pos_umOa);  //for CALC Lx,y,z  //****//(Bias is introduced)

	Calc_Lx(); //****//(Bias is introduced)
    CalcMat_6x6_6x6_6x6(KItheo_T, Lx, KItheo,  Nx_KItheo);

	Calc_Ly(); //****//(Bias is introduced)
	CalcMat_6x6_6x6_6x6(KItheo_T, Ly, KItheo,  Ny_KItheo);
	
	Calc_Lz(); //****//(Bias is introduced)
    CalcMat_6x6_6x6_6x6(KItheo_T, Lz, KItheo,  Nz_KItheo);
}



void CALCULATION::Calc_Nxyz_KIreal(double Pos_umOa[3])
{
	Calc_Pre_L(Pos_umOa);   //for CALC Lx,y,z

	Calc_Lx(); 
    CalcMat_6x6_6x6_6x6(KIreal_T, Lx, KIreal,  Nx_KIreal);

	Calc_Ly(); 
	CalcMat_6x6_6x6_6x6(KIreal_T, Ly, KIreal,  Ny_KIreal);

	Calc_Lz();
    CalcMat_6x6_6x6_6x6(KIreal_T, Lz, KIreal,  Nz_KIreal);
}


void CALCULATION::Calc_FNor_FromSixCurr_KItheo(double Pos_umOa[3], double Inor_P1toP6[6])   //Pos is um, result F is normalized
{
	Calc_Nxyz_KItheo(Pos_umOa); //Obtained: Nx_KItheo & Ny_KItheo & Nz_KItheo
	
    FNor_Curr_KItheo[0] = CalcMat_1x6_6x6_6x1(Inor_P1toP6, Nx_KItheo, Inor_P1toP6);
    FNor_Curr_KItheo[1] = CalcMat_1x6_6x6_6x1(Inor_P1toP6, Ny_KItheo, Inor_P1toP6);
    FNor_Curr_KItheo[2] = CalcMat_1x6_6x6_6x1(Inor_P1toP6, Nz_KItheo, Inor_P1toP6);   //normalized (*ki_head becomes pN)
}


void CALCULATION::Calc_FNor_FromSixCurr_KIreal(double Pos_umOa[3], double Inor_P1toP6[6])
{
	Calc_Nxyz_KIreal(Pos_umOa); //Obtained: Nx_KIreal & Ny_KIreal & Nz_KIreal
	
    FNor_Curr_KIreal[0] = CalcMat_1x6_6x6_6x1(Inor_P1toP6, Nx_KIreal, Inor_P1toP6);
    FNor_Curr_KIreal[1] = CalcMat_1x6_6x6_6x1(Inor_P1toP6, Ny_KIreal, Inor_P1toP6);
    FNor_Curr_KIreal[2] = CalcMat_1x6_6x6_6x1(Inor_P1toP6, Nz_KIreal, Inor_P1toP6);   //normalized (*ki_head becomes pN)
}


void CALCULATION::Calc_NextBeadPos (double PosBef_umOa[3], double PosAft_umOa[3], int THEO_or_REAL)  //PosBef,PosAft: in um
{
	//****** This is the Real Force Applied on the bead: ******
	if (Index<16120)
	{
		F_Ext_x_pN_Apply[Index] = 0.0;
		F_Ext_y_pN_Apply[Index] = 0.0;
		F_Ext_z_pN_Apply[Index] = 0.0;
	}
	else
	{
		F_Ext_x_pN_Apply[Index] = 0.0;
		F_Ext_y_pN_Apply[Index] = 0.0; //1.0;
		F_Ext_z_pN_Apply[Index] = 0.0;
	}
	//****** (end) This is the Real Force Applied on the bead: ******


	PosAft_umOa[X] = PosBef_umOa[X];
	PosAft_umOa[Y] = PosBef_umOa[Y];
	PosAft_umOa[Z] = PosBef_umOa[Z];

	Px_umOa_k1 = PosBef_umOa[X];
	Py_umOa_k1 = PosBef_umOa[Y];
	Pz_umOa_k1 = PosBef_umOa[Z];//****//

	for (int i=0; i<Simu_StepNum; i++)
	{
		if (THEO_or_REAL == THEO)
		{
			// ******* KItheo: ************
			Calc_FNor_FromSixCurr_KItheo(PosAft_umOa, I1toI6_KItheo);    //calculate real force; Input: pos & current; Output:F(norlize)
			
			Fx_pNOa_k = FNor_Curr_KItheo[X]*FGain_x + ThmForce_X_NOa_RD[ThmF_Counter]*N2pN + F_Ext_x_pN_Apply[Index];
			Fy_pNOa_k = FNor_Curr_KItheo[Y]*FGain_y + ThmForce_Y_NOa_RD[ThmF_Counter]*N2pN + F_Ext_y_pN_Apply[Index];
			Fz_pNOa_k = FNor_Curr_KItheo[Z]*FGain_z + ThmForce_Z_NOa_RD[ThmF_Counter]*N2pN + F_Ext_z_pN_Apply[Index]; //MT force + thermal force = total force

			Px_umOa_k = Px_umOa_k1 + ( (SimuIntv/DampWtr_X_Oa_RD[ThmF_Counter])*(Fx_pNOa_k)*pN2N )*m2um;
			Py_umOa_k = Py_umOa_k1 + ( (SimuIntv/DampWtr_Y_Oa_RD[ThmF_Counter])*(Fy_pNOa_k)*pN2N )*m2um;
			Pz_umOa_k = Pz_umOa_k1 + ( (SimuIntv/DampWtr_Z_Oa_RD[ThmF_Counter])*(Fz_pNOa_k)*pN2N )*m2um;

			PosAft_umOa[X] = Px_umOa_k; //PosBef[]: um
			PosAft_umOa[Y] = Py_umOa_k;    
			PosAft_umOa[Z] = Pz_umOa_k;

			//****** history update: ******
			Px_umOa_k1 = Px_umOa_k;
			Py_umOa_k1 = Py_umOa_k;
			Pz_umOa_k1 = Pz_umOa_k;
		}
		else
		{
			// ******* KIreal: ************
			Calc_FNor_FromSixCurr_KIreal(PosAft_umOa, I1toI6_KIreal);    //calculate real force; Input: pos & current; Output:F(norlize)

			Fx_pNOa_k = FNor_Curr_KIreal[X]*FGain_x + ThmForce_X_NOa_RD[ThmF_Counter]*N2pN + F_Ext_x_pN_Apply[Index];
			Fy_pNOa_k = FNor_Curr_KIreal[Y]*FGain_y + ThmForce_Y_NOa_RD[ThmF_Counter]*N2pN + F_Ext_x_pN_Apply[Index];
			Fz_pNOa_k = FNor_Curr_KIreal[Z]*FGain_z + ThmForce_Z_NOa_RD[ThmF_Counter]*N2pN + F_Ext_x_pN_Apply[Index];

			Px_umOa_k = Px_umOa_k1 + ( (SimuIntv/DampWtr_X_Oa_RD[ThmF_Counter])*(Fx_pNOa_k)*pN2N )*m2um;
			Py_umOa_k = Py_umOa_k1 + ( (SimuIntv/DampWtr_Y_Oa_RD[ThmF_Counter])*(Fy_pNOa_k)*pN2N )*m2um;
			Pz_umOa_k = Pz_umOa_k1 + ( (SimuIntv/DampWtr_Z_Oa_RD[ThmF_Counter])*(Fz_pNOa_k)*pN2N )*m2um;

			PosAft_umOa[X] = Px_umOa_k; //PosBef[]: um
			PosAft_umOa[Y] = Py_umOa_k;    
			PosAft_umOa[Z] = Pz_umOa_k;

			//****** history update: ******
			Px_umOa_k1 = Px_umOa_k;
			Py_umOa_k1 = Py_umOa_k;
			Pz_umOa_k1 = Pz_umOa_k;
		}

		ThmF_Counter++;
	}
}


double CALCULATION::CalcCurrNorm (double I1toI6[6])
{
    return pow( pow(I1toI6[0],2.0) + 
		        pow(I1toI6[1],2.0) + 
				pow(I1toI6[2],2.0) + 
		        pow(I1toI6[3],2.0) + 
				pow(I1toI6[4],2.0) + 
				pow(I1toI6[5],2.0) ,0.5);
}


double CALCULATION::CalcForceNorm(double Fd_pN[3])
{
	return sqrt( pow(Fd_pN[0],2.0) + 
		         pow(Fd_pN[1],2.0) + 
				 pow(Fd_pN[2],2.0) );
}



void CALCULATION::OptInvMdl_Curr_FromLS_KItheo(double Fd_pN[3],double Pos_um[3])
{
	Fd_pN[0] *= 10.0/FGain_x;
	Fd_pN[1] *= 10.0/FGain_y;
	Fd_pN[2] *= 10.0/FGain_z;  //Modify according to FGain
    
	Fdsir_Mag_pN = CalcForceNorm(Fd_pN);

	AngleThe = atan2( Fd_pN[1],Fd_pN[0] );
	AnglePhi = asin(  Fd_pN[2]/Fdsir_Mag_pN );   //Nominal Orientation

	if (Pos_um[0]>=0 && Pos_um[1]>=0 && Pos_um[2]>=0)  //Octant1
	{
	   TheOct1 = AngleThe;
	   PhiOct1 = AnglePhi;
	}
	else if (Pos_um[0]<0 && Pos_um[1]>=0 && Pos_um[2]>=0)   //Octant2
	{
		if (AngleThe>=0)
		{ 
			TheOct1 = PI - AngleThe;
		}
		else
		{
		    TheOct1 = -PI - AngleThe;
		}
		PhiOct1 = AnglePhi;
	}
	else if (Pos_um[0]<=0 && Pos_um[1]<0 && Pos_um[2]>=0)   //Octant3
	{
		if (AngleThe>=0)
		{
			TheOct1 = -PI + AngleThe;
		}
		else
		{
			TheOct1 = PI + AngleThe;
		}
		PhiOct1 = AnglePhi;
	}
    else if (Pos_um[0]>0 && Pos_um[1]<0 && Pos_um[2]>=0)    //Octant4
	{
	    TheOct1 = -AngleThe;
		PhiOct1 = AnglePhi;
	}
    else if (Pos_um[0]>=0 && Pos_um[1]>=0 && Pos_um[2]<0)  //Octant5
	{
	    TheOct1 = AngleThe;
		PhiOct1 = -AnglePhi;
	}
	else if (Pos_um[0]<0 && Pos_um[1]>=0 && Pos_um[2]<0)   //Octant6
	{
		if (AngleThe>=0)
		{
			TheOct1 = PI - AngleThe;
		}
		else
		{
			TheOct1 = -PI - AngleThe;
		}
		PhiOct1 = - AnglePhi;
	}
	else if (Pos_um[0]<=0 && Pos_um[1]<0 && Pos_um[2]<0)  //Octant7
	{
		if (AngleThe>=0)
		{
			TheOct1 = -PI + AngleThe;
		}
		else
		{
			TheOct1 = PI + AngleThe;
		}
		PhiOct1 = - AnglePhi;
	}
	else //(Pos_um[0]>0 && Pos_um[1]<0 && Pos_um[2]<0)   //Octant8
	{
	    TheOct1 = -AngleThe;
		PhiOct1 = -AnglePhi;
	}

	IntThe = floor( (TheOct1 + PI)/CalcAngStep );
	IntPhi = floor( (PhiOct1 + PI/2)/CalcAngStep );
	FracThe = (TheOct1 + PI)/CalcAngStep - IntThe;
	FracPhi = (PhiOct1 + PI/2)/CalcAngStep - IntPhi;

	ind1 = IntThe*31 + IntPhi;
	ind2 = ind1 + 1;
	ind3 = ind1 + 31;
	ind4 = ind3 + 1;

	I1_ind1 = 0.0; I1_ind2 = 0.0; I1_ind3 = 0.0; I1_ind4 = 0.0;
	I2_ind1 = 0.0; I2_ind2 = 0.0; I2_ind3 = 0.0; I2_ind4 = 0.0;
	I3_ind1 = 0.0; I3_ind2 = 0.0; I3_ind3 = 0.0; I3_ind4 = 0.0;
	I4_ind1 = 0.0; I4_ind2 = 0.0; I4_ind3 = 0.0; I4_ind4 = 0.0;
	I5_ind1 = 0.0; I5_ind2 = 0.0; I5_ind3 = 0.0; I5_ind4 = 0.0;
	I6_ind1 = 0.0; I6_ind2 = 0.0; I6_ind3 = 0.0; I6_ind4 = 0.0;

	PosCoeff[0] = 1;
	PosCoeff[1] = abs(Pos_um[0])/R_norm;
	PosCoeff[2] = abs(Pos_um[1])/R_norm;
	PosCoeff[3] = abs(Pos_um[2])/R_norm;
	PosCoeff[4] = pow(PosCoeff[1],2.0);
	PosCoeff[5] = pow(PosCoeff[2],2.0); 
	PosCoeff[6] = pow(PosCoeff[3],2.0);
	PosCoeff[7] = PosCoeff[1]*PosCoeff[2]; 
	PosCoeff[8] = PosCoeff[1]*PosCoeff[3]; 
	PosCoeff[9] = PosCoeff[2]*PosCoeff[3];

	if ( ind1 < OptInvMdl_RowNum )
	{
		for (int i=0; i<OptInvMdl_ColNum; i++)
		{
			I1_ind1 += PosCoeff[i]*InvMdl_Coeff_I1[ind1][i];
			I2_ind1 += PosCoeff[i]*InvMdl_Coeff_I2[ind1][i];
			I3_ind1 += PosCoeff[i]*InvMdl_Coeff_I3[ind1][i];
			I4_ind1 += PosCoeff[i]*InvMdl_Coeff_I4[ind1][i];
			I5_ind1 += PosCoeff[i]*InvMdl_Coeff_I5[ind1][i];
			I6_ind1 += PosCoeff[i]*InvMdl_Coeff_I6[ind1][i];
		}
	}

	if ( ind2 < OptInvMdl_RowNum)
	{
		for (int i=0; i<OptInvMdl_ColNum; i++)
		{
			I1_ind2 += PosCoeff[i]*InvMdl_Coeff_I1[ind2][i];
			I2_ind2 += PosCoeff[i]*InvMdl_Coeff_I2[ind2][i];
			I3_ind2 += PosCoeff[i]*InvMdl_Coeff_I3[ind2][i];
			I4_ind2 += PosCoeff[i]*InvMdl_Coeff_I4[ind2][i];
			I5_ind2 += PosCoeff[i]*InvMdl_Coeff_I5[ind2][i];
			I6_ind2 += PosCoeff[i]*InvMdl_Coeff_I6[ind2][i];
		}
	}

	if ( ind3 < OptInvMdl_RowNum)
	{
		for (int i=0; i<OptInvMdl_ColNum; i++)
		{
			I1_ind3 += PosCoeff[i]*InvMdl_Coeff_I1[ind3][i];
			I2_ind3 += PosCoeff[i]*InvMdl_Coeff_I2[ind3][i];
			I3_ind3 += PosCoeff[i]*InvMdl_Coeff_I3[ind3][i];
			I4_ind3 += PosCoeff[i]*InvMdl_Coeff_I4[ind3][i];
			I5_ind3 += PosCoeff[i]*InvMdl_Coeff_I5[ind3][i];
			I6_ind3 += PosCoeff[i]*InvMdl_Coeff_I6[ind3][i];
		}
	}

	if ( ind4 < OptInvMdl_RowNum)
	{
		for (int i=0; i<OptInvMdl_ColNum; i++)
		{
			I1_ind4 += PosCoeff[i]*InvMdl_Coeff_I1[ind4][i];
			I2_ind4 += PosCoeff[i]*InvMdl_Coeff_I2[ind4][i];
			I3_ind4 += PosCoeff[i]*InvMdl_Coeff_I3[ind4][i];
			I4_ind4 += PosCoeff[i]*InvMdl_Coeff_I4[ind4][i];
			I5_ind4 += PosCoeff[i]*InvMdl_Coeff_I5[ind4][i];
			I6_ind4 += PosCoeff[i]*InvMdl_Coeff_I6[ind4][i];
		}
	}


	if (FracThe != 0.0 && FracPhi != 0.0)
	{
		I1_mid13 = I1_ind1 + FracThe*(I1_ind3 - I1_ind1);
		I1_mid24 = I1_ind2 + FracThe*(I1_ind4 - I1_ind2);
		I1_interp= ( I1_mid13 + FracPhi*(I1_mid24 - I1_mid13) )*sqrt(Fdsir_Mag_pN);

		I2_mid13 = I2_ind1 + FracThe*(I2_ind3 - I2_ind1);
		I2_mid24 = I2_ind2 + FracThe*(I2_ind4 - I2_ind2);
		I2_interp= ( I2_mid13 + FracPhi*(I2_mid24 - I2_mid13) )*sqrt(Fdsir_Mag_pN);

		I3_mid13 = I3_ind1 + FracThe*(I3_ind3 - I3_ind1);
		I3_mid24 = I3_ind2 + FracThe*(I3_ind4 - I3_ind2);
		I3_interp =( I3_mid13 + FracPhi*(I3_mid24 - I3_mid13) )*sqrt(Fdsir_Mag_pN);

		I4_mid13 = I4_ind1 + FracThe*(I4_ind3 - I4_ind1);
		I4_mid24 = I4_ind2 + FracThe*(I4_ind4 - I4_ind2);
		I4_interp= ( I4_mid13 + FracPhi*(I4_mid24 - I4_mid13) )*sqrt(Fdsir_Mag_pN);

		I5_mid13 = I5_ind1 + FracThe*(I5_ind3 - I5_ind1);
		I5_mid24 = I5_ind2 + FracThe*(I5_ind4 - I5_ind2);
		I5_interp= ( I5_mid13 + FracPhi*(I5_mid24 - I5_mid13) )*sqrt(Fdsir_Mag_pN);

		I6_mid13 = I6_ind1 + FracThe*(I6_ind3 - I6_ind1);
		I6_mid24 = I6_ind2 + FracThe*(I6_ind4 - I6_ind2);
		I6_interp= ( I6_mid13 + FracPhi*(I6_mid24 - I6_mid13) )*sqrt(Fdsir_Mag_pN);
	}

	if (FracThe == 0.0 && FracPhi != 0.0)
	{
		I1_interp = ( I1_ind1 + FracPhi*(I1_ind2 - I1_ind1) )*sqrt(Fdsir_Mag_pN);
		I2_interp = ( I2_ind1 + FracPhi*(I2_ind2 - I2_ind1) )*sqrt(Fdsir_Mag_pN);
		I3_interp = ( I3_ind1 + FracPhi*(I3_ind2 - I3_ind1) )*sqrt(Fdsir_Mag_pN);
		I4_interp = ( I4_ind1 + FracPhi*(I4_ind2 - I4_ind1) )*sqrt(Fdsir_Mag_pN);
		I5_interp = ( I5_ind1 + FracPhi*(I5_ind2 - I5_ind1) )*sqrt(Fdsir_Mag_pN);
		I6_interp = ( I6_ind1 + FracPhi*(I6_ind2 - I6_ind1) )*sqrt(Fdsir_Mag_pN);
	}

	if (FracThe != 0.0 && FracPhi == 0.0)
	{
		I1_interp = (I1_ind1 + FracThe*(I1_ind3 - I1_ind1))*sqrt(Fdsir_Mag_pN);
		I2_interp = (I2_ind1 + FracThe*(I2_ind3 - I2_ind1))*sqrt(Fdsir_Mag_pN);
		I3_interp = (I3_ind1 + FracThe*(I3_ind3 - I3_ind1))*sqrt(Fdsir_Mag_pN);
		I4_interp = (I4_ind1 + FracThe*(I4_ind3 - I4_ind1))*sqrt(Fdsir_Mag_pN);
		I5_interp = (I5_ind1 + FracThe*(I5_ind3 - I5_ind1))*sqrt(Fdsir_Mag_pN);
		I6_interp = (I6_ind1 + FracThe*(I6_ind3 - I6_ind1))*sqrt(Fdsir_Mag_pN);
	}

	if (FracThe == 0.0 && FracPhi == 0.0 )
	{
		I1_interp = I1_ind1*sqrt(Fdsir_Mag_pN);
		I2_interp = I2_ind1*sqrt(Fdsir_Mag_pN);
		I3_interp = I3_ind1*sqrt(Fdsir_Mag_pN);
		I4_interp = I4_ind1*sqrt(Fdsir_Mag_pN);
		I5_interp = I5_ind1*sqrt(Fdsir_Mag_pN);
		I6_interp = I6_ind1*sqrt(Fdsir_Mag_pN);
	}


	if (Pos_um[0]>=0 && Pos_um[1]>=0 && Pos_um[2]>=0)  //Octant1
	{
       I1toI6_KItheo[0] = I1_interp;
	   I1toI6_KItheo[1] = I2_interp;
	   I1toI6_KItheo[2] = I3_interp;
	   I1toI6_KItheo[3] = I4_interp;
	   I1toI6_KItheo[4] = I5_interp;
	   I1toI6_KItheo[5] = I6_interp;
	}
	if (Pos_um[0]<0 && Pos_um[1]>=0 && Pos_um[2]>=0)   //Octant2
	{
       I1toI6_KItheo[0] = I2_interp;
	   I1toI6_KItheo[1] = I1_interp;
	   I1toI6_KItheo[2] = I3_interp;
	   I1toI6_KItheo[3] = I4_interp;
	   I1toI6_KItheo[4] = I5_interp;
	   I1toI6_KItheo[5] = I6_interp;
	}
	if (Pos_um[0]<=0 && Pos_um[1]<0 && Pos_um[2]>=0)   //Octant3
	{
       I1toI6_KItheo[0] = I2_interp;
	   I1toI6_KItheo[1] = I1_interp;
	   I1toI6_KItheo[2] = I4_interp;
	   I1toI6_KItheo[3] = I3_interp;
	   I1toI6_KItheo[4] = I5_interp;
	   I1toI6_KItheo[5] = I6_interp;
	}
    if (Pos_um[0]>0 && Pos_um[1]<0 && Pos_um[2]>=0)    //Octant4
	{
       I1toI6_KItheo[0] = I1_interp;
	   I1toI6_KItheo[1] = I2_interp;
	   I1toI6_KItheo[2] = I4_interp;
	   I1toI6_KItheo[3] = I3_interp;
	   I1toI6_KItheo[4] = I5_interp;
	   I1toI6_KItheo[5] = I6_interp;
	}
    if (Pos_um[0]>=0 && Pos_um[1]>=0 && Pos_um[2]<0)  //Octant5
	{
       I1toI6_KItheo[0] = I1_interp;
	   I1toI6_KItheo[1] = I2_interp;
	   I1toI6_KItheo[2] = I3_interp;
	   I1toI6_KItheo[3] = I4_interp;
	   I1toI6_KItheo[4] = I6_interp;
	   I1toI6_KItheo[5] = I5_interp;
	}
	if (Pos_um[0]<0 && Pos_um[1]>=0 && Pos_um[2]<0)   //Octant6
	{
       I1toI6_KItheo[0] = I2_interp;
	   I1toI6_KItheo[1] = I1_interp;
	   I1toI6_KItheo[2] = I3_interp;
	   I1toI6_KItheo[3] = I4_interp;
	   I1toI6_KItheo[4] = I6_interp;
	   I1toI6_KItheo[5] = I5_interp;
	}
	if (Pos_um[0]<=0 && Pos_um[1]<0 && Pos_um[2]<0)  //Octant7
	{
       I1toI6_KItheo[0] = I2_interp;
	   I1toI6_KItheo[1] = I1_interp;
	   I1toI6_KItheo[2] = I4_interp;
	   I1toI6_KItheo[3] = I3_interp;
	   I1toI6_KItheo[4] = I6_interp;
	   I1toI6_KItheo[5] = I5_interp;
	}
	if (Pos_um[0]>0 && Pos_um[1]<0 && Pos_um[2]<0)   //Octant8
	{
       I1toI6_KItheo[0] = I1_interp;
	   I1toI6_KItheo[1] = I2_interp;
	   I1toI6_KItheo[2] = I4_interp;
	   I1toI6_KItheo[3] = I3_interp;
	   I1toI6_KItheo[4] = I6_interp;
	   I1toI6_KItheo[5] = I5_interp;
	}
}


void CALCULATION::OptInvMdl_Curr_FromLS_KIreal(double Fd_pN[3],double Pos_um[3])
{
	OptInvMdl_Curr_FromLS_KItheo( Fd_pN, Pos_um ); //Obtained: I1toI6_KItheo
	double KItheo_Multi_I1toI6_KItheo[6];
	CalcMat_6x6_6x1( KItheo, I1toI6_KItheo, KItheo_Multi_I1toI6_KItheo );  
	CalcMat_6x6_6x1( InvKIreal, KItheo_Multi_I1toI6_KItheo, I1toI6_KIreal );  //satisfy the force
}


double CALCULATION::MaxAbs_I1toI6( double I1toI6[6] )
{
    double max = fabs(I1toI6[0]);
	for (int i=1; i<6; i++)
	{
	    if ( fabs(I1toI6[i])> max )
		{
		    max = fabs(I1toI6[i]);
		}
	}
	return max;
}


void CALCULATION::Calc_Ctrl_Coeff_Force()
{
	Coeff_ErrPos_pNOm[X]  = (Igain[X]*pow(SplIntv,2.0) + 4*Dgain[X] + 2*Pgain[X]*SplIntv)/(2*SplIntv);  //pow(x,y) = x^y SplIntv = sampling inverval
	Coeff_ErrPos1_pNOm[X] = (2*Igain[X]*pow(SplIntv,2.0) - 8*Dgain[X])/(2*SplIntv);
	Coeff_ErrPos2_pNOm[X] = (Igain[X]*pow(SplIntv,2.0) + 4*Dgain[X] - 2*Pgain[X]*SplIntv)/(2*SplIntv);

	Coeff_ErrPos_pNOm[Y]  = (Igain[Y]*pow(SplIntv,2.0) + 4*Dgain[Y] + 2*Pgain[Y]*SplIntv)/(2*SplIntv);
	Coeff_ErrPos1_pNOm[Y] = (2*Igain[Y]*pow(SplIntv,2.0) - 8*Dgain[Y])/(2*SplIntv);
	Coeff_ErrPos2_pNOm[Y] = (Igain[Y]*pow(SplIntv,2.0) + 4*Dgain[Y] - 2*Pgain[Y]*SplIntv)/(2*SplIntv);

	Coeff_ErrPos_pNOm[Z]  = (Igain[Z]*pow(SplIntv,2.0) + 4*Dgain[Z] + 2*Pgain[Z]*SplIntv)/(2*SplIntv);
	Coeff_ErrPos1_pNOm[Z] = (2*Igain[Z]*pow(SplIntv,2.0) - 8*Dgain[Z])/(2*SplIntv);
	Coeff_ErrPos2_pNOm[Z] = (Igain[Z]*pow(SplIntv,2.0) + 4*Dgain[Z] - 2*Pgain[Z]*SplIntv)/(2*SplIntv);

	ERRpos_umOm[X]  = 0; ERRpos_umOm[Y]  = 0; ERRpos_umOm[Z]  = 0;
	ERRpos1_umOm[X] = 0; ERRpos1_umOm[Y] = 0; ERRpos1_umOm[Z] = 0;
	ERRpos2_umOm[X] = 0; ERRpos2_umOm[Y] = 0; ERRpos2_umOm[Z] = 0;

	Fd_pNOm[X]  = 0; Fd_pNOm[Y]  = 0; Fd_pNOm[Z] = 0;
	Fd1_pNOm[X] = 0; Fd1_pNOm[Y] = 0; Fd1_pNOm[Z] = 0;
	Fd2_pNOm[X] = 0; Fd2_pNOm[Y] = 0; Fd2_pNOm[Z] = 0;

	Fd_pNOa[X]  = 0; Fd_pNOa[Y]  = 0; Fd_pNOa[Z]  = 0;
	Fd1_pNOa[X] = 0; Fd1_pNOa[Y] = 0; Fd1_pNOa[Z] = 0;
	Fd2_pNOa[X] = 0; Fd2_pNOa[Y] = 0; Fd2_pNOa[Z] = 0;

	Vsuptwist_pNOm[X]  = 0.0; Vsuptwist_pNOm[Y]  = 0.0; Vsuptwist_pNOm[Z]  = 0.0;
	V1suptwist_pNOm[X] = 0.0; V1suptwist_pNOm[Y] = 0.0; V1suptwist_pNOm[Z] = 0.0;

	PosCtrl_Counter = 0;
}


void CALCULATION::Calc_Ctrl_Effort_Force(int MODE)
{
    if (MODE == P_ctrl) //P control
	{
		Fd_pNOm[X] = Pgain[X]*ERRpos_umOm[X]; //pN Fx
		Fd_pNOm[Y] = Pgain[Y]*ERRpos_umOm[Y]; //pN Fy
		Fd_pNOm[Z] = Pgain[Z]*ERRpos_umOm[Z]; //pN Fz	
	}


	if (MODE == PID_ctrl)
	{
		Fd_pNOm[X] = Coeff_ErrPos_pNOm[X]*ERRpos_umOm[X] + Coeff_ErrPos1_pNOm[X]*ERRpos1_umOm[X] + Coeff_ErrPos2_pNOm[X]*ERRpos2_umOm[X] + Fd2_pNOm[X];    //Force obtained from tustin
		Fd_pNOm[Y] = Coeff_ErrPos_pNOm[Y]*ERRpos_umOm[Y] + Coeff_ErrPos1_pNOm[Y]*ERRpos1_umOm[Y] + Coeff_ErrPos2_pNOm[Y]*ERRpos2_umOm[Y] + Fd2_pNOm[Y];    //Force obtained from tustin
		Fd_pNOm[Z] = Coeff_ErrPos_pNOm[Z]*ERRpos_umOm[Z] + Coeff_ErrPos1_pNOm[Z]*ERRpos1_umOm[Z] + Coeff_ErrPos2_pNOm[Z]*ERRpos2_umOm[Z] + Fd2_pNOm[Z];    //Force obtained from tustin
	}


	if (MODE==PID_DS_ctrl)  //with FF
	{
		// FB part:		
		Fd_pNOm[X] = Coeff_ErrPos_pNOm[X]*ERRpos_umOm[X] + Coeff_ErrPos1_pNOm[X]*ERRpos1_umOm[X] + Coeff_ErrPos2_pNOm[X]*ERRpos2_umOm[X] + Fd2_pNOm[X];  //with feedforward force
		Fd_pNOm[Y] = Coeff_ErrPos_pNOm[Y]*ERRpos_umOm[Y] + Coeff_ErrPos1_pNOm[Y]*ERRpos1_umOm[Y] + Coeff_ErrPos2_pNOm[Y]*ERRpos2_umOm[Y] + Fd2_pNOm[Y];  //with feedforward force
		Fd_pNOm[Z] = Coeff_ErrPos_pNOm[Z]*ERRpos_umOm[Z] + Coeff_ErrPos1_pNOm[Z]*ERRpos1_umOm[Z] + Coeff_ErrPos2_pNOm[Z]*ERRpos2_umOm[Z] + Fd2_pNOm[Z];  //with feedforward force
	}


	if (MODE == SM_ctrl)   //SM
	{
	    Fd_pNOm[X] = SM_Mag*sign(ERRpos_umOm[X]);
		Fd_pNOm[Y] = SM_Mag*sign(ERRpos_umOm[Y]);
		Fd_pNOm[Z] = SM_Mag*sign(ERRpos_umOm[Z]);
	}


    if (MODE == SupTwistSM_ctrl)  //Sup twisting SM
	{
		Vsuptwist_pNOm[X] = SplIntv/2*(SupTwist_M*sign(ERRpos_umOm[X]) + SupTwist_M*sign(ERRpos1_umOm[X])) + V1suptwist_pNOm[X];
		Vsuptwist_pNOm[Y] = SplIntv/2*(SupTwist_M*sign(ERRpos_umOm[Y]) + SupTwist_M*sign(ERRpos1_umOm[Y])) + V1suptwist_pNOm[Y];
		Vsuptwist_pNOm[Z] = SplIntv/2*(SupTwist_M*sign(ERRpos_umOm[Z]) + SupTwist_M*sign(ERRpos1_umOm[Z])) + V1suptwist_pNOm[Z];

		Fd_pNOm[X] = SupTwist_a*sqrt(fabs(ERRpos_umOm[X]))*sign(ERRpos_umOm[X]) + Vsuptwist_pNOm[X];
		Fd_pNOm[Y] = SupTwist_a*sqrt(fabs(ERRpos_umOm[Y]))*sign(ERRpos_umOm[Y]) + Vsuptwist_pNOm[Y];
		Fd_pNOm[Z] = SupTwist_a*sqrt(fabs(ERRpos_umOm[Z]))*sign(ERRpos_umOm[Z]) + Vsuptwist_pNOm[Z];

		// **** History Update of SupTwist: ****
		V1suptwist_pNOm[X] = Vsuptwist_pNOm[X];
		V1suptwist_pNOm[Y] = Vsuptwist_pNOm[Y];
		V1suptwist_pNOm[Z] = Vsuptwist_pNOm[Z];
		// **** (end) History Update of SupTwist ****
	}

	//*****************************
	//****** History Update: ******
	//*****************************
	if ( PosCtrl_Counter >= 1 )
	{
		Fd2_pNOm[X] = Fd1_pNOm[X]; 
		Fd2_pNOm[Y] = Fd1_pNOm[Y]; 
		Fd2_pNOm[Z] = Fd1_pNOm[Z];
		Fd1_pNOm[X] = Fd_pNOm[X];
		Fd1_pNOm[Y] = Fd_pNOm[Y];
		Fd1_pNOm[Z] = Fd_pNOm[Z];

		ERRpos2_umOm[X] = ERRpos1_umOm[X]; 
		ERRpos2_umOm[Y] = ERRpos1_umOm[Y]; 
		ERRpos2_umOm[Z] = ERRpos1_umOm[Z];
		ERRpos1_umOm[X] = ERRpos_umOm[X];   
		ERRpos1_umOm[Y] = ERRpos_umOm[Y];   
		ERRpos1_umOm[Z] = ERRpos_umOm[Z];
	}
	if ( PosCtrl_Counter == 0 )
	{
		Fd1_pNOm[X] = Fd_pNOm[X];
		Fd1_pNOm[Y] = Fd_pNOm[Y];
		Fd1_pNOm[Z] = Fd_pNOm[Z];

		ERRpos1_umOm[X] = ERRpos_umOm[X];   
		ERRpos1_umOm[Y] = ERRpos_umOm[Y];   
		ERRpos1_umOm[Z] = ERRpos_umOm[Z];
	}
	//**********************************
	//****** (end) History Update ******
	//**********************************

	PosCtrl_Counter ++;
}


void CALCULATION::Set_PIDgain(double Px, double Py, double Pz, double Ix, double Iy, double Iz, double Dx, double Dy, double Dz)
{
	Pgain[X] = Px;
	Pgain[Y] = Py;
	Pgain[Z] = Pz;
	
	Igain[X] = Ix;
	Igain[Y] = Iy;
	Igain[Z] = Iz;

	Dgain[X] = Dx;
	Dgain[Y] = Dy;
	Dgain[Z] = Dz;
}


void CALCULATION::Set_CircTrk_Dia_Spd(double CirD_um, double CirSpd_ums, int CirNum)
{
   double CIR_D           = CirD_um,   // diameter (um)
          CIR_R_OUTER     = CIR_D/2.0, // radius (um)
		  CIR_R_INNER     = CIR_R_OUTER/2.0,
          CIR_SPD         = CirSpd_ums,
          CIR_OMEGA_OUTER = CIR_SPD/CIR_R_OUTER;  //AngSpd of OutCircle
   int    CIR_NUM         = CirNum;

   double T_semi     = PI/CIR_OMEGA_OUTER, //sec (NOTE: 1/2*a*T^2=PI & a*T = 2*OEMGA_OUR, because the radius is only half)
		  a_semi_sec = 2*pow(CIR_OMEGA_OUTER,2.0)/PI,
		  a_semi_spl = a_semi_sec/SplRate,
	      CIR_OMEGA_OUTER_spl = CIR_OMEGA_OUTER/SplRate;

   int    SAMPLES_semi = T_semi*SplRate,
	      SAMPLES_CIR  = (2*PI*CIR_NUM)/CIR_OMEGA_OUTER_spl;

   LEN_Simu_Meas = SAMPLES_semi + 
	               SAMPLES_CIR + 
			       SAMPLES_semi + 1;
   Simu_Real_LEN = LEN_Simu_Meas + DELAY_STEP_Meas + 1;
   
   Simu_TX_umOm = new double[LEN_Simu_Meas];  Simu_TX_umOa = new double[LEN_Simu_Meas];  
   Simu_TY_umOm = new double[LEN_Simu_Meas];  Simu_TY_umOa = new double[LEN_Simu_Meas];
   Simu_TZ_umOm = new double[LEN_Simu_Meas];  Simu_TZ_umOa = new double[LEN_Simu_Meas];

   Simu_RX_umOm = new double[LEN_Simu_Meas];  Simu_RX_umOa = new double[LEN_Simu_Meas];
   Simu_RY_umOm = new double[LEN_Simu_Meas];  Simu_RY_umOa = new double[LEN_Simu_Meas];
   Simu_RZ_umOm = new double[LEN_Simu_Meas];  Simu_RZ_umOa = new double[LEN_Simu_Meas];

   Simu_MX_umOm = new double[LEN_Simu_Meas];  Simu_MX_umOa = new double[LEN_Simu_Meas]; 
   Simu_MY_umOm = new double[LEN_Simu_Meas];  Simu_MY_umOa = new double[LEN_Simu_Meas];
   Simu_MZ_umOm = new double[LEN_Simu_Meas];  Simu_MZ_umOa = new double[LEN_Simu_Meas];

   TRK_Vx_Om = new double[LEN_Simu_Meas];  Trk_Vx_Oa = new double[LEN_Simu_Meas];
   TRK_Vy_Om = new double[LEN_Simu_Meas];  Trk_Vy_Oa = new double[LEN_Simu_Meas];
   TRK_Vz_Om = new double[LEN_Simu_Meas];  Trk_Vz_Oa = new double[LEN_Simu_Meas];

   Simu_I1 = new double[LEN_Simu_Meas]; 
   Simu_I2 = new double[LEN_Simu_Meas];
   Simu_I3 = new double[LEN_Simu_Meas]; 
   Simu_I4 = new double[LEN_Simu_Meas];
   Simu_I5 = new double[LEN_Simu_Meas]; 
   Simu_I6 = new double[LEN_Simu_Meas];


   double omega, angle;
   int    S0 = 0;
   for (int i=S0; i<SAMPLES_semi; i++)   //Acceleration Mode (1st Semi Circle)
   {
	   omega = (i + 1)*a_semi_spl;
	   angle = pow(omega,2.0)/(2*a_semi_sec);  //(Vt^2 - Vo^2)/(2a)

	   Simu_TX_umOm[i] = CIR_R_INNER*( 1 + cos( -PI + angle) );
	   Simu_TY_umOm[i] = CIR_R_INNER*sin( -PI + angle);
	   Simu_TZ_umOm[i] = 0.0;

	   TRK_Vx_Om[i] = omega*CIR_R_INNER*( -sin( -PI + angle) );
	   TRK_Vy_Om[i] = omega*CIR_R_INNER*cos( -PI + angle );
	   TRK_Vz_Om[i] = 0.0;
   }
   S0 += SAMPLES_semi;

   for (int i = S0; i< S0 + SAMPLES_CIR; i++ )  //Circular motion mode
   {
	   angle = 0 + CIR_OMEGA_OUTER_spl*(i - SAMPLES_semi + 1);

	   Simu_TX_umOm[i] = CIR_R_OUTER*cos(angle);
	   Simu_TY_umOm[i] = CIR_R_OUTER*sin(angle);
	   Simu_TZ_umOm[i] = 0.0;

	   TRK_Vx_Om[i] = CIR_SPD*(-sin(angle));
	   TRK_Vy_Om[i] = CIR_SPD*cos(angle);
	   TRK_Vz_Om[i] = 0.0;
   }
   S0 += SAMPLES_CIR;

   for (int i = S0; i < S0 + SAMPLES_semi; i++)  //Deceleration Mode (final Semi Circle)
   {
	   omega = 2*CIR_OMEGA_OUTER - (i-SAMPLES_semi-SAMPLES_CIR + 1)*a_semi_spl;
	   angle = (pow(2*CIR_OMEGA_OUTER,2.0) - pow(omega,2.0))/(2*a_semi_sec);

	   Simu_TX_umOm[i] = (CIR_R_OUTER/2.0)*(1 + cos(angle));
	   Simu_TY_umOm[i] = (CIR_R_OUTER/2.0)*sin(angle);
	   Simu_TZ_umOm[i] = 0.0;

	   TRK_Vx_Om[i] = (omega*CIR_R_OUTER/2.0)*(-sin(angle));
	   TRK_Vy_Om[i] = (omega*CIR_R_OUTER/2.0)*cos(angle);
	   TRK_Vz_Om[i] = 0.0;
   }

   Simu_TX_umOm[LEN_Simu_Meas - 1] = 0.0;
   Simu_TY_umOm[LEN_Simu_Meas - 1] = 0.0;
   Simu_TZ_umOm[LEN_Simu_Meas - 1] = 0.0;

   TRK_Vx_Om[LEN_Simu_Meas - 1] = 0.0;
   TRK_Vy_Om[LEN_Simu_Meas - 1] = 0.0;
   TRK_Vz_Om[LEN_Simu_Meas - 1] = 0.0;
}


void CALCULATION::Set_RampTrk_Dia_Spd(double RangeXY_um, double RangeZ_um, int UP_or_DOWN, double RampSpd_ums, double RampAngle_deg) //Traj. Plan
{
	double RAMP_HOR_ANG = RampAngle_deg*PI/180.0,
		   RAMP_VER_ANG = atan2( RangeZ_um, RangeXY_um/2.0 ),
		
		   RAMP_SEMI_DIAG    = sqrt( pow(RangeXY_um/2.0,2.0) + pow(RangeZ_um,2.0) ),
		   RAMP_SEMI_SPD_sec = 2.0,
		   RAMP_SEMI_SPD_spl = RAMP_SEMI_SPD_sec/SplRate,
		   RAMP_SEMI_T_sec   = RAMP_SEMI_DIAG/RAMP_SEMI_SPD_sec, //Time

		   RAMP_SPD_sec = RampSpd_ums,
		   RAMP_SPD_spl = RAMP_SPD_sec/SplRate,

		   RAMP_ACCE_L     = 10.0,    //um
		   RAMP_ACCE_T_sec = 2.0*RAMP_ACCE_L/RAMP_SPD_sec,   //Time
		   RAMP_ACCE_A_sec = pow(RAMP_SPD_sec,2.0)/(2*RAMP_ACCE_L),
		   RAMP_ACCE_A_spl = RAMP_ACCE_A_sec/SplRate,

		   RAMP_RANGE_DIAG = sqrt( pow(RangeXY_um,2.0) + pow(2*RangeZ_um,2.0) ),
		   RAMP_LINE_L     = RAMP_RANGE_DIAG - 2.0*RAMP_ACCE_L,
		   RAMP_LINE_T_Sec = RAMP_LINE_L/RAMP_SPD_sec;       //Time
		   
    int    RAMP_SEMI_T_SAMPLES = RAMP_SEMI_T_sec*SplRate,
	       RAMP_ACCE_T_SAMPLES = RAMP_ACCE_T_sec*SplRate,
	       RAMP_LINE_T_SAMPLES = RAMP_LINE_T_Sec*SplRate,
		   STAY_SAMPLES = 1000;

    LEN_Simu_Meas = RAMP_SEMI_T_SAMPLES + 
			        STAY_SAMPLES +
				    RAMP_ACCE_T_SAMPLES + 
				    RAMP_LINE_T_SAMPLES +
				    RAMP_ACCE_T_SAMPLES +
				    STAY_SAMPLES +
				    RAMP_SEMI_T_SAMPLES;
   Simu_Real_LEN = LEN_Simu_Meas + DELAY_STEP_Meas + 1;

   Simu_TX_umOm = new double[LEN_Simu_Meas];  Simu_TX_umOa = new double[LEN_Simu_Meas];  
   Simu_TY_umOm = new double[LEN_Simu_Meas];  Simu_TY_umOa = new double[LEN_Simu_Meas];
   Simu_TZ_umOm = new double[LEN_Simu_Meas];  Simu_TZ_umOa = new double[LEN_Simu_Meas];

   Simu_RX_umOm = new double[Simu_Real_LEN];  Simu_RX_umOa = new double[Simu_Real_LEN];
   Simu_RY_umOm = new double[Simu_Real_LEN];  Simu_RY_umOa = new double[Simu_Real_LEN];
   Simu_RZ_umOm = new double[Simu_Real_LEN];  Simu_RZ_umOa = new double[Simu_Real_LEN];

   Simu_MX_umOm = new double[LEN_Simu_Meas];  Simu_MX_umOa = new double[LEN_Simu_Meas]; 
   Simu_MY_umOm = new double[LEN_Simu_Meas];  Simu_MY_umOa = new double[LEN_Simu_Meas];
   Simu_MZ_umOm = new double[LEN_Simu_Meas];  Simu_MZ_umOa = new double[LEN_Simu_Meas];

   TRK_Vx_Om = new double[LEN_Simu_Meas];  Trk_Vx_Oa = new double[LEN_Simu_Meas];
   TRK_Vy_Om = new double[LEN_Simu_Meas];  Trk_Vy_Oa = new double[LEN_Simu_Meas];
   TRK_Vz_Om = new double[LEN_Simu_Meas];  Trk_Vz_Oa = new double[LEN_Simu_Meas];

   Simu_I1 = new double[LEN_Simu_Meas]; Simu_I2 = new double[LEN_Simu_Meas];
   Simu_I3 = new double[LEN_Simu_Meas]; Simu_I4 = new double[LEN_Simu_Meas];
   Simu_I5 = new double[LEN_Simu_Meas]; Simu_I6 = new double[LEN_Simu_Meas];

   double spd_now_sec;

   // 1: reach the edge diagonally
   int S0 = 0;
   for (int i = S0; i < RAMP_SEMI_T_SAMPLES - 1; i++)
   {
	   Simu_TX_umOm[i] = (i+1)*( RAMP_SEMI_SPD_spl*cos(RAMP_VER_ANG) )*cos(RAMP_HOR_ANG);
	   Simu_TY_umOm[i] = (i+1)*( RAMP_SEMI_SPD_spl*cos(RAMP_VER_ANG) )*sin(RAMP_HOR_ANG);
	   Simu_TZ_umOm[i] = (i+1)*( RAMP_SEMI_SPD_spl*sin(RAMP_VER_ANG) )*pow(-1.0,UP_or_DOWN);
   }
   Simu_TX_umOm[ RAMP_SEMI_T_SAMPLES - 1 ] = ( RAMP_SEMI_DIAG*cos(RAMP_VER_ANG) )*cos(RAMP_HOR_ANG);
   Simu_TY_umOm[ RAMP_SEMI_T_SAMPLES - 1 ] = ( RAMP_SEMI_DIAG*cos(RAMP_VER_ANG) )*sin(RAMP_HOR_ANG);
   Simu_TZ_umOm[ RAMP_SEMI_T_SAMPLES - 1 ] = ( RAMP_SEMI_DIAG*sin(RAMP_VER_ANG) )*pow(-1.0,UP_or_DOWN);

   // 2: stay 
   S0 += RAMP_SEMI_T_SAMPLES;
   for (int i = S0; i < S0 + STAY_SAMPLES; i++)
   {
	   Simu_TX_umOm[i] = ( RAMP_SEMI_DIAG*cos(RAMP_VER_ANG) )*cos(RAMP_HOR_ANG);
	   Simu_TY_umOm[i] = ( RAMP_SEMI_DIAG*cos(RAMP_VER_ANG) )*sin(RAMP_HOR_ANG);
	   Simu_TZ_umOm[i] = ( RAMP_SEMI_DIAG*sin(RAMP_VER_ANG) )*pow(-1.0,UP_or_DOWN);
   }

   // 3: accelerate
   S0 += STAY_SAMPLES;
   for (int i = S0; i < S0 + RAMP_ACCE_T_SAMPLES - 1; i++)
   {
	   spd_now_sec = (i - S0 + 1)*RAMP_ACCE_A_spl;
	   Simu_TX_umOm[i] = ( (RAMP_SEMI_DIAG - pow(spd_now_sec,2.0)/(2*RAMP_ACCE_A_sec))*cos(RAMP_VER_ANG) )*cos(RAMP_HOR_ANG);
	   Simu_TY_umOm[i] = ( (RAMP_SEMI_DIAG - pow(spd_now_sec,2.0)/(2*RAMP_ACCE_A_sec))*cos(RAMP_VER_ANG) )*sin(RAMP_HOR_ANG);
	   Simu_TZ_umOm[i] = ( (RAMP_SEMI_DIAG - pow(spd_now_sec,2.0)/(2*RAMP_ACCE_A_sec))*sin(RAMP_VER_ANG) )*pow(-1.0,UP_or_DOWN);
   }
   Simu_TX_umOm[ S0 + RAMP_ACCE_T_SAMPLES - 1 ] = ( (RAMP_SEMI_DIAG - RAMP_ACCE_L)*cos(RAMP_VER_ANG) )*cos(RAMP_HOR_ANG);
   Simu_TY_umOm[ S0 + RAMP_ACCE_T_SAMPLES - 1 ] = ( (RAMP_SEMI_DIAG - RAMP_ACCE_L)*cos(RAMP_VER_ANG) )*sin(RAMP_HOR_ANG);
   Simu_TZ_umOm[ S0 + RAMP_ACCE_T_SAMPLES - 1 ] = ( (RAMP_SEMI_DIAG - RAMP_ACCE_L)*sin(RAMP_VER_ANG) )*pow(-1.0,UP_or_DOWN);

   // 4: linear motion:
   S0 += RAMP_ACCE_T_SAMPLES;
   for (int i = S0; i < S0 + RAMP_LINE_T_SAMPLES - 1; i++ )
   {
	   Simu_TX_umOm[i] = ( (RAMP_SEMI_DIAG - RAMP_ACCE_L - (i - S0 + 1)*RAMP_SPD_spl)*cos(RAMP_VER_ANG) )*cos(RAMP_HOR_ANG);
	   Simu_TY_umOm[i] = ( (RAMP_SEMI_DIAG - RAMP_ACCE_L - (i - S0 + 1)*RAMP_SPD_spl)*cos(RAMP_VER_ANG) )*sin(RAMP_HOR_ANG);
	   Simu_TZ_umOm[i] = ( (RAMP_SEMI_DIAG - RAMP_ACCE_L - (i - S0 + 1)*RAMP_SPD_spl)*sin(RAMP_VER_ANG) )*pow(-1.0,UP_or_DOWN);
   }
   Simu_TX_umOm[ S0 + RAMP_LINE_T_SAMPLES - 1 ] = ( (-RAMP_SEMI_DIAG + RAMP_ACCE_L)*cos(RAMP_VER_ANG) )*cos(RAMP_HOR_ANG);
   Simu_TY_umOm[ S0 + RAMP_LINE_T_SAMPLES - 1 ] = ( (-RAMP_SEMI_DIAG + RAMP_ACCE_L)*cos(RAMP_VER_ANG) )*sin(RAMP_HOR_ANG);
   Simu_TZ_umOm[ S0 + RAMP_LINE_T_SAMPLES - 1 ] = ( (-RAMP_SEMI_DIAG + RAMP_ACCE_L)*sin(RAMP_VER_ANG) )*pow(-1.0,UP_or_DOWN);

   // 5: deceleration
   S0 += RAMP_LINE_T_SAMPLES;
   for (int i = S0; i < S0 + RAMP_ACCE_T_SAMPLES - 1; i++)
   {
	   spd_now_sec = RAMP_SPD_sec - (i - S0 + 1)*RAMP_ACCE_A_spl;
	   Simu_TX_umOm[i] = ( (-RAMP_SEMI_DIAG + RAMP_ACCE_L - (pow(RAMP_SPD_sec,2.0)-pow(spd_now_sec,2.0))/(2*RAMP_ACCE_A_sec))*cos(RAMP_VER_ANG) )*cos(RAMP_HOR_ANG);
	   Simu_TY_umOm[i] = ( (-RAMP_SEMI_DIAG + RAMP_ACCE_L - (pow(RAMP_SPD_sec,2.0)-pow(spd_now_sec,2.0))/(2*RAMP_ACCE_A_sec))*cos(RAMP_VER_ANG) )*sin(RAMP_HOR_ANG);
	   Simu_TZ_umOm[i] = ( (-RAMP_SEMI_DIAG + RAMP_ACCE_L - (pow(RAMP_SPD_sec,2.0)-pow(spd_now_sec,2.0))/(2*RAMP_ACCE_A_sec))*sin(RAMP_VER_ANG) )*pow(-1.0,UP_or_DOWN);
   }
   Simu_TX_umOm[ S0 + RAMP_ACCE_T_SAMPLES - 1] = ( (-RAMP_SEMI_DIAG)*cos(RAMP_VER_ANG) )*cos(RAMP_HOR_ANG);
   Simu_TY_umOm[ S0 + RAMP_ACCE_T_SAMPLES - 1] = ( (-RAMP_SEMI_DIAG)*cos(RAMP_VER_ANG) )*sin(RAMP_HOR_ANG);
   Simu_TZ_umOm[ S0 + RAMP_ACCE_T_SAMPLES - 1] = ( (-RAMP_SEMI_DIAG)*sin(RAMP_VER_ANG) )*pow(-1.0,UP_or_DOWN);

   // 6: stay
   S0 += RAMP_ACCE_T_SAMPLES;
   for (int i = S0; i < S0 + STAY_SAMPLES; i++)
   {
	   Simu_TX_umOm[i] = ( (-RAMP_SEMI_DIAG)*cos(RAMP_VER_ANG) )*cos(RAMP_HOR_ANG);
	   Simu_TY_umOm[i] = ( (-RAMP_SEMI_DIAG)*cos(RAMP_VER_ANG) )*sin(RAMP_HOR_ANG);
	   Simu_TZ_umOm[i] = ( (-RAMP_SEMI_DIAG)*sin(RAMP_VER_ANG) )*pow(-1.0,UP_or_DOWN);
   }

   // 7: return to the center from edge:
   S0 += STAY_SAMPLES;
   for (int i = S0; i < S0 + RAMP_SEMI_T_SAMPLES - 1; i++)
   {
	   Simu_TX_umOm[i] = ( (-RAMP_SEMI_DIAG + (i - S0 + 1)*RAMP_SEMI_SPD_spl)*cos(RAMP_VER_ANG) )*cos(RAMP_HOR_ANG);
	   Simu_TY_umOm[i] = ( (-RAMP_SEMI_DIAG + (i - S0 + 1)*RAMP_SEMI_SPD_spl)*cos(RAMP_VER_ANG) )*sin(RAMP_HOR_ANG);
	   Simu_TZ_umOm[i] = ( (-RAMP_SEMI_DIAG + (i - S0 + 1)*RAMP_SEMI_SPD_spl)*sin(RAMP_VER_ANG) )*pow(-1.0,UP_or_DOWN);
   }
   Simu_TX_umOm[ S0 + RAMP_SEMI_T_SAMPLES - 1 ] = 0.0;
   Simu_TY_umOm[ S0 + RAMP_SEMI_T_SAMPLES - 1 ] = 0.0;
   Simu_TZ_umOm[ S0 + RAMP_SEMI_T_SAMPLES - 1 ] = 0.0;
}



void CALCULATION::CoordTrans_Vect_m2a(double Vect_m[3], double Vect_a[3])
{
	CalcMat_3x3_3x1(id_m2a,Vect_m,Vect_a);
}

void CALCULATION::CoordTrans_Vect_a2m(double Vect_a[3], double Vect_m[3])
{
	CalcMat_3x3_3x1(id_a2m,Vect_a,Vect_m);
}

void CALCULATION::CoordTrans_Elem_m2a(double Xm, double Ym, double Zm, double *Xa, double *Ya, double *Za)
{
	*Xa = id_m2a[0][0]*Xm + id_m2a[0][1]*Ym + id_m2a[0][2]*Zm;
	*Ya = id_m2a[1][0]*Xm + id_m2a[1][1]*Ym + id_m2a[1][2]*Zm;
	*Za = id_m2a[2][0]*Xm + id_m2a[2][1]*Ym + id_m2a[2][2]*Zm;
}

void CALCULATION::CoordTrans_Elem_a2m(double Xa, double Ya, double Za, double *Xm, double *Ym, double *Zm)
{
	*Xm = id_a2m[0][0]*Xa + id_a2m[0][1]*Ya + id_a2m[0][2]*Za;
	*Ym = id_a2m[1][0]*Xa + id_a2m[1][1]*Ya + id_a2m[1][2]*Za;
	*Zm = id_a2m[2][0]*Xa + id_a2m[2][1]*Ya + id_a2m[2][2]*Za;
}


int CALCULATION::sign(double e)
{
    return ( (e>0) - (e<0) );
}



//***** check if every parameter is right: *************
void CALCULATION::OutputParameter()
{
  //  cout<<"*********************************************************"<<endl
	 //   <<"ThmNoiseAmp(nm):             "<<ThmNoiseAmp<<endl     
	 //   <<"R_norm(um):                  "<<R_norm<<endl
		//<<"SplRate:                     "<<SplRate<<endl     
		//<<"SplIntv(sec):                "<<SplIntv<<endl 
		//<<"SimuRate:                     "<<SimuRate<<endl         
		//<<"SimuIntv(sec):          "<<SimuIntv<<endl     
		//<<"pi:                          "<<PI<<endl          	
	 //   <<"m2um:                        "<<m2um<<endl         	
		//<<"nm2um:                       "<<nm2um<<endl        	
		//<<"um2m:                        "<<um2m<<endl         	
		//<<"pN2N:                        "<<pN2N<<endl         	
		//<<"N2pN:                        "<<N2pN<<endl         	           	
		//<<"Feedback gain (x,y,z):       "<<Pgain[X]<<"\t"<<Pgain[Y]<<"\t"<<Pgain[Z]<<endl           	
		//<<"Damping(lambda):             "<<DAMPING_WATER<<endl
		//<<"Simu_StepNum:                "<<Simu_StepNum<<endl
		//<<"*********************************************************"<<endl;
}


//******** Matrix manipulation: *******************************************************************************************
void CALCULATION::CalcMat_6x6_6x6_6x6(double Matrix1[][6],double Matrix2[][6],double Matrix3[][6],double result[][6]) 
{
   double intm[6][6];
   for (int i=0;i<6;i++)
	   for (int j=0;j<6;j++)
	   {
	      intm[i][j]=Matrix1[i][0]*Matrix2[0][j] + Matrix1[i][1]*Matrix2[1][j] + Matrix1[i][2]*Matrix2[2][j] + Matrix1[i][3]*Matrix2[3][j] + Matrix1[i][4]*Matrix2[4][j] + Matrix1[i][5]*Matrix2[5][j];
	   }

   for (int i=0;i<6;i++)
	   for (int j=0;j<6;j++)
	   {
	      result[i][j]=intm[i][0]*Matrix3[0][j] + intm[i][1]*Matrix3[1][j]+intm[i][2]*Matrix3[2][j]+intm[i][3]*Matrix3[3][j]+intm[i][4]*Matrix3[4][j]+intm[i][5]*Matrix3[5][j];
	   }
}

double CALCULATION::CalcMat_1x6_6x6_6x1(double before[6],double Mid[6][6],double after[6])
{
   double intm[6];
   for (int i=0;i<6;i++)
   {
      intm[i]=before[0]*Mid[0][i]+before[1]*Mid[1][i]+before[2]*Mid[2][i]+before[3]*Mid[3][i]+before[4]*Mid[4][i]+before[5]*Mid[5][i];
   }
   return (intm[0]*after[0]+intm[1]*after[1]+intm[2]*after[2]+intm[3]*after[3]+intm[4]*after[4]+intm[5]*after[5]);
}



void CALCULATION::CalcMat_1x3_3x3(double Mat_3x3[3][3],double result[3])
{
   double temp[3];
   for (int i=0;i<3;i++)
   {
      temp[i]=result[i];
   }
   for (int i=0;i<3;i++)
   {
      result[i] = temp[0]*Mat_3x3[0][i]+temp[1]*Mat_3x3[1][i]+temp[2]*Mat_3x3[2][i];
   }
}


void CALCULATION::CalcMat_3x3_3x1(double Mat_3x3[3][3],double Mat_3x1[3],double result[3])
{
	for (int i=0;i<3;i++)
   {
      result[i]=+Mat_3x3[i][0]*Mat_3x1[0]+Mat_3x3[i][1]*Mat_3x1[1]+Mat_3x3[i][2]*Mat_3x1[2];
   }
}


double CALCULATION::CalcMat_1x3_3x1(double Mat_1x3[3],double Mat_3x1[3])
{
   return (Mat_1x3[0]*Mat_3x1[0] + Mat_1x3[1]*Mat_3x1[1] + Mat_1x3[2]*Mat_3x1[2]);
}


void CALCULATION::CalcInverseMat_3x3(double before[][3],double after[][3])
{
   double Det = before[0][0]*before[1][1]*before[2][2] + before[0][1]*before[1][2]*before[2][0] + before[0][2]*before[1][0]*before[2][1]
              - before[0][2]*before[1][1]*before[2][0] - before[0][1]*before[1][0]*before[2][2] - before[0][0]*before[1][2]*before[2][1];
   double Adj[3][3]={{before[1][1]*before[2][2]-before[1][2]*before[2][1], before[0][2]*before[2][1]-before[0][1]*before[2][2], before[0][1]*before[1][2]-before[0][2]*before[1][1]},
                     {before[1][2]*before[2][0]-before[1][0]*before[2][2], before[0][0]*before[2][2]-before[0][2]*before[2][0], before[0][2]*before[1][0]-before[0][0]*before[1][2]},
                     {before[1][0]*before[2][1]-before[1][1]*before[2][0], before[0][1]*before[2][0]-before[0][0]*before[2][1], before[0][0]*before[1][1]-before[0][1]*before[1][0]}};
   for (int i=0;i<3;i++)
	   for (int j=0;j<3;j++)
	   {
	      after[i][j]=Adj[i][j]/Det;
	   }
}


void CALCULATION::CalcMat_6x6_6x1(double Mat1[6][6], double Mat2[6], double Result[6])
{
	for (int i=0; i<6; i++)
	{
		Result[i] = 0.0;
		for (int j=0; j<6; j++)
		{
			Result[i] += Mat1[i][j]*Mat2[j];
		}
	}
}


double CALCULATION::CalcMat_1x3_3x3_3x1(double Mat1_13[3], double Mat2_33[3][3], double Mat3_31[3])
{
   double temp = 0.0;

   for (int i=0;i<3;i++)
   {
	   for (int j=0; j<3; j++)
	   {
		   temp += Mat1_13[i]*Mat2_33[i][j]*Mat3_31[j];
	   }
   }

   return (temp);
}


void CALCULATION::CalcMat_3x3_3x3(double Mat1_33[3][3], double Mat2_33[3][3], double Rst_33[3][3])
{
	for (int i=0; i<3; i++)
		for (int j=0; j<3; j++)
		{
			Rst_33[i][j] = Mat1_33[i][0]*Mat2_33[0][j] + 
				           Mat1_33[i][1]*Mat2_33[1][j] + 
						   Mat1_33[i][2]*Mat2_33[2][j];
		}
}


void CALCULATION::CalcMat_3x3_3x3_3x3(double Mat1_33[3][3], double Mat2_33[3][3], double Mat3_33[3][3], double Rst_33[3][3])
{
    double temp[3][3];
	for (int i=0; i<3; i++)
		for (int j=0; j<3; j++)
		{
		   temp[i][j] = Mat1_33[i][0]*Mat2_33[0][j] + 
			            Mat1_33[i][1]*Mat2_33[1][j] + 
						Mat1_33[i][2]*Mat2_33[2][j];
		}

	for (int i=0; i<3; i++)
		for (int j=0; j<3; j++)
		{
		   Rst_33[i][j] = temp[i][0]*Mat3_33[0][j] + 
			              temp[i][1]*Mat3_33[1][j] + 
						  temp[i][2]*Mat3_33[2][j];
		}
}

void CALCULATION::CalcMatSum_3x3(double Mat1_33[3][3], double Mat2_33[3][3], double Rst_33[3][3])
{
	for (int i=0; i<3; i++)
		for (int j=0; j<3; j++)
		{
			Rst_33[i][j] = Mat1_33[i][j] + Mat2_33[i][j];
		}
}

void CALCULATION::CalcVectDev_3x1(double Vect1_31[3], double devide)
{
    for (int i=0; i<3; i++)
	{
	    Vect1_31[i] = Vect1_31[i]/devide;
	}
}
//*********************************************************************************************************


void CALCULATION::Write_SimuRst_Target_Meas_Real_Curr_Force( char* fname)
{
	FILE *fp;
	fp = fopen(fname,"wt");
	if (fp == NULL)
	{
	    cout<<"Creating file fail"<<endl;
		exit(1);
	}
	else
	{
		fprintf(fp,"%s \n","CtrlRate");
		fprintf(fp,"%d \n",SplRate);

		fprintf(fp,"\n");

		fprintf(fp,"%16s \t %16s \t %16s \t %16s \t %16s \t %16s \t %16s \t %16s \t %16s \t %16s \t %16s \t %16s \t %16s \t %16s \t %16s \t %16s \t %16s \t %16s \t %16s \t %16s \t %16s \t %16s \t %16s \t %16s \t %16s \t %16s \t %16s \t %16s \t %16s \t %16s \t %16s \t %16s \t %16s\n",
			"TX_Om(um)(1)", "TY_Om(um)(2)", "TZ_Om(um)(3)", 
			"MX_Om(um)(4)", "MY_Om(um)(5)", "MZ_Om(um)(6)",

			"TX_Oa(um)(7)", "TY_Oa(um)(8)", "TZ_Oa(um)(9)",
			"MX_Oa(um)(10)","MY_Oa(um)(11)","MZ_Oa(um)(12)",

			"RX_Om(um)(13)","RY_Om(um)(14)","RZ_Om(um)(15)",
			"RX_Oa(um)(16)","RY_Oa(um)(17)","RZ_Oa(um)(18)",
			
			"I1(19)","I2(20)","I3(21)","I4(22)","I5(23)","I6(24)",
			
			"FXdsir_pN_Om(25)", "FYdsir_pN_Om(26)", "FZdsir_pN_Om(27)",
			"FXdsir_pN_Oa(28)", "FYdsir_pN_Oa(29)", "FZdsir_pN_Oa(30)",
			"FXcalc_pN_Oa(31)", "FYcalc_pN_Oa(32)", "FZcalc_pN_Oa(33)" //for Force Calc Debugging
			);
		for (int i=0; i<LEN_Simu_Meas; i++)
		{
		    fprintf(fp,"%16.10E \t %16.10E \t %16.10E \t %16.10E \t %16.10E \t %16.10E \t %16.10E \t %16.10E \t %16.10E \t %16.10E \t %16.10E \t %16.10E \t %16.10E \t %16.10E \t %16.10E \t %16.10E \t %16.10E \t %16.10E \t %16.10E \t %16.10E \t %16.10E \t %16.10E \t %16.10E \t %16.10E \t %16.10E \t %16.10E \t %16.10E \t %16.10E \t %16.10E \t %16.10E \t %16.10E \t %16.10E \t %16.10E \n",
				Simu_TX_umOm[i], Simu_TY_umOm[i], Simu_TZ_umOm[i],
				Simu_MX_umOm[i], Simu_MY_umOm[i], Simu_MZ_umOm[i],
				
				Simu_TX_umOa[i], Simu_TY_umOa[i], Simu_TZ_umOa[i],
				Simu_MX_umOa[i], Simu_MY_umOa[i], Simu_MZ_umOa[i],

				Simu_RX_umOm[ i+DELAY_STEP_Meas ],Simu_RY_umOm[ i+DELAY_STEP_Meas ],Simu_RZ_umOm[ i+DELAY_STEP_Meas ],
				Simu_RX_umOa[ i+DELAY_STEP_Meas ],Simu_RY_umOa[ i+DELAY_STEP_Meas ],Simu_RZ_umOa[ i+DELAY_STEP_Meas ],
				
				Simu_I1[i], Simu_I2[i], Simu_I3[i], Simu_I4[i], Simu_I5[i], Simu_I6[i],
				
				Simu_FdX_pNOm[i], Simu_FdY_pNOm[i], Simu_FdZ_pNOm[i],
				Simu_FdX_pNOa[i], Simu_FdY_pNOa[i], Simu_FdZ_pNOa[i],
				Simu_FXcalc_pNOa[i], Simu_FYcalc_pNOa[i], Simu_FZcalc_pNOa[i]);
		}
	}
	fclose(fp);
}


void  CALCULATION::Write_EstiRst_xyz(char *fnamex, char *fnamey, char *fnamez)
{
	using namespace std;

	cout<<"writing in x direction..."<<endl;
	//****** X DIRECTION: ******
	FILE *fpx;
	fpx = fopen(fnamex,"wt");
	if (fpx == NULL)
	{
		cout<<"Open file "<<fnamex<<" FAIL!!";
	}
	else
	{
	    fprintf( fpx,"%15s %15s %15s %15s %15s %15s\n", 
			         "pN/Damping(1)", 
					 "Damping(2)", 
					 "Motn_Ext(3)", 
					 "F_Ext(4)", 
					 "Vari_Tot(5)", 
					 "Vari Meas(6)" );
		for (int i=0; i<LEN_Simu_Meas; i++)
		{
            fprintf(fpx,"%15.10E %15.10E %15.10E %15.10E %15.10E %15.10E \n", 
				        pN_O_Damp_x_ESTI[i], 
						Damp_x_ESTI[i], 
						M_Ext_x_ESTI[i], 
						F_Ext_x_ESTI[i], 
						Vari_Tot_x_ESTI[i], 
						Vari_Meas_x_ESTI[i] );
		}
	}
	fclose(fpx);
	//**************************
	cout<<"writing in x direction finished!"<<endl;


	cout<<"writing in y direction..."<<endl;
	//****** Y DIRECTION: ******
	FILE *fpy;
	fpy = fopen(fnamey,"wt");
	if (fpy == NULL)
	{
		cout<<"Open file "<<fnamey<<" FAIL!!";
	}
	else
	{
	    fprintf( fpy,"%15s %15s %15s %15s %15s %15s\n", "pN/Damping(1)", "Damping(2)", "Motn_Ext(3)", "F_Ext(4)", "Vari_Tot(5)", "Vari Meas(6)" );
		for (int i=0; i<LEN_Simu_Meas; i++)
		{
            fprintf(fpy,"%15.10E %15.10E %15.10E %15.10E %15.10E %15.10E \n", pN_O_Damp_y_ESTI[i], Damp_y_ESTI[i], M_Ext_y_ESTI[i], F_Ext_y_ESTI[i], Vari_Tot_y_ESTI[i], Vari_Meas_y_ESTI[i] );
		}
	}
	fclose(fpy);
	//**************************
	cout<<"writing in y direction finished!"<<endl;


	cout<<"writing in z direction..."<<endl;
	//****** Z DIRECTION: ******
	FILE *fpz;
	fpz = fopen(fnamez,"wt");
	if (fpz == NULL)
	{
		cout<<"Open file "<<fnamez<<" FAIL!!";
	}
	else
	{
	    fprintf( fpz,"%15s %15s %15s %15s %15s %15s\n", 
			         "pN/Damping(1)", 
					 "Damping(2)", 
					 "Motn_Ext(3)", 
					 "F_Ext(4)", 
					 "Vari_Tot(5)", 
					 "Vari Meas(6)" );
		for (int i=0; i<LEN_Simu_Meas; i++)
		{
            fprintf(fpz,"%15.10E %15.10E %15.10E %15.10E %15.10E %15.10E \n", pN_O_Damp_z_ESTI[i], Damp_z_ESTI[i], M_Ext_z_ESTI[i], F_Ext_z_ESTI[i], Vari_Tot_z_ESTI[i], Vari_Meas_z_ESTI[i] );
		}
	}
	fclose(fpz);
	//***************************
	cout<<"writing in z direction finished!"<<endl;
}

