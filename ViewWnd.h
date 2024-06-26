//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ViewWnd.h
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Copyright (c) 2019-2023 Adrian Maurer. All rights reserved.
// Distributed under the MIT software license (http://www.opensource.org/licenses/mit-license.php).
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#pragma once
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include <fstream>
#include <filesystem>
#include <format>
#include "Function.h"
#include "InterpolationCurve.h"
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CViewWnd
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
using DataPoint = ama::DataPoint<double>;
using Curve = ama::InterpolationCurve<double>;
using DataPointVect = ama::DataPointVect<double>;
using ControlPointVect = ama::ControlPointVect<double>;
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class CViewWnd : public CWnd
{
// ENUMS
	enum class MouseState {none, left, leftCtrl, right};

// ELEMENT DATA
private:
	Curve* const p_curve;
	CDC memoryDc;
	CBitmap memoryBitmap;
	CPen gridPen;
	CPen controlLinePen;
	CPen curvePen;
	int pointSize;
	CSize wndSize;
	int contentOrigin;
	CSize contentSize;
	bool gridVisible;
	bool dataPointsVisible;
	bool controlPointsVisible;
	bool labelsVisible;
	bool curveVisible;
	DataPoint* p_selectedDataPoint;
	MouseState mouseState;

// CONSTRUCTION / DESTRUCTION / ASSIGNMENT
public:
	CViewWnd(Curve* const);

// ELEMENT FUNCTIONS
private:
	int xOut(const double x) const {return ama::round(x * contentSize.cx + contentOrigin);}
	int yOut(const double y) const {return ama::round((1.0 - y) * contentSize.cy + contentOrigin);}
	CPoint pOut(const ama::Point<double>& p) const {return {xOut(p.x), yOut(p.y)};}
	double xIn(const int x) const {return (x - contentOrigin) / static_cast<double>(contentSize.cx);}
	double yIn(const int y) const {return (wndSize.cy - y - contentOrigin) / static_cast<double>(contentSize.cy);}
	ama::Point<double> pIn(const CPoint& p) const {return {xIn(p.x), yIn(p.y)};}
	ama::Point<double> pIn(const int x, const int y) const {return {xIn(x), yIn(y)};}
	void moveTo(const ama::Point<double>& p) {memoryDc.MoveTo(pOut(p));}
	void lineTo(const ama::Point<double>& p) {memoryDc.LineTo(pOut(p));}
	void drawPoint(const ama::Point<double>& p, const COLORREF c) {memoryDc.FillSolidRect(xOut(p.x) - pointSize / 2, yOut(p.y) - pointSize / 2, pointSize, pointSize, c);}
	void drawLabel(const ama::Point<double>&);
	ama::Rectangle<double> createSelectionRectangle(const ama::Point<double>&) const;
	void draw();
	void refresh();
public:
	BOOL Create(CWnd*, UINT);

// MESSAGE MAP FUNCTIONS
protected:
	afx_msg int OnCreate(LPCREATESTRUCT);
	afx_msg void OnSize(UINT, int, int);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT, CPoint);
	afx_msg void OnLButtonUp(UINT, CPoint);
	afx_msg void OnRButtonDown(UINT, CPoint);
	afx_msg void OnRButtonUp(UINT, CPoint);
	afx_msg void OnMouseMove(UINT, CPoint);
	afx_msg BOOL OnMouseWheel(UINT, short, CPoint);
	afx_msg void OnKeyDown(UINT, UINT, UINT);
	afx_msg void OnKeyUp(UINT, UINT, UINT);
	afx_msg void OnFileSaveImageAs();
	afx_msg void OnCurveType(UINT);
	afx_msg void OnUpdateCurveType(CCmdUI*);
	afx_msg void OnViewGrid();
	afx_msg void OnUpdateViewGrid(CCmdUI*);
	afx_msg void OnViewDataPoints();
	afx_msg void OnUpdateViewDataPoints(CCmdUI*);
	afx_msg void OnViewControlPoints();
	afx_msg void OnUpdateViewControlPoints(CCmdUI*);
	afx_msg void OnViewLabels();
	afx_msg void OnUpdateViewLabels(CCmdUI*);
	afx_msg void OnViewCurve();
	afx_msg void OnUpdateViewCurve(CCmdUI*);
	DECLARE_MESSAGE_MAP()
};
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
