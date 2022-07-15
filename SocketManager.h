/////////////////////////////////////////////////////////////////////////////
//	SocketManager.h
/////////////////////////////////////////////////////////////////////////////
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

	bool GetPluginFolder(LPCTSTR pluginName, TCHAR * path);
	int  InitServer(KREON_CALLBACK KreonCallBack,HWND hWnd,UINT KreonMessage,TCHAR* ServerName=NULL,BYTE PortIndex=0);
	bool OnDataSocket();


	//	KREON SDK functions
	void InitKreon(BYTE MachineType,char* CALIBRATION_FILE,UINT CodeClient,char* Language=NULL, HWND hWndApp=NULL);
	void SetApplicationHwnd(HWND hWndApp);
	void EndKreon(BYTE SensorOff);
	bool IsSocketConnected();
	void GetLastErrorString(BYTE ErrorCode, bool bUnicode);
	//	Calibration file management
	void GetCalibrationFileList();
	void GetLaserPlaneCorners(bool bReduced=false);
	//	3 axes machines
	void InitMachine(KR_MACHINE_CONFIG &Machine);
	void SetPresetAxis(double* AxisPosition);
	void GetPresetAxis();
	//	Video settings window
	void SetVideoSettings(BYTE Lut,BYTE IntegrationTime,BYTE LaserPower);
	void GetVideoSettings();
	void OpenVideoSettingWindow(bool bVisible=true, bool bMinimized = false);
	void IsVideoSettingWindowOpened();
	void LockVideoSettingsWindow();
	void UnlockVideoSettingsWindow();
	void CloseVideoSettingsWindow();
	void GetVideoSettingsList();
	void GetVideoSettingsRecorded();
	void SetVideoSettingByName(char* Name);
	//	Parameters
	void SetLaserPermanent(bool bPermanent);
	void SetBlueLED(bool bOn);
	void SetHardwareFilter(BYTE Filter);
	void SetExternalTriggerDuration(WORD TriggerDuration);
	void SetAcquisitionDelay(WORD Delay);
	void OpenDrawingParametersWindow();
	//	Positioning process
	void SphereCalibration(KR_LASER_LINE* CalibrationScan, double* CalibrationMatrix, bool Type5axes, long SupportDirection, double SphereDiameter);
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
	void InitAcquisition(BYTE ScanMethod,int  Frequency,float Step,KR_INDEX_EXT * Index,UINT Decimation);
	void BeginAcquisition();	//	Used also in positioning
	void EndAcquisition();		//	Used also in positioning
	void GetXYZLaserLine();
	//	Touch probes
	void ProbeOpenCalibrationWindow(KR_TOUCH_PROBE* Probes,BYTE NbProbes,bool bVisible,double SphereDiameter=-1.0,bool ballowAddDel=false);
	void ProbeAddPosition(double* Matrix,bool bSound);
	void ProbeDelLastPosition(bool bSound);
	void ProbeGetCalibration(BYTE ProbeIndex);
	void BeginProbeCalibration(BYTE Method);
	void EndProbeCalibration();
	//	Misc.
	void GetSensorType();
	void SetSensorState(bool bOn);
	void GetSensorState();
	void OpenScannerPropertiesWindow(HWND hWndParent);
	void GetHardwareFilter();
	void OpenLicenseDialog();
	void OpenLicenseDialog(HWND hWndParent);
	void SetOutput(BYTE output);
	void GetInput();
	//  Misc. computations
	void FitPlane(double pPoints[], DWORD nbPoints);
	void FitPoint(double pPoints[], DWORD nbPoints);
	void FitSphere(double pPoints[], DWORD nbPoints);
	void FitCircle(double pPoints[], DWORD nbPoints);
	void FitCylinder(double pPoints[], DWORD nbPoints);
	void FitLine(double pPoints[], DWORD nbPoints);

	void Smoother(double pPoints[], DWORD nbPoints, double smootherRadius, double filterRadius, bool bSmooth, bool bFilter);
	void GetProcessedPoints(DWORD nbPoints);

private:
	void OnSend(char *Buffer,int Size);
	void SendInputPoints(double pPoints[], DWORD nbPoints);	// Used by Fit* commands

	KREON_CALLBACK	m_KreonCallBack;
	SOCKET			m_Socket;
	char*			m_Command;
	DWORD			m_CommandSize;
};



#endif