/////////////////////////////////////////////////////////////////////////////
//	ArmManager.cpp
//
//		Make the interface between any of the KREON arm dll and your program.
//
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ArmManager.h"
#include <Stdio.h>
#include "VXElementsWrapper.h"
#include "PA_Sockets.hpp"
#include "PA_CB.hpp"
#include "PA_Protocol.hpp"
#include "Positionning_Kreon_From_VAL3.hpp"
#include "Eigen/Dense"

#include <thread>
#include <string>

#pragma warning(disable:4302)	// Disable cast void* to WORD warning

PA_Communication::CircularBuffer cobotRxBuffer(1024);
PA_Communication::UdpSocketManager cobotSocketManager(1980, "192.168.0.254", 1980);

bool programEnded = false;
bool isRetrievingPose = false;
float pose[6] = { 0 };
Eigen::Matrix3f rotationMatrix;

int acquisitionMode = KR_ARM_MODE_CURRENT_POS;
int NbButtons = 0;
WORD TriggerDuration = 1000;
DWORD AcquisitionFrequency = 0;
KR_TOUCH_PROBE Probe;


/////////////////////////////////////////////////////////////////////////////
CArmManager::CArmManager(TCHAR* DLLName, PA_Enums::PositioningDevice armType)
{
	programEnded = false;

	m_hModule			= NULL;
	m_bDllOk			= false;
	m_NbButtons			= -1;
	m_TriggerDuration	= (DWORD)-1;

	m_lpfnDllIsConnected			= NULL;
	m_lpfnDllBeginArm				= NULL;
	m_lpfnDllEndArm					= NULL;
	m_lpfnDllBeginAcquisition		= NULL;
	m_lpfnDllGetAcquisitionMode		= NULL;
	m_lpfnDllEndAcquisition			= NULL;
	m_lpfnDllGetCurrentPosition		= NULL;
	m_lpfnDllGetTriggeredPosition	= NULL;
	m_lpfnDllGetButtonState			= NULL;
	m_lpfnDllGetParameter			= NULL;
	m_lpfnDllSetParameter			= NULL;
	m_lpfnDllHasPropertiesWindow	= NULL;
	m_lpfnDllPropertiesWindow		= NULL;
	m_lpfnDllProbeCalibration		= NULL;

	//	Loads the DLL

	m_DllName = DLLName;

	// Retrieve PA_ArmType
	m_ArmType = armType;

	// Begin threads
	m_rcvThread = new std::thread(rcvCobotMessage, std::ref(cobotRxBuffer));
	m_readCBThread = new std::thread(readBuffer, std::ref(cobotRxBuffer));

	//m_hModule = ::LoadLibrary(DLLName);
	//if(m_hModule)
//	if(true)
//	{
//		m_lpfnDllIsConnected			= (LPFNDLLISCONNECTED)			&VXElementsWrapper::IsConnected;//GetProcAddress(m_hModule,"IsConnected");
//		m_lpfnDllBeginArm				= (LPFNDLLBEGINARM)				wrapper.BeginArm;//GetProcAddress(m_hModule,"BeginArm");
//		m_lpfnDllEndArm					= (LPFNDLLENDARM)				wrapper.EndArm;//GetProcAddress(m_hModule,"EndArm");
//		m_lpfnDllBeginAcquisition		= (LPFNDLLBEGINACQUISITION)		wrapper.BeginAcquisition;//GetProcAddress(m_hModule,"BeginAcquisition");
//		m_lpfnDllGetAcquisitionMode		= (LPFNDLLGETACQUISITIONMODE)	wrapper.GetAcquisitionMode;//GetProcAddress(m_hModule,"GetAcquisitionMode");
//		m_lpfnDllEndAcquisition			= (LPFNDLLENDACQUISITION)		wrapper.EndAcquisition;//GetProcAddress(m_hModule,"EndAcquisition");
//		m_lpfnDllGetCurrentPosition		= (LPFNDLLGETCURRENTPOSITION)	wrapper.GetCurrentPosition;//GetProcAddress(m_hModule,"GetCurrentPosition");
//		m_lpfnDllGetTriggeredPosition	= (LPFNDLLGETTRIGGEREDPOSITION)	wrapper.GetTriggeredPosition;//GetProcAddress(m_hModule,"GetTriggeredPosition");
//		m_lpfnDllGetButtonState			= (LPFNDLLGETBUTTONSTATE)		wrapper.GetButtonState;//GetProcAddress(m_hModule,"GetButtonState");
//		m_lpfnDllGetParameter			= (LPFNDLLGETPARAMETER)			wrapper.GetParameter;//GetProcAddress(m_hModule,"GetParameter");
//		m_lpfnDllSetParameter			= (LPFNDLLSETPARAMETER)			wrapper.SetParameter;//GetProcAddress(m_hModule,"SetParameter");
//		m_lpfnDllHasPropertiesWindow	= (LPFNDLLHASPROPERTIESWINDOW)	wrapper.HasPropertiesWindow;//GetProcAddress(m_hModule,"HasPropertiesWindow");
//		m_lpfnDllPropertiesWindow		= (LPFNDLLPROPERTIESWINDOW)		wrapper.PropertiesWindow;//GetProcAddress(m_hModule,"PropertiesWindow");
//		m_lpfnDllProbeCalibration		= (LPFNDLLPROBECALIBRATION)		wrapper.ProbeCalibration;//GetProcAddress(m_hModule,"ProbeCalibration");
//
//		if(	m_lpfnDllIsConnected			&&
//			m_lpfnDllBeginArm				&&	m_lpfnDllEndArm					&&
//			m_lpfnDllBeginAcquisition		&&	m_lpfnDllEndAcquisition			&&
//			m_lpfnDllGetAcquisitionMode		&&
//			m_lpfnDllGetCurrentPosition		&&	m_lpfnDllGetTriggeredPosition	&&
//			m_lpfnDllGetButtonState			&&
//			m_lpfnDllGetParameter			&&	m_lpfnDllSetParameter			&&
//			m_lpfnDllHasPropertiesWindow	&&	m_lpfnDllPropertiesWindow		&&
//			m_lpfnDllProbeCalibration)
//			m_bDllOk = true;
//		else
//			printf("Error in CArmManager::CArmManager : Invalid arm DLL\n");
//	}
//	else
//		printf("Error in CArmManager::CArmManager : LoadLibrary failed\n");
} 

/////////////////////////////////////////////////////////////////////////////
bool CArmManager::IsInitialized()
{
	if (m_ArmType == PA_Enums::Ctrack)
		return true;
	else if (m_ArmType == PA_Enums::CobotTx2Touch)
		return true;//return cobotSocketManager.IsInitialized();
	
	return false;
}

bool CArmManager::IsConnected()
{
	if (m_ArmType == PA_Enums::Ctrack)
		return CArmManager::wrapper.IsConnected();
	else if (m_ArmType == PA_Enums::CobotTx2Touch)
		return cobotSocketManager.IsInitialized();
	
	return false;
}

int CArmManager::BeginArm()
{
	if (m_ArmType == PA_Enums::Ctrack)
		return CArmManager::wrapper.BeginArm();
	else if (m_ArmType == PA_Enums::CobotTx2Touch)
	{
		if (!cobotSocketManager.IsInitialized())
			cobotSocketManager.Initialize();
		return cobotSocketManager.IsInitialized();
	}
	
	return false;
}

int	CArmManager::EndArm()
{
	if (m_ArmType == PA_Enums::Ctrack)
		return CArmManager::wrapper.EndArm();
	else if (m_ArmType == PA_Enums::CobotTx2Touch)
	{
		cobotSocketManager.Close();
		return 1;
	}
	
	return 1;
}

int	CArmManager::BeginAcquisition(int Mode)
{
	if (m_ArmType == PA_Enums::Ctrack)
		return CArmManager::wrapper.BeginAcquisition(Mode);
	else if (m_ArmType == PA_Enums::CobotTx2Touch)
	{
		std::string msg;
		msg.append(0);
		msg[0] = (unsigned char)PA_Enums::Acquire;
		msg.append("\n");
		int charSent = cobotSocketManager.Send(msg.data());
		if (charSent > 0)
			return 1;
		else
			return 0;
	}
	
	return 0;
}

int	CArmManager::GetAcquisitionMode(void)
{
	if (m_ArmType == PA_Enums::Ctrack)
		return CArmManager::wrapper.GetAcquisitionMode();
	else if (m_ArmType == PA_Enums::CobotTx2Touch)
		return KR_ARM_MODE_CURRENT_POS;
	
	return KR_ARM_MODE_NOT_SUPPORTED;
}

int	CArmManager::EndAcquisition()
{
	if (m_ArmType == PA_Enums::Ctrack)
		return CArmManager::wrapper.EndAcquisition();
	else if (m_ArmType == PA_Enums::CobotTx2Touch)
	{
		std::string msg;
		msg.append(0);
		msg[0] = (unsigned char)PA_Enums::EndAcquire;
		msg.append("\n");
		int charSent = cobotSocketManager.Send(msg.data());
		if (charSent > 0)
			return 1;
		else
			return 0;
	}

	return 1;
}

int	CArmManager::GetCurrentPosition(double* Matrix,bool* ButtonState)
{
	if (m_ArmType == PA_Enums::Ctrack)
		return CArmManager::wrapper.GetCurrentPosition(Matrix, ButtonState);
	else if (m_ArmType == PA_Enums::CobotTx2Touch)
	{
		while (isRetrievingPose) continue;

		float x = pose[0];
		float y = pose[1];
		float z = pose[2];

		Matrix[0] = rotationMatrix(0, 0);	Matrix[4] = rotationMatrix(0, 1);	Matrix[8]	= rotationMatrix(0, 2);	Matrix[12] = x;
		Matrix[1] = rotationMatrix(1, 0);	Matrix[5] = rotationMatrix(1, 1);	Matrix[9]	= rotationMatrix(1, 2);	Matrix[13] = y;
		Matrix[2] = rotationMatrix(2, 0);	Matrix[6] = rotationMatrix(2, 1);	Matrix[10]	= rotationMatrix(2, 2);	Matrix[14] = z;
		Matrix[3] = 0;						Matrix[7] = 0;						Matrix[11]	= 0;					Matrix[15] = 1;

		return 1;
	}

	return -2;
}

int	CArmManager::GetTriggeredPosition(double* Matrix,bool* ButtonState)
{
	if (m_ArmType == PA_Enums::Ctrack)
		return CArmManager::wrapper.GetTriggeredPosition(Matrix, ButtonState);
	else if (m_ArmType == PA_Enums::CobotTx2Touch)
	{
		while (isRetrievingPose) continue;

		float x = pose[0];
		float y = pose[1];
		float z = pose[2];

		Matrix[0] = rotationMatrix(0, 0);	Matrix[4] = rotationMatrix(0, 1);	Matrix[8] = rotationMatrix(0, 2);	Matrix[12] = x;
		Matrix[1] = rotationMatrix(1, 0);	Matrix[5] = rotationMatrix(1, 1);	Matrix[9] = rotationMatrix(1, 2);	Matrix[13] = y;
		Matrix[2] = rotationMatrix(2, 0);	Matrix[6] = rotationMatrix(2, 1);	Matrix[10] = rotationMatrix(2, 2);	Matrix[14] = z;
		Matrix[3] = 0;						Matrix[7] = 0;						Matrix[11] = 0;					Matrix[15] = 1;

		return 1;
	}

	return -2;
}

int	CArmManager::GetButtonState(bool* ButtonState)
{
	if (m_ArmType == PA_Enums::Ctrack)
		return CArmManager::wrapper.GetButtonState(ButtonState);
	else if (m_ArmType == PA_Enums::CobotTx2Touch)
		return -2;

	return 0;
}

bool CArmManager::GetParameter(char* Name,void* Value)
{
	if (m_ArmType == PA_Enums::Ctrack)
		return CArmManager::wrapper.GetParameter(Name, Value);
	else if (m_ArmType == PA_Enums::CobotTx2Touch)
	{
		std::string Nom(Name);

		if (Nom == "NbButtons")
		{
			*(int*)Value = NbButtons;
			return true;
		}
		else if (Nom == "TriggerDuration")
		{
			*((WORD*)Value) = TriggerDuration;		//	microsecondes
			return true;
		}
		else
		{
			return false;
		}
	}

	return false;
}

bool CArmManager::SetParameter(char* Name,void* Value)
{
	if (m_ArmType == PA_Enums::Ctrack)
		return CArmManager::wrapper.SetParameter(Name, Value);
	else if (m_ArmType == PA_Enums::CobotTx2Touch)
	{
		std::string nameString(Name);
		if (nameString == "NbButtons")
		{

			NbButtons = (int)Value;
			return true;
		}
		else if (nameString == "TriggerDuration")
		{
			TriggerDuration = (WORD)Value;
			return true;
		}
		else if (nameString == "AcquisitionFrequency")
		{
			AcquisitionFrequency = (DWORD)Value;
			return true;
		}
		else if (nameString == "Probe")
		{
			KR_TOUCH_PROBE * refProbe = &Probe;
			refProbe = (KR_TOUCH_PROBE *)Value;
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
}

bool CArmManager::HasPropertiesWindow(void)
{
	if (m_ArmType == PA_Enums::Ctrack)
		return CArmManager::wrapper.HasPropertiesWindow();
	else if (m_ArmType == PA_Enums::CobotTx2Touch)
		return false;

	return false;
}

void CArmManager::PropertiesWindow(HWND hWndParent)
{
	if (m_ArmType == PA_Enums::Ctrack)
		return CArmManager::wrapper.PropertiesWindow(hWndParent);
	else if (m_ArmType == PA_Enums::CobotTx2Touch)
		return;

	return;
}

int	CArmManager::ProbeCalibration(int nPositions, KR_POSITION Positions[], int CalibrationMethod, KR_TOUCH_PROBE * pProbe, double SphereDiameter)
{
	if (m_ArmType == PA_Enums::Ctrack)
		return CArmManager::wrapper.ProbeCalibration(nPositions, Positions, CalibrationMethod, pProbe, SphereDiameter);
	else if (m_ArmType == PA_Enums::Ctrack)
		return 0;

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
CArmManager::~CArmManager()
{
	programEnded = true;

	printf("CArmManager::~CArmManager\n");

	if(m_hModule)
	{
		if(IsConnected())
			EndArm();

		::FreeLibrary(m_hModule);
		m_hModule = NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////
int CArmManager::GetNbButtons(void)
{
	if ( (m_NbButtons==-1) && m_lpfnDllGetParameter )
		GetParameter("NbButtons",&m_NbButtons);
	return m_NbButtons;
}

/////////////////////////////////////////////////////////////////////////////
void CArmManager::SetFrequency(int Frequency)
{
	SetParameter("AcquisitionFrequency",&Frequency);
}

/////////////////////////////////////////////////////////////////////////////
DWORD CArmManager::GetTriggerDuration(void)
{
	if(m_TriggerDuration==-1)
	{
		if(m_lpfnDllGetParameter)
			GetParameter("TriggerDuration",&m_TriggerDuration);

		if(m_TriggerDuration==-1)
			return 200;					//	Default value for most arms
	}

	return m_TriggerDuration;
}


void rcvCobotMessage(PA_Communication::CircularBuffer &cbRx)
{
	while (!programEnded)
	{
		if (!cobotSocketManager.IsInitialized())
			continue;

		sockaddr_in from;
		char buff[1500] = { 0 };
		int charReceived = cobotSocketManager.Receive(buff, from);
		if (charReceived <= 0)
		{
			cobotSocketManager.Close();
			cobotSocketManager.Initialize();
			continue;
		}

		cbRx.Add(buff, charReceived);
	}
}


void readBuffer(PA_Communication::CircularBuffer &cbRx)
{
	while (!programEnded)
	{
		if (!cbRx.IsDataAvailable())
			continue;

		if (!PA_Protocol::decodeMessage(cbRx.Get()))
			continue;

		isRetrievingPose = true;
		PA_Protocol::retrievePose(pose);
		PA_Positionning::tx2ToKreonTransform(pose);

		float rx = pose[3];
		float ry = pose[4];
		float rz = pose[5];

		rotationMatrix = PA_Positionning::getRotationMatrix(rx, ry, rz);
		isRetrievingPose = false;
	}
}