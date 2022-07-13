//	-------------------------------------------------------------------------
// VXElementsWrapper.h
//		Make the interface between the KREON API and the VXELEMENTS API by simulating a robot arm dll.
//
//	Methods :
//
//		IsConnected
//			returns true if the CTrack is connected to the PC, else false
//
//		BeginArm
//			Initializes the arm and memory. You need to call this method
//			before using the arm.
//			returns KR_ARM_OK (1) if the initialization has been correctly done
//			and a negative value if there was a problem.
//
//		EndArm
//			Frees memory and disconnects from the arm.
//			returns 1 if it is correct and a negative value if there was a
//			problem.
//
//		BeginAcquisition(int Mode)
//			Puts the arm in an acquisition mode. 4 acquisition modes are
//			available, but there are not supported by all arms :
//			1 : the arm sends his current position when you ask it
//				(with the GetCurrentPosition method).
//			2 : touch probe mode : the arm sends his position triggered by
//				a button (use GetTriggeredPosition)
//			3 : asynchronous scan mode (use GetTriggeredPosition)
//			4 : synchronous scan mode (use GetTriggeredPosition)
//			5 : used to scan the buttons (use GetButtonState)
//			returns :
//			1 : ok
//			-2: mode not supported by the arm
//			any negative value : error
//
//		GetAcquisitionMode
//			Returns the current value of 'Mode', see BeginAcquisition
//
//		EndAcquisition
//			Stops the acquisition mode
//			returns 1 if it is correct and a negative value if there was a
//			problem.
//
//		GetCurrentPosition(double* Matrix,bool* ButtonState)
//			Used in mode 1 (see BeginAcquisition) to get the current position
//			of the arm.
//			Matrix : array of 16 doubles allocated by your program. The matrix
//			is organized with this order :
//			Matrix[0]	Matrix[4]	Matrix[8]	Matrix[12]
//			Matrix[1]	Matrix[5]	Matrix[9]	Matrix[13]
//			Matrix[2]	Matrix[6]	Matrix[10]	Matrix[14]
//			Matrix[3]	Matrix[7]	Matrix[11]	Matrix[15]
//			ButtonState : array of booleans allocated by your program (see
//			GetParameter to know how many buttons the arm has). This parameter
//			can be NULL if you only need to get the position.
//			returns :
//			1 : ok
//			0 : the "pause" button has been pressed
//			negative value : error
//
//		GetTriggeredPosition(double* Matrix,bool* ButtonState)
//			See GetCurrentPosition
//
//		GetButtonState(bool* ButtonState)
//			ButtonState : array of booleans allocated by your program (see
//			GetParameter to know how many buttons the arm has).
//			returns 1 if it is correct and a negative value if there was a
//			problem.
//
//		GetParameter(char* ParamName,char* ParamValue)
//			Allow your program to get some arm parameters.
//			ParamName is the name of the parameter, and ParamValue is a pointer
//			to a variable whose type depends of ParamName.
//
//			ParamName					Type	Meaning
//			NbButtons					int		Number of buttons for this arm 
//			TriggerDuration				WORD	Duration of the trigger needed
//												by the arm (in micro seconds)
//			AsynchronousModeSupported	bool
//			SynchronousModeSupported	bool
//
//			returns true if the parameter is recognized, else false.
//
//		SetParameter(char* ParamName,char* Value)
//			Allow your program to set an arm parameter.
//			Not implemented yet.
//
//		ProbeCalibration(int nPositions, KR_POSITION Positions[], int CalibrationMethod, KR_TOUCH_PROBE * pProbe, double SphereDiameter)
//			Computes a probe calibration (pointed by pProbe) from the
//			nPositions positions stored in the Positions vector.
//			CalibrationMethod can be 1 (by sphere) or 2 (by hole).
//			SphereDiameter is required if calibration method 1 is chosen.
//			The returned value is an error code from:
//			- KR_ERR_NO_ERROR
//			- KR_ERR_PROBE_CALIBRATION_UNKNOWN_METHOD
//			- KR_ERR_PROBE_CALIBRATION_NOT_ENOUGH_DATA

//
//	---------------------------------------------------------------------------
#pragma once

#include <Windows.h>

#define	KR_NAME_LENGTH					21

//typedef struct
//{
//	double	Matrix[16];
//}	KR_POSITION;

//typedef struct
//{
//	char	Name[KR_NAME_LENGTH];			//	Name of the probe
//	double	X, Y, Z;							//	Calibration
//	double	Sigma;							//	Standard deviation of the calibration process
//	double	Diameter;						//	Diameter of the probe
//	bool	bTriggered;						//	true if the probe is triggered (TP2 for instance)
//}	KR_TOUCH_PROBE;

class VXElementsWrapperPrivate;

class __declspec(dllexport) VXElementsWrapper
{
public:
	VXElementsWrapperPrivate*  _private;

	// Variables
	int acquisitionMode;
	int NbButtons;
	WORD TriggerDuration;
	DWORD AcquisitionFrequency;
	KR_TOUCH_PROBE Probe;
	

public:
	VXElementsWrapper();
	~VXElementsWrapper();


	// Functions loaded by the kreon api
	bool IsConnected();
	int BeginArm();
	int EndArm();
	int BeginAcquisition(int Mode);
	int GetAcquisitionMode(void);
	int EndAcquisition();
	int GetCurrentPosition(double* Matrix, bool* ButtonState);
	int GetTriggeredPosition(double *Matrix, bool* ButtonState);
	int GetButtonState(bool* ButtonState);
	bool GetParameter(char* Name, void* Value);
	bool SetParameter(char* Name, void* Value);
	bool HasPropertiesWindow(void);
	void PropertiesWindow(HWND hWndParend);
	int ProbeCalibration(int nPositions, KR_POSITION Positions[], int CalibrationMethod, KR_TOUCH_PROBE * pProbe, double SphereDiameter);

	bool IsTrackingStarted();
	void DetectModel();
};


