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



/////////////////////////////////////////////////////////////////////////////
CArmManager::CArmManager(TCHAR* DLLName)
{
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
	return true;
}

bool CArmManager::IsConnected()
{
	return CArmManager::wrapper.IsConnected();
}

int CArmManager::BeginArm()
{
	return CArmManager::wrapper.BeginArm();
}

int	CArmManager::EndArm()
{
	return CArmManager::wrapper.EndArm();
}

int	CArmManager::BeginAcquisition(int Mode)
{
	return CArmManager::wrapper.BeginAcquisition(Mode);
}

int	CArmManager::GetAcquisitionMode(void)
{
	return CArmManager::wrapper.GetAcquisitionMode();
}

int	CArmManager::EndAcquisition()
{
	return CArmManager::wrapper.EndAcquisition();
}

int	CArmManager::GetCurrentPosition(double* Matrix,bool* ButtonState)
{
	return CArmManager::wrapper.GetCurrentPosition(Matrix, ButtonState);
}

int	CArmManager::GetTriggeredPosition(double* Matrix,bool* ButtonState)
{
	return CArmManager::wrapper.GetTriggeredPosition(Matrix, ButtonState);
}

int	CArmManager::GetButtonState(bool* ButtonState)
{
	return CArmManager::wrapper.GetButtonState(ButtonState);
}

bool CArmManager::GetParameter(char* Name,void* Value)
{
	return CArmManager::wrapper.GetParameter(Name, Value);
}

bool CArmManager::SetParameter(char* Name,void* Value)
{
	return CArmManager::wrapper.SetParameter(Name, Value);
}

bool CArmManager::HasPropertiesWindow(void)
{
	return CArmManager::wrapper.HasPropertiesWindow();
}

void CArmManager::PropertiesWindow(HWND hWndParent)
{
	return CArmManager::wrapper.PropertiesWindow(hWndParent);
}

int	CArmManager::ProbeCalibration(int nPositions, KR_POSITION Positions[], int CalibrationMethod, KR_TOUCH_PROBE * pProbe, double SphereDiameter)
{
	return CArmManager::wrapper.ProbeCalibration(nPositions, Positions, CalibrationMethod, pProbe, SphereDiameter);
}

/////////////////////////////////////////////////////////////////////////////
CArmManager::~CArmManager()
{
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