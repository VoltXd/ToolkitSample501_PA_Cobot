// KR_Specific.cpp: implementation of the KR_Specific class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KR_Specific.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CKR_Specific::CKR_Specific()
{
}

CKR_Specific::~CKR_Specific()
{

}
////////////////////////////////////////////////////////////////////////
// Read the positioning matrix from a CWK file
// The file (CWKFile) must be open in binary mode, e.g. "rb".
// The matrix (Matrix) must be allocated with 16 "double"
////////////////////////////////////////////////////////////////////////
bool CKR_Specific::CWK_GetGeneral(FILE* CWKFile, double* Matrix, int*LineNumber)
{
	if (!CWKFile)
		return false;

	short ID = 12;	// positioning matrix ID

	char buffer[256];

	// Read the header of the file
	rewind(CWKFile);
	fread(buffer,sizeof(char),20,CWKFile);
	if (strncmp(buffer,"CWK_____________",16)!=0)
		return false;

	// Read the chunks to find the ID we are looking for
	short RecordType=0;
	long  RecordSize=0;
	while ((RecordType!=ID) && !feof(CWKFile))
	{
		fread(&RecordType, sizeof(short),1,CWKFile);
		fread(&RecordSize, sizeof(long),1,CWKFile);
		if (RecordType!=ID)
			fseek(CWKFile, RecordSize, SEEK_CUR);
		else
			fread(Matrix,sizeof(double),16,CWKFile);
	}
	long lineNb;
	ID = 2;	// General information ID
	while ((RecordType!=ID) && !feof(CWKFile))
	{
		fread(&RecordType, sizeof(short),1,CWKFile);
		fread(&RecordSize, sizeof(long),1,CWKFile);
		if (RecordType!=ID)
			fseek(CWKFile, RecordSize, SEEK_CUR);
		else
		{
			fseek(CWKFile, 28L, SEEK_CUR);
			fread(&lineNb, sizeof(long),1,CWKFile);
			*LineNumber = (int) lineNb;
			fseek(CWKFile, 4L, SEEK_CUR);
			return true;
		}
	}
	return false;
}

////////////////////////////////////////////////////////////////////////
// Read the next laser line in the CWK file
// The file (CWKFile) must be open in binary mode, e.g. "rb".
// The matrix (PassageMatrix) must be allocated with 16 "double"
// The laser line (LaserLine) must be allocated with at least 2000 points
// The number of points (LaserLineSize) must point to an allocated integer
////////////////////////////////////////////////////////////////////////
bool CKR_Specific::CWK_GetNextLine(FILE* CWKFile, double* LaserLine, UINT* LaserLineSize, double* PassageMatrix)
{
	if (!CWKFile)
		return false;

	short size;
	float pts[3];
	char  camera;

	short ID = 3;	// laser line ID
	// Lecture d'une entête
	short RecordType=0;
	long  RecordSize=0;
	while ((RecordType!=ID) && !feof(CWKFile))
	{
		fread(&RecordType, sizeof(short),1,CWKFile);
		fread(&RecordSize, sizeof(long),1,CWKFile);
		if (RecordType!=ID)
			fseek(CWKFile, RecordSize, SEEK_CUR);
		else
		{
			fread(PassageMatrix,sizeof(double),16,CWKFile);
			fread(&size,sizeof(short),1,CWKFile);
			*LaserLineSize=(UINT)size;
			int k=0;
			for (int i=0;i<size;i++)
			{
				fread(pts,sizeof(float),3,CWKFile);
				fread(&camera,sizeof(char),1,CWKFile);
				LaserLine[k++]=(double)pts[0];
				LaserLine[k++]=(double)pts[1];
				LaserLine[k++]=(double)pts[2];
			}
			return true;
		}
	}
	return false;
}

/////////////////////////////////////////////////////////////////////////////
// 4x4 matrix multiplication: m1=m1*m2
/////////////////////////////////////////////////////////////////////////////
void CKR_Specific::MatrixMultiply(double* m1, double* m2)
{
	double buf[16];
	memcpy(buf,m1, 16 *sizeof(double));
	for (int i=0;i<4;i++)
	{
		for (int j=0;j<4;j++)
		{
			m1[i+4*j] = 0.; 
			for (int k=0;k<4;k++)
				m1[i+4*j] += buf[i+4*k] * m2[k+4*j];
		}
	}

}
/////////////////////////////////////////////////////////////////////////////
// Orthogonal 4x4 matrix inversion: m1 = inverse(m1)
/////////////////////////////////////////////////////////////////////////////
void CKR_Specific::MatrixInvert(double* m1)
{
	double buf[16];
	memcpy(buf,m1, 16 *sizeof(double));

	int i,j;
	for(i=0; i<3; i++)
	for(j=0; j<3; j++)
		m1[j*4+i] = buf[i*4+j];

	m1[12]=-(m1[0]*buf[12]+m1[4]*buf[13]+m1[8]*buf[14]);
	m1[13]=-(m1[1]*buf[12]+m1[5]*buf[13]+m1[9]*buf[14]);
	m1[14]=-(m1[2]*buf[12]+m1[6]*buf[13]+m1[10]*buf[14]);
}

/////////////////////////////////////////////////////////////////////////////
bool CKR_Specific::CheckMachine()
{
	CString FileName,Sensor;
	ReadRegister(SOFTWARE_VERSION,HKEY_POLYGONIA_CONFIG_HEAD,HKEY_CONFIG_MACHINE,_T(""),FileName);
	ReadRegister(SOFTWARE_VERSION,HKEY_POLYGONIA_CONFIG_HEAD,HKEY_CONFIG_SENSOR,_T(""),Sensor);

	if (FileName.IsEmpty() || Sensor.IsEmpty())
		return false;

	static CString st_DefaultHardwarePath=_T("");
	if (st_DefaultHardwarePath.IsEmpty())
	{
		TCHAR szPath[MAX_PATH];
		SHGetSpecialFolderPath(NULL,szPath,CSIDL_COMMON_APPDATA,FALSE);
		st_DefaultHardwarePath = szPath;
		st_DefaultHardwarePath+= _T("\\");
		st_DefaultHardwarePath+= HKEY_KREON_TECHNOLOGIES;
	}
	CString hardWareDir;
	ReadRegister(_T(""),_T(""),HKEY_CONFIG_HARDWAREDIRECTORY,st_DefaultHardwarePath,hardWareDir);
	CString PathName = hardWareDir + _T("\\") + SOFTWARE_VERSION + _T("\\") + Sensor + _T("\\") + FileName; 

	
	CFile archiveFile;
	if (archiveFile.Open(PathName, CFile::modeRead))
	{
		CArchive ar(&archiveFile, CArchive::load);
		CObject::Serialize(ar);
		if (ar.GetFile()->GetLength() > 0)
		{
			USHORT nV;
			ar >> nV;
			if(nV>=12)
			{
				int T;
				ar >> T;
				if(T==1)
					return true;
			}
		}
	}
	return false;
}

/////////////////////////////////////////////////////////////////////////////
bool CKR_Specific::ReadRegister(LPCTSTR lpszRoot, LPCTSTR lpszHead, LPCTSTR lpszKey,
								CString defaultValue, CString& Result, bool bCurrentUser)
{
	bool isOpenKey = FALSE;
	DWORD lbuf=MAX_PATH;
	TCHAR resultRead[MAX_PATH];

	HKEY key;
	CString path;
	path = bCurrentUser ? ROOT_REGISTRY : ROOT_LOCAL;

	if (!path.IsEmpty())
	{
		path += _T("\\");
		path += HKEY_KREON_TECHNOLOGIES;
		path += _T("\\");
	}
	path += lpszRoot;

	if (lstrlen(lpszHead))
	{
		path += _T("\\");
		path += lpszHead;
	}

	if (bCurrentUser)
		isOpenKey = RegOpenKeyEx(HKEY_ROOT_REGISTRY, (LPCTSTR) path, 0, KEY_READ, &key)==ERROR_SUCCESS;
	else
#ifdef _WIN64
		isOpenKey = RegOpenKeyEx(HKEY_ROOT_LOCAL, (LPCTSTR) path, 0, KEY_READ | KEY_WOW64_64KEY, &key)==ERROR_SUCCESS;
#else
		isOpenKey = RegOpenKeyEx(HKEY_ROOT_LOCAL, (LPCTSTR) path, 0, KEY_READ | KEY_WOW64_32KEY, &key)==ERROR_SUCCESS;
#endif
	if (isOpenKey && key)
	{		
		DWORD type;
		lbuf = sizeof(resultRead);
		long bRet = RegQueryValueEx(key, lpszKey, (LPDWORD) NULL, &type , (LPBYTE) resultRead, &lbuf); //Retour de "isOpenKey" si erreur de lecture
		isOpenKey = (bRet==ERROR_SUCCESS) && (type==REG_SZ);

		RegCloseKey(key);

		if (isOpenKey)
			Result = resultRead;
	}

	if (!isOpenKey)
	{	
		// Error: key not found
		if (bCurrentUser)
		{
			// If it was read in CURRENT_USER, try LOCAL_MACHINE now
			isOpenKey = ReadRegister(lpszRoot,lpszHead,lpszKey,defaultValue,Result,false);
			if (isOpenKey)
			{
				// If successful in LOCAL_MACHINE, update CURRENT_USER
				WriteRegister(lpszRoot,lpszHead,lpszKey,Result,true);
			}
		}
		else
		{
			// Key neither exists in CURRENT_USER nor LOCAL_MACHINE => use default value!
			Result = defaultValue;
			isOpenKey = FALSE;
		}
	}

	return isOpenKey;
}

/////////////////////////////////////////////////////////////////////////////
bool CKR_Specific::WriteRegister(CString Root, CString Head, CString Key, CString Value, bool bCurrentUser)
{
	bool isOpenKey = FALSE;
	HKEY key;
	DWORD resultAccess;

	CString path;
	path = bCurrentUser ? ROOT_REGISTRY : ROOT_LOCAL;

	if (!path.IsEmpty())
	{
		path += _T("\\");
		path += HKEY_KREON_TECHNOLOGIES;
		path += _T("\\");
	}
	path += Root;

	if (!Head.IsEmpty())
	{	
		path += "\\";
		path += Head;
	}

	if (bCurrentUser)
		isOpenKey = RegCreateKeyEx(HKEY_ROOT_REGISTRY, (LPCTSTR) path, (DWORD) 0, _T("noClass"), REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &key, &resultAccess)==ERROR_SUCCESS;
	else
		isOpenKey = RegCreateKeyEx(HKEY_ROOT_LOCAL, (LPCTSTR) path, (DWORD) 0, _T("noClass"), REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &key, &resultAccess)==ERROR_SUCCESS;

	if (isOpenKey && key)
	{
		TCHAR buf[MAX_PATH];
		lstrcpy(buf, Value);
		isOpenKey  = (RegSetValueEx(key, LPCTSTR(Key), NULL, REG_SZ, (LPBYTE) buf, (Value.GetLength()+1)*sizeof(TCHAR))==ERROR_SUCCESS);
		isOpenKey &= (RegCloseKey(key)==ERROR_SUCCESS);
	}

	return isOpenKey;
}

/////////////////////////////////////////////////////////////////////////////
bool CKR_Specific::WriteRegister(CString Root, CString Head, CString Key, bool Value, bool bCurrentUser)
{
	bool isOpenKey = FALSE;
	HKEY key;
	DWORD resultAccess;
	CString path;
	path = bCurrentUser ? ROOT_REGISTRY : ROOT_LOCAL;

	if (!path.IsEmpty())
	{
		path += _T("\\");
		path += HKEY_KREON_TECHNOLOGIES;
		path += _T("\\");
	}
	path+=Root;

	if (!Head.IsEmpty())
	{
		path += _T("\\");
		path += Head;
	}

	if (bCurrentUser)
		isOpenKey = RegCreateKeyEx(HKEY_ROOT_REGISTRY, (LPCTSTR) path, (DWORD) 0, _T("noClass"), REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &key, &resultAccess)==ERROR_SUCCESS;
	else
		isOpenKey = RegCreateKeyEx(HKEY_ROOT_LOCAL, (LPCTSTR) path, (DWORD) 0, _T("noClass"), REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &key, &resultAccess)==ERROR_SUCCESS;

	if (isOpenKey && key)
	{
		isOpenKey = RegSetValueEx(key, LPCTSTR(Key), NULL, REG_BINARY, (BYTE*) &Value, sizeof(bool))==ERROR_SUCCESS;
		if (isOpenKey)
			isOpenKey = (RegCloseKey(key)==ERROR_SUCCESS);
	}

	return isOpenKey;
}

/////////////////////////////////////////////////////////////////////////////
bool CKR_Specific::IndexHeadSave()
{
	CString FileName,Sensor;
	ReadRegister(SOFTWARE_VERSION,HKEY_POLYGONIA_CONFIG_HEAD,HKEY_CONFIG_HEAD,_T(""),FileName);
	ReadRegister(SOFTWARE_VERSION,HKEY_POLYGONIA_CONFIG_HEAD,HKEY_CONFIG_SENSOR,_T(""),Sensor);
	if (FileName.IsEmpty() || Sensor.IsEmpty())
		return false;

	CString hardWareDir;
	ReadRegister(_T(""),_T(""),HKEY_CONFIG_HARDWAREDIRECTORY,_T(""),hardWareDir);

	CString PathName;
	if (hardWareDir.IsEmpty())
		PathName = FileName;
	else
		PathName = hardWareDir + _T("\\") + SOFTWARE_VERSION + _T("\\") + Sensor + _T("\\") + FileName; 

	CFile archiveFile;
	if (archiveFile.Open(PathName, CFile::modeCreate | CFile::modeWrite))
	{
		CArchive ar(&archiveFile, CArchive::store);
		IndexHeadSerialize(ar);
		return true;
	}
	else
		return false;
}

/////////////////////////////////////////////////////////////////////////////
bool CKR_Specific::IndexHeadRead()
{
	CString FileName,Sensor;
	ReadRegister(SOFTWARE_VERSION,HKEY_POLYGONIA_CONFIG_HEAD,HKEY_CONFIG_HEAD,_T(""),FileName);
	ReadRegister(SOFTWARE_VERSION,HKEY_POLYGONIA_CONFIG_HEAD,HKEY_CONFIG_SENSOR,_T(""),Sensor);
	if (FileName.IsEmpty())
		return false;

	bool NoError = false;
	CString hardWareDir=_T("");
	ReadRegister(_T(""),_T(""),HKEY_CONFIG_HARDWAREDIRECTORY,_T(""),hardWareDir);

	CString PathName;
	if (hardWareDir.IsEmpty())
		PathName = FileName;
	else
		PathName = hardWareDir + _T("\\") + SOFTWARE_VERSION + _T("\\") + Sensor + _T("\\") + FileName; 

	CFile archiveFile;
	if (archiveFile.Open(PathName, CFile::modeRead))
	{
		CArchive ar(&archiveFile, CArchive::load);
		if (ar.GetFile()->GetLength() > 0)
		{
			NoError=true;
			try
			{
				IndexHeadSerialize(ar);
			}
			catch(...)
			{
				NoError=FALSE;
			}
		}	
		return NoError;
	}
	return false;
}

/////////////////////////////////////////////////////////////////////////////
void CKR_Specific::SetScanServerState(CString msg)
{
	WriteRegister(SOFTWARE_VERSION,HKEY_POLYGONIA_CONFIG_HEAD,HKEY_CONFIG_SCANSERVERSTATE,msg);
}

/////////////////////////////////////////////////////////////////////////////
void CKR_Specific::IndexHeadSerialize(CArchive &ar)
{
	CObject::Serialize(ar);
	int i;

	if(ar.IsStoring())
	{
		ar << (USHORT) 0x0001;
		ar << (int) 1;
		ar << (BOOL) false;
		ar << (int) 0;
		ar << (int) 0;
		ar << (int) 0;
		ar << (int) 0;
		ar << (double) 0.;
		ar << (double) 0.;
		ar << (double) 0.;
		ar << (USHORT) 0x0001;
		for (i=0; i<16; i++)
			ar << (double) 0.;

		CString st(_T("index"));
		ar << st;
		ar << (double) 0.;
		ar << (double) 0.;
		ar << (double) 0.;
		ar << (BOOL) true;	// m_positioningDone
		ar << (USHORT) 0x0001;
		for (i=0; i<16; i++)
			ar << (double) m_Index.PositioningMatrix[i];

		ar << 1;
	}
	else
	{
		USHORT nVersion;
		ar >> nVersion;
		switch (nVersion)
		{
			case 0x0001:
			case 0x0002:
				{
					BOOL BOOL_;
					int  int_;
					double double_;
					USHORT USHORT_;
					ar >> int_;
					ar >> BOOL_;
					ar >> int_;
					ar >> int_;
					ar >> int_;
					ar >> int_;
					ar >> double_;
					ar >> double_;
					ar >> double_;
					ar >> USHORT_;
					for (i=0; i<16; i++)
						ar >> double_;

					CString Name;
					ar >> Name;
#ifdef _UNICODE
					wcstombs_s(NULL,m_Index.Name,_countof(m_Index.Name),Name,_TRUNCATE);
#else
					strcpy_s(m_Index.Name,_countof(m_Index.Name), (LPCTSTR)Name);
#endif
					ar >> double_;
					ar >> double_;
					ar >> double_;
					ar >> BOOL_;
					ar >> USHORT_;
					for (i=0; i<16; i++)
						ar >> m_Index.PositioningMatrix[i];
				}
				break;

			default:
				break;
		}
	}
}


IMPLEMENT_SERIAL(CKR_Specific, CObject, VERSIONABLE_SCHEMA | CKR_SPECIFIC_SHEME)