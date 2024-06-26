//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// MainFrame.h
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Copyright (c) 2019-2023 Adrian Maurer. All rights reserved.
// Distributed under the MIT software license (http://www.opensource.org/licenses/mit-license.php).
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#pragma once
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "InterpolationCurve.h"
#include "ViewWnd.h"
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CMainFrame
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class CMainFrame : public CFrameWnd
{
// STATIC DATA
private:
	static const std::array<UINT, 3> indicatorArray;

// ELEMENT DATA
private:
	Curve curve;
	CStatusBar statusBar;
	CViewWnd viewWnd;

// CONSTRUCTION / DESTRUCTION / ASSIGNMENT
public:
	CMainFrame();
protected: 
	DECLARE_DYNAMIC(CMainFrame)

// ELEMENT FUNCTIONS
public:
	void refreshCurveTypeStatus();
	void refreshIterationCountStatus();

// OVERRIDES
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT&) final;
	virtual BOOL OnCmdMsg(UINT, int, void*, AFX_CMDHANDLERINFO*) final;

// MESSAGE MAP FUNCTIONS
protected:
	afx_msg int OnCreate(LPCREATESTRUCT);
	afx_msg void OnSetFocus(CWnd*);
	afx_msg LRESULT OnCurveTypeChanged(WPARAM, LPARAM);
	afx_msg LRESULT OnIterationCountChanged(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()
};
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
