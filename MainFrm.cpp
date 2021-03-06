//	--------------------------------------------------------------
//	MainFrm.cpp : main file of the KREON Toolkit example 5 project
//
//
//		This example shows how to work with a Zephyr sensor with
//	the Kreon toolkit in "server" mode and an arm to make
//	digitizations.
//		A documentation is available in ArmManager.h to know how
//	to work with arms.
//
//	Notes : the library "WSock32.lib" is needed to build this
//			sample (see project settings / link)
//			KreonServer.exe, KreonCore.dll, KR<ArmName>.dll and
//			KRZLS.dll are needed to run it.
//	--------------------------------------------------------------
#include "stdafx.h"
#include "ToolkitSample5.h"
#include "MainFrm.h"
#include "ArmManager.h"
#include "KreonServer.h"
#include "SocketManager.h"
#include "KR_Specific.h"
#include "resource.h"
#include <fstream> //matthias
#include <thread>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



//	Arm's buttons states polling
#define	ARM_BUTTONS_TIMER_ID		1
#define	ARM_BUTTONS_TIMER_PERIOD	50		//	ms
//	Arm's touch probe position polling
#define	ARM_TOUCHPROBE_TIMER_ID		2
#define	ARM_TOUCHPROBE_TIMER_PERIOD	100		//	ms




//#define	ARM_NAME			_T("Cimcore")	// Romer-Cimcore Stinger2/3000i/Infinite
//#define	ARM_NAME			_T("Faro")		// Faro Silver/Gold/Millenium
//#define	ARM_NAME			_T("FaroUSB")	// Faro Platinum/Titanium/Advantage/Quantum/Fusion
//#define	ARM_NAME			_T("Garda")		// Garda
//#define	ARM_NAME			_T("Romer")		// Romer France System6/Armony (Serial)
//#define	ARM_NAME			_T("RomerEth")	// Romer France Sigma (Ethernet)
#define	ARM_NAME			_T("VXElementsWrapper")


KR_CALIBRATION_FILE_LIST*			volatile st_CalibrationFileList;
CSocketManager						*SocketManager;
CArmManager							*ArmManager;
CChildView							CMainFrame::m_wndView;
DWORD								StartTime;
HWND								CMainFrame::m_StatichWnd;
double								PositioningMatrix[16];
bool								g_bButtonState[4]={0,0,0,0};
int									NbLines;


/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_KREONTOOLKIT_VIDEOSETUP, OnKreontoolkitVideosetup)
	ON_COMMAND(ID_KREONTOOLKIT_STARTSCAN, OnKreontoolkitStartscan)
	ON_COMMAND(ID_KREONTOOLKIT_POSITIONING, OnKreontoolkitPositioning)
	ON_COMMAND(ID_KREONTOOLKIT_PAUSESCAN, OnKreontoolkitPausescan)
	ON_UPDATE_COMMAND_UI(ID_KREONTOOLKIT_STARTSCAN, OnUpdateKreontoolkitStartscan)
	ON_UPDATE_COMMAND_UI(ID_KREONTOOLKIT_PAUSESCAN, OnUpdateKreontoolkitPausescan)
	ON_COMMAND(ID_KREONTOOLKIT_TOUCHPROBECALIBRATION, OnKreonToolkitProbeCalibration)
	ON_COMMAND(ID_KREONTOOLKIT_ARMPROPERTIES, OnKreonArmProperties)
	ON_COMMAND(ID_KREONTOOLKIT_SCANNERPROPERTIES, OnKreonScannerProperties)
	ON_COMMAND(ID_KREONTOOLKIT_LICENSE, OnKreontoolkitLicense)
	ON_COMMAND(ID_POSITIONINGDEVICE_COBOTTX2, OnPositioningDeviceCobot)
	ON_COMMAND(ID_POSITIONINGDEVICE_CTRACK, OnPositioningDeviceCtrack)
	ON_MESSAGE(WM_SOCKET_MESSAGE, OnDataSocket)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
CMainFrame::CMainFrame()
{
	ArmManager=NULL;
	m_bKreonInitialized=false;
	m_bCobotCommunicationInitialized = false;
	m_bScanning=false;

	positioningDevice = PA_Enums::CobotTx2Touch;
}

/////////////////////////////////////////////////////////////////////////////
CMainFrame::~CMainFrame()
{
	if (SocketManager)
	{
		// End the KREON system and the thread
		BYTE SensorOff=0;
		SocketManager->EndKreon(SensorOff);

		delete SocketManager;
		SocketManager=NULL;
	}
	if(ArmManager)
	{
		delete ArmManager;
		ArmManager=NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	// create a view to occupy the client area of the frame
	if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("Failed to create view window\n");
		return -1;
	}

	m_StatichWnd = m_hWnd;

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{
	// forward focus to the view window
	m_wndView.SetFocus();
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// let the view have first crack at the command
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CMainFrame::OnClose()
{
	if (m_bScanning)
	{
		SocketManager->EndAcquisition();
		ArmManager->EndAcquisition();
		m_bScanning=false;
		ArmManager->EndArm();

		MSG Msg;
		while (PeekMessage(&Msg,NULL,0,0,PM_REMOVE))
		{
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
			Sleep(1);
		}
		Sleep(500);
	}
	CFrameWnd::OnClose();
}

/////////////////////////////////////////////////////////////////////////////
void OnKreonCallback(char *Command)
{
	//	When the server receives a command, it runs it, and then, sends an ack to the client
	//	You should always read this ack to know if the command ran correctly or not.
	//	The answer is composed of a minimum of 3 bytes (except KR_SOCKET_CALLBACK):
	//	Command[0] = Name of the command (e.g. KR_SOCKET_INITKREON, KR_SOCKET_GETXYZLASERLINE...)
	//	Command[1] = Return value of the command (most of the time: true if ok, else false)
	//	Command[2] = New value of the "LastErrorCode"
	switch(Command[0])
	{
		case KR_SOCKET_INITKREON:
			if(!Command[1])
			{
				SocketManager->GetLastErrorString((BYTE)Command[2]);
			}
			break;

		case KR_SOCKET_GETLASTERRORSTRING:
			if(Command[1])
			{
				CString tmp;
#ifdef _UNICODE
				TCHAR	unicodeErrorMsg[1000];
				mbstowcs_s(NULL,unicodeErrorMsg,_countof(unicodeErrorMsg),&Command[3],strlen(&Command[3])+1);
				tmp.Format(_T("Error: %s"),unicodeErrorMsg);
#else
				tmp.Format(_T("Error: %s"),&Command[3]);
#endif
				AfxMessageBox(tmp);
			}
			break;

		case KR_SOCKET_SPHERECALIBRATION:
			if (Command[1])
			{
				// If you called this:
				// SocketManager->SphereCalibration(CalibrationScan,CalibrationMatrix,Type5axes,SupportDirection,SphereDiameter);
				// then, the following data items are returned here:
				double SphereCenter[3];
				double Sigma;
				double CalibrationMatrix[16];
				memcpy(SphereCenter,&Command[3],3*sizeof(double));
				memcpy(&Sigma,&Command[3+3*sizeof(double)],sizeof(double));
				memcpy(CalibrationMatrix,&Command[3+sizeof(double)+3*sizeof(double)],16*sizeof(double));

				CString output;
				output.Format(_T("SphereCalibration results:\nSigma = %f"),Sigma);
				AfxMessageBox(output);
			}
			else
			{
				AfxMessageBox(_T("SphereCalibration failed"));
				SocketManager->GetLastErrorString((BYTE)Command[2]);
			}
			break;

		case KR_SOCKET_GETCALIBRATIONFILELIST:
			{
				// Allocate memory for the list
				KR_CALIBRATION_FILE_LIST* list = new KR_CALIBRATION_FILE_LIST;
				memcpy(list,&Command[3],sizeof(KR_CALIBRATION_FILE_LIST));

				if (Command[1])
				{
					/*
					// This sample code displays the calibration file list in a message box
					CString output = "Calibration file list:\n";
					int offsetFilename = 0;	// point towards the first file name
					for (UINT i=0; i<list->nbFiles; i++)
					{
						output += list->list+offsetFilename; // Add the file name to the output string
						if (list->offsetCurrentFile==offsetFilename)
							 output += " (current)";
						output += "\n";
						offsetFilename += strlen(list->list+offsetFilename)+1; // point towards the next file name
					}
					AfxMessageBox(output);
					*/
				}

				st_CalibrationFileList = list; // tells CMainFrame::Init() to stop waiting for a reply from the server
			}
			break;

		case KR_SOCKET_OPENVIDEOSETTINGWINDOW:
		case KR_SOCKET_OPENPOSITIONINGWINDOW:
		case KR_SOCKET_OPENPOSITIONINGWINDOWEXT:
		case KR_SOCKET_PROBEOPENCALIBRATIONWINDOW:
			if(Command[1])
			{
				//	You can get the HWND of the windows created by the Kr?on SDK here.
				HWND hwnd;
				memcpy(&hwnd,&Command[3],sizeof(HWND));
			}
			break;

		case KR_SOCKET_GETCALIBRATIONMATRIX:
			if(Command[1])
			{
				//	you should save somewhere the calibration matrix
				//	(not done here as it is just an example)
				//double PositioningMatrix[16];
				memcpy(PositioningMatrix,&Command[3],16*sizeof(double));

				//	you can also get the standard deviation from the toolkit if
				//	you need it
				double Sigma;
				memcpy(&Sigma,&Command[3 + 16*sizeof(double)],sizeof(double));

				//	and the shape default...
				double ShapeDefault;
				memcpy(&ShapeDefault,&Command[3 + 17*sizeof(double)],sizeof(double));

				double StatisticShapeDefault;
				memcpy(&StatisticShapeDefault,&Command[3 + 18*sizeof(double)],sizeof(double));
			}
			break;

		case KR_SOCKET_PROBEGETCALIBRATION:
			if(Command[1])
			{
				//BYTE ProbeIndex = Command[3];
				KR_TOUCH_PROBE	Probe;
				memcpy(&Probe,&Command[4],sizeof(KR_TOUCH_PROBE));

				//	Some devices need to know the calibration of the probe
				ArmManager->SetParameter("Probe",&Probe);

				//	Do not forget to record the calibration (if Probe.Sigma has an acceptable value)
			}
			break;

		case KR_SOCKET_GETXYZLASERLINE:
			if(Command[1])
			{
				//	Get the laser line
				CDC*	cdc=CMainFrame::m_wndView.GetDC();
				CString txt;
				int		n;
				memcpy(&n,&Command[3],sizeof(int));
				txt.Format(_T("%d scanline(s) ready in the buffer"),n);
				cdc->TextOut(10, 100, txt);

				if(n>=0)
				{
					DWORD LaserLineSize,LaserLineSizeInBytes;
					memcpy(&LaserLineSize,&Command[3+sizeof(int)],sizeof(DWORD));
					double * LaserLine = new double[3*LaserLineSize];
					LaserLineSizeInBytes = LaserLineSize*3*sizeof(double);
					memcpy(LaserLine,&Command[3+sizeof(int)+sizeof(DWORD)],LaserLineSizeInBytes);

					double	MachineMatrix[16];
					memcpy(MachineMatrix,&Command[3+sizeof(int)+sizeof(DWORD)+LaserLineSizeInBytes],16*sizeof(double));
					/*
						The Machine Matrix gives the position and direction of the machine
						MachineMatrix[ 0] == Xleft
						MachineMatrix[ 1] == Yleft
						MachineMatrix[ 2] == Zleft
						MachineMatrix[ 3] == 0
						MachineMatrix[ 4] == Xup
						MachineMatrix[ 5] == Yup
						MachineMatrix[ 6] == Zup
						MachineMatrix[ 7] == 0
						MachineMatrix[ 8] == Xdirection
						MachineMatrix[ 9] == Ydirection
						MachineMatrix[10] == Zdirection
						MachineMatrix[11] == 0
						MachineMatrix[12] == Xposition
						MachineMatrix[13] == Yposition
						MachineMatrix[14] == Zposition
						MachineMatrix[15] == 1
					*/

					txt.Format(_T("LaserLine has %d points    "), LaserLineSize);
					cdc->TextOut(10, 120, txt);
					txt.Format(_T("Coordinates of the first point: (%e , %e , %e)            "),
						LaserLine[0],LaserLine[1],LaserLine[2]);
					cdc->TextOut(10,140,txt);
					txt.Format(_T("Coordinates of the last point : (%e , %e , %e)            "),
						LaserLine[(LaserLineSize-1)*3],LaserLine[(LaserLineSize-1)*3+1],LaserLine[(LaserLineSize-1)*3+2]);
					cdc->TextOut(10,160,txt);
					if(NbLines==0)
						StartTime=GetTickCount();
					else
					{
						int Time=(GetTickCount()-StartTime)/1000;
						if(Time)
						{
							txt.Format(_T("Received lines: %03d, time: %03d, Frequency: %2.1f   "),
								NbLines,Time,(float)NbLines/Time);
							cdc->TextOut(10,180,txt);
						}
					}
					txt.Format(_T("Buttons state: %d %d %d %d                    "),g_bButtonState[0],g_bButtonState[1],g_bButtonState[2],g_bButtonState[3]);
					cdc->TextOut(10, 200, txt);

					std::ofstream fichier("points.txt", std::ios::out | std::ios::app); //Matthias
					if(fichier)
					{
						unsigned int target;
						target = (unsigned int)LaserLineSize;
						unsigned int zero = 0;
						if(target!=zero)
						{
						CString text;
						//text.Format(_T("Coordinates of the point: (%e , %e , %e)            "),
						//LaserLine[0],LaserLine[1],LaserLine[2]);
						for(unsigned int a=0; a<target;a++)
						{
						fichier << LaserLine[a*3] << " " << LaserLine[(a*3)+1] << " " << LaserLine[(a*3)+2] << " ";
						}
						fichier << std::endl;
						fichier.close();
						}
					}
					else
					{
						AfxMessageBox(_T("Impossible d'ouvrir fichier"));
					}

					delete [] LaserLine;
					NbLines++;
				}

				CMainFrame::m_wndView.ReleaseDC(cdc);
			}
			break;

		case KR_SOCKET_GETLASERPLANECORNERS:
			if(Command[1])
			{
				double corners[4*2];	//	4 UV coordinates
				memcpy(corners,&Command[3],4*2*sizeof(double));
				CString strCorners;
				strCorners.Format(_T("Corners:\n")
								  _T("   (%7.3f,%7.3f) (%7.3f,%7.3f)\n")
								  _T("(%7.3f,%7.3f)       (%7.3f,%7.3f)\n"),
								  corners[6],corners[7], corners[4],corners[5],
								  corners[0],corners[1], corners[2],corners[3]);
				AfxMessageBox(strCorners);
			}
			break;

		case KR_SOCKET_CALLBACK:
			{
				UCHAR Cmd;
				Cmd=Command[1];
				switch(Cmd)
				{
					case KR_CMD_POSITIONING_WINDOW_CLOSED:
						{
							::KillTimer(CMainFrame::m_StatichWnd,ARM_BUTTONS_TIMER_ID);
							ArmManager->EndAcquisition();
							DWORD  Param;
							memcpy(&Param,&Command[2],sizeof(DWORD));
							if (Param==1)
							{
								//	The positioning process returned 1 index
								//	To get its resulting matrix, just send a "KR_SOCKET_GETCALIBRATIONMATRIX"
								//	command to the server.
								BYTE Index = 0;
								SocketManager->GetCalibrationMatrix(Index);
							}
						}
						break;

					case KR_CMD_VIDEOSETUP_WINDOW_CLOSED:
						//	The video setup window has been closed by the user
						//	(or by a call to the KR_SOCKET_CLOSEVIDEOSETTINGWINDOW command)
						break;

					case KR_CMD_SCAN_FREQUENCY:
						{
							//	Some arms need to know the frequency they will work at
							DWORD  Param;
							memcpy(&Param,&Command[2],sizeof(DWORD));
							ArmManager->SetParameter("AcquisitionFrequency",&Param);
						}
						break;

					case KR_CMD_SCAN_STARTED:
						//	The scan is starting...
						//	So, we can stop polling the arm buttons
						::KillTimer(CMainFrame::m_StatichWnd,ARM_BUTTONS_TIMER_ID);
						ArmManager->EndAcquisition();
						//	And start the acquisition mode of the arm
						ArmManager->BeginAcquisition(KR_ARM_MODE_SYNCHRONOUS);
						//	Finally, as the arm is ready, start the scanner digitization.
						SocketManager->StartScannerAcquisitionInPositioningWindow();
						break;

					case KR_CMD_SCAN_PAUSED:
						ArmManager->EndAcquisition();
						ArmManager->BeginAcquisition(KR_ARM_MODE_BUTTONS);
						::SetTimer(CMainFrame::m_StatichWnd,ARM_BUTTONS_TIMER_ID,ARM_BUTTONS_TIMER_PERIOD,NULL);
						break;

					case KR_CMD_SCAN_STOPPED:
						::KillTimer(CMainFrame::m_StatichWnd,ARM_BUTTONS_TIMER_ID);
						ArmManager->EndAcquisition();
						break;

					case KR_CMD_SCAN_LINE_READY:
						{
							//	Ask the laser line to the toolkit
							SocketManager->GetXYZLaserLine();
						}
						break;

					case KR_CMD_TRIGGERED_POSITION_NEEDED:
						{
							//	If your machine is fast enough, you can ask the position here,
							//	and send a "KR_SOCKET_SETMACHINEPOSITION" command to the server
							double	PositionMatrix[16];
							int Result=ArmManager->GetTriggeredPosition(PositionMatrix,g_bButtonState);
							if ((Result==KR_ARM_OK) || (Result==KR_ARM_OK_BUT_POSITION_UNAVAILABLE) || (Result==KR_ARM_MECHANICAL_STOP))
							{
								bool bPauseButton=false;
								if (Result==KR_ARM_OK)
								{
									int NbButtons = 0;
									ArmManager->GetParameter("NbButtons",&NbButtons);
									if (NbButtons>0)
									{
										//	Button 1 is usually used as the "start" button,
										//	unless there is only one button available.
										//	We will also use it as Pause button.
										if (NbButtons==1)
											bPauseButton = g_bButtonState[0];
										else
											bPauseButton = g_bButtonState[1];
									}
								}

								//	You may want to stop the acquisition if a button is pressed
								if (bPauseButton)
									SocketManager->EndAcquisition();
								else
								{
									//	The Kreon SDK needs a triggered position from the arm
									if (Result==KR_ARM_OK)
										SocketManager->SetMachinePosition(PositionMatrix);
									else	// Or at least know that there was no position available
										SocketManager->SetNoMachinePositionAvailable();

									//	These functions automatically request for a new
									//	acquisition from the scanner (which will need
									//	a triggered position, and so on...)
								}
							}
							else
								SocketManager->EndAcquisition();
						}
						break;

					case KR_CMD_SCANNER_ERROR:
						SocketManager->EndAcquisition();
						break;

					case KR_CMD_KEY_PRESSED:
						//	A key has been pressed in the positioning window or in the video setup window
						DWORD Key;
						memcpy(&Key,&Command[3],sizeof(DWORD));
						if(Key==VK_F1)
						{
							//	...
						}
						break;

					case KR_CMD_TP_SPHERE_CALIBRATION_STARTED:
					case KR_CMD_TP_HOLE_CALIBRATION_STARTED:
						{
							//	The user wants to calibrate a touch probe
							//	Some arms/devices need to know that we are starting a probe calibration.
							//	For this, we send a blank KR_TOUCH_PROBE structure with Sigma set to a negative value.
							KR_TOUCH_PROBE Probe = {"",0,0,0, -1.0 , 0,false};
							ArmManager->SetParameter("Probe",&Probe);

							//	We need to start a timer and the arm to give positions to the toolkit regularly.
							ArmManager->BeginAcquisition(KR_ARM_MODE_TOUCH_PROBE);
							::SetTimer(CMainFrame::m_StatichWnd,ARM_TOUCHPROBE_TIMER_ID,ARM_TOUCHPROBE_TIMER_PERIOD,NULL);
						}
						break;

					case KR_CMD_TP_SPHERE_CALIBRATION_ENDED:
					case KR_CMD_TP_HOLE_CALIBRATION_ENDED:
						//	End of calibration
						::KillTimer(CMainFrame::m_StatichWnd,ARM_TOUCHPROBE_TIMER_ID);
						ArmManager->EndAcquisition();
						break;

					case KR_CMD_TP_CALIBRATION_WINDOW_CLOSED_EXT:
						int  NbProbes;
						memcpy(&NbProbes,&Command[2],sizeof(int));
						//	If NbProbes==-1, it means that the dialog has been closed without saving or with the red cross button
						//	If NbProbes==0, then the list has been saved but is empty
						//	Otherwise a probe may have been recalibrated. Read it and save it if needed
						for (int i=0 ; i<NbProbes ; i++)
							SocketManager->ProbeGetCalibration((BYTE)i);
						break;

					case KR_CMD_POSITIONING_DELETED:
						//	The last positioning has been deleted in the positioning window.
						//	You should restart the timer to read the arm buttons again.
						ArmManager->BeginAcquisition(KR_ARM_MODE_BUTTONS);
						::SetTimer(CMainFrame::m_StatichWnd,ARM_BUTTONS_TIMER_ID,ARM_BUTTONS_TIMER_PERIOD,NULL);
						break;
				}
			}
			break;
	}
}

/////////////////////////////////////////////////////////////////////////////
bool CMainFrame::Init()
{
	if(!m_bKreonInitialized)
	{
		if(SocketManager)
		{
			delete SocketManager;
			SocketManager=NULL;
		}
		if (ArmManager)
		{
			if (ArmManager->IsConnected())
				ArmManager->EndArm();
			delete ArmManager;
			ArmManager=NULL;
		}

		//	Load the library (DLL) of the arm
		TCHAR DLLName[MAX_PATH];
		_stprintf_s(DLLName,_countof(DLLName),_T("%s.DLL"),ARM_NAME);
		ArmManager=new CArmManager(DLLName, positioningDevice);
		if(!ArmManager->IsInitialized())
		{
			CString ErrMsg;
			ErrMsg.Format(_T("Error: unable to load library %s.dll."),ARM_NAME);
			AfxMessageBox(ErrMsg);
			return false;
		}

		//	Initialize and run the Kreon server
		SocketManager = new CSocketManager();
		if(SocketManager->InitServer(OnKreonCallback,m_hWnd,WM_SOCKET_MESSAGE)!=KR_ERR_NO_ERROR)
		{
			delete SocketManager;
			SocketManager=NULL;
			delete ArmManager;
			ArmManager=NULL;
			return false;
		}

		//	Get the calibration files list (Field "needsCalFile" from KR_CALIBRATION_FILE_LIST
		//  tells whether the scanner needs a calibration file or not so it can be called with any scanner)
		//	One or more calibration files should be present in the <Hardware>\Calibration directory
		//	i.e. C:\Documents and settings\All Users\Application Data\Polygonia\HardwareZLS\Calibration
		st_CalibrationFileList = NULL;
		SocketManager->GetCalibrationFileList();

		// Wait until we receive the calibration file list from the server
		while (!st_CalibrationFileList)
		{
			MSG Msg;
			if (PeekMessage(&Msg,NULL,0,0,PM_REMOVE))
			{
				TranslateMessage(&Msg);
				DispatchMessage(&Msg);
			}
			Sleep(0);
		}

		char * CalibrationFile;
		if (st_CalibrationFileList->needsCalFile)
		{
			if (!st_CalibrationFileList->nbFiles)
				return false;	// Fail if there is no calibration file available
			if (st_CalibrationFileList->offsetCurrentFile == -1)	// No calibration file was selected?
				st_CalibrationFileList->offsetCurrentFile = 0;		// ...so we take the first one.
			CalibrationFile = st_CalibrationFileList->list+st_CalibrationFileList->offsetCurrentFile;
		}
		else
			CalibrationFile = NULL;

		//	Initialize the Kreon Toolkit
		BYTE MachineType = 99;  // See KR_SOCKET_INITKREON HTML documentation for a complete list of machine types
		UINT CodeClient  = 0;	// <- insert your client code here (provided by Kr?on)
		SocketManager->InitKreon(MachineType, CalibrationFile, CodeClient /*,"FR"*/); // <- optional language parameter

		//	Free CalibrationFileList (if you don't need it any more)
		delete st_CalibrationFileList;
		st_CalibrationFileList = NULL;

		//	Initialize the positioning matrix
		PositioningMatrix[0]=1.0;	PositioningMatrix[4]=0.0;	PositioningMatrix[8]=0.0;	PositioningMatrix[12]=0.0;
		PositioningMatrix[1]=0.0;	PositioningMatrix[5]=1.0;	PositioningMatrix[9]=0.0;	PositioningMatrix[13]=0.0;
		PositioningMatrix[2]=0.0;	PositioningMatrix[6]=0.0;	PositioningMatrix[10]=1.0;	PositioningMatrix[14]=0.0;
		PositioningMatrix[3]=0.0;	PositioningMatrix[7]=0.0;	PositioningMatrix[11]=0.0;	PositioningMatrix[15]=1.0;

		m_bKreonInitialized=true;
	}

	return m_bKreonInitialized;
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnKreontoolkitLicense() 
{
	if(!m_bKreonInitialized)
		if(!Init())
			return;

	// Please provide some way to display this dialog in your application
	SocketManager->OpenLicenseDialog(m_hWnd);
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnKreontoolkitVideosetup() 
{
	if(!m_bKreonInitialized)
		if(!Init())
		{
			return;
		}

	SocketManager->OpenVideoSettingWindow(true);
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnKreontoolkitPositioning() 
{
	// One should obviously not try positioning while scanning
	if (m_bScanning)
		OnKreontoolkitPausescan();

	if(!m_bKreonInitialized)
		if(!Init())
			return;

#define WINDOWED_CALIBRATION 1 // If you want to make your own positioning window, see the code in #else branch
#if WINDOWED_CALIBRATION

	//	Begin the arm
	if(!ArmManager->IsConnected())
	{
		if(ArmManager->BeginArm()!=KR_ARM_OK)
		{
			CString ErrMsg;
			ErrMsg.Format(_T("Error: Unable to initialize the arm. Please check the connection"));
			AfxMessageBox(ErrMsg);
			return;
		}
	}

	//	The arm and the ECU are correctly initialized.
	//	We need to inform the ECU about the duration of the external trigger.
	//	To get this information, we use the "GetParameter" method of CArmManager
	//	(see ArmManager.h)
	WORD TriggerDuration;
	if(ArmManager->GetParameter("TriggerDuration",&TriggerDuration))
		SocketManager->SetExternalTriggerDuration(TriggerDuration);
	else
	{
		//	Oops... It should normally never happen with a Kreon Arm DLL
		AfxMessageBox(_T("Error: unable to get the trigger duration from the arm DLL."));
		return;
	}

	//	Some coordinate measuring devices also need to be
	//	triggered before the sensor actually acquires a laser line
	WORD AcquisitionDelay;
	if (ArmManager->GetParameter("AcquisitionDelay",&AcquisitionDelay))
		SocketManager->SetAcquisitionDelay(AcquisitionDelay);
	else
		SocketManager->SetAcquisitionDelay(0);

	//	Now the arm is initialized properly.
	//	We need to get the button state regularly to start the acquisition
	//	when the button is be pressed
	if(ArmManager->BeginAcquisition(KR_ARM_MODE_BUTTONS)!=KR_ARM_OK)
	{
		AfxMessageBox(_T("Error: BeginAcquisition failed"));
		return;
	}

	//	Start a timer
	SetTimer(ARM_BUTTONS_TIMER_ID,ARM_BUTTONS_TIMER_PERIOD,NULL);

	BYTE nbIndex = 1;	// For an arm, we use only one index
	KR_INDEX_EXT myIndex;
	memset(&myIndex,0,sizeof(myIndex));
	memcpy(myIndex.PositioningMatrix,PositioningMatrix,16*sizeof(double));
	strcpy_s(myIndex.Name,_countof(myIndex.Name),"MyIndex");
	SocketManager->OpenPositioningWindowExt(&myIndex,nbIndex,
		KR_SCAN_METHOD_SYNCHRONOUS,true,15,0,false,false,m_hWnd,false,0xFF);

#else

	// The code below prepares all the input data
	// for SphereCalibration (positioning without window)

	KR_LASER_LINE* CalibrationScan = NULL; // List of laser lines
	//double PositioningMatrix[16];
	bool Type5axes = true; // The machine has 5 axes or more (typically an arm)
	long SupportDirection = 0;
	double SphereDiameter = 25; // millimeters

	// For this example, we read the laser lines from a file
	int	LineNumber;
	FILE* SphereCWKFile;
	fopen_s(&SphereCWKFile,"machine.cwk","rb");
	if (!SphereCWKFile)
	{
		AfxMessageBox(_T("Failed to open simulation file!"));
		return;
	}
	else
		CKR_Specific::CWK_GetGeneral(SphereCWKFile, PositioningMatrix, &LineNumber);

	double LaserLine[2048*3];
	UINT LaserLineSize;
	double Matrix[16];

	for  (int i=0; i<LineNumber; i++)
	{
		if (CKR_Specific::CWK_GetNextLine(SphereCWKFile, LaserLine, &LaserLineSize, Matrix))
		{
			double matPosInv[16];
			memcpy(matPosInv, PositioningMatrix, 16*sizeof(double));
			CKR_Specific::MatrixInvert(matPosInv);
			CKR_Specific::MatrixMultiply(Matrix, matPosInv);

			KR_LASER_LINE* KRLaserLine = new KR_LASER_LINE;
			KRLaserLine->Size = LaserLineSize;
			memcpy(KRLaserLine->Matrix, Matrix, 16*sizeof(double));
			KRLaserLine->Points = new double [3*LaserLineSize];
			memcpy(KRLaserLine->Points, LaserLine, 3*LaserLineSize*sizeof(double));
			KRLaserLine->NextLine = CalibrationScan;
			CalibrationScan = KRLaserLine;
		}
	}

	// Everything is ready to compute the positioning data...
	SocketManager->SphereCalibration(CalibrationScan,PositioningMatrix,Type5axes,SupportDirection,SphereDiameter);

#endif
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnTimer(UINT nIDEvent)
{
	if(nIDEvent==ARM_BUTTONS_TIMER_ID)
	{
		int NbButtons = 0;
		ArmManager->GetParameter("NbButtons",&NbButtons);

		if(NbButtons>0)
		{
			bool* bButtonState = new bool [NbButtons];
			if(ArmManager->GetButtonState(bButtonState)==KR_ARM_OK)
			{
				bool bButtonPressed = false;
				for (int i=0; i<NbButtons ; i++)
					if (bButtonState[i])
					{
						bButtonPressed = true;
						break;
					}

				if (bButtonPressed)
				{
					//	Wait until the button is released
					while (bButtonPressed)
					{
						bButtonPressed = false;
						if (ArmManager->GetButtonState(bButtonState)==KR_ARM_OK)
							for (int i=0; i<NbButtons ; i++)
								if (bButtonState[i])
								{
									bButtonPressed = true;
									break;
								}
					}

					//	Tell the positioning window that the user wants
					//	to start the acquisition
					SocketManager->BeginAcquisition();
				}
			}
			delete [] bButtonState;
		}
	}
	else if(nIDEvent==ARM_TOUCHPROBE_TIMER_ID)
	{
		int NbButtons = 0;
		ArmManager->GetParameter("NbButtons",&NbButtons);

		if(NbButtons>0)
		{
			bool*	bButtonState = new bool [NbButtons];
			memset(bButtonState,0,sizeof(bool)*NbButtons);
			double	Matrix[16];
			int Result = ArmManager->GetCurrentPosition(Matrix,bButtonState);
			if(Result==KR_ARM_OK  ||  Result==KR_ARM_ONLY_BUTTONS_ARE_VALID)
			{
				bool bFrontButton,bBackButton;
				if (NbButtons>=2)
				{
					bFrontButton = bButtonState[1];
					bBackButton  = bButtonState[0];
				} 
				else if (NbButtons==1)
				{
					bFrontButton = bButtonState[0];
					bBackButton  = false;
				}
				else
				{
					bFrontButton = false;
					bBackButton  = false;
				}
				// It is recommended to wait here for the button to be released when using sphere calibration method
/*
				bool bButtonPressed = bFrontButton || bBackButton;
				if (bButtonPressed)
				{
					double	dummyMatrix[16];
					memset(bButtonState,0,sizeof(bool)*NbButtons);
					int waitResult;
					do
					{
						waitResult = ArmManager->GetCurrentPosition(dummyMatrix,bButtonState);
						bButtonPressed = false;
						if (Result==KR_ARM_OK  ||  Result==KR_ARM_ONLY_BUTTONS_ARE_VALID)
						{
							for (int i=0; i<NbButtons; i++)
							{
								if (bButtonState[i])
								{
									bButtonPressed = true;
									break;
								}
							}
						}
					} while (bButtonPressed);
				}
*/
				if(bBackButton)
					SocketManager->ProbeDelLastPosition(true);
				else if(bFrontButton  &&  Result==KR_ARM_OK)
					SocketManager->ProbeAddPosition(Matrix,false);
			}
			delete [] bButtonState;
		}
	}

	CFrameWnd::OnTimer(nIDEvent);
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnKreontoolkitStartscan() 
{
	if(!m_bKreonInitialized)
		if(!Init())
			return;

	//	Begin the arm
	if(!ArmManager->IsConnected())
	{
		if(ArmManager->BeginArm()!=KR_ARM_OK)
		{
			CString ErrMsg;
			ErrMsg.Format(_T("Error : unable to initialize the arm. Check the connection"));
			AfxMessageBox(ErrMsg);
			return;
		}
	}

	//	The arm and the ECU are correctly initialized.
	//	We need to inform the ECU about the duration of the external trigger.
	//	To get this information, we use the "GetParameter" method of CArmManager
	//	(see ArmManager.h)
	WORD TriggerDuration;
	if(ArmManager->GetParameter("TriggerDuration",&TriggerDuration))
		SocketManager->SetExternalTriggerDuration(TriggerDuration);
	else
	{
		//	Oops...	It should normally never happen with a KREON Arm dll
		AfxMessageBox(_T("Error: unable to get the trigger duration from the arm DLL."));
		return;
	}

	//	Some coordinate measuring devices also need to be
	//	triggered before the sensor actually acquires a laser line
	WORD AcquisitionDelay;
	if (ArmManager->GetParameter("AcquisitionDelay",&AcquisitionDelay))
		SocketManager->SetAcquisitionDelay(AcquisitionDelay);
	else
		SocketManager->SetAcquisitionDelay(0);

	BYTE  Frequency = 10;
	float Step=0.2f;
	UINT  Decimation = 1;
	SocketManager->InitAcquisition(
						KR_SCAN_METHOD_SYNCHRONOUS,
						Frequency,		// not used in synchronous mode
						Step,
						PositioningMatrix,
						Decimation);
	ArmManager->BeginAcquisition(KR_ARM_MODE_SYNCHRONOUS);	// synchronous scan mode
	SocketManager->BeginAcquisition();

	NbLines=0;
	m_bScanning=true;
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnUpdateKreontoolkitStartscan(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!m_bScanning);
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnKreontoolkitPausescan() 
{
	SocketManager->EndAcquisition();
	ArmManager->EndAcquisition();
	m_bScanning=false;
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnUpdateKreontoolkitPausescan(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_bScanning);
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnKreonToolkitProbeCalibration()
{
	if(!m_bKreonInitialized)
		if(!Init())
			return;

	//	Begin the arm
	if(!ArmManager->IsConnected())
	{
		if(ArmManager->BeginArm()!=KR_ARM_OK)
		{
			CString ErrMsg;
			ErrMsg.Format(_T("Error: unable to initialize the arm. Check the connection"));
			AfxMessageBox(ErrMsg);
			return;
		}
	}

	//	Open the touch probe calibration window (in this example, 2 probes are initially created)
	KR_TOUCH_PROBE TouchProbe[2];
	memset(TouchProbe,0,sizeof(TouchProbe));
	strcpy_s(TouchProbe[0].Name,_countof(TouchProbe[0].Name),"FirstTouchProbe");
	TouchProbe[0].Diameter=6;
	strcpy_s(TouchProbe[1].Name,_countof(TouchProbe[1].Name),"SecondTouchProbe");
	TouchProbe[1].Diameter=3;
	SocketManager->ProbeOpenCalibrationWindow(TouchProbe,2,true);
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnKreonArmProperties(void)
{
	if(!m_bKreonInitialized)
		if(!Init())
			return;

	//ArmManager->PropertiesWindow(m_hWnd);
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnKreonScannerProperties(void)
{
	if(!m_bKreonInitialized)
		if(!Init())
			return;

	SocketManager->OpenScannerPropertiesWindow(m_hWnd);
}

void CMainFrame::OnPositioningDeviceCobot()
{
	CMenu *pMenu = GetMenu();
	if (pMenu != nullptr)
	{
		pMenu->CheckMenuItem(ID_POSITIONINGDEVICE_COBOTTX2, MF_CHECKED | MF_BYCOMMAND);
		pMenu->CheckMenuItem(ID_POSITIONINGDEVICE_CTRACK, MF_UNCHECKED | MF_BYCOMMAND);
	}

	positioningDevice = PA_Enums::CobotTx2Touch;

	// Restart Kreon
	m_bKreonInitialized = false;

	// Restart arm manager
	if (ArmManager != NULL)
		if (ArmManager->IsConnected())
			ArmManager->EndArm();
	delete ArmManager;
	ArmManager = NULL;

	ArmManager = new CArmManager(ARM_NAME, positioningDevice);
}

void CMainFrame::OnPositioningDeviceCtrack()
{
	CMenu *pMenu = GetMenu();
	if (pMenu != nullptr)
	{
		pMenu->CheckMenuItem(ID_POSITIONINGDEVICE_COBOTTX2, MF_UNCHECKED | MF_BYCOMMAND);
		pMenu->CheckMenuItem(ID_POSITIONINGDEVICE_CTRACK, MF_CHECKED | MF_BYCOMMAND);
	}


	positioningDevice = PA_Enums::Ctrack;

	// Restart Kreon
	m_bKreonInitialized = false;

	// Restart arm manager
	if (ArmManager != NULL)
		if (ArmManager->IsConnected())
			ArmManager->EndArm();
	delete ArmManager;
	ArmManager = NULL;

	ArmManager = new CArmManager(ARM_NAME, positioningDevice);
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CMainFrame::OnDataSocket(WPARAM /*wParam*/,LPARAM /*lParam*/)
{
	SocketManager->OnDataSocket();
	return 0L;
}