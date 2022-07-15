// ToolkitSample5.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "ToolkitSample5.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CToolkitSample5App

BEGIN_MESSAGE_MAP(CToolkitSample5App, CWinApp)
	//{{AFX_MSG_MAP(CToolkitSample5App)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CToolkitSample5App construction

CToolkitSample5App::CToolkitSample5App()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CToolkitSample5App object

CToolkitSample5App theApp;

/////////////////////////////////////////////////////////////////////////////
// CToolkitSample5App initialization

BOOL CToolkitSample5App::InitInstance()
{
	AfxEnableControlContainer();

	// Change the registry key under which our settings are stored.
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));


	CMainFrame* pFrame = new CMainFrame;
	m_pMainWnd = pFrame;

	// create and load the frame with its resources
	pFrame->LoadFrame(IDR_MAINFRAME, WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL, NULL);

	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();

	return CWinApp::InitInstance();
}

BOOL CToolkitSample5App::ExitInstance()
{
	CWinApp::ExitInstance();
	delete (CMainFrame*)m_pMainWnd;
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CToolkitSample5App message handlers





/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CToolkitSample5App::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CToolkitSample5App message handlers

