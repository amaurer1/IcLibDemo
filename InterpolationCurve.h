//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// InterpolationCurve.h
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Copyright (c) 2020 Adrian Maurer. All rights reserved.
// Distributed under the MIT software license (http://www.opensource.org/licenses/mit-license.php).
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#pragma once
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Rect.h"
#include <vector>
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// TDataPoint
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template <class T>
class TDataPoint : public TPoint<T>
{
// ELEMENT DATA
public:
	bool angular;

// CONSTRUCTION / DESTRUCTION / ASSIGNMENT
public:
	TDataPoint(const TPoint<T> = {}, const bool = false);
};
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// TDataPoint - CONSTRUCTION / DESTRUCTION / ASSIGNMENT
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template <class T>
inline TDataPoint<T>::TDataPoint(const TPoint<T> p, const bool a) : 
	TPoint<T>(p), 
	angular(a)
{
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// TInterpolationCurve
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template <class T> using TDataPointVect = std::vector<TDataPoint<T>>;
template <class T> using TControlPointVect = std::vector<TPoint<T>>;
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
enum class ECurveType {lineSegments, cubicSpline, finiteSpline};
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template <class T>
class TInterpolationCurve : public TDataPointVect<T>
{
// ELEMENT DATA
private:
	ECurveType curveType;
	int iterationCount;
	TControlPointVect<T> controlPointVect;

// CONSTRUCTION / DESTRUCTION / ASSIGNMENT
public:
	TInterpolationCurve(const ECurveType = ECurveType::finiteSpline);
	TInterpolationCurve(const ECurveType, std::initializer_list<TDataPoint<T>>);
	TInterpolationCurve& operator=(std::initializer_list<TDataPoint<T>>);

// ELEMENT FUNCTIONS
private:
	void initCubicSpline();
	void initFiniteSpline();
public:
	TDataPoint<T>* findDataPoint(const TRect<T>&);
	void addDataPoint(const TPoint<T>&, const bool);
	void removeDataPoint(const TDataPoint<T>* const);
	void limitDataPointPosition(TDataPoint<T>* const);
	void setCurveType(const ECurveType t) {curveType = t;}
	ECurveType getCurveType() const {return curveType;}
	bool setIterationCount(const int);
	int getIterationCount() const {return iterationCount;}
	void initControlPointVect();
	const TControlPointVect<T>& getControlPointVect() const {return controlPointVect;}
	T calcValue(const T) const;
};
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// TInterpolationCurve - CONSTRUCTION / DESTRUCTION / ASSIGNMENT
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template <class T>
inline TInterpolationCurve<T>::TInterpolationCurve(const ECurveType t) : 
	TDataPointVect<T>(), 
	curveType(t), 
	iterationCount(5), 
	controlPointVect()
{
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template <class T>
inline TInterpolationCurve<T>::TInterpolationCurve(const ECurveType t, std::initializer_list<TDataPoint<T>> l) : 
	TDataPointVect<T>(l), 
	curveType(t), 
	iterationCount(5), 
	controlPointVect()
{
	initControlPointVect();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template <class T>
inline TInterpolationCurve<T>& TInterpolationCurve<T>::operator=(std::initializer_list<TDataPoint<T>> l)
{
	static_cast<TDataPointVect<T>*>(this)->operator=(l);
	initControlPointVect();
	return *this;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// TInterpolationCurve - ELEMENT FUNCTIONS
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template <class T>
inline void TInterpolationCurve<T>::initCubicSpline()
{
	const TDataPointVect<T>& dpv = *this;
	TControlPointVect<T>& cpv = controlPointVect;
	for (int j = 0; j < iterationCount; ++j)
	{
		size_t i = 0;
		size_t ii = 0;
		// First point
		cpv[ii + 1].y = (cpv[i].y + cpv[i + 2].y) / T(2.0);
		// Intermediate points
		for (i = 1, ii = 3; i < dpv.size() - 1; ++i, ii += 3)
		{
			if (!dpv[i].angular)
			{
				const TPoint<T> m(cpv[ii] - cpv[ii - 2]);
				const TPoint<T> n(cpv[ii + 2] - cpv[ii]);
				const TPoint<T> r(m * n.x * n.x + n * m.x * m.x);
				if (r.x != T(0.0))
				{
					const T s = r.y / r.x;
					cpv[ii - 1].y = cpv[ii].y - s * (cpv[ii].x - cpv[ii - 1].x);
					cpv[ii + 1].y = cpv[ii].y + s * (cpv[ii + 1].x - cpv[ii].x);
				}
				else
				{
					cpv[ii - 1].y = cpv[ii + 1].y = cpv[ii].y;
				}
			}
			else
			{
				cpv[ii - 1].y = (cpv[ii - 2].y + cpv[ii].y) / T(2.0);
				cpv[ii + 1].y = (cpv[ii].y + cpv[ii + 2].y) / T(2.0);
			}
		}
		// Last point
		cpv[ii - 1].y = (cpv[ii - 2].y + cpv[ii].y) / T(2.0);
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template <class T>
inline void TInterpolationCurve<T>::initFiniteSpline()
{
	const TDataPointVect<T>& dpv = *this;
	TControlPointVect<T>& cpv = controlPointVect;
	for (int j = 0; j < iterationCount; ++j)
	{
		size_t i = 0;
		size_t ii = 0;
		// First point
		cpv[ii + 1].y = dpv[i].angular ? (cpv[i].y + cpv[i + 2].y) / T(2.0) : dpv[i].y;
		// Intermediate points
		for (i = 1, ii = 3; i < dpv.size() - 1; ++i, ii += 3)
		{
			if (!dpv[i].angular)
			{
				const TPoint<T> m(cpv[ii] - cpv[ii - 2]);
				const TPoint<T> n(cpv[ii + 2] - cpv[ii]);
				const TPoint<T> r(m * n.y + n * m.y);
				if (r.x != T(0.0) && r.y > T(0.0))
				{
					const T s = r.y / r.x;
					cpv[ii - 1].y = cpv[ii].y - s * (cpv[ii].x - cpv[ii - 1].x);
					cpv[ii + 1].y = cpv[ii].y + s * (cpv[ii + 1].x - cpv[ii].x);
				}
				else
				{
					cpv[ii - 1].y = cpv[ii + 1].y = cpv[ii].y;
				}
			}
			else
			{
				cpv[ii - 1].y = (cpv[ii - 2].y + cpv[ii].y) / T(2.0);
				cpv[ii + 1].y = (cpv[ii].y + cpv[ii + 2].y) / T(2.0);
			}
		}
		// Last point
		cpv[ii - 1].y = dpv[i].angular ? (cpv[ii - 2].y + cpv[ii].y) / T(2.0) : dpv[i].y;
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template <class T>
inline TDataPoint<T>* TInterpolationCurve<T>::findDataPoint(const TRect<T>& sr)
{
	auto it(find_if(begin(), end(), [this, &sr](const TDataPoint<T>& p)
	{
		return p.x > sr[0].x && p.x < sr[1].x && p.y > sr[0].y && p.y < sr[1].y;
	}));
	return it != end() ? &(*it) : nullptr;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template <class T>
inline void TInterpolationCurve<T>::addDataPoint(const TPoint<T>& np, const bool c)
{
	if (empty())
	{
		emplace_back(np, c);
	}
	else
	{
		auto it(find_if(cbegin(), cend(), [&np](const TDataPoint<T>& p)
		{
			return np.x < p.x;
		}));
		emplace(it, np, c);
		initControlPointVect();
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template <class T>
inline void TInterpolationCurve<T>::removeDataPoint(const TDataPoint<T>* const p_sp)
{
	auto it(find_if(cbegin(), cend(), [&p_sp](const TDataPoint<T>& p)
	{
		return p_sp == &p;
	}));
	erase(it);
	initControlPointVect();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template <class T>
inline void TInterpolationCurve<T>::limitDataPointPosition(TDataPoint<T>* const p_sp)
{
	if (p_sp && size() > 1)
	{
		T px = front().x;
		for (auto it(++begin()); it != end(); ++it)
		{
			if (&(*it) == p_sp)
			{
				if (it->x < px) it->x = px;
				break;
			}
			px = it->x;
		}
		T nx = back().x;
		for (auto it(++rbegin()); it != rend(); ++it)
		{
			if (&(*it) == p_sp)
			{
				if (it->x > nx) it->x = nx;
				break;
			}
			nx = it->x;
		}
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template <class T>
inline bool TInterpolationCurve<T>::setIterationCount(const int c)
{
	if (c >= 0)
	{
		iterationCount = c;
		initControlPointVect();
		return true;
	}
	return false;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template<class T>
inline void TInterpolationCurve<T>::initControlPointVect()
{
	const TDataPointVect<T>& dpv = *this;
	TControlPointVect<T>& cpv = controlPointVect;
	if (dpv.size() > 1)
	{
		// First initizialization
		cpv.resize(size() * 3 - 2);
		cpv[0] = dpv[0];
		for (size_t i = 1; i < dpv.size(); ++i)
		{
			cpv[i * 3 - 2] = (dpv[i] - dpv[i - 1]) / T(3.0) + dpv[i - 1];
			cpv[i * 3 - 1] = (dpv[i] - dpv[i - 1]) / T(1.5) + dpv[i - 1];
			cpv[i * 3 - 0] = dpv[i];
		}
		// Iterative alignment
		switch (curveType)
		{
			case ECurveType::cubicSpline: initCubicSpline(); break;
			case ECurveType::finiteSpline: initFiniteSpline(); break;
			default: break;
		}
	}
	else
	{
		cpv.clear();
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template <class T>
inline T TInterpolationCurve<T>::calcValue(const T x) const
{
	const TControlPointVect<T>& cpv = controlPointVect;
	for (size_t i = 3; i < cpv.size(); i += 3)
	{
		if (x <= cpv[i].x)
		{
			if (const T d = cpv[i].x - cpv[i - 3].x)
			{
				const T& p1 = cpv[i - 3].y;
				const T& p2 = cpv[i - 2].y;
				const T& p3 = cpv[i - 1].y;
				const T& p4 = cpv[i - 0].y;
				const T c1 = T(1.0) * p1;
				const T c2 = T(3.0) * p2 - T(3.0) * p1;
				const T c3 = T(3.0) * p3 - T(6.0) * p2 + T(3.0) * p1;
				const T c4 = T(1.0) * p4 - T(3.0) * p3 + T(3.0) * p2 - T(1.0) * p1;
				const T xx = (x - cpv[i - 3].x) / d;
				return ((c4 * xx + c3) * xx + c2) * xx + c1;
			}
		}
	}
	return T(0.0);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
