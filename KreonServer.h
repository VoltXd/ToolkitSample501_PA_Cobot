//	---------------------------------------------------------------------------
//	KreonServer.h
//
//	KREON Technologies toolkit
//	Server mode constants
//	---------------------------------------------------------------------------
#ifndef	__KREON_SERVER__
#define	__KREON_SERVER__



#include <tchar.h>
#include "KreonDefines.h"		//	Toolkit constants



//	General constants
#define	KR_DEFAULT_SERVER_PORT						8000
#define	KR_DEFAULT_SERVER_NAME						_T("KreonServer.exe")
#define	KR_DEFAULT_SERVER_ADDRESS					_T("127.0.0.1")
#define	KR_HEADER_SIZE								(sizeof(DWORD)+sizeof(UCHAR))



//	Constants to call the functions via the sockets
#define	KR_SOCKET_INITKREON							4
#define	KR_SOCKET_ENDKREON							5
#define	KR_SOCKET_INITMACHINE						6
#define	KR_SOCKET_SETPRESETAXIS						7
#define	KR_SOCKET_GETPRESETAXIS						8
#define	KR_SOCKET_SETVIDEOSETTINGS					9
#define	KR_SOCKET_GETVIDEOSETTINGS					10
#define	KR_SOCKET_OPENVIDEOSETTINGWINDOW			11
#define	KR_SOCKET_ISVIDEOSETTINGWINDOWOPENED		12
#define	KR_SOCKET_LOCKVIDEOSETTING					13
#define	KR_SOCKET_UNLOCKVIDEOSETTING				14
#define	KR_SOCKET_CLOSEVIDEOSETTINGWINDOW			15
#define	KR_SOCKET_GETVIDEOSETTINGRECORDED			16	// You should consider using KR_SOCKET_GETVIDEOSETTINGSLIST instead
#define	KR_SOCKET_SETVIDEOSETTINGBYNAME				17
#define	KR_SOCKET_SETLASERPERMANENT					18
#define	KR_SOCKET_SETBLUELED						19
#define	KR_SOCKET_SETHARDWAREFILTER					20
#define	KR_SOCKET_SETEXTERNALTRIGGERDURATION		21
#define	KR_SOCKET_OPENDRAWINGPARAMETERSWINDOW		22
#define	KR_SOCKET_SETSPHERECENTER					23
#define	KR_SOCKET_OPENPOSITIONINGWINDOW				24
#define	KR_SOCKET_ISPOSITIONINGWINDOWOPENED			25
#define	KR_SOCKET_GETCALIBRATIONMATRIX				26
#define	KR_SOCKET_STARTSCAN							KR_SOCKET_BEGINACQUISITION
#define	KR_SOCKET_PAUSESCAN							KR_SOCKET_ENDACQUISITION
#define	KR_SOCKET_SETMACHINEPOSITION				29
#define	KR_SOCKET_INITACQUISITION					30
#define	KR_SOCKET_BEGINACQUISITION					31
#define	KR_SOCKET_ENDACQUISITION					32
#define	KR_SOCKET_GETXYZLASERLINE					33
#define	KR_SOCKET_OPENSCANWINDOW					34
#define	KR_SOCKET_GETLASERLINE						35
#define	KR_SOCKET_GETSENSORTYPE						36
#define	KR_SOCKET_CALLBACK							37
#define	KR_SOCKET_SETSENSORSTATE					38
#define	KR_SOCKET_GETHARDWAREFILTER					39
#define	KR_SOCKET_LICENCE							40
#define	KR_SOCKET_LICENSE							40
#define	KR_SOCKET_STOPSCAN							41
#define KR_SOCKET_GETLASTERRORSTRING				42
#define	KR_SOCKET_PROBEOPENCALIBRATIONWINDOW		43
#define	KR_SOCKET_PROBEADDPOSITION					44
#define	KR_SOCKET_PROBEDELLASTPOSITION				45
#define	KR_SOCKET_PROBEGETCALIBRATION				46
#define	KR_SOCKET_OPENSCANNERPROPERTIESWINDOW		47
#define KR_SOCKET_SETOUTPUT							48
#define KR_SOCKET_GETINPUT							49
#define	KR_SOCKET_GETPOSITIONINGTOOLPATH			50
#define	KR_SOCKET_GETSENSORSTATE					51
#define	KR_SOCKET_STARTSCANNERACQPOSITIONING		52
#define KR_SOCKET_GETCALIBRATIONFILELIST			53
#define KR_SOCKET_SPHERECALIBRATIONBEGIN			54
#define KR_SOCKET_SPHERECALIBRATIONADDLASERLINE		55
#define KR_SOCKET_SPHERECALIBRATIONEND				56
#define KR_SOCKET_SPHERECALIBRATION					KR_SOCKET_SPHERECALIBRATIONEND
#define KR_SOCKET_SETNOMACHINEPOSITIONAVAILABLE		57
#define KR_SOCKET_OPENPOSITIONINGWINDOWEXT			58
#define	KR_SOCKET_OPENSCANWINDOWEXT					59
#define KR_SOCKET_SETAPPLICATIONHWND				60
#define KR_SOCKET_PROBECALIBRATIONBEGIN				61
#define KR_SOCKET_PROBECALIBRATIONEND				62

#define	KR_SOCKET_GETVIDEOSETTINGSLIST				65	//	Meant to replace KR_SOCKET_GETVIDEOSETTINGRECORDED
#define KR_SOCKET_GETLASERPLANECORNERS				66
#define KR_SOCKET_SETACQUISITIONDELAY				67

#define KR_SOCKET_SETNBINPUTPOINTS					70
#define KR_SOCKET_ADDINPUTPOINTS					71
#define KR_SOCKET_FITPLANE							72
#define KR_SOCKET_FITPOINT							73
#define KR_SOCKET_FITSPHERE							74
#define KR_SOCKET_FITCIRCLE							75
#define KR_SOCKET_FITCYLINDER						76
#define KR_SOCKET_FITLINE							77
#define KR_SOCKET_SMOOTHER							78
#define KR_SOCKET_GETPROCESSEDPOINTS				79


//	Explicit name of each command
const TCHAR KR_CommandCodeToString[256][100]=
{
	_T("Unknown"),									//	0
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("KR_SOCKET_INITKREON"),
	_T("KR_SOCKET_ENDKREON"),						//	5
	_T("KR_SOCKET_INITMACHINE"),
	_T("KR_SOCKET_SETPRESETAXIS"),
	_T("KR_SOCKET_GETPRESETAXIS"),
	_T("KR_SOCKET_SETVIDEOSETTINGS"),
	_T("KR_SOCKET_GETVIDEOSETTINGS"),				//	10
	_T("KR_SOCKET_OPENVIDEOSETTINGWINDOW"),
	_T("KR_SOCKET_ISVIDEOSETTINGWINDOWOPENED"),
	_T("KR_SOCKET_LOCKVIDEOSETTING"),
	_T("KR_SOCKET_UNLOCKVIDEOSETTING"),
	_T("KR_SOCKET_CLOSEVIDEOSETTINGWINDOW"),		//	15
	_T("KR_SOCKET_GETVIDEOSETTINGRECORDED"),
	_T("KR_SOCKET_SETVIDEOSETTINGBYNAME"),
	_T("KR_SOCKET_SETLASERPERMANENT"),
	_T("KR_SOCKET_SETBLUELED"),
	_T("KR_SOCKET_SETHARDWAREFILTER"),				//	20
	_T("KR_SOCKET_SETEXTERNALTRIGGERDURATION"),
	_T("KR_SOCKET_OPENDRAWINGPARAMETERSWINDOW"),
	_T("KR_SOCKET_SETSPHERECENTER"),
	_T("KR_SOCKET_OPENPOSITIONINGWINDOW"),
	_T("KR_SOCKET_ISPOSITIONINGWINDOWOPENED"),		//	25
	_T("KR_SOCKET_GETCALIBRATIONMATRIX"),
	_T("KR_SOCKET_STARTSCAN"),
	_T("KR_SOCKET_PAUSESCAN"),
	_T("KR_SOCKET_SETMACHINEPOSITION"),
	_T("KR_SOCKET_INITACQUISITION"),				//	30
	_T("KR_SOCKET_BEGINACQUISITION"),
	_T("KR_SOCKET_ENDACQUISITION"),
	_T("KR_SOCKET_GETXYZLASERLINE"),
	_T("KR_SOCKET_OPENSCANWINDOW"),
	_T("KR_SOCKET_GETLASERLINE"),					//	35
	_T("KR_SOCKET_GETSENSORTYPE"),
	_T("KR_SOCKET_CALLBACK"),
	_T("KR_SOCKET_SETSENSORSTATE"),
	_T("KR_SOCKET_GETHARDWAREFILTER"),
	_T("KR_SOCKET_LICENSE"),						//	40
	_T("KR_SOCKET_STOPSCAN"),
	_T("KR_SOCKET_GETLASTERRORSTRING"),
	_T("KR_SOCKET_PROBEOPENCALIBRATIONWINDOW"),
	_T("KR_SOCKET_PROBEADDPOSITION"),
	_T("KR_SOCKET_PROBEDELLASTPOSITION"),			//	45
	_T("KR_SOCKET_PROBEGETCALIBRATION"),
	_T("KR_SOCKET_OPENSCANNERPROPERTIESWINDOW"),
	_T("KR_SOCKET_SETOUTPUT"),
	_T("KR_SOCKET_GETINPUT"),
	_T("KR_SOCKET_GETPOSITIONINGTOOLPATH"),			//	50
	_T("KR_SOCKET_GETSENSORSTATE"),
	_T("KR_SOCKET_STARTSCANNERACQPOSITIONING"),
	_T("KR_SOCKET_GETCALIBRATIONFILELIST"),
	_T("KR_SOCKET_SPHERECALIBRATIONBEGIN"),
	_T("KR_SOCKET_SPHERECALIBRATIONADDLASERLINE"),	//	55
	_T("KR_SOCKET_SPHERECALIBRATION"),
	_T("KR_SOCKET_SETNOMACHINEPOSITIONAVAILABLE"),
	_T("KR_SOCKET_OPENPOSITIONINGWINDOWEXT"),
	_T("KR_SOCKET_OPENSCANWINDOWEXT"),
	_T("KR_SOCKET_SETAPPLICATIONHWND"),				//	60
	_T("KR_SOCKET_BEGINPROBECALIBRATION"),
	_T("KR_SOCKET_PROBECALIBRATIONEND"),
	_T("Unknown"),
	_T("Unknown"),
	_T("KR_SOCKET_GETVIDEOSETTINGSLIST"),			//	65
	_T("KR_SOCKET_GETLASERPLANECORNERS"),
	_T("KR_SOCKET_SETACQUISITIONDELAY"),
	_T("Unknown"),
	_T("Unknown"),
	_T("KR_SOCKET_SETNBINPUTPOINTS"),				//	70
	_T("KR_SOCKET_ADDINPUTPOINTS"),
	_T("KR_SOCKET_FITPLANE"),
	_T("KR_SOCKET_FITPOINT"),
	_T("KR_SOCKET_FITSPHERE"),
	_T("KR_SOCKET_FITCIRCLE"),						//	75
	_T("KR_SOCKET_FITCYLINDER"),
	_T("KR_SOCKET_FITLINE"),
	_T("KR_SOCKET_SMOOTHER"),
	_T("KR_SOCKET_GETPROCESSEDPOINTS"),
	_T("Unknown"),									//	80
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	85
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	90
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	95
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("KR_SOCKET_HL_INITIALIZEKREONTOOLKIT"),		//	100
	_T("KR_SOCKET_HL_VIDEOSETUP"),
	_T("KR_SOCKET_HL_POSITIONING"),
	_T("KR_SOCKET_HL_SCAN"),
	_T("KR_SOCKET_HL_OPTIONS"),
	_T("KR_SOCKET_HL_SCAN_FINISHED"),				//	105
	_T("KR_SOCKET_HL_LICENSE"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	110
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	115
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	120
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	125
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	130
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	135
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	140
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	145
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	150
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	155
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	160
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	165
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	170
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	175
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	180
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	185
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	190
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	195
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	200
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	205
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	210
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	215
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	220
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	225
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	230
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	235
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	240
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	245
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	250
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown")									//	255
};



const TCHAR KR_CallbackCmdToString[256][100]=
{
	_T("Unknown"),									//	0
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	5
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("KR_CMD_POSITIONING_WINDOW_CLOSED"),			//	10
	_T("Unknown"),
	_T("KR_CMD_SCAN_STARTED"),
	_T("KR_CMD_SCAN_PAUSED"),
	_T("KR_CMD_SCAN_STOPPED"),
	_T("KR_CMD_KEY_PRESSED"),						//	15
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	20
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	25
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("KR_CMD_SCAN_LINE_READY"),					//	30
	_T("KR_CMD_TRIGGERED_POSITION_NEEDED"),
	_T("KR_CMD_SCAN_WINDOW_CLOSED"),
	_T("KR_CMD_SCAN_FREQUENCY"),
	_T("Unknown"),
	_T("Unknown"),									//	35
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	40
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	45
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	50
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	55
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("KR_CMD_TP_SPHERE_CALIBRATION_STARTED"),		//	60
	_T("KR_CMD_TP_SPHERE_CALIBRATION_ENDED"),
	_T("KR_CMD_TP_HOLE_CALIBRATION_STARTED"),
	_T("KR_CMD_TP_HOLE_CALIBRATION_ENDED"),
	_T("KR_CMD_TP_CALIBRATION_WINDOW_CLOSED"),
	_T("KR_CMD_TP_CALIBRATION_WINDOW_CLOSED_EXT"),	//	65
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	70
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	75
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	80
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	85
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	90
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	95
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	100
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	105
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	110
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	115
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	120
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	125
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	130
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	135
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	140
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	145
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	150
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	155
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	160
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	165
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	170
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	175
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	180
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	185
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	190
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	195
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	200
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	205
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	210
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	215
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	220
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	225
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	230
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	235
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	240
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	245
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),									//	250
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("Unknown"),
	_T("KR_CMD_SCANNER_ERROR")						//	255
};



#endif