//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ViewWnd.h
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Copyright (c) 2020 Adrian Maurer. All rights reserved.
// Distributed under the MIT software license (http://www.opensource.org/licenses/mit-license.php).
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#pragma once
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Function.h"
#include "InterpolationCurve.h"
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CViewWnd
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
using CDataPoint = TDataPoint<double>;
using CCurve = TInterpolationCurve<double>;
using CDataPointVect = TDataPointVect<double>;
using CControlPointVect = TControlPointVect<double>;
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
enum class EMouseState {none, left, leftCtrl, right};
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class CViewWnd : public CWnd
{
// ELEMENT DATA
private:
	CCurve* const p_curve;
	CSize wndSize;
	const int pointSize;
	const int contentOrigin;
	CSize contentSize;
	CDC memDc;
	CBitmap memBmp;
	const COLORREF backgroundColor;
	const COLORREF gridColor;
	const COLORREF textColor;
	const COLORREF controlPointColor;
	const COLORREF controlLineColor;
	CPen gridPen;
	CPen controlLinePen;
	CPen curvePen;
	CPen derivative1Pen;
	CPen derivative2Pen;
	bool gridVisible;
	bool dataPointsVisible;
	bool controlPointsVisible;
	bool labelsVisible;
	bool curveVisible;
	CDataPoint* p_selectedDataPoint;
	CPoint moveOffset;
	EMouseState mouseState;

// CONSTRUCTION / DESTRUCTION / ASSIGNMENT
public:
	CViewWnd(CCurve* const);

// ELEMENT FUNCTIONS
private:
	int xOut(const double x) const {return roundTo<int>(x * contentSize.cx + contentOrigin);}
	int yOut(const double y) const {return roundTo<int>((1.0 - y) * contentSize.cy + contentOrigin);}
	CPoint pOut(const TPoint<double>& p) const {return {xOut(p.x), yOut(p.y)};}
	double xIn(const int x) const {return (x - contentOrigin) / static_cast<double>(contentSize.cx);}
	double yIn(const int y) const {return (wndSize.cy - y - contentOrigin) / static_cast<double>(contentSize.cy);}
	TPoint<double> pIn(const CPoint& p) const {return {xIn(p.x), yIn(p.y)};}
	TPoint<double> pIn(const int x, const int y) const {return {xIn(x), yIn(y)};}
	void moveTo(const TPoint<double>& p) {memDc.MoveTo(pOut(p));}
	void lineTo(const TPoint<double>& p) {memDc.LineTo(pOut(p));}
	void drawPoint(const TPoint<double>& p, const COLORREF c) {memDc.FillSolidRect(xOut(p.x) - pointSize / 2, yOut(p.y) - pointSize / 2, pointSize, pointSize, c);}
	void drawLabel(const TPoint<double>&);
	TRect<double> createSelectionRect(const TPoint<double>&) const;
public:
	BOOL Create(CWnd*, UINT);
	void draw();
	void refresh();

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
