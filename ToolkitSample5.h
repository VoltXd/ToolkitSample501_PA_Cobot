// ToolkitSample5.h : main header file for the TOOLKITSAMPLE5 application
//

#if !defined(AFX_TOOLKITSAMPLE5_H__655C9DE8_77D5_44BB_9BDC_5FAB14D428CD__INCLUDED_)
#define AFX_TOOLKITSAMPLE5_H__655C9DE8_77D5_44BB_9BDC_5FAB14D428CD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CToolkitSample5App:
// See ToolkitSample5.cpp for the implementation of this class
//

class CToolkitSample5App : public CWinApp
{
public:
	CToolkitSample5App();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CToolkitSample5App)
	public:
	virtual BOOL InitInstance();
	virtual BOOL ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

public:
	//{{AFX_MSG(CToolkitSample5App)
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TOOLKITSAMPLE5_H__655C9DE8_77D5_44BB_9BDC_5FAB14D428CD__INCLUDED_)
