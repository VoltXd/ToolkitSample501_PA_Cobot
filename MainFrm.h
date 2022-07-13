// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__017EAAD9_8F94_4465_8FE2_6B12E44D15C4__INCLUDED_)
#define AFX_MAINFRM_H__017EAAD9_8F94_4465_8FE2_6B12E44D15C4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ChildView.h"


#define		WM_SOCKET_MESSAGE		(WM_APP + 0x1000)



class CMainFrame : public CFrameWnd
{
public:
	CMainFrame();
protected: 
	DECLARE_DYNAMIC(CMainFrame)

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual void OnTimer(UINT nIDEvent);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	static CChildView    m_wndView;

private:
	bool	Init();
	bool	m_bKreonInitialized;
	bool	m_bScanning;

public:
	static HWND		m_StatichWnd;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	afx_msg void OnKreontoolkitVideosetup();
	afx_msg void OnKreontoolkitStartscan();
	afx_msg void OnKreontoolkitPositioning();
	afx_msg void OnKreontoolkitPausescan();
	afx_msg void OnUpdateKreontoolkitStartscan(CCmdUI* pCmdUI);
	afx_msg void OnUpdateKreontoolkitPausescan(CCmdUI* pCmdUI);
	afx_msg void OnKreonToolkitProbeCalibration();
	afx_msg void OnKreonArmProperties();
	afx_msg void OnKreonScannerProperties();
	afx_msg void OnKreontoolkitLicense();
	afx_msg LRESULT OnDataSocket(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__017EAAD9_8F94_4465_8FE2_6B12E44D15C4__INCLUDED_)
