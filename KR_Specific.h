// KR_Specific.h: interface for the KR_Specific class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KR_SPECIFIC_H__195100D0_44B0_468A_BE3B_2BABC7E311BF__INCLUDED_)
#define AFX_KR_SPECIFIC_H__195100D0_44B0_468A_BE3B_2BABC7E311BF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define CKR_SPECIFIC_SHEME	0x0001	// Version serialization

#include "KreonDefines.h"

#if !defined(__KR_REGISTER_H__)
// Constants for accessing the Registry
static const TCHAR BASED_CODE SOFTWARE_VERSION[]				= _T("v3.3.2.1");
static const TCHAR BASED_CODE HKEY_POLYGONIA_CONFIG_HEAD[]		= _T("Config");
static const TCHAR BASED_CODE HKEY_CONFIG_MACHINE[]				= _T("CurrentMachine");
static const TCHAR BASED_CODE HKEY_CONFIG_HARDWAREDIRECTORY[]	= _T("HardwarePath");		//	CString : Directory containing the HardWare inforation
static const TCHAR BASED_CODE ROOT_REGISTRY[]					= _T("Software");
static const TCHAR BASED_CODE ROOT_LOCAL[]						= _T("SOFTWARE");
static const TCHAR BASED_CODE HKEY_KREON_TECHNOLOGIES[]			= _T("Kreon Technologies");
static const TCHAR BASED_CODE HKEY_CONFIG_HEAD[]				= _T("CurrentHead");		//	CString : File name of machine head (like PH10)
static const TCHAR BASED_CODE HKEY_CONFIG_SCANSERVERSTATE[]		= _T("ScanServerState");
static const TCHAR BASED_CODE HKEY_CONFIG_CALIBRATION_NAME[]	= _T("CurrentSensor");		//	CString : File name of current scanner calibration
static const TCHAR BASED_CODE HKEY_CONFIG_SENSOR[]				= _T("Sensor");				//	CString : Codename of current scanner
static const TCHAR BASED_CODE HKEY_CONFIG_LANGUAGE[]			= _T("Language");			//	CString : Language code ("US","FR","DE","JP","IT","RU","CN"...)
#endif
#define HKEY_ROOT_REGISTRY	HKEY_CURRENT_USER 
#define HKEY_ROOT_LOCAL		HKEY_LOCAL_MACHINE

class CKR_Specific : public CObject 
{
	DECLARE_SERIAL(CKR_Specific)
public:
	CKR_Specific();
	virtual ~CKR_Specific();

	bool CheckMachine();
	bool IndexHeadSave();
	bool IndexHeadRead();
	void IndexHeadSerialize(CArchive &ar);
	void SetScanServerState(CString msg);
	bool ReadRegister(LPCTSTR lpszRoot, LPCTSTR lpszHead, LPCTSTR lpszKey,
					  CString defaultValue, CString& Result, bool bCurrentUser=true);

	static bool CWK_GetGeneral(FILE* CWKFile, double* Matrix, int*LineNumber);
	static bool CWK_GetNextLine(FILE* CWKFile, double* LaserLine, UINT* LaserLineSize, double* PassageMatrix);
	static void MatrixInvert(double* m1);
	static void MatrixMultiply(double* m1, double* m2);

	KR_INDEX m_Index;
private:
	bool WriteRegister(CString Root, CString Head, CString Key, bool Value, bool bCurrentUser=true);
	bool WriteRegister(CString Root, CString Head, CString Key, CString Value, bool bCurrentUser=true);
};

#endif // !defined(AFX_KR_SPECIFIC_H__195100D0_44B0_468A_BE3B_2BABC7E311BF__INCLUDED_)
