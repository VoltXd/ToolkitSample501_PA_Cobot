/////////////////////////////////////////////////////////////////////////////
//	SocketManager.h
/////////////////////////////////////////////////////////////////////////////
#pragma warning(disable:4996)
#ifndef __SOCKET_MANAGER__
#define	__SOCKET_MANAGER__



#include "KreonDefines.h"
#include <winsock.h>



/////////////////////////////////////////////////////////////////////////////
class CSocketManager
{
public:
	CSocketManager();
	~CSocketManager();

	int  InitServer(KREON_CALLBACK KreonCallBack,HWND hWnd,UINT KreonMessage,TCHAR* ServerName=NULL);
	bool OnDataSocket(void);


	//	KREON SDK functions
	void InitKreon(BYTE MachineType,char* CALIBRATION_FILE,UINT CodeClient,char* Language=NULL, HWND hWndApp=NULL);
	void SetApplicationHwnd(HWND hWndApp);
	void EndKreon(BYTE SensorOff);
	void GetLastErrorString(BYTE ErrorCode);
	//	Calibration file management
	void GetCalibrationFileList();
	void GetLaserPlaneCorners();
	//	3 axes machines
	void InitMachine(KR_MACHINE_CONFIG &Machine);
	void SetPresetAxis(double* AxisPosition);
	void GetPresetAxis();
	//	Video settings window
	void SetVideoSettings(BYTE Lut,BYTE IntegrationTime,BYTE LaserPower);
	void GetVideoSettings(void);
	void OpenVideoSettingWindow(bool bVisible=true);
	void IsVideoSettingWindowOpened();
	void LockVideoSettingsWindow();
	void UnlockVideoSettingsWindow();
	void CloseVideoSettingsWindow();
	void GetVideoSettingsRecorded();
	void SetVideoSettingByName(char* Name);
	//	Parameters
	void SetLaserPermanent(bool bPermanent);
	void SetBlueLED(bool bOn);
	void SetHardwareFilter(BYTE Filter);
	void SetExternalTriggerDuration(WORD TriggerDuration);
	void SetAcquisitionDelay(WORD Delay);
	void OpenDrawingParametersWindow(void);
	//	Positioning process
	void SphereCalibration(KR_LASER_LINE* CalibrationScan, double* CalibrationMatrix, bool Type5axes, long SupportDirection, double SphereDiameter);
	void UpdateOffset(KR_LASER_LINE* CalibrationScan, double* CalibrationMatrix, bool Type5axes, long SupportDirection, double SphereDiameter);
	void SetSphereCenter(double x,double y,double z);
	void OpenPositioningWindow(KR_INDEX* Index,BYTE NbIndex,
							   BYTE ScanMethod,bool bVisible=true,BYTE Frequency=15,
							   double SphereDiameter=0,bool bAllowAddDelIndex=false,
							   bool bDisplayOnOffButtons=false,HWND hWndParent=NULL,
							   bool bAutomatic=false,
							   BYTE SupportDirection=0xFF); // 0xFF => take the last selected value
	void OpenPositioningWindowExt(KR_INDEX_EXT* Index,BYTE NbIndex,
								  BYTE ScanMethod,bool bVisible=true,BYTE Frequency=15,
								  double SphereDiameter=0,bool bAllowAddDelIndex=false,
								  bool bDisplayOnOffButtons=false,HWND hWndParent=NULL,
								  bool bAutomatic=false,
								  BYTE SupportDirection=0xFF); // 0xFF => take the last selected value
	void IsPositioningWindowOpened(void);
	void GetCalibrationMatrix(BYTE index);
	void StopScan();
	void GetPositioningToolpath(double SphereDiameter);
	void StartScannerAcquisitionInPositioningWindow();
	//	Scan
	void SetMachinePosition(double* PositionMatrix);
	void SetNoMachinePositionAvailable();
	void InitAcquisition(BYTE ScanMethod,BYTE Frequency,float Step,double* PositioningMatrix,UINT Decimation);
	void InitAcquisition(BYTE ScanMethod,int  Frequency,float Step,double* PositioningMatrix,UINT Decimation);
	void BeginAcquisition();	//	Used also in positioning
	void EndAcquisition();		//	Used also in positioning
	void GetXYZLaserLine();
	void OpenScanWindow();
	//	Touch probes
	void ProbeOpenCalibrationWindow(KR_TOUCH_PROBE* Probes,BYTE NbProbes,bool bVisible,double SphereDiameter=-1.0,bool ballowAddDel=false);
	void ProbeAddPosition(double* Matrix,bool bSound);
	void ProbeDelLastPosition(bool bSound);
	void ProbeGetCalibration(BYTE ProbeIndex);
	void BeginProbeCalibration(BYTE Method);
	void EndProbeCalibration(void);
	//	Misc.
	void GetSensorType(void);
	void SetSensorState(bool bOn);
	void OpenScannerPropertiesWindow(HWND hWndParent);
	void GetHardwareFilter(void);
	void OpenLicenseDialog(void);
	void OpenLicenseDialog(HWND hWndParent);


private:
	void OnSend(char *Buffer,int Size);

	KREON_CALLBACK	m_KreonCallBack;
	SOCKET			m_Socket;
	char*			m_Command;
	DWORD			m_CommandSize;
	bool			m_bBusy;
};



#endif