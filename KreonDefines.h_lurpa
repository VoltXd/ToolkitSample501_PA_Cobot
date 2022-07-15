//	---------------------------------------------------------------------------
//	KreonDefines.h
//
//	KREON Technologies toolkit
//	Data types and constants definition
//	---------------------------------------------------------------------------
#ifndef	__KREON_DEFINES__
#define	__KREON_DEFINES__



#pragma pack(push,BeforeKreon)
#pragma pack(8)					// Structure alignment



//	---------------------------------------------------------------------------
//	Datatypes
//	---------------------------------------------------------------------------
typedef unsigned int			UINT;
typedef unsigned char			UCHAR;
typedef unsigned char			BYTE;
typedef unsigned long			DWORD;
typedef void (*KREON_CALLBACK)(char*);
typedef void (*KRHL_SCAN_COMPLETE_CALLBACK)(UINT NbLaserLine);

#define KR_CUSTOM_VIDEO_SETUP	   "<Custom>"
#define    CUSTOM_VIDEO_SETUP	_T("<Custom>")
#define KR_DEFAULT_VIDEO_SETUP	   "<Default>"
#define    DEFAULT_VIDEO_SETUP	_T("<Default>")

#define	KR_NB_MAX_AXIS					6
#define	KR_VERSION_LENGTH				12
#define	KR_NAME_LENGTH					21
#define KR_CALIBRATION_FILE_LIST_SIZE	2000
#define KR_NB_MAX_INDICES				60


typedef struct
{
	bool	needsCalFile;						//  Tells whether the other fields are relevant or not
	DWORD	nbFiles;							//	Number of calibration file names in the list.
	int		offsetCurrentFile;					//	Offset of the current calibration file name in the list (-1 if no current file selected)
	DWORD	listSize;							//	Number of bytes (including zeros) used by the list
	char	list[KR_CALIBRATION_FILE_LIST_SIZE];//	List of calibration file names (e.g.: "file1.cal\0file2.cal\0");
}	KR_CALIBRATION_FILE_LIST;

typedef struct
{
	BYTE	PlugNumber[KR_NB_MAX_AXIS];		//	where each axis is plugged on the E.C.U.
	double	Resolution[KR_NB_MAX_AXIS];		//	nb tops / mm (can be negative)
}	KR_MACHINE_CONFIG;

typedef struct
{
	char	Name[KR_NAME_LENGTH];			//	name of the index, 20 characters max.
	double	PositioningMatrix[16];			//	the matrix describing the index
	double	Angles[3];						//	angles of the indexed head (not used yet)
}	KR_INDEX;

typedef struct
{
	char	Name[KR_NAME_LENGTH];			//	name of the index, 20 characters max.
	double	PositioningMatrix[16];			//	the matrix describing the index
	double	Angles[3];						//	angles of the indexed head (not used yet)
	bool	PositioningDone;				//	the index has be positioned
	double	Sigma;							//	|
	double	ShapeDefault;					//	|- Statitistics computed after positioning
	double	StatisticShapeDefault;			//	|
}	KR_INDEX_EXT;

typedef struct
{
	double	U,V,W;							//	Coordinates relative to the scanner
	double	Step;							//	Step required between two lines
}	KR_TOOLPATH_LINE;

typedef struct kr_laser_line
{
	DWORD	Size;							//	Size of the line (number of 3D points) = SizeA+SizeB
	DWORD	SizeA,SizeB;					//	Number of 3D points from cameras A and B (for bi-camera scanners)
	double*	Points;							//	Points coordinate (three double XYZ by point)
	double	Matrix[16];						//	machine matrix
	BYTE	IO;								//	I/O states
	struct kr_laser_line* NextLine;			//	Pointer to the next line
}	KR_LASER_LINE;

typedef struct
{
	char	Name[KR_NAME_LENGTH];			//	Name of the probe
	double	X,Y,Z;							//	Calibration
	double	Sigma;							//	Standard deviation of the calibration process
	double	Diameter;						//	Diameter of the probe
	bool	bTriggered;						//	true if the probe is triggered (TP2 for instance)
}	KR_TOUCH_PROBE;

typedef struct
{
	double	Matrix[16];
}	KR_POSITION;



//	---------------------------------------------------------------------------
//	Constants
//	---------------------------------------------------------------------------
//	GetSensorType
#define	KR_SENSOR_UNKNOWN								0
#define	KR_SENSOR_KLS51									1
#define	KR_SENSOR_KLS171								2
#define	KR_SENSOR_ZLS50									3
#define	KR_SENSOR_ZLS100								4
#define	KR_SENSOR_ZLS25									5
//	GetLastError
#define	KR_ERR_NO_ERROR									0
#define	KR_ERR_NO_CALIBRATION							1
#define	KR_ERR_BAD_INSTALLATION							2
#define	KR_ERR_SENSOR_OVERFLOW							3
#define	KR_ERR_CALIBRATION_FILE_NOT_FOUND				4
#define	KR_ERR_CALIBRATION_FILE_CORRUPTED				5
#define	KR_ERR_UNKNOW_MACHINE_TYPE						6
#define	KR_ERR_SCANLINE_BUFFER_EMPTY					7
#define	KR_ERR_SCANLINE_BUFFER_FULL						8
#define	KR_ERR_NULL_CALLBACK_FUNCTION					9
#define	KR_ERR_LUT_OVER_RANGE							10
#define	KR_ERR_LASER_POWER_OVER_RANGE					11
#define	KR_ERR_INTEGRATION_TIME_OVER_RANGE				12
#define	KR_ERR_ECU_NOT_CONNECTED						13
#define	KR_ERR_UNABLE_TO_READ_PAR_FILE					14
#define	KR_ERR_METHOD_OVER_RANGE						15
#define	KR_ERR_FREQUENCY_OVER_RANGE						16
//#define	KR_ERR_STEP_OVER_RANGE						17		//	Obsolete, no more considered as an error
#define	KR_ERR_POSITIONING_MATRIX_NOT_VALID				18
#define	KR_ERR_VIDEO_SETTING_NOT_FOUND					19
#define	KR_ERR_SYNC_MUTEX_SCANNER_TIMEOUT				20
#define	KR_ERR_SYNC_MUTEX_MACHINE_TIMEOUT				21
#define	KR_ERR_MACHINE_POSITION_BUFFER_EMPTY			22
#define	KR_ERR_MACHINE_POSITION_BUFFER_FULL				23
#define	KR_ERR_TOOLKIT_NOT_INITIALIZED					24
#define	KR_ERR_FUNCTION_NOT_IMPLEMENTED					25
#define	KR_ERR_BAD_DONGLE_KEY							26
#define	KR_ERR_MACHINE_TYPE_DEFINITION_NOT_FOUND		28
#define	KR_ERR_SENSOR_NOT_CONNECTED						29
#define	KR_ERR_UNABLE_TO_GET_SENSOR_PARAMETERS			30
#define	KR_ERR_FILTER_OVER_RANGE						31
//#define	KR_ERR_UNABLE_TO_LOAD_LANGUAGE_RESOURCE		32		//	Obsolete, no more considered as an error
#define	KR_ERR_UNABLE_TO_LOAD_THE_SENSOR				34
#define	KR_ERR_MACHINE_UNINITIALIZED					35
#define	KR_ERR_NOT_ENOUGH_MEMORY						36
#define	KR_ERR_INDEX_OVER_RANGE							37
#define	KR_ERR_SCAN_WINDOW_ALREADY_OPENED				38
#define	KR_ERR_POSITIONING_WINDOW_ALREADY_OPENED		39
#define	KR_ERR_PLUGNUMBER_OVER_RANGE					40
#define	KR_ERR_INTERNAL_ERROR							41
#define	KR_ERR_UNABLE_TO_FIND_KREON_SERVER				42
#define	KR_ERR_SOCKET_ERROR								43
#define	KR_ERR_SCAN_IN_PROGRESS							45
#define	KR_ERR_UNAUTHORIZED_OPTION						46
#define	KR_ERR_UNAUTHORIZED_SOFTWARE					47
#define	KR_ERR_DECIMATION_OVER_RANGE					48
#define	KR_ERR_SCAN_NOT_INITIALIZED						49
#define	KR_ERR_SCANNER_SWITCH_OFF						50
#define KR_ERR_CALIBRATION_FILE_LIST_TOO_LONG			51
#define KR_ERR_SPHERE_CALIBRATION_FAILED				52
#define KR_ERR_SPHERE_CALIBRATION_BAD_RESULT			53
#define KR_ERR_SPHERE_CALIBRATION_INCORRECT_DIAMETER	54
#define KR_ERR_SPHERE_CALIBRATION_NOT_ENOUGH_POINTS		55
#define KR_ERR_SPHERE_CALIBRATION_POSITIONS_TOO_CLOSE	56
#define KR_ERR_PROBE_CALIBRATION_ALREADY_RUNNING		57
#define KR_ERR_PROBE_CALIBRATION_NOT_RUNNING			58
#define KR_ERR_PROBE_CALIBRATION_UNKNOWN_METHOD			59
#define KR_ERR_PROBE_CALIBRATION_NOT_ENOUGH_DATA		60

//	SetFilter
#define	KR_FILTER_NONE								0
#define	KR_FILTER_LIGHT								1
#define	KR_FILTER_MEDIUM1							2
#define	KR_FILTER_MEDIUM2							3
#define	KR_FILTER_HIGH								4
#define	KR_SOFTWARE_FILTER_NONE						0
#define	KR_SOFTWARE_FILTER_LIGHT					1
#define	KR_SOFTWARE_FILTER_MEDIUM1					2
#define	KR_SOFTWARE_FILTER_HIGH						3
#define	KR_SOFTWARE_FILTER_HIGH						3

//	InitAcquisition and OpenPositioningWindow
#define KR_SCAN_METHOD_BIT_SYNCHRONOUS				0x01
#define	KR_SCAN_METHOD_BIT_EXTERNAL					0x02
#define KR_SCAN_METHOD_BIT_POSITION_ONLY			0x04
#define KR_SCAN_METHOD_BIT_SHORTCUT					0x08
#define KR_SCAN_METHOD_BIT_RAW						0x10

#define	KR_SCAN_METHOD_ASYNCHRONOUS					(!KR_SCAN_METHOD_BIT_SYNCHRONOUS)
#define	KR_SCAN_METHOD_SYNCHRONOUS					KR_SCAN_METHOD_BIT_SYNCHRONOUS
#define	KR_SCAN_METHOD_ASYNCHRONOUS_EXTERNAL		(KR_SCAN_METHOD_ASYNCHRONOUS | KR_SCAN_METHOD_BIT_EXTERNAL)
#define	KR_SCAN_METHOD_SYNCHRONOUS_EXTERNAL			(KR_SCAN_METHOD_SYNCHRONOUS | KR_SCAN_METHOD_BIT_EXTERNAL)
#define	KR_SCAN_METHOD_POSITION_ONLY				KR_SCAN_METHOD_BIT_POSITION_ONLY




//	Callback commands
#define	KR_CMD_POSITIONING_WINDOW_CLOSED			((UCHAR)10)
#define	KR_CMD_VIDEOSETUP_WINDOW_CLOSED				((UCHAR)11)
#define	KR_CMD_SCAN_STARTED							((UCHAR)12)
#define	KR_CMD_SCAN_PAUSED							((UCHAR)13)
#define	KR_CMD_SCAN_STOPPED							((UCHAR)14)
#define	KR_CMD_KEY_PRESSED							((UCHAR)15)
#define KR_CMD_POSITIONING_DELETED					((UCHAR)16)
#define	KR_CMD_SCAN_LINE_READY						((UCHAR)30)
#define	KR_CMD_TRIGGERED_POSITION_NEEDED			((UCHAR)31)
#define	KR_CMD_SCAN_WINDOW_CLOSED					((UCHAR)32)
#define	KR_CMD_SCAN_FREQUENCY						((UCHAR)33)
#define	KR_CMD_TP_SPHERE_CALIBRATION_STARTED		((UCHAR)60)
#define	KR_CMD_TP_SPHERE_CALIBRATION_ENDED			((UCHAR)61)
#define	KR_CMD_TP_HOLE_CALIBRATION_STARTED			((UCHAR)62)
#define	KR_CMD_TP_HOLE_CALIBRATION_ENDED			((UCHAR)63)
#define	KR_CMD_TP_CALIBRATION_WINDOW_CLOSED			((UCHAR)64)	//	returns 0 probes on cancel               |_ Please use one or the
#define	KR_CMD_TP_CALIBRATION_WINDOW_CLOSED_EXT		((UCHAR)65)	//	returns -1 as number of probes on cancel |  other but not both.
#define	KR_CMD_SCANNER_ERROR						((UCHAR)255)



//	Codes returned by the ToolKit when the Positioning window is closed
enum KR_POSITIONING_EXIT_CODES
{
	KR_POSEXIT_OK,
	KR_POSEXIT_USER_DID_NOT_SAVE,
	KR_POSEXIT_AUTO_INVALID_DIAMETER,
	KR_POSEXIT_AUTO_CANNOT_COMPUTE,
	KR_POSEXIT_AUTO_BAD_RESULT
};


//	== Constants related to arms ==

//	Arm Error Definitions
#define	KR_ARM_OK								1
#define KR_ARM_OK_BUT_POSITION_UNAVAILABLE		2
#define	KR_ARM_FAILED							(-1)
#define	KR_ARM_NOT_CONNECTED					(-2)
#define	KR_ARM_MECHANICAL_STOP					(-3)
#define	KR_ARM_COMMUNICATION_PROBLEM			(-4)
#define	KR_ARM_NO_RESPONSE_FROM_ARM				(-5)
#define	KR_ARM_UNABLE_TO_INITIALIZE				(-6)
#define	KR_ARM_FORBIDDEN_BY_DONGLE				(-7)
#define	KR_ARM_UNABLE_TO_INITIALIZE_SOFTWARE	(-8)
#define	KR_ARM_BAD_FIRMWARE						(-9)
#define	KR_ARM_MODE_NOT_SUPPORTED				(-10)
#define	KR_ARM_BAD_MODE							(-11)
//#define	KR_ARM_PAUSE_BUTTON_PRESSED			(-12)		//	obsolete
#define	KR_ARM_NO_TRIGGER						(-13)
#define	KR_ARM_ONLY_BUTTONS_ARE_VALID			(-14)

//	Acquisition Modes
#define	KR_ARM_MODE_IDLE						0
#define	KR_ARM_MODE_CURRENT_POS					1
#define	KR_ARM_MODE_TOUCH_PROBE					2
#define	KR_ARM_MODE_ASYNCHRONOUS				3
#define	KR_ARM_MODE_SYNCHRONOUS					4
#define	KR_ARM_MODE_BUTTONS						5



#pragma pack(pop,BeforeKreon)



#endif