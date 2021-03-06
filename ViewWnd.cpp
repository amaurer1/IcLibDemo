//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ViewWnd.cpp
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Copyright (c) 2020 Adrian Maurer. All rights reserved.
// Distributed under the MIT software license (http://www.opensource.org/licenses/mit-license.php).
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "pch.h"
#include "framework.h"
#include "IcLibDemoApp.h"
#include <fstream>
#include <filesystem>
#include "ViewWnd.h"
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CViewWnd
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CViewWnd - CONSTRUCTION / DESTRUCTION / ASSIGNMENT
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
CViewWnd::CViewWnd(CCurve* const p_c) : 
	CWnd(), 
	p_curve(p_c), 
	wndSize(0, 0), 
	contentOrigin(50), 
	contentSize(0, 0), 
	pointSize(15), 
	memDc(), 
	memBmp(), 
	backgroundColor(::GetSysColor(COLOR_WINDOW)), 
	gridColor(RGB(128, 128, 128)), 
	textColor(::GetSysColor(COLOR_WINDOWTEXT)), 
	controlPointColor(RGB(0, 0, 255)), 
	controlLineColor(RGB(128, 128, 128)), 
	gridPen(PS_SOLID, 1, gridColor), 
	controlLinePen(PS_SOLID, 2, controlLineColor), 
	curvePen(PS_SOLID, 3, ::GetSysColor(COLOR_WINDOWTEXT)), 
	derivative1Pen(PS_DASH, 3, RGB(0, 128, 255)), 
	derivative2Pen(PS_SOLID, 3, RGB(0, 255, 128)), 
	gridVisible(true), 
	dataPointsVisible(true), 
	controlPointsVisible(false), 
	labelsVisible(false), 
	curveVisible(true), 
	p_selectedDataPoint(nullptr), 
	moveOffset(0, 0), 
	mouseState(EMouseState::none)
{
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CViewWnd - ELEMENT FUNCTIONS
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CViewWnd::drawLabel(const TPoint<double>& vp)
{
	memDc.SetBkMode(OPAQUE);
	memDc.SetBkColor(backgroundColor);
	memDc.SetTextColor(textColor);
	CString s;
	s.Format(L"x = %.15f", vp.x);
	const CSize te(memDc.GetTextExtent(s));
	const CPoint tp(xOut(vp.x) - te.cx / 2, yOut(vp.y) - pointSize / 2 - 5);
	memDc.TextOut(tp.x, tp.y - te.cy * 2, s);
	s.Format(L"y = %.15f", vp.y);
	memDc.TextOut(tp.x, tp.y - te.cy * 1, s);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
TRect<double> CViewWnd::createSelectionRect(const TPoint<double>& p) const
{
	const TPoint<double> rs(pointSize / 2.0 / TPoint<double>(contentSize.cx, contentSize.cy));
	return TRect<double>(p, p).expand(rs);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BOOL CViewWnd::Create(CWnd* p_pw, UINT id)
{
	LPCTSTR cn = ::AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW, ::LoadCursor(nullptr, IDC_ARROW));
	return CWnd::CreateEx(WS_EX_CLIENTEDGE, cn, nullptr, WS_CHILD | WS_VISIBLE, {0, 0, 0, 0}, p_pw, id);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CViewWnd::draw()
{
	memDc.FillSolidRect(0, 0, wndSize.cx, wndSize.cy, backgroundColor);
	// Border
	if (CPen* const p_op = memDc.SelectObject(&gridPen))
	{
		memDc.MoveTo(contentOrigin, contentOrigin);
		memDc.LineTo(contentOrigin, contentOrigin + contentSize.cy);
		memDc.MoveTo(contentOrigin, contentOrigin);
		memDc.LineTo(contentOrigin + contentSize.cx, contentOrigin);
		memDc.MoveTo(contentOrigin + contentSize.cx, contentOrigin);
		memDc.LineTo(contentOrigin + contentSize.cx, contentOrigin + contentSize.cy);
		memDc.MoveTo(contentOrigin, contentOrigin + contentSize.cy);
		memDc.LineTo(contentOrigin + contentSize.cx, contentOrigin + contentSize.cy);
		memDc.SelectObject(p_op);
	}
	// Grid
	if (gridVisible)
	{
		if (CPen* const p_op = memDc.SelectObject(&gridPen))
		{
			for (int i = 1; i < 10; ++i)
			{
				const int x = static_cast<int>(i / 10.0 * contentSize.cx);
				memDc.MoveTo(contentOrigin + x, contentOrigin);
				memDc.LineTo(contentOrigin + x, contentOrigin + contentSize.cy);
				const int y = static_cast<int>(i / 10.0 * contentSize.cy);
				memDc.MoveTo(contentOrigin, contentOrigin + y);
				memDc.LineTo(contentOrigin + contentSize.cx, contentOrigin + y);
			}
			memDc.SelectObject(p_op);
		}
	}
	if (p_curve->size() > 1)
	{
		// Control point lines
		if (controlPointsVisible)
		{
			const CControlPointVect& cpv = p_curve->getControlPointVect();
			if (CPen* const p_op = memDc.SelectObject(&controlLinePen))
			{
				for (int i = 3; i < static_cast<int>(cpv.size()); i += 3)
				{
					if (cpv[i].x > cpv[i - 3].x)
					{
						moveTo(cpv[i - 3]);
						for (int j = i - 2; j < i + 1; ++j)
						{
							lineTo(cpv[j]);
						}
					}
				}
				memDc.SelectObject(p_op);
			}
		}
		// Curve
		if (curveVisible)
		{
			if (CPen* const p_op = memDc.SelectObject(&curvePen))
			{
				const CDataPointVect& dpv = *p_curve;
				for (int i = 1; i < static_cast<int>(dpv.size()); ++i)
				{
					if (dpv[i].x > dpv[i - 1].x)
					{
						moveTo(dpv[i - 1]);
						for (int j = xOut(dpv[i - 1].x) + 1; j < xOut(dpv[i].x) - 1; ++j)
						{
							memDc.LineTo(j, yOut(p_curve->calcValue(xIn(j))));
						}
						lineTo(dpv[i]);
					}
				}
				memDc.SelectObject(p_op);
			}
		}
		// Control points
		if (controlPointsVisible)
		{
			const CControlPointVect& cpv = p_curve->getControlPointVect();
			for (int i = 3; i < static_cast<int>(cpv.size()); i += 3)
			{
				if (cpv[i].x > cpv[i - 3].x)
				{
					if (!dataPointsVisible)
					{
						drawPoint(cpv[i - 3], controlPointColor);
						drawPoint(cpv[i - 0], controlPointColor);
					}
					drawPoint(cpv[i - 2], controlPointColor);
					drawPoint(cpv[i - 1], controlPointColor);
				}
			}
		}
	}
	// Data points
	if (dataPointsVisible)
	{
		for (const CDataPoint& p : *p_curve)
		{
			drawPoint(p, p.angular ? RGB(0, 255, 0) : RGB(255, 0, 0));
		}
	}
	// Labels
	if (labelsVisible)
	{
		if (controlPointsVisible)
		{
			const CControlPointVect& cpv = p_curve->getControlPointVect();
			for (int i = 3; i < static_cast<int>(cpv.size()); i += 3)
			{
				if (cpv[i].x > cpv[i - 3].x)
			{
					if (!dataPointsVisible)
					{
						drawLabel(cpv[i - 3]);
						drawLabel(cpv[i - 0]);
					}
					drawLabel(cpv[i - 2]);
					drawLabel(cpv[i - 1]);
				}
			}
		}
		if (dataPointsVisible)
		{
			for (const CDataPoint& p : *p_curve)
			{
				drawLabel(p);
			}
		}
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CViewWnd::refresh()
{
	Invalidate(FALSE);
	UpdateWindow();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CViewWnd - MESSAGE MAP FUNCTIONS
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CViewWnd, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_COMMAND(ID_FILE_SAVE_IMAGE_AS, &CViewWnd::OnFileSaveImageAs)
	ON_COMMAND_RANGE(ID_CURVE_TYPE_LINE_SEGMENTS, ID_CURVE_TYPE_FINITE_SPLINE, &CViewWnd::OnCurveType)
	ON_UPDATE_COMMAND_UI_RANGE(ID_CURVE_TYPE_LINE_SEGMENTS, ID_CURVE_TYPE_FINITE_SPLINE, &CViewWnd::OnUpdateCurveType)
	ON_COMMAND(ID_VIEW_GRID, &CViewWnd::OnViewGrid)
	ON_UPDATE_COMMAND_UI(ID_VIEW_GRID, &CViewWnd::OnUpdateViewGrid)
	ON_COMMAND(ID_VIEW_DATA_POINTS, &CViewWnd::OnViewDataPoints)
	ON_UPDATE_COMMAND_UI(ID_VIEW_DATA_POINTS, &CViewWnd::OnUpdateViewDataPoints)
	ON_COMMAND(ID_VIEW_CONTROL_POINTS, &CViewWnd::OnViewControlPoints)
	ON_UPDATE_COMMAND_UI(ID_VIEW_CONTROL_POINTS, &CViewWnd::OnUpdateViewControlPoints)
	ON_COMMAND(ID_VIEW_LABELS, &CViewWnd::OnViewLabels)
	ON_UPDATE_COMMAND_UI(ID_VIEW_LABELS, &CViewWnd::OnUpdateViewLabels)
	ON_COMMAND(ID_VIEW_CURVE, &CViewWnd::OnViewCurve)
	ON_UPDATE_COMMAND_UI(ID_VIEW_CURVE, &CViewWnd::OnUpdateViewCurve)
END_MESSAGE_MAP()
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int CViewWnd::OnCreate(LPCREATESTRUCT p_cs)
{
	if (CWnd::OnCreate(p_cs) == -1) return -1;
	// Create graphic resources
	const CSize bs(::GetSystemMetrics(SM_CXSCREEN), ::GetSystemMetrics(SM_CYSCREEN));
	CClientDC dc(this);
	if (!memDc.CreateCompatibleDC(&dc)) return -1;
	if (!memBmp.CreateCompatibleBitmap(&dc, bs.cx, bs.cy)) return -1;
	if (!memDc.SelectObject(&memBmp)) return -1;
	return 0;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CViewWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	wndSize = {cx, cy};
	contentSize = {cx - 2 * contentOrigin, cy - 2 * contentOrigin};
	draw();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CViewWnd::OnPaint() 
{
	CPaintDC dc(this);
	dc.BitBlt(0, 0, wndSize.cx, wndSize.cy, &memDc, 0, 0, SRCCOPY);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CViewWnd::OnLButtonDown(UINT flags, CPoint point)
{
	SetFocus();
	SetCapture();
	if (dataPointsVisible)
	{
		// Find point at mouse position
		const TRect<double> sr(createSelectionRect(pIn(point)));
		p_selectedDataPoint = p_curve->findDataPoint(sr);
		if (p_selectedDataPoint)
		{
			moveOffset = point - pOut(*p_selectedDataPoint);
			mouseState = flags & MK_CONTROL ? EMouseState::leftCtrl : EMouseState::left;
			draw();
			refresh();
		}
	}
	CWnd::OnLButtonDown(flags, point);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CViewWnd::OnLButtonUp(UINT flags, CPoint point)
{
	if (p_selectedDataPoint)
	{
		if (mouseState == EMouseState::leftCtrl)
		{
			p_selectedDataPoint->angular = !p_selectedDataPoint->angular;
			p_curve->initControlPointVect();
		}
		draw();
		refresh();
	}
	p_selectedDataPoint = nullptr;
	mouseState = EMouseState::none;
	::ReleaseCapture();
	CWnd::OnLButtonUp(flags, point);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CViewWnd::OnRButtonDown(UINT flags, CPoint point)
{
	SetCapture();
	SetFocus();
	if (dataPointsVisible)
	{
		// Find point at mouse position
		const TRect<double> sr(createSelectionRect(pIn(point)));
		p_selectedDataPoint = p_curve->findDataPoint(sr);
		if (p_selectedDataPoint) mouseState = EMouseState::right;
	}
	CWnd::OnRButtonDown(flags, point);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CViewWnd::OnRButtonUp(UINT flags, CPoint point)
{
	if (dataPointsVisible)
	{
		if (p_selectedDataPoint)
		{
			// Remove point at mouse position
			p_curve->removeDataPoint(p_selectedDataPoint);
		}
		else
		{
			// Insert point at mouse position
			const int x = limit<int>(point.x, contentOrigin, contentOrigin + contentSize.cx);
			const int y = limit<int>(point.y, contentOrigin, contentOrigin + contentSize.cy);
			p_curve->addDataPoint(pIn(x, y), flags & MK_CONTROL);
		}
		draw();
		refresh();
	}
	p_selectedDataPoint = nullptr;
	mouseState = EMouseState::none;
	::ReleaseCapture();
	CWnd::OnRButtonUp(flags, point);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CViewWnd::OnMouseMove(UINT flags, CPoint point)
{
	if (p_selectedDataPoint && mouseState == EMouseState::left)
	{
		p_selectedDataPoint->x = limit(xIn(point.x - moveOffset.x), 0.0, 1.0);
		p_selectedDataPoint->y = limit(yIn(point.y - moveOffset.y), 0.0, 1.0);
		p_curve->limitDataPointPosition(p_selectedDataPoint);
		p_curve->initControlPointVect();
		draw();
		refresh();
	}
	CWnd::OnMouseMove(flags, point);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BOOL CViewWnd::OnMouseWheel(UINT flags, short delta, CPoint point)
{
	if (mouseState == EMouseState::none && flags & MK_CONTROL)
	{
		const int c = p_curve->getIterationCount();
		if (p_curve->setIterationCount(delta > 0 ? c + 1 : c - 1))
		{
			draw();
			refresh();
			sendMessageToMainWnd(DM_ITERATION_COUNT_CHANGED);
		}
	}
	return CWnd::OnMouseWheel(flags, delta, point);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CViewWnd::OnKeyDown(UINT ch, UINT, UINT)
{
	if (mouseState == EMouseState::none)
	{
		switch (ch)
		{
			case 'G': gridVisible = !gridVisible; break;
			case 'D': dataPointsVisible = !dataPointsVisible; break;
			case 'C': controlPointsVisible = !controlPointsVisible; break;
			case 'F': curveVisible = !curveVisible; break;
			default: return;
		}
		draw();
		refresh();
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CViewWnd::OnKeyUp(UINT ch, UINT, UINT)
{
	if (p_selectedDataPoint && mouseState == EMouseState::left)
	{
		if (ch == VK_SHIFT)
		{
			draw();
			refresh();
		}
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CViewWnd::OnFileSaveImageAs()
{
	CFileDialog dlg(
		FALSE, 0, 0,
		OFN_ENABLESIZING,
		L"SVG Files (*.svg)|*.svg||");
	if (dlg.DoModal() == IDOK)
	{
		CString pn(dlg.GetPathName());
		if (pn.Right(4).CompareNoCase(L".svg")) pn += L".svg";
		if (std::filesystem::exists(LPCTSTR(pn)))
		{
			CString ms;
			ms.Format(L"The file %s already exists.\nDo you want to replace it?", LPCTSTR(pn));
			if (::AfxMessageBox(ms, MB_YESNO | MB_ICONEXCLAMATION | MB_DEFBUTTON2) == IDNO)
			{
				::AfxGetApp()->DoWaitCursor(-1);
				return;
			}
		}
		std::ofstream os(pn);
		os.setf(std::ios::showpoint | std::ios::fixed, std::ios::floatfield);
		os.precision(4);
		os << R"*(<?xml version="1.0" encoding="utf-8" standalone="no"?>)*" << std::endl;
		os << R"*(<svg)*" << std::endl;
		os << R"*(	version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink")*" << std::endl;
		os << R"*(	viewBox="-0.1 -0.1 1.2 1.2" width="500" height="500" style="transform:scaleY(-1.0); background-color:Window; color:WindowText;" >)*" << std::endl;
		os << R"*(	<!-- Border -->)*" << std::endl;
		os << R"*(	<rect x="0.0" y="0.0" width="1.0" height="1.0" fill="none" stroke="gray" stroke-width="0.0025px" />)*" << std::endl;
		if (gridVisible)
		{
			os << R"*(	<!-- Grid -->)*" << std::endl;
			for (int i = 1; i < 10; ++i)
			{
				const double v = i / 10.0;
				os << R"*(	<line x1=")*" << v << R"*(" y1="0.0000" x2=")*" << v << 
					R"*(" y2="1.0000" fill="none" stroke="gray" stroke-width="0.005px" />)*" << std::endl;
				os << R"*(	<line x1="0.0000" y1=")*" << v << R"*(" x2="1.0000" y2=")*" << v << 
					R"*(" fill="none" stroke="gray" stroke-width="0.005px" />)*" << std::endl;
			}
		}
		if (p_curve->size() > 1)
		{
			const CControlPointVect& cpv = p_curve->getControlPointVect();
			if (controlPointsVisible)
			{
				os << R"*(	<!-- Control point lines -->)*" << std::endl;
				os << R"*(	<path d=")*";
				for (int i = 3; i < cpv.size(); i += 3)
				{
					if (cpv[i - 0].x > cpv[i - 3].x)
					{
						os << R"*(M)*" << 
							cpv[i - 3].x << R"*(, )*" << cpv[i - 3].y << R"*( )*" << 
							cpv[i - 2].x << R"*(, )*" << cpv[i - 2].y << R"*( )*" << 
							cpv[i - 1].x << R"*(, )*" << cpv[i - 1].y << R"*( )*" << 
							cpv[i - 0].x << R"*(, )*" << cpv[i - 0].y << R"*( )*";
					}
				}
				os << R"*(" fill="none" stroke="gray" stroke-width="0.005px" />)*" << std::endl;
			}
			if (curveVisible)
			{
				os << R"*(	<!-- Curve -->)*" << std::endl;
				os << R"*(	<path d=")*";
				for (int i = 3; i < cpv.size(); i += 3)
				{
					if (cpv[i - 0].x > cpv[i - 3].x)
					{
						if (p_curve->getCurveType() == ECurveType::lineSegments)
						{
							os << R"*(M)*" << 
								cpv[i - 3].x << R"*(, )*" << cpv[i - 3].y << R"*( )*" << 
								cpv[i - 0].x << R"*(, )*" << cpv[i - 0].y << R"*( )*";
						}
						else
						{
							os << R"*(M)*" << 
								cpv[i - 3].x << R"*(, )*" << cpv[i - 3].y << R"*( C)*" << 
								cpv[i - 2].x << R"*(, )*" << cpv[i - 2].y << R"*( )*" << 
								cpv[i - 1].x << R"*(, )*" << cpv[i - 1].y << R"*( )*" << 
								cpv[i - 0].x << R"*(, )*" << cpv[i - 0].y << R"*( )*";
						}
					}
				}
				os << R"*(" fill="none" stroke="WindowText" stroke-width="0.005px" />)*" << std::endl;
			}
			if (controlPointsVisible)
			{
				os << R"*(	<!-- Control points -->)*" << std::endl;
				for (int i = 3; i < cpv.size(); i += 3)
				{
					if (cpv[i - 0].x > cpv[i - 3].x)
					{
						if (!dataPointsVisible)
						{
							os << R"*(	<rect x="-0.015" y="-0.015)*" << 
								R"*(" width="0.03" height="0.03" fill="blue" transform="translate()*" << 
								cpv[i - 3].x << R"*(, )*" << cpv[i - 3].y << R"*()" />)*" << std::endl;
						}
						os << R"*(	<rect x="-0.015" y="-0.015)*" << 
							R"*(" width="0.03" height="0.03" fill="blue" transform="translate()*" << 
							cpv[i - 2].x << R"*(, )*" << cpv[i - 2].y << R"*()" />)*" << std::endl;
						os << R"*(	<rect x="-0.015" y="-0.015)*" << 
							R"*(" width="0.03" height="0.03" fill="blue" transform="translate()*" << 
							cpv[i - 1].x << R"*(, )*" << cpv[i - 1].y << R"*()" />)*" << std::endl;
						if (!dataPointsVisible)
						{
							os << R"*(	<rect x="-0.015" y="-0.015)*" << 
								R"*(" width="0.03" height="0.03" fill="blue" transform="translate()*" << 
								cpv[i - 0].x << R"*(, )*" << cpv[i - 0].y << R"*()" />)*" << std::endl;
						}
					}
				}
			}
		}
		if (dataPointsVisible)
		{
			os << R"*(	<!-- Data points -->)*" << std::endl;
			for (const CDataPoint& p : *p_curve)
			{
				os << R"*(	<rect x="-0.015" y="-0.015)*" << 
					R"*(" width="0.03" height="0.03" fill=")*" << 
					(p.angular ? R"*(lime)*" : R"*(red)*") << 
					R"*(" transform="translate()*" << 
					p.x << R"*(, )*" << p.y << R"*()" />)*" << std::endl;
			}
		}
		os << R"*(</svg>)*" << std::endl;
		os.close();
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CViewWnd::OnCurveType(UINT id)
{
	p_curve->setCurveType(static_cast<ECurveType>(id - ID_CURVE_TYPE_LINE_SEGMENTS));
	p_curve->initControlPointVect();
	draw();
	refresh();
	sendMessageToMainWnd(DM_CURVE_TYPE_CHANGED);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CViewWnd::OnUpdateCurveType(CCmdUI* p_cmdUI)
{
	p_cmdUI->SetRadio(static_cast<UINT>(p_curve->getCurveType()) == p_cmdUI->m_nID - ID_CURVE_TYPE_LINE_SEGMENTS);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CViewWnd::OnViewGrid()
{
	gridVisible = !gridVisible;
	draw();
	refresh();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CViewWnd::OnUpdateViewGrid(CCmdUI* p_cmdUI)
{
	p_cmdUI->SetCheck(gridVisible);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CViewWnd::OnViewDataPoints()
{
	dataPointsVisible = !dataPointsVisible;
	draw();
	refresh();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CViewWnd::OnUpdateViewDataPoints(CCmdUI* p_cmdUI)
{
	p_cmdUI->SetCheck(dataPointsVisible);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CViewWnd::OnViewControlPoints()
{
	controlPointsVisible = !controlPointsVisible;
	draw();
	refresh();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CViewWnd::OnUpdateViewControlPoints(CCmdUI* p_cmdUI)
{
	p_cmdUI->SetCheck(controlPointsVisible);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CViewWnd::OnViewLabels()
{
	labelsVisible = !labelsVisible;
	draw();
	refresh();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CViewWnd::OnUpdateViewLabels(CCmdUI* p_cmdUI)
{
	p_cmdUI->SetCheck(labelsVisible);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CViewWnd::OnViewCurve()
{
	curveVisible = !curveVisible;
	draw();
	refresh();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CViewWnd::OnUpdateViewCurve(CCmdUI* p_cmdUI)
{
	p_cmdUI->SetCheck(curveVisible);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
