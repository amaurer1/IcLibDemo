//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// MainFrame.cpp
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Copyright (c) 2019-2023 Adrian Maurer. All rights reserved.
// Distributed under the MIT software license (http://www.opensource.org/licenses/mit-license.php).
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "pch.h"
#include "framework.h"
#include "MainFrame.h"
#include "IcLibDemoApp.h"
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CMainFrame
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CMainFrame - STATIC DATA
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const std::array<UINT, 3> CMainFrame::indicatorArray = {ID_SEPARATOR, ID_SEPARATOR, ID_SEPARATOR};
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CMainFrame - CONSTRUCTION / DESTRUCTION / ASSIGNMENT
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
CMainFrame::CMainFrame() : 
	CFrameWnd(), 
	curve(Curve::Type::finiteSpline), 
	statusBar(), 
	viewWnd(&curve)
{
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CMainFrame - ELEMENT FUNCTIONS
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CMainFrame::refreshCurveTypeStatus()
{
	switch (curve.getType())
	{
		case Curve::Type::lineSegments: statusBar.SetPaneText(1, L"Line Segments Curve"); break;
		case Curve::Type::cubicSpline: statusBar.SetPaneText(1, L"Cubic Spline Curve"); break;
		case Curve::Type::finiteSpline: statusBar.SetPaneText(1, L"Finite Spline Curve"); break;
		default: break;
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CMainFrame::refreshIterationCountStatus()
{
	statusBar.SetPaneText(2, std::format(L"Iteration Count = {}", curve.getIterationCount()).data());
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CMainFrame - OVERRIDES
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if (CFrameWnd::PreCreateWindow(cs))
	{
		cs.style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | FWS_ADDTOTITLE;
		cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
		cs.lpszClass = ::AfxRegisterWndClass(0);
		return TRUE;
	}
	return FALSE;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// let the viewWnd have first crack at the command
	if (viewWnd.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo)) return TRUE;
	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CMainFrame - MESSAGE MAP FUNCTIONS
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_MESSAGE(DM_CURVE_TYPE_CHANGED, &CMainFrame::OnCurveTypeChanged)
	ON_MESSAGE(DM_ITERATION_COUNT_CHANGED, &CMainFrame::OnIterationCountChanged)
END_MESSAGE_MAP()
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == 0)
	{
		if (statusBar.Create(this))
		{
			statusBar.SetIndicators(
				indicatorArray.data(), 
				static_cast<int>(indicatorArray.size()));
			const int sx = ::GetSystemMetrics(SM_CXSCREEN);
			statusBar.SetPaneInfo(1, IDI_CURVE_TYPE, SBPS_NORMAL, sx * 7 / 100);
			statusBar.SetPaneInfo(2, IDI_ITERATION_COUNT, SBPS_NORMAL, sx * 7 / 100);
			if (viewWnd.Create(this, AFX_IDW_PANE_FIRST))
			{
				refreshCurveTypeStatus();
				refreshIterationCountStatus();
				return 0;
			}
		}
	}
	return -1;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{
	viewWnd.SetFocus();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
LRESULT CMainFrame::OnCurveTypeChanged(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	refreshCurveTypeStatus();
	return 0;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
LRESULT CMainFrame::OnIterationCountChanged(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	refreshIterationCountStatus();
	return 0;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
