//	-------------------------------------------------------------------------
//	SocketManager.cpp
//
//	KREON Technologies Toolkit
//	Makes the interface between your software and the Kreon Server
//	-------------------------------------------------------------------------

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WS2tcpip.h>
#include "SocketManager.h"
#include "CircBuf.h"
#include <process.h>
#include "KreonServer.h"
#include "KreonDefines.h"
#include <stdio.h>


#define	RUN_SERVER				//	Used by Kreon for debug mode


static DWORD st_LocalCommandSize;
#define	SET_COMMAND_SIZE(X)		st_LocalCommandSize=(X);\
								if(st_LocalCommandSize>m_CommandSize)\
								{\
									if(m_Command)\
									{\
										delete [] m_Command;\
										m_Command=NULL;\
									}\
									m_CommandSize = st_LocalCommandSize;\
									m_Command = new char[m_CommandSize];\
								}\
								memcpy(m_Command,&st_LocalCommandSize,sizeof(DWORD));
#define	SEND_COMMAND()			OnSend(m_Command,st_LocalCommandSize);



CCircBuf CircBuf(KR_CIRCBUF_SIZE);



/////////////////////////////////////////////////////////////////////////////
CSocketManager::CSocketManager()
{
	m_Socket=INVALID_SOCKET;
	m_KreonCallBack=NULL;
	m_CommandSize=0;
	m_Command=NULL;
}

/////////////////////////////////////////////////////////////////////////////
CSocketManager::~CSocketManager()
{
	if(m_Socket!=INVALID_SOCKET)
	{
		closesocket(m_Socket);
		m_Socket=INVALID_SOCKET;
	}

	if(m_Command)
	{
		delete [] m_Command;
		m_Command=NULL;
	}

	WSACleanup();
}

/////////////////////////////////////////////////////////////////////////////
//	Some plug-ins may need to know where they were installed.
//	You may ignore this function if you have your own way to locate the plug-in folder (usually simply a subfolder of your application).
//  IN: plug-in name (or identifier)
//  OUT: full path to plugin folder (may not end with a backslash => you might want to add it if necessary)
//	Returned valued: true if successful; false if failed or path is empty
/////////////////////////////////////////////////////////////////////////////
bool CSocketManager::GetPluginFolder(LPCTSTR pluginName, TCHAR * path)
{
	if (!*pluginName)
		return false;
	HKEY key;
	TCHAR szKey[MAX_PATH];
	_tcscpy_s(szKey,_T("SOFTWARE\\Kreon Technologies\\Plugins\\"));
	_tcscat_s(szKey,pluginName);
	bool isOpenKey = RegOpenKeyEx(HKEY_LOCAL_MACHINE, szKey, 0, KEY_READ, &key)==ERROR_SUCCESS;
	if (isOpenKey && key)
	{		
		DWORD type;
		DWORD lbuf = MAX_PATH;
		long bRet = RegQueryValueEx(key, _T("Path"), (LPDWORD) NULL, &type , (LPBYTE)path, &lbuf);
		if (bRet==ERROR_SUCCESS)
		{
			RegCloseKey(key);
			if ((type==REG_SZ) && *path)
				return true;
		}
	}
	return false;
}

/////////////////////////////////////////////////////////////////////////////
//	Run the server and connect a socket
/////////////////////////////////////////////////////////////////////////////
int CSocketManager::InitServer(KREON_CALLBACK KreonCallBack,HWND hWnd,UINT SocketMessage,TCHAR* ServerName,BYTE PortIndex)
{
	//	Save the parameter
	m_KreonCallBack=KreonCallBack;

	//	If file KreonServer.ini exists in KreonServer.exe's directory, its port value will be used
	TCHAR CommandLine[MAX_PATH];
	if(ServerName==NULL)
	{
		GetModuleFileName(GetModuleHandle(NULL),CommandLine,_countof(CommandLine));
		*(_tcsrchr(CommandLine,_T('\\'))+1)=0;
		_tcscat_s(CommandLine,_countof(CommandLine),KR_DEFAULT_SERVER_NAME);
	}
	else
		_tcscpy_s(CommandLine,_countof(CommandLine),ServerName);

	TCHAR IniFilePath[MAX_PATH];
	_tcscpy_s(IniFilePath,_countof(IniFilePath),CommandLine);
	int i = (int)_tcslen(IniFilePath)-1;
	while ((i>=0) && (IniFilePath[i]!=_T('.')))
		IniFilePath[i--] = 0;
	_tcscat_s(IniFilePath,_countof(IniFilePath),_T("ini"));

	TCHAR PortName[8];
	if (PortIndex<2)
		_tcscpy_s(PortName,_countof(PortName),_T("PORT"));
	else
		_stprintf_s(PortName,_countof(PortName),_T("PORT%u"),PortIndex);
	int uPort = GetPrivateProfileInt(_T("SOCKET"),PortName,KR_DEFAULT_SERVER_PORT,IniFilePath);
	if (uPort<1024 || uPort>65535)
	{
		printf("Invalid port; using default port %u\n",KR_DEFAULT_SERVER_PORT);
		uPort = KR_DEFAULT_SERVER_PORT;
	}
	//	Run the server
#ifdef RUN_SERVER
	TCHAR Parameters[MAX_PATH];
	_stprintf_s(Parameters,_countof(Parameters),_T(" Port%u Instance%u"),uPort,PortIndex);
	_tcscat_s(CommandLine,_countof(CommandLine),Parameters);

	STARTUPINFO			siStartupInfo;
	PROCESS_INFORMATION	piProcessInfo;
	memset(&siStartupInfo,0,sizeof(siStartupInfo));
	memset(&piProcessInfo,0,sizeof(piProcessInfo));
	siStartupInfo.cb = sizeof(siStartupInfo);
	if (CreateProcess(NULL,CommandLine,NULL,NULL,FALSE,
		NORMAL_PRIORITY_CLASS /*IDLE_PRIORITY_CLASS*/ | CREATE_DEFAULT_ERROR_MODE,
		NULL,NULL,&siStartupInfo,&piProcessInfo)==0)
	{
		DWORD err = GetLastError();
		printf("Erreur = %d\n", err);
		return KR_ERR_UNABLE_TO_FIND_KREON_SERVER;
	}

	Sleep(1000);	//	KreonServer may take some time to initialize
#endif

	//	Connect a socket
	if (m_Socket!=INVALID_SOCKET)
	{
		printf("Socket already exists => Close.\n");
		closesocket(m_Socket);
		m_Socket=INVALID_SOCKET;
	}

	struct	sockaddr_in remote_addr;
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,1),&wsaData);

	int NbRetry=0;
	while (NbRetry++<5)
	{
		printf("Socket creation (attempt #%d)\n",NbRetry);
		m_Socket=socket(AF_INET, SOCK_STREAM, 0);
		if(m_Socket==INVALID_SOCKET)
			Sleep(1000);
		else
		{
			printf("Socket created.\n");
			break;
		}
	}

	if(m_Socket==INVALID_SOCKET)
	{
		int		Error	= WSAGetLastError();
		TCHAR*	Msg		= _T("");
		switch(Error)
		{
			case WSANOTINITIALISED:		Msg=_T("A successful WSAStartup call must occur before using this function.");
			case WSAENETDOWN:			Msg=_T("The network subsystem or the associated service provider has failed.");
			case WSAEAFNOSUPPORT:		Msg=_T("The specified address family is not supported.");
			case WSAEINPROGRESS:		Msg=_T("A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.");
			case WSAEMFILE:				Msg=_T("No more socket descriptors are available.");
			case WSAENOBUFS:			Msg=_T("No buffer space is available. The socket cannot be created.");
			case WSAEPROTONOSUPPORT:	Msg=_T("The specified protocol is not supported.");
			case WSAEPROTOTYPE:			Msg=_T("The specified protocol is the wrong type for this socket.");
			case WSAESOCKTNOSUPPORT:	Msg=_T("The specified socket type is not supported in this address family.");
			default:					Msg=_T("Unknown error");
		}

		_tprintf(_T("socket failed, error code=%d, msg=%s\n"),Error,Msg);
		return KR_ERR_SOCKET_ERROR;
	}

	remote_addr.sin_family = AF_INET;     
	remote_addr.sin_port = htons((USHORT)uPort); 
	inet_pton(AF_INET,"127.0.0.1",&remote_addr.sin_addr.s_addr);
	NbRetry=0;
	bool bConnected=false;
	while (!bConnected && (NbRetry++<10))
	{
		printf("Connection to the server (attempt #%d)\n",NbRetry);
		if (connect(m_Socket, (struct sockaddr *)&remote_addr, sizeof(struct sockaddr))==SOCKET_ERROR)
			Sleep(1000);
		else
		{
			bConnected = true;
			printf("Connection established.\n");
		}
	}
	if (!bConnected)
	{
		printf("connect failed\n");
		return KR_ERR_SOCKET_ERROR;
	}

	DWORD ul=1;
	ioctlsocket(m_Socket,FIONBIO,&ul);

	if(SocketMessage)
	{
		if(WSAAsyncSelect(m_Socket,hWnd,SocketMessage,FD_READ)==SOCKET_ERROR)
		{
			printf("WSAAsyncSelect failed\n");
			return KR_ERR_SOCKET_ERROR;
		}
	}

	CircBuf.SetBuffer(KR_CIRCBUF_SIZE);
	return KR_ERR_NO_ERROR;
}

/////////////////////////////////////////////////////////////////////////////
//	Send the application HWND to the server in order to block the application
//	when any modal message box is shown.
/////////////////////////////////////////////////////////////////////////////
void CSocketManager::SetApplicationHwnd(HWND hWndApp)
{
	SET_COMMAND_SIZE(KR_HEADER_SIZE+sizeof(HWND));
	m_Command[sizeof(DWORD)]=KR_SOCKET_SETAPPLICATIONHWND;
	memcpy(&m_Command[KR_HEADER_SIZE],&hWndApp,sizeof(HWND));
	SEND_COMMAND();
}

/////////////////////////////////////////////////////////////////////////////
//	Initialize the toolkit
/////////////////////////////////////////////////////////////////////////////
void CSocketManager::InitKreon(BYTE MachineType,char* CalibrationFile,
							   UINT ClientCode,char* Language, HWND hWndApp)
{
	if (hWndApp)
		SetApplicationHwnd(hWndApp);

	DWORD CalibrationFileLength;
	if (CalibrationFile)
		CalibrationFileLength = (DWORD)strlen(CalibrationFile)+1;
	else
		CalibrationFileLength = 1;

	DWORD LanguageLength=0;
	DWORD CommandSize = KR_HEADER_SIZE+1+sizeof(DWORD)+CalibrationFileLength;

	if (Language)
	{
		LanguageLength = (DWORD)strlen(Language)+1;
		CommandSize += LanguageLength;
	}

	SET_COMMAND_SIZE(CommandSize);
	m_Command[sizeof(DWORD)]=KR_SOCKET_INITKREON;
	m_Command[KR_HEADER_SIZE]=MachineType;
	memcpy(&m_Command[KR_HEADER_SIZE+1],&ClientCode,sizeof(DWORD));

	if (CalibrationFile)
		memcpy(&m_Command[KR_HEADER_SIZE+1+sizeof(DWORD)],CalibrationFile,CalibrationFileLength);
	else
		m_Command[KR_HEADER_SIZE+1+sizeof(DWORD)]='\0';

	if (Language)
		memcpy(&m_Command[KR_HEADER_SIZE+1+sizeof(DWORD)+CalibrationFileLength],Language,LanguageLength);
	SEND_COMMAND();
}

/////////////////////////////////////////////////////////////////////////////
//	Stop the KREON Toolkit
/////////////////////////////////////////////////////////////////////////////
void CSocketManager::EndKreon(BYTE SensorOff)
{
	SET_COMMAND_SIZE(KR_HEADER_SIZE+1);
	m_Command[sizeof(DWORD)]=KR_SOCKET_ENDKREON;
	m_Command[KR_HEADER_SIZE]=SensorOff;
	SEND_COMMAND();
}

bool CSocketManager::IsSocketConnected()
{
	int optval;
	int optlen = sizeof(optval);
       
    int res = getsockopt(m_Socket,SOL_SOCKET,SO_ERROR,(char*)&optval, &optlen);

    return (optval==0 && res==0);
}

/////////////////////////////////////////////////////////////////////////////
//	Get an explicit message describing the error corresponding to the code
//	given in parameter. bUnicode tells whether you would like to get the
//  error message as Unicode (wide chars) or MultiByte string.
/////////////////////////////////////////////////////////////////////////////
void CSocketManager::GetLastErrorString(BYTE ErrorCode, bool bUnicode)
{
	SET_COMMAND_SIZE(KR_HEADER_SIZE+2);
	m_Command[sizeof(DWORD)]=KR_SOCKET_GETLASTERRORSTRING;
	m_Command[KR_HEADER_SIZE]=ErrorCode;
	m_Command[KR_HEADER_SIZE+1]=bUnicode;
	SEND_COMMAND();
}

/////////////////////////////////////////////////////////////////////////////
//	Init the machine axes
/////////////////////////////////////////////////////////////////////////////
void CSocketManager::InitMachine(KR_MACHINE_CONFIG &Machine)
{
	SET_COMMAND_SIZE(KR_HEADER_SIZE + sizeof(KR_MACHINE_CONFIG));
	m_Command[sizeof(DWORD)]=KR_SOCKET_INITMACHINE;
	memcpy(&m_Command[KR_HEADER_SIZE],&Machine,sizeof(KR_MACHINE_CONFIG));
	SEND_COMMAND();
}

/////////////////////////////////////////////////////////////////////////////
void CSocketManager::GetCalibrationFileList()
{
	SET_COMMAND_SIZE(KR_HEADER_SIZE);
	m_Command[sizeof(DWORD)]=KR_SOCKET_GETCALIBRATIONFILELIST;
	SEND_COMMAND();
}

/////////////////////////////////////////////////////////////////////////////
void CSocketManager::GetLaserPlaneCorners(bool bReduced)
{
	if (bReduced)
	{
		SET_COMMAND_SIZE(KR_HEADER_SIZE+1);
		m_Command[sizeof(DWORD)]=KR_SOCKET_GETLASERPLANECORNERS;
		m_Command[sizeof(DWORD)+1]=bReduced;
	}
	else
	{
		SET_COMMAND_SIZE(KR_HEADER_SIZE);
		m_Command[sizeof(DWORD)]=KR_SOCKET_GETLASERPLANECORNERS;
	}
	SEND_COMMAND();
}

/////////////////////////////////////////////////////////////////////////////
void CSocketManager::SetPresetAxis(double* AxisPosition)
{
	SET_COMMAND_SIZE(KR_HEADER_SIZE + 3*sizeof(double));
	m_Command[sizeof(DWORD)]=KR_SOCKET_SETPRESETAXIS;
	memcpy(&m_Command[KR_HEADER_SIZE],AxisPosition,3*sizeof(double));
	SEND_COMMAND();
}

/////////////////////////////////////////////////////////////////////////////
void CSocketManager::GetPresetAxis(void)
{
	SET_COMMAND_SIZE(KR_HEADER_SIZE);
	m_Command[sizeof(DWORD)]=KR_SOCKET_GETPRESETAXIS;
	SEND_COMMAND();
}

/////////////////////////////////////////////////////////////////////////////
void CSocketManager::SetVideoSettings(BYTE Lut,BYTE IntegrationTime,BYTE LaserPower)
{
	SET_COMMAND_SIZE(KR_HEADER_SIZE+3);
	m_Command[sizeof(DWORD)]=KR_SOCKET_SETVIDEOSETTINGS;
	m_Command[KR_HEADER_SIZE]=Lut;
	m_Command[KR_HEADER_SIZE+1]=IntegrationTime;
	m_Command[KR_HEADER_SIZE+2]=LaserPower;
	SEND_COMMAND();
}

/////////////////////////////////////////////////////////////////////////////
void CSocketManager::GetVideoSettings(void)
{
	SET_COMMAND_SIZE(KR_HEADER_SIZE);
	m_Command[sizeof(DWORD)]=KR_SOCKET_GETVIDEOSETTINGS;
	SEND_COMMAND();
}

/////////////////////////////////////////////////////////////////////////////
//	Open the VideoSetting window
//	bVisible : 0 if you want the window to be created with the "hidden"
//	status, else 1
/////////////////////////////////////////////////////////////////////////////
void CSocketManager::OpenVideoSettingWindow(bool bVisible, bool bMinimized)
{
	SET_COMMAND_SIZE(KR_HEADER_SIZE+2);
	m_Command[sizeof(DWORD)]=KR_SOCKET_OPENVIDEOSETTINGWINDOW;
	m_Command[KR_HEADER_SIZE]=bVisible;
	m_Command[KR_HEADER_SIZE+1]=bMinimized;
	SEND_COMMAND();
}

/////////////////////////////////////////////////////////////////////////////
void CSocketManager::IsVideoSettingWindowOpened(void)
{
	SET_COMMAND_SIZE(KR_HEADER_SIZE);
	m_Command[sizeof(DWORD)]=KR_SOCKET_ISVIDEOSETTINGWINDOWOPENED;
	SEND_COMMAND();
}

/////////////////////////////////////////////////////////////////////////////
void CSocketManager::LockVideoSettingsWindow(void)
{
	SET_COMMAND_SIZE(KR_HEADER_SIZE);
	m_Command[sizeof(DWORD)]=KR_SOCKET_LOCKVIDEOSETTING;
	SEND_COMMAND();
}

/////////////////////////////////////////////////////////////////////////////
void CSocketManager::UnlockVideoSettingsWindow(void)
{
	SET_COMMAND_SIZE(KR_HEADER_SIZE);
	m_Command[sizeof(DWORD)]=KR_SOCKET_UNLOCKVIDEOSETTING;
	SEND_COMMAND();
}

/////////////////////////////////////////////////////////////////////////////
void CSocketManager::CloseVideoSettingsWindow(void)
{
	SET_COMMAND_SIZE(KR_HEADER_SIZE);
	m_Command[sizeof(DWORD)]=KR_SOCKET_CLOSEVIDEOSETTINGWINDOW;
	SEND_COMMAND();
}

/////////////////////////////////////////////////////////////////////////////
void CSocketManager::GetVideoSettingsList(void)
{
	SET_COMMAND_SIZE(KR_HEADER_SIZE);
	m_Command[sizeof(DWORD)]=KR_SOCKET_GETVIDEOSETTINGSLIST;
	SEND_COMMAND();
}

/////////////////////////////////////////////////////////////////////////////
void CSocketManager::GetVideoSettingsRecorded(void)
{
	SET_COMMAND_SIZE(KR_HEADER_SIZE);
	m_Command[sizeof(DWORD)]=KR_SOCKET_GETVIDEOSETTINGRECORDED;
	SEND_COMMAND();
}

/////////////////////////////////////////////////////////////////////////////
void CSocketManager::SetVideoSettingByName(char* Name)
{
	SET_COMMAND_SIZE(KR_HEADER_SIZE+(DWORD)strlen(Name)+1);
	m_Command[sizeof(DWORD)]=KR_SOCKET_SETVIDEOSETTINGBYNAME;
	memcpy(&m_Command[KR_HEADER_SIZE],Name,strlen(Name)+1);
	SEND_COMMAND();
}

/////////////////////////////////////////////////////////////////////////////
void CSocketManager::SetLaserPermanent(bool bPermanent)
{
	SET_COMMAND_SIZE(KR_HEADER_SIZE+1);
	m_Command[sizeof(DWORD)]=KR_SOCKET_SETLASERPERMANENT;
	m_Command[KR_HEADER_SIZE]=bPermanent;
	SEND_COMMAND();
}

/////////////////////////////////////////////////////////////////////////////
void CSocketManager::SetBlueLED(bool bOn)
{
	SET_COMMAND_SIZE(KR_HEADER_SIZE+1);
	m_Command[sizeof(DWORD)]=KR_SOCKET_SETBLUELED;
	m_Command[KR_HEADER_SIZE]=bOn;
	SEND_COMMAND();
}

/////////////////////////////////////////////////////////////////////////////
void CSocketManager::SetHardwareFilter(BYTE Filter)
{
	SET_COMMAND_SIZE(KR_HEADER_SIZE+1);
	m_Command[sizeof(DWORD)]=KR_SOCKET_SETHARDWAREFILTER;
	m_Command[KR_HEADER_SIZE]=Filter;
	SEND_COMMAND();
}

/////////////////////////////////////////////////////////////////////////////
//	Set the trigger duration
/////////////////////////////////////////////////////////////////////////////
void CSocketManager::SetExternalTriggerDuration(WORD TriggerDuration)
{
	SET_COMMAND_SIZE(KR_HEADER_SIZE+sizeof(WORD));
	m_Command[sizeof(DWORD)]=KR_SOCKET_SETEXTERNALTRIGGERDURATION;
	memcpy(&m_Command[KR_HEADER_SIZE],&TriggerDuration,sizeof(WORD));
	SEND_COMMAND();
}

/////////////////////////////////////////////////////////////////////////////
void CSocketManager::SetAcquisitionDelay(WORD Delay)
{
	SET_COMMAND_SIZE(KR_HEADER_SIZE+sizeof(WORD));
	m_Command[sizeof(DWORD)]=KR_SOCKET_SETACQUISITIONDELAY;
	memcpy(&m_Command[KR_HEADER_SIZE],&Delay,sizeof(WORD));
	SEND_COMMAND();
}

/////////////////////////////////////////////////////////////////////////////
void CSocketManager::OpenDrawingParametersWindow(void)
{
	SET_COMMAND_SIZE(KR_HEADER_SIZE);
	m_Command[sizeof(DWORD)]=KR_SOCKET_OPENDRAWINGPARAMETERSWINDOW;
	SEND_COMMAND();
}

/////////////////////////////////////////////////////////////////////////////
void CSocketManager::SetSphereCenter(double x,double y,double z)
{
	SET_COMMAND_SIZE(KR_HEADER_SIZE+3*sizeof(double));
	m_Command[sizeof(DWORD)]=KR_SOCKET_SETSPHERECENTER;
	memcpy(&m_Command[KR_HEADER_SIZE],&x,sizeof(double));
	memcpy(&m_Command[KR_HEADER_SIZE+sizeof(double)],&y,sizeof(double));
	memcpy(&m_Command[KR_HEADER_SIZE+2*sizeof(double)],&z,sizeof(double));
	SEND_COMMAND();
}

/////////////////////////////////////////////////////////////////////////////
void CSocketManager::SphereCalibration(KR_LASER_LINE* CalibrationScan, double* CalibrationMatrix, bool Type5axes, long SupportDirection, double SphereDiameter)
{
	SET_COMMAND_SIZE(KR_HEADER_SIZE+16*sizeof(double)+sizeof(bool)+sizeof(long)+sizeof(double));
	m_Command[sizeof(DWORD)]=KR_SOCKET_SPHERECALIBRATIONBEGIN;
	memcpy(&m_Command[KR_HEADER_SIZE],CalibrationMatrix,16*sizeof(double));
	memcpy(&m_Command[KR_HEADER_SIZE+16*sizeof(double)],&Type5axes,sizeof(bool));
	memcpy(&m_Command[KR_HEADER_SIZE+16*sizeof(double)+sizeof(bool)],&SupportDirection,sizeof(long));
	memcpy(&m_Command[KR_HEADER_SIZE+16*sizeof(double)+sizeof(bool)+sizeof(long)],&SphereDiameter,sizeof(double));
	SEND_COMMAND();

	for (KR_LASER_LINE* LaserLine=CalibrationScan; LaserLine; LaserLine=LaserLine->NextLine)
	{
		SET_COMMAND_SIZE(KR_HEADER_SIZE + sizeof(DWORD) + 16*sizeof(double) + sizeof(BYTE) + (3*sizeof(double)*LaserLine->SizeA) + sizeof(DWORD) + (3*sizeof(double)*LaserLine->SizeB));
		m_Command[sizeof(DWORD)]=KR_SOCKET_SPHERECALIBRATIONADDLASERLINE;
		memcpy(&m_Command[KR_HEADER_SIZE],&(LaserLine->SizeA),sizeof(DWORD));
		memcpy(&m_Command[KR_HEADER_SIZE+sizeof(DWORD)],LaserLine->Matrix,16*sizeof(double));
		memcpy(&m_Command[KR_HEADER_SIZE+sizeof(DWORD)+16*sizeof(double)],&(LaserLine->IO),sizeof(BYTE));
		memcpy(&m_Command[KR_HEADER_SIZE+sizeof(DWORD)+16*sizeof(double)+sizeof(BYTE)],LaserLine->Points,3*sizeof(double)*LaserLine->SizeA);
		memcpy(&m_Command[KR_HEADER_SIZE+sizeof(DWORD)+16*sizeof(double)+sizeof(BYTE)+(3*sizeof(double)*LaserLine->SizeA)],&(LaserLine->SizeB),sizeof(DWORD));
		memcpy(&m_Command[KR_HEADER_SIZE+sizeof(DWORD)+16*sizeof(double)+sizeof(BYTE)+(3*sizeof(double)*LaserLine->SizeA)+sizeof(DWORD)],LaserLine->Points+(3*LaserLine->SizeA),3*sizeof(double)*LaserLine->SizeB);
		SEND_COMMAND();
	}

	SET_COMMAND_SIZE(KR_HEADER_SIZE);
	m_Command[sizeof(DWORD)]=KR_SOCKET_SPHERECALIBRATIONEND;
	SEND_COMMAND();
}

/////////////////////////////////////////////////////////////////////////////
//	Open the Positioning window
//	bVisible : false if you want the window to be created with the "hidden"
//	status, else true
//	Frequency : frequency of the scan (used only if ScanMethod is
//	KR_SCAN_METHOD_ASYNCHRONOUS)
//	SphereDiameter : diameter of the calibration sphere, in mm
//					 (0 if unknown)
//	bAllowAddDelIndex : enable or not two buttons, that allow the user
//	to add and del indexes directly from the positioning window
//	bDisplayOnOffButtons : enable or not two buttons, that allow the user
//	to switch on or off the scanner
//	hWndParent : handle of the parent window
/////////////////////////////////////////////////////////////////////////////
void CSocketManager::OpenPositioningWindowExt(KR_INDEX_EXT *Index,BYTE NbIndex,
											  BYTE ScanMethod,bool bVisible,
											  BYTE Frequency,
											  double SphereDiameter,
											  bool bAllowAddDelIndex,
											  bool bDisplayOnOffButtons,
											  HWND hWndParent,
											  bool bAutomatic,
											  BYTE SupportDirection)
{
	SET_COMMAND_SIZE(KR_HEADER_SIZE+1+1+sizeof(KR_INDEX_EXT)*NbIndex+1+1+sizeof(double)+1+1+sizeof(HWND)+1+1);
	m_Command[sizeof(DWORD)]	= KR_SOCKET_OPENPOSITIONINGWINDOWEXT;
	m_Command[KR_HEADER_SIZE]	= ScanMethod;
	m_Command[KR_HEADER_SIZE+1]	= NbIndex;
	memcpy(&m_Command[KR_HEADER_SIZE+2],Index,sizeof(KR_INDEX_EXT)*NbIndex);
	m_Command[KR_HEADER_SIZE+2+sizeof(KR_INDEX_EXT)*NbIndex]=bVisible;
	m_Command[KR_HEADER_SIZE+2+sizeof(KR_INDEX_EXT)*NbIndex+1]=Frequency;
	memcpy(&m_Command[sizeof(DWORD)+3+sizeof(KR_INDEX_EXT)*NbIndex+1+1],&SphereDiameter,sizeof(double));
	m_Command[KR_HEADER_SIZE+2+sizeof(KR_INDEX_EXT)*NbIndex+1+1+sizeof(double)]=bAllowAddDelIndex;
	m_Command[KR_HEADER_SIZE+2+sizeof(KR_INDEX_EXT)*NbIndex+1+1+sizeof(double)+1]=bDisplayOnOffButtons;
	memcpy(&m_Command[KR_HEADER_SIZE+2+sizeof(KR_INDEX_EXT)*NbIndex+1+1+sizeof(double)+1+1],&hWndParent,sizeof(HWND));
	m_Command[KR_HEADER_SIZE+2+sizeof(KR_INDEX_EXT)*NbIndex+1+1+sizeof(double)+1+1+sizeof(HWND)]=bAutomatic;
	m_Command[KR_HEADER_SIZE+2+sizeof(KR_INDEX_EXT)*NbIndex+1+1+sizeof(double)+1+1+sizeof(HWND)+1]=SupportDirection;
	SEND_COMMAND();
}

//	The following method is kept for older applications only.
//	If you are making a new program or willing to update an older one, you should use the above method,
//	especially if you want to use several head positions (indices).
void CSocketManager::OpenPositioningWindow(KR_INDEX *Index,BYTE NbIndex,
										   BYTE ScanMethod,bool bVisible,
										   BYTE Frequency,
										   double SphereDiameter,
										   bool bAllowAddDelIndex,
										   bool bDisplayOnOffButtons,
										   HWND hWndParent,
										   bool bAutomatic,
										   BYTE SupportDirection)
{
	SET_COMMAND_SIZE(KR_HEADER_SIZE+1+1+sizeof(KR_INDEX)*NbIndex+1+1+sizeof(double)+1+1+sizeof(HWND)+1+1);
	m_Command[sizeof(DWORD)]	= KR_SOCKET_OPENPOSITIONINGWINDOW;
	m_Command[KR_HEADER_SIZE]	= ScanMethod;
	m_Command[KR_HEADER_SIZE+1]	= NbIndex;
	memcpy(&m_Command[KR_HEADER_SIZE+2],Index,sizeof(KR_INDEX)*NbIndex);
	m_Command[KR_HEADER_SIZE+2+sizeof(KR_INDEX)*NbIndex]=bVisible;
	m_Command[KR_HEADER_SIZE+2+sizeof(KR_INDEX)*NbIndex+1]=Frequency;
	memcpy(&m_Command[sizeof(DWORD)+3+sizeof(KR_INDEX)*NbIndex+1+1],&SphereDiameter,sizeof(double));
	m_Command[KR_HEADER_SIZE+2+sizeof(KR_INDEX)*NbIndex+1+1+sizeof(double)]=bAllowAddDelIndex;
	m_Command[KR_HEADER_SIZE+2+sizeof(KR_INDEX)*NbIndex+1+1+sizeof(double)+1]=bDisplayOnOffButtons;
	memcpy(&m_Command[KR_HEADER_SIZE+2+sizeof(KR_INDEX)*NbIndex+1+1+sizeof(double)+1+1],&hWndParent,sizeof(HWND));
	m_Command[KR_HEADER_SIZE+2+sizeof(KR_INDEX)*NbIndex+1+1+sizeof(double)+1+1+sizeof(HWND)]=bAutomatic;
	m_Command[KR_HEADER_SIZE+2+sizeof(KR_INDEX)*NbIndex+1+1+sizeof(double)+1+1+sizeof(HWND)+1]=SupportDirection;
	SEND_COMMAND();
}

/////////////////////////////////////////////////////////////////////////////
void CSocketManager::IsPositioningWindowOpened(void)
{
	SET_COMMAND_SIZE(KR_HEADER_SIZE);
	m_Command[sizeof(DWORD)]=KR_SOCKET_ISPOSITIONINGWINDOWOPENED;
	SEND_COMMAND();
}

/////////////////////////////////////////////////////////////////////////////
//	Stop the acquisition in the positioning window
//	(BeginAcquisition and EndAcquisition are used to run and pause the scan
//	in the positioning window)
/////////////////////////////////////////////////////////////////////////////
void CSocketManager::StopScan()
{
	SET_COMMAND_SIZE(KR_HEADER_SIZE);
	m_Command[sizeof(DWORD)]=KR_SOCKET_STOPSCAN;
	SEND_COMMAND();
}

/////////////////////////////////////////////////////////////////////////////
//	Get the toolpath advised by Kreon during the positioning process
//	Note : SphereDiameter=-1 means using the value typed by the user
//	in the positioning window
/////////////////////////////////////////////////////////////////////////////
void CSocketManager::GetPositioningToolpath(double SphereDiameter)
{
	m_CommandSize=KR_HEADER_SIZE+sizeof(double);
	memcpy(m_Command,&m_CommandSize,sizeof(DWORD));
	m_Command[sizeof(DWORD)]	= KR_SOCKET_GETPOSITIONINGTOOLPATH;
	memcpy(&m_Command[KR_HEADER_SIZE],&SphereDiameter,sizeof(double));
	OnSend(m_Command,m_CommandSize);
}

/////////////////////////////////////////////////////////////////////////////
void CSocketManager::StartScannerAcquisitionInPositioningWindow(void)
{
	SET_COMMAND_SIZE(KR_HEADER_SIZE);
	m_Command[sizeof(DWORD)]=KR_SOCKET_STARTSCANNERACQPOSITIONING;
	SEND_COMMAND();
}

/////////////////////////////////////////////////////////////////////////////
//	Get the calibration matrix number "index"
//	index from 0 to n
/////////////////////////////////////////////////////////////////////////////
void CSocketManager::GetCalibrationMatrix(BYTE index)
{
	SET_COMMAND_SIZE(KR_HEADER_SIZE+1);
	m_Command[sizeof(DWORD)]  =KR_SOCKET_GETCALIBRATIONMATRIX;
	m_Command[KR_HEADER_SIZE]=index;
	SEND_COMMAND();
}

/////////////////////////////////////////////////////////////////////////////
//	Set the machine position
/////////////////////////////////////////////////////////////////////////////
void CSocketManager::SetMachinePosition(double* PositionMatrix)
{
	SET_COMMAND_SIZE(KR_HEADER_SIZE + 16*sizeof(double));
	m_Command[sizeof(DWORD)]=KR_SOCKET_SETMACHINEPOSITION;
	memcpy(&m_Command[KR_HEADER_SIZE],PositionMatrix,16*sizeof(double));
	SEND_COMMAND();
}

void CSocketManager::SetNoMachinePositionAvailable()
{
	SET_COMMAND_SIZE(KR_HEADER_SIZE);
	m_Command[sizeof(DWORD)]=KR_SOCKET_SETNOMACHINEPOSITIONAVAILABLE;
	SEND_COMMAND();
}

/////////////////////////////////////////////////////////////////////////////
//	Initialize acquisition
/////////////////////////////////////////////////////////////////////////////
void CSocketManager::InitAcquisition(BYTE ScanMethod,BYTE Frequency,float Step,
									 double* PositioningMatrix,UINT Decimation)
{
	SET_COMMAND_SIZE(KR_HEADER_SIZE+2+sizeof(float)+16*sizeof(double)+sizeof(DWORD));
	m_Command[sizeof(DWORD)]  =KR_SOCKET_INITACQUISITION;
	m_Command[KR_HEADER_SIZE]=ScanMethod;
	m_Command[KR_HEADER_SIZE+1]=Frequency;
	memcpy(&m_Command[KR_HEADER_SIZE+2],&Step,sizeof(float));
	memcpy(&m_Command[KR_HEADER_SIZE+2+sizeof(float)],PositioningMatrix,16*sizeof(double));
	DWORD dwDecimation=Decimation;
	memcpy(&m_Command[KR_HEADER_SIZE+2+sizeof(float)+16*sizeof(double)], &dwDecimation, sizeof(DWORD));
	SEND_COMMAND();
}

//	Extension of parameter Frequency to support higher frequencies (for fast scanners)
void CSocketManager::InitAcquisition(BYTE ScanMethod,int Frequency,float Step,
									 double* PositioningMatrix,UINT Decimation)
{
	SET_COMMAND_SIZE(KR_HEADER_SIZE+1+sizeof(int)+sizeof(float)+16*sizeof(double)+sizeof(DWORD));
	m_Command[sizeof(DWORD)]  =KR_SOCKET_INITACQUISITION;
	m_Command[KR_HEADER_SIZE]=ScanMethod;
	memcpy(&m_Command[KR_HEADER_SIZE+1],&Frequency,sizeof(int));
	memcpy(&m_Command[KR_HEADER_SIZE+1+sizeof(int)],&Step,sizeof(float));
	memcpy(&m_Command[KR_HEADER_SIZE+1+sizeof(int)+sizeof(float)],PositioningMatrix,16*sizeof(double));
	DWORD dwDecimation=Decimation;
	memcpy(&m_Command[KR_HEADER_SIZE+1+sizeof(int)+sizeof(float)+16*sizeof(double)], &dwDecimation, sizeof(DWORD));
	SEND_COMMAND();
}

//	Replacement of PositioningMatrix with the Index structure to provide more information about the positioning.
//	This is especially required for 3-axis machines if you plan to support Kreon's axis compensation method.
void CSocketManager::InitAcquisition(BYTE ScanMethod,int Frequency,float Step,
									 KR_INDEX_EXT * Index,UINT Decimation)
{
	SET_COMMAND_SIZE(KR_HEADER_SIZE+1+sizeof(int)+sizeof(float)+sizeof(KR_INDEX_EXT)+sizeof(DWORD));
	m_Command[sizeof(DWORD)]  =KR_SOCKET_INITACQUISITION;
	m_Command[KR_HEADER_SIZE]=ScanMethod;
	memcpy(&m_Command[KR_HEADER_SIZE+1],&Frequency,sizeof(int));
	memcpy(&m_Command[KR_HEADER_SIZE+1+sizeof(int)],&Step,sizeof(float));
	memcpy(&m_Command[KR_HEADER_SIZE+1+sizeof(int)+sizeof(float)],Index,sizeof(KR_INDEX_EXT));
	DWORD dwDecimation=Decimation;
	memcpy(&m_Command[KR_HEADER_SIZE+1+sizeof(int)+sizeof(float)+sizeof(KR_INDEX_EXT)], &dwDecimation, sizeof(DWORD));
	SEND_COMMAND();
}

/////////////////////////////////////////////////////////////////////////////
//	Start the acquisition
/////////////////////////////////////////////////////////////////////////////
void CSocketManager::BeginAcquisition()
{
	SET_COMMAND_SIZE(KR_HEADER_SIZE);
	m_Command[sizeof(DWORD)]  =KR_SOCKET_BEGINACQUISITION;
	SEND_COMMAND();
}

/////////////////////////////////////////////////////////////////////////////
//	Stop the acquisition
/////////////////////////////////////////////////////////////////////////////
void CSocketManager::EndAcquisition()
{
	SET_COMMAND_SIZE(KR_HEADER_SIZE);
	m_Command[sizeof(DWORD)]=KR_SOCKET_ENDACQUISITION;
	SEND_COMMAND();
}

/////////////////////////////////////////////////////////////////////////////
//	Get the laser lines captured
/////////////////////////////////////////////////////////////////////////////
void CSocketManager::GetXYZLaserLine()
{
	SET_COMMAND_SIZE(KR_HEADER_SIZE);
	m_Command[sizeof(DWORD)]=KR_SOCKET_GETXYZLASERLINE;
	SEND_COMMAND();
}

/////////////////////////////////////////////////////////////////////////////
void CSocketManager::ProbeOpenCalibrationWindow(KR_TOUCH_PROBE* Probe,BYTE NbProbes,
												bool bVisible, double SphereDiameter, bool bAllowAddDel)
{
	SET_COMMAND_SIZE(KR_HEADER_SIZE+1+sizeof(KR_TOUCH_PROBE)*NbProbes+1+sizeof(double)+1);
	m_Command[sizeof(DWORD)]=KR_SOCKET_PROBEOPENCALIBRATIONWINDOW;
	m_Command[KR_HEADER_SIZE]=NbProbes;
	memcpy(&m_Command[KR_HEADER_SIZE+1],Probe,sizeof(KR_TOUCH_PROBE)*NbProbes);
	m_Command[KR_HEADER_SIZE+1+sizeof(KR_TOUCH_PROBE)*NbProbes]=bVisible;
	memcpy(&m_Command[KR_HEADER_SIZE+1+(sizeof(KR_TOUCH_PROBE)*NbProbes)+1],&SphereDiameter,sizeof(double));
	m_Command[KR_HEADER_SIZE+1+sizeof(KR_TOUCH_PROBE)*NbProbes+1+sizeof(double)]=bAllowAddDel;
	SEND_COMMAND();
}

/////////////////////////////////////////////////////////////////////////////
void CSocketManager::BeginProbeCalibration(BYTE CalibrationMethod)
{
	SET_COMMAND_SIZE(KR_HEADER_SIZE+1);
	m_Command[sizeof(DWORD)]=KR_SOCKET_PROBECALIBRATIONBEGIN;
	m_Command[KR_HEADER_SIZE]=CalibrationMethod;
	SEND_COMMAND();
}

/////////////////////////////////////////////////////////////////////////////
void CSocketManager::EndProbeCalibration(void)
{
	SET_COMMAND_SIZE(KR_HEADER_SIZE);
	m_Command[sizeof(DWORD)]=KR_SOCKET_PROBECALIBRATIONEND;
	SEND_COMMAND();
}

/////////////////////////////////////////////////////////////////////////////
void CSocketManager::ProbeAddPosition(double* Matrix,bool bSound)
{
	SET_COMMAND_SIZE(KR_HEADER_SIZE+16*sizeof(double)+1);
	m_Command[sizeof(DWORD)]=KR_SOCKET_PROBEADDPOSITION;
	memcpy(&m_Command[KR_HEADER_SIZE],Matrix,16*sizeof(double));
	m_Command[KR_HEADER_SIZE+16*sizeof(double)]=bSound;
	SEND_COMMAND();
}

/////////////////////////////////////////////////////////////////////////////
void CSocketManager::ProbeDelLastPosition(bool bSound)
{
	SET_COMMAND_SIZE(KR_HEADER_SIZE+1);
	m_Command[sizeof(DWORD)]=KR_SOCKET_PROBEDELLASTPOSITION;
	m_Command[KR_HEADER_SIZE]=bSound;
	SEND_COMMAND();
}

/////////////////////////////////////////////////////////////////////////////
void CSocketManager::ProbeGetCalibration(BYTE ProbeIndex)
{
	SET_COMMAND_SIZE(KR_HEADER_SIZE+1);
	m_Command[sizeof(DWORD)]	= KR_SOCKET_PROBEGETCALIBRATION;
	m_Command[KR_HEADER_SIZE]	= ProbeIndex;
	SEND_COMMAND();
}

/////////////////////////////////////////////////////////////////////////////
void CSocketManager::GetSensorType(void)
{
	SET_COMMAND_SIZE(KR_HEADER_SIZE);
	m_Command[sizeof(DWORD)]=KR_SOCKET_GETSENSORTYPE;
	SEND_COMMAND();
}

/////////////////////////////////////////////////////////////////////////////
void CSocketManager::SetSensorState(bool bOn)
{
	SET_COMMAND_SIZE(KR_HEADER_SIZE+1);
	m_Command[sizeof(DWORD)]=KR_SOCKET_SETSENSORSTATE;
	m_Command[KR_HEADER_SIZE]=bOn;
	SEND_COMMAND();
}

/////////////////////////////////////////////////////////////////////////////
void CSocketManager::GetSensorState(void)
{
	SET_COMMAND_SIZE(KR_HEADER_SIZE);
	m_Command[sizeof(DWORD)]=KR_SOCKET_GETSENSORSTATE;
	SEND_COMMAND();
}

/////////////////////////////////////////////////////////////////////////////
void CSocketManager::OpenScannerPropertiesWindow(HWND hWndParent)
{
	SET_COMMAND_SIZE(KR_HEADER_SIZE+sizeof(HWND));
	m_Command[sizeof(DWORD)]=KR_SOCKET_OPENSCANNERPROPERTIESWINDOW;
	memcpy(&m_Command[KR_HEADER_SIZE],&hWndParent,sizeof(HWND));
	SEND_COMMAND();
}

/////////////////////////////////////////////////////////////////////////////
void CSocketManager::GetHardwareFilter(void)
{
	SET_COMMAND_SIZE(KR_HEADER_SIZE);
	m_Command[sizeof(DWORD)]=KR_SOCKET_GETHARDWAREFILTER;
	SEND_COMMAND();
}

/////////////////////////////////////////////////////////////////////////////
void CSocketManager::OpenLicenseDialog(void)
{
	SET_COMMAND_SIZE(KR_HEADER_SIZE);
	m_Command[sizeof(DWORD)]=KR_SOCKET_LICENSE;
	SEND_COMMAND();
}

/////////////////////////////////////////////////////////////////////////////
void CSocketManager::OpenLicenseDialog(HWND hWndParent)
{
	SET_COMMAND_SIZE(KR_HEADER_SIZE+sizeof(hWndParent));
	m_Command[sizeof(DWORD)]=KR_SOCKET_LICENSE;
	memcpy(&m_Command[KR_HEADER_SIZE],&hWndParent,sizeof(hWndParent));
	SEND_COMMAND();
}

/////////////////////////////////////////////////////////////////////////////
void CSocketManager::SetOutput(BYTE output)
{
	SET_COMMAND_SIZE(KR_HEADER_SIZE+1);
	m_Command[sizeof(DWORD)]=KR_SOCKET_SETOUTPUT;
	m_Command[KR_HEADER_SIZE] = output;
	SEND_COMMAND();
}

/////////////////////////////////////////////////////////////////////////////
void CSocketManager::GetInput()
{
	SET_COMMAND_SIZE(KR_HEADER_SIZE);
	m_Command[sizeof(DWORD)]=KR_SOCKET_GETINPUT;
	SEND_COMMAND();
}

/////////////////////////////////////////////////////////////////////////////
bool CSocketManager::OnDataSocket(void)
{
	char	Buffer[4096];
	int		NbBytesReceived=0;

	NbBytesReceived=recv(m_Socket,Buffer,sizeof(Buffer),0);
	if (!NbBytesReceived || (NbBytesReceived==SOCKET_ERROR))
		return false;	//	Nothing to read

	//	Is there enough space in the circular buffer to store the new message?
	if (CircBuf.SizeFree()>=NbBytesReceived)
	{
		//	Yes... store the new message in the circular buffer
		CircBuf.Write(Buffer,NbBytesReceived);
	}
	else
	{
		//	No... We need to free the buffer by processing the message(s)
		//	we already have in it. The message will be processed later.
	}

	//	Can we read the header?
	//	The header is composed of 5 bytes :
	//		4 bytes (DWORD) for the complete size of the message (including the header)
	//		1 byte for the id of the command (KR_SOCKET_CALLBACK, KR_SOCKET_INITKREON...)
	bool bRemainingMessage=true;
	while (bRemainingMessage)
	{
		DWORD AvailableSize=CircBuf.SizeBuffered();
		if (AvailableSize>=KR_HEADER_SIZE)
		{
			//	Yes...
			//	Read the header
			//	We use the "ReadWithoutRemoving function", because if the message is not
			//	complete, we will make a new "ReadWithoutRemoving" the next time we will
			//	receive something in the socket
			char Header[KR_HEADER_SIZE];
			CircBuf.ReadWithoutRemoving(Header,KR_HEADER_SIZE);

			DWORD CommandSize;
			memcpy(&CommandSize,Header,sizeof(DWORD));

			//	Can we read the complete message?
			if (AvailableSize>=CommandSize)
			{
				//	Yes...
				//	Since the message is complete in the circular buffer, we can read it
				char *Command=new char [CommandSize];
				CircBuf.Read(Command,CommandSize);
				//	We have a message to process...
				m_KreonCallBack(Command+sizeof(DWORD));

				if (Command[sizeof(DWORD)] == KR_SOCKET_ENDKREON)
				{
					CircBuf.RemoveBuffer();

					//	Close the socket
					closesocket(m_Socket);
					m_Socket=INVALID_SOCKET;
				}

				delete [] Command;
			}
			else
			{
				//	Since the message is not complete, we know that we can't
				//	find another message in the circular buffer
				bRemainingMessage=false;
			}
		}
		else
			bRemainingMessage=false;
	}
	return true;
}

/////////////////////////////////////////////////////////////////////////////
void CSocketManager::OnSend(char *Buffer,int Size)
{
	DWORD	SizeToEmit	 = Size;
	char*	BufferToEmit = Buffer;
	MSG	msg;
	DWORD	TimeOutMs=1;
	DWORD	StartTime;

	while (SizeToEmit)
	{
		int NbBytesSent=send(m_Socket,BufferToEmit,SizeToEmit,0);
		if (NbBytesSent!=SOCKET_ERROR)
		{
			SizeToEmit	 -= NbBytesSent;
			BufferToEmit += NbBytesSent;
		}
		else
		{
			if (WSAGetLastError()==WSAECONNRESET)
				return;
			StartTime=GetTickCount();
			while (GetTickCount()-StartTime<TimeOutMs)
			{
				PeekMessage(&msg,0,0,0,PM_REMOVE);
				DispatchMessage(&msg);
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
//                       MISCELLANEOUS COMPUTATIONS                        //
/////////////////////////////////////////////////////////////////////////////

#define MAX_POINTS_TO_SEND 1000
void CSocketManager::SendInputPoints(double pPoints[], DWORD nbPoints)
{
	SET_COMMAND_SIZE(KR_HEADER_SIZE+sizeof(DWORD));
	m_Command[sizeof(DWORD)]=KR_SOCKET_SETNBINPUTPOINTS;
	memcpy(&m_Command[KR_HEADER_SIZE],&nbPoints,sizeof(DWORD));
	SEND_COMMAND();

	DWORD remainingPoints = nbPoints;
	DWORD pointsToSend;
	while (remainingPoints)
	{
		if (remainingPoints>MAX_POINTS_TO_SEND)
			pointsToSend = MAX_POINTS_TO_SEND;
		else
			pointsToSend = remainingPoints;
		SET_COMMAND_SIZE(KR_HEADER_SIZE + sizeof(DWORD) + pointsToSend*3*sizeof(double));
		m_Command[sizeof(DWORD)]=KR_SOCKET_ADDINPUTPOINTS;
		memcpy(&m_Command[KR_HEADER_SIZE],&pointsToSend,sizeof(DWORD));
		memcpy(&m_Command[KR_HEADER_SIZE+sizeof(DWORD)],pPoints+3*(nbPoints-remainingPoints),pointsToSend*3*sizeof(double));
		SEND_COMMAND();
		remainingPoints -= pointsToSend;
	}
}

void CSocketManager::FitPlane(double pPoints[], DWORD nbPoints)
{
	SendInputPoints(pPoints,nbPoints);
	SET_COMMAND_SIZE(KR_HEADER_SIZE);
	m_Command[sizeof(DWORD)]=KR_SOCKET_FITPLANE;
	SEND_COMMAND();
}

void CSocketManager::FitPoint(double pPoints[], DWORD nbPoints)
{
	SendInputPoints(pPoints,nbPoints);
	SET_COMMAND_SIZE(KR_HEADER_SIZE);
	m_Command[sizeof(DWORD)]=KR_SOCKET_FITPOINT;
	SEND_COMMAND();
}

void CSocketManager::FitSphere(double pPoints[], DWORD nbPoints)
{
	SendInputPoints(pPoints,nbPoints);
	SET_COMMAND_SIZE(KR_HEADER_SIZE);
	m_Command[sizeof(DWORD)]=KR_SOCKET_FITSPHERE;
	SEND_COMMAND();
}

void CSocketManager::FitCircle(double pPoints[], DWORD nbPoints)
{
	SendInputPoints(pPoints,nbPoints);
	SET_COMMAND_SIZE(KR_HEADER_SIZE);
	m_Command[sizeof(DWORD)]=KR_SOCKET_FITCIRCLE;
	SEND_COMMAND();
}

void CSocketManager::FitCylinder(double pPoints[], DWORD nbPoints)
{
	SendInputPoints(pPoints,nbPoints);
	SET_COMMAND_SIZE(KR_HEADER_SIZE);
	m_Command[sizeof(DWORD)]=KR_SOCKET_FITCYLINDER;
	SEND_COMMAND();
}

void CSocketManager::FitLine(double pPoints[], DWORD nbPoints)
{
	SendInputPoints(pPoints,nbPoints);
	SET_COMMAND_SIZE(KR_HEADER_SIZE);
	m_Command[sizeof(DWORD)]=KR_SOCKET_FITLINE;
	SEND_COMMAND();
}

void CSocketManager::Smoother(double pPoints[], DWORD nbPoints,
							  double smootherRadius, double filterRadius,
							  bool bSmooth, bool bFilter)
{
	SendInputPoints(pPoints,nbPoints);
	SET_COMMAND_SIZE(KR_HEADER_SIZE+2*sizeof(double)+2);
	m_Command[sizeof(DWORD)]=KR_SOCKET_SMOOTHER;
	memcpy(&m_Command[KR_HEADER_SIZE],&smootherRadius,sizeof(double));
	memcpy(&m_Command[KR_HEADER_SIZE+sizeof(double)],&filterRadius,sizeof(double));
	m_Command[KR_HEADER_SIZE+2*sizeof(double)]=bSmooth;
	m_Command[KR_HEADER_SIZE+2*sizeof(double)+1]=bFilter;
	SEND_COMMAND();
}

void CSocketManager::GetProcessedPoints(DWORD nbPoints)
{
	DWORD remainingPoints = nbPoints;
	DWORD pointsToSend;
	while (remainingPoints)
	{
		if (remainingPoints>MAX_POINTS_TO_SEND)
			pointsToSend = MAX_POINTS_TO_SEND;
		else
			pointsToSend = remainingPoints;
		SET_COMMAND_SIZE(KR_HEADER_SIZE + sizeof(DWORD));
		m_Command[sizeof(DWORD)]=KR_SOCKET_GETPROCESSEDPOINTS;
		memcpy(&m_Command[KR_HEADER_SIZE],&pointsToSend,sizeof(DWORD));
		SEND_COMMAND();
		remainingPoints -= pointsToSend;
	}
}
