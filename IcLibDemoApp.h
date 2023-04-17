//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// IcLibDemoApp.h
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Copyright (c) 2023 Adrian Maurer. All rights reserved.
// Distributed under the MIT software license (http://www.opensource.org/licenses/mit-license.php).
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#pragma once
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifndef __AFXWIN_H__
#error "include 'pch.h' before including this file for PCH"
#endif
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "resource.h"
#include "Message.h"
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CAboutDlg
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class CAboutDlg : public CDialog
{
// DIALOG DATA
#ifdef AFX_DESIGN_TIME
	enum {IDD = IDD_ABOUT_DLG};
#endif

// CONSTRUCTION / DESTRUCTION / ASSIGNMENT
public:
	CAboutDlg();

// MESSAGE MAP FUNCTIONS
protected:
	DECLARE_MESSAGE_MAP()
};
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CIcLibDemoApp
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class CIcLibDemoApp : public CWinApp
{
// CONSTRUCTION / DESTRUCTION / ASSIGNMENT
public:
	CIcLibDemoApp();

// OVERRIDES
public:
	virtual BOOL InitInstance() final;

// MESSAGE MAP FUNCTIONS
protected:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern CIcLibDemoApp theApp;
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
inline void sendMessageToMainWnd(UINT msg, WPARAM wp = 0)
{
	if (CWnd* const p_mw = ::AfxGetMainWnd())
	{
		p_mw->SendMessage(msg, wp);
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
