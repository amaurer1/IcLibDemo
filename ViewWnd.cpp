//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ViewWnd.cpp
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Copyright (c) 2019-2023 Adrian Maurer. All rights reserved.
// Distributed under the MIT software license (http://www.opensource.org/licenses/mit-license.php).
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "pch.h"
#include "framework.h"
#include "ViewWnd.h"
#include "IcLibDemoApp.h"
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CViewWnd
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CViewWnd - CONSTRUCTION / DESTRUCTION / ASSIGNMENT
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
CViewWnd::CViewWnd(Curve* const p_c) : 
	CWnd(), 
	p_curve(p_c), 
	memoryDc(), 
	memoryBitmap(), 
	gridPen(), 
	controlLinePen(), 
	curvePen(), 
	pointSize(0), 
	wndSize(0, 0), 
	contentOrigin(0), 
	contentSize(0, 0), 
	gridVisible(true), 
	dataPointsVisible(true), 
	controlPointsVisible(false), 
	labelsVisible(false), 
	curveVisible(true), 
	p_selectedDataPoint(nullptr), 
	mouseState(MouseState::none)
{
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CViewWnd - ELEMENT FUNCTIONS
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CViewWnd::drawLabel(const ama::Point<double>& p)
{
	memoryDc.SetBkMode(OPAQUE);
	memoryDc.SetBkColor(::GetSysColor(COLOR_WINDOW));
	memoryDc.SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
	const std::wstring xs(std::format(L"x = {:.15f}", p.x));
	const CSize te(memoryDc.GetTextExtent(xs.data()));
	const CPoint tp(xOut(p.x) - te.cx / 2, yOut(p.y) - pointSize / 2 - 5);
	memoryDc.TextOut(tp.x, tp.y - te.cy * 2, xs.data());
	const std::wstring ys(std::format(L"y = {:.15f}", p.y));
	memoryDc.TextOut(tp.x, tp.y - te.cy * 1, ys.data());
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
ama::Rectangle<double> CViewWnd::createSelectionRectangle(const ama::Point<double>& p) const
{
	const ama::Point<double> rs(pointSize / 2.0 / ama::Point<double>(contentSize.cx, contentSize.cy));
	return ama::Rectangle<double>(p, p).expand(rs);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CViewWnd::draw()
{
	memoryDc.FillSolidRect(0, 0, wndSize.cx, wndSize.cy, ::GetSysColor(COLOR_WINDOW));
	// Border
	if (CPen* const p_op = memoryDc.SelectObject(&gridPen))
	{
		memoryDc.MoveTo(contentOrigin, contentOrigin);
		memoryDc.LineTo(contentOrigin, contentOrigin + contentSize.cy);
		memoryDc.MoveTo(contentOrigin, contentOrigin);
		memoryDc.LineTo(contentOrigin + contentSize.cx, contentOrigin);
		memoryDc.MoveTo(contentOrigin + contentSize.cx, contentOrigin);
		memoryDc.LineTo(contentOrigin + contentSize.cx, contentOrigin + contentSize.cy);
		memoryDc.MoveTo(contentOrigin, contentOrigin + contentSize.cy);
		memoryDc.LineTo(contentOrigin + contentSize.cx, contentOrigin + contentSize.cy);
		memoryDc.SelectObject(p_op);
	}
	// Grid
	if (gridVisible)
	{
		if (CPen* const p_op = memoryDc.SelectObject(&gridPen))
		{
			for (int i = 1; i < 10; ++i)
			{
				const int x = static_cast<int>(i / 10.0 * contentSize.cx);
				memoryDc.MoveTo(contentOrigin + x, contentOrigin);
				memoryDc.LineTo(contentOrigin + x, contentOrigin + contentSize.cy);
				const int y = static_cast<int>(i / 10.0 * contentSize.cy);
				memoryDc.MoveTo(contentOrigin, contentOrigin + y);
				memoryDc.LineTo(contentOrigin + contentSize.cx, contentOrigin + y);
			}
			memoryDc.SelectObject(p_op);
		}
	}
	if (p_curve->getDataPointVect().size() > 1)
	{
		// Control point lines
		if (controlPointsVisible)
		{
			const ControlPointVect& cpv = p_curve->getControlPointVect();
			if (CPen* const p_op = memoryDc.SelectObject(&controlLinePen))
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
				memoryDc.SelectObject(p_op);
			}
		}
		// Curve
		if (curveVisible)
		{
			if (CPen* const p_op = memoryDc.SelectObject(&curvePen))
			{
				const DataPointVect& dpv = p_curve->getDataPointVect();
				for (int i = 1; i < static_cast<int>(dpv.size()); ++i)
				{
					if (dpv[i].x > dpv[i - 1].x)
					{
						moveTo(dpv[i - 1]);
						for (int j = xOut(dpv[i - 1].x) + 1; j < xOut(dpv[i].x) - 1; ++j)
						{
							memoryDc.LineTo(j, yOut(p_curve->calcValue(xIn(j))));
						}
						lineTo(dpv[i]);
					}
				}
				memoryDc.SelectObject(p_op);
			}
		}
		// Control points
		if (controlPointsVisible)
		{
			const COLORREF cpc = RGB(0, 0, 255); // Control point color
			const ControlPointVect& cpv = p_curve->getControlPointVect();
			for (int i = 3; i < static_cast<int>(cpv.size()); i += 3)
			{
				if (cpv[i].x > cpv[i - 3].x)
				{
					if (!dataPointsVisible)
					{
						drawPoint(cpv[i - 3], cpc);
						drawPoint(cpv[i - 0], cpc);
					}
					drawPoint(cpv[i - 2], cpc);
					drawPoint(cpv[i - 1], cpc);
				}
			}
		}
	}
	// Data points
	if (dataPointsVisible)
	{
		for (const DataPoint& p : p_curve->getDataPointVect())
		{
			drawPoint(p, p.angular ? RGB(0, 255, 0) : RGB(255, 0, 0));
		}
	}
	// Labels
	if (labelsVisible)
	{
		if (controlPointsVisible)
		{
			const ControlPointVect& cpv = p_curve->getControlPointVect();
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
			for (const DataPoint& p : p_curve->getDataPointVect())
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
BOOL CViewWnd::Create(CWnd* p_pw, UINT id)
{
	LPCTSTR cn = ::AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW, ::LoadCursor(nullptr, IDC_ARROW));
	return CWnd::CreateEx(WS_EX_CLIENTEDGE, cn, nullptr, WS_CHILD | WS_VISIBLE, {0, 0, 0, 0}, p_pw, id);
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
int CViewWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == 0)
	{
		if (memoryDc.CreateCompatibleDC(nullptr))
		{
			CClientDC dc(this);
			const CSize mws( // Maximum window size
				::GetSystemMetrics(SM_CXSCREEN), 
				::GetSystemMetrics(SM_CYSCREEN));
			if (memoryBitmap.CreateCompatibleBitmap(&dc, mws.cx, mws.cy))
			{
				if (memoryDc.SelectObject(&memoryBitmap))
				{
					const int lw = (std::max)(static_cast<int>(mws.cy) / 1000, 1);
					gridPen.CreatePen(PS_SOLID, lw, RGB(128, 128, 128));
					controlLinePen.CreatePen(PS_SOLID, lw * 2, RGB(128, 128, 128));
					curvePen.CreatePen(PS_SOLID, lw * 3, ::GetSysColor(COLOR_WINDOWTEXT));
					pointSize = mws.cy / 80;
					contentOrigin = dc.GetTextExtent(L"A").cy * 3;
					return 0;
				}
			}
		}
	}
	return -1;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CViewWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	wndSize = {cx, cy};
	contentSize = {cx - 2 * contentOrigin, cy - 2 * contentOrigin};
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CViewWnd::OnPaint() 
{
	draw();
	CPaintDC dc(this);
	dc.BitBlt(0, 0, wndSize.cx, wndSize.cy, &memoryDc, 0, 0, SRCCOPY);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CViewWnd::OnLButtonDown(UINT nflags, CPoint pnt)
{
	SetFocus();
	SetCapture();
	if (dataPointsVisible)
	{
		// Find point at mouse position
		const ama::Rectangle<double> sra(createSelectionRectangle(pIn(pnt)));
		p_selectedDataPoint = p_curve->findDataPoint(sra);
		if (p_selectedDataPoint)
		{
			mouseState = nflags & MK_CONTROL ? MouseState::leftCtrl : MouseState::left;
			refresh();
		}
	}
	CWnd::OnLButtonDown(nflags, pnt);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CViewWnd::OnLButtonUp(UINT nflags, CPoint pnt)
{
	if (p_selectedDataPoint)
	{
		if (mouseState == MouseState::leftCtrl)
		{
			p_selectedDataPoint->angular = !p_selectedDataPoint->angular;
			p_curve->initControlPointVect();
		}
		refresh();
	}
	p_selectedDataPoint = nullptr;
	mouseState = MouseState::none;
	::ReleaseCapture();
	CWnd::OnLButtonUp(nflags, pnt);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CViewWnd::OnRButtonDown(UINT nflags, CPoint pnt)
{
	SetCapture();
	SetFocus();
	if (dataPointsVisible)
	{
		// Find point at mouse position
		const ama::Rectangle<double> sra(createSelectionRectangle(pIn(pnt)));
		p_selectedDataPoint = p_curve->findDataPoint(sra);
		if (p_selectedDataPoint) mouseState = MouseState::right;
	}
	CWnd::OnRButtonDown(nflags, pnt);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CViewWnd::OnRButtonUp(UINT nflags, CPoint pnt)
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
			const int x = ama::limit<int>(pnt.x, contentOrigin, contentOrigin + contentSize.cx);
			const int y = ama::limit<int>(pnt.y, contentOrigin, contentOrigin + contentSize.cy);
			p_curve->addDataPoint(pIn(x, y), nflags & MK_CONTROL);
		}
		refresh();
	}
	p_selectedDataPoint = nullptr;
	mouseState = MouseState::none;
	::ReleaseCapture();
	CWnd::OnRButtonUp(nflags, pnt);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CViewWnd::OnMouseMove(UINT nflags, CPoint pnt)
{
	static CPoint prevPoint;
	if (p_selectedDataPoint && mouseState == MouseState::left)
	{
		const ama::Point<double> dp(pIn(pnt) - pIn(prevPoint)); // Difference point
		p_selectedDataPoint->x += dp.x;
		p_selectedDataPoint->y += dp.y;
		p_selectedDataPoint->x = ama::limit(p_selectedDataPoint->x, 0.0, 1.0);
		p_selectedDataPoint->y = ama::limit(p_selectedDataPoint->y, 0.0, 1.0);
		p_curve->limitDataPointPosition(p_selectedDataPoint);
		p_curve->initControlPointVect();
		refresh();
	}
	prevPoint = pnt;
	CWnd::OnMouseMove(nflags, pnt);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BOOL CViewWnd::OnMouseWheel(UINT nflags, short zDelta, CPoint pnt)
{
	if (mouseState == MouseState::none && nflags & MK_CONTROL)
	{
		const int c = p_curve->getIterationCount();
		if (p_curve->setIterationCount(zDelta > 0 ? c + 1 : c - 1))
		{
			refresh();
			sendMessageToMainWnd(DM_ITERATION_COUNT_CHANGED);
		}
	}
	return CWnd::OnMouseWheel(nflags, zDelta, pnt);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CViewWnd::OnKeyDown(UINT nChar, UINT /*nRepCnt*/, UINT /*nFlags*/)
{
	if (mouseState == MouseState::none)
	{
		switch (nChar)
		{
			case 'G': gridVisible = !gridVisible; break;
			case 'D': dataPointsVisible = !dataPointsVisible; break;
			case 'C': controlPointsVisible = !controlPointsVisible; break;
			case 'F': curveVisible = !curveVisible; break;
			default: return;
		}
		refresh();
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CViewWnd::OnKeyUp(UINT nChar, UINT /*nRepCnt*/, UINT /*nFlags*/)
{
	if (p_selectedDataPoint && mouseState == MouseState::left)
	{
		if (nChar == VK_SHIFT)
		{
			refresh();
		}
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CViewWnd::OnFileSaveImageAs()
{
	CFileDialog dlg(FALSE, 0, 0, 
		OFN_ENABLESIZING, 
		L"SVG Files (*.svg)|*.svg||", 
		nullptr, 0, FALSE);
	if (dlg.DoModal() == IDOK)
	{
		std::filesystem::path fp(dlg.GetPathName().GetString());
		std::wstring fe(fp.extension());
		transform(fe.cbegin(), fe.cend(), fe.begin(), towlower);
		if (fe != L".svg") fp += L".svg";
		if (std::filesystem::exists(fp.c_str()))
		{
			if (::AfxMessageBox(std::format(L"The file {} already exists.\nDo you want to replace it?", fp.c_str()).data(), 
				MB_YESNO | MB_ICONEXCLAMATION | MB_DEFBUTTON2) == IDNO)
			{
				::AfxGetApp()->DoWaitCursor(-1);
				return;
			}
		}
		std::ofstream os(fp);
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
		if (p_curve->getDataPointVect().size() > 1)
		{
			const ControlPointVect& cpv = p_curve->getControlPointVect();
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
						if (p_curve->getType() == Curve::Type::lineSegments)
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
			for (const DataPoint& p : p_curve->getDataPointVect())
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
	p_curve->setType(static_cast<Curve::Type>(id - ID_CURVE_TYPE_LINE_SEGMENTS));
	p_curve->initControlPointVect();
	refresh();
	sendMessageToMainWnd(DM_CURVE_TYPE_CHANGED);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CViewWnd::OnUpdateCurveType(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(static_cast<UINT>(p_curve->getType()) == pCmdUI->m_nID - ID_CURVE_TYPE_LINE_SEGMENTS);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CViewWnd::OnViewGrid()
{
	gridVisible = !gridVisible;
	refresh();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CViewWnd::OnUpdateViewGrid(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(gridVisible);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CViewWnd::OnViewDataPoints()
{
	dataPointsVisible = !dataPointsVisible;
	refresh();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CViewWnd::OnUpdateViewDataPoints(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(dataPointsVisible);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CViewWnd::OnViewControlPoints()
{
	controlPointsVisible = !controlPointsVisible;
	refresh();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CViewWnd::OnUpdateViewControlPoints(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(controlPointsVisible);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CViewWnd::OnViewLabels()
{
	labelsVisible = !labelsVisible;
	refresh();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CViewWnd::OnUpdateViewLabels(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(labelsVisible);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CViewWnd::OnViewCurve()
{
	curveVisible = !curveVisible;
	refresh();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CViewWnd::OnUpdateViewCurve(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(curveVisible);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
