//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// InterpolationCurve.h
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Copyright (c) 2023 Adrian Maurer. All rights reserved.
// Distributed under the MIT software license (http://www.opensource.org/licenses/mit-license.php).
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifndef AMA_INTERPOLATION_CURVE_H
#define AMA_INTERPOLATION_CURVE_H
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Rectangle.h"
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
namespace ama {
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// DataPoint
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template <class T>
class DataPoint : public Point<T>
{
// ELEMENT DATA
public:
	bool angular;

// CONSTRUCTION / DESTRUCTION / ASSIGNMENT
public:
	DataPoint(const Point<T> = {}, const bool = false);
};
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// DataPoint - CONSTRUCTION / DESTRUCTION / ASSIGNMENT
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template <class T>
inline DataPoint<T>::DataPoint(const Point<T> p, const bool a) : 
	Point<T>(p), 
	angular(a)
{
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// InterpolationCurve
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template <class T> using DataPointVect = std::vector<DataPoint<T>>;
template <class T> using ControlPointVect = std::vector<Point<T>>;
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template <class T>
class InterpolationCurve
{
// ENUMS
public:
	enum class Type {lineSegments, cubicSpline, finiteSpline};

// ELEMENT DATA
private:
	Type type;
	int iterationCount;
	DataPointVect<T> dataPointVect;
	ControlPointVect<T> controlPointVect;

// CONSTRUCTION / DESTRUCTION / ASSIGNMENT
public:
	InterpolationCurve(const Type = Type::finiteSpline);

// ELEMENT FUNCTIONS
private:
	void initCubicSpline();
	void initFiniteSpline();
public:
	void setType(const Type t) {type = t;}
	Type getType() const {return type;}
	bool setIterationCount(const int);
	int getIterationCount() const {return iterationCount;}
	const DataPointVect<T>& getDataPointVect() const {return dataPointVect;}
	const ControlPointVect<T>& getControlPointVect() const {return controlPointVect;}
	void resetDataPointVect() {dataPointVect = {{{T(0.0), T(0.0)}, true}, {{T(1.0), T(1.0)}, true}};}
	DataPoint<T>* findDataPoint(const ama::Rectangle<T>&);
	void addDataPoint(const Point<T>&, const bool);
	void removeDataPoint(const DataPoint<T>* const);
	void limitDataPointPosition(DataPoint<T>* const);
	void initControlPointVect();
	T calcValue(const T) const;
};
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// InterpolationCurve - CONSTRUCTION / DESTRUCTION / ASSIGNMENT
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template <class T>
inline InterpolationCurve<T>::InterpolationCurve(const Type t) : 
	type(t), 
	iterationCount(5), 
	dataPointVect(), 
	controlPointVect()
{
	resetDataPointVect();
	initControlPointVect();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// InterpolationCurve - ELEMENT FUNCTIONS
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template <class T>
inline void InterpolationCurve<T>::initCubicSpline()
{
	const DataPointVect<T>& dpv = dataPointVect;
	ControlPointVect<T>& cpv = controlPointVect;
	for (int j = 0; j < iterationCount; ++j)
	{
		size_t i1 = 0;
		size_t i2 = 0;
		// First point
		cpv[i2 + 1].y = (cpv[i1].y + cpv[i1 + 2].y) * T(0.5);
		// Intermediate points
		for (i1 = 1, i2 = 3; i1 < dpv.size() - 1; ++i1, i2 += 3)
		{
			if (!dpv[i1].angular)
			{
				const Point<T> m(cpv[i2] - cpv[i2 - 2]);
				const Point<T> n(cpv[i2 + 2] - cpv[i2]);
				const Point<T> r(m * n.x * n.x + n * m.x * m.x);
				if (r.x != T(0.0))
				{
					const T s = r.y / r.x;
					cpv[i2 - 1].y = cpv[i2].y - s * (cpv[i2].x - cpv[i2 - 1].x);
					cpv[i2 + 1].y = cpv[i2].y + s * (cpv[i2 + 1].x - cpv[i2].x);
				}
				else
				{
					cpv[i2 - 1].y = cpv[i2 + 1].y = cpv[i2].y;
				}
			}
			else
			{
				cpv[i2 - 1].y = (cpv[i2 - 2].y + cpv[i2].y) * T(0.5);
				cpv[i2 + 1].y = (cpv[i2].y + cpv[i2 + 2].y) * T(0.5);
			}
		}
		// Last point
		cpv[i2 - 1].y = (cpv[i2 - 2].y + cpv[i2].y) * T(0.5);
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template <class T>
inline void InterpolationCurve<T>::initFiniteSpline()
{
	const DataPointVect<T>& dpv = dataPointVect;
	ControlPointVect<T>& cpv = controlPointVect;
	for (int j = 0; j < iterationCount; ++j)
	{
		size_t i1 = 0;
		size_t i2 = 0;
		// First point
		cpv[i2 + 1].y = dpv[i1].angular ? (cpv[i1].y + cpv[i1 + 2].y) * T(0.5) : dpv[i1].y;
		// Intermediate points
		for (i1 = 1, i2 = 3; i1 < dpv.size() - 1; ++i1, i2 += 3)
		{
			if (!dpv[i1].angular)
			{
				const Point<T> m(cpv[i2] - cpv[i2 - 2]);
				const Point<T> n(cpv[i2 + 2] - cpv[i2]);
				const Point<T> r(m * n.y + n * m.y);
				if (r.x != T(0.0) && r.y > T(0.0))
				{
					const T s = r.y / r.x;
					cpv[i2 - 1].y = cpv[i2].y - s * (cpv[i2].x - cpv[i2 - 1].x);
					cpv[i2 + 1].y = cpv[i2].y + s * (cpv[i2 + 1].x - cpv[i2].x);
				}
				else
				{
					cpv[i2 - 1].y = cpv[i2 + 1].y = cpv[i2].y;
				}
			}
			else
			{
				cpv[i2 - 1].y = (cpv[i2 - 2].y + cpv[i2].y) * T(0.5);
				cpv[i2 + 1].y = (cpv[i2].y + cpv[i2 + 2].y) * T(0.5);
			}
		}
		// Last point
		cpv[i2 - 1].y = dpv[i1].angular ? (cpv[i2 - 2].y + cpv[i2].y) * T(0.5) : dpv[i1].y;
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template <class T>
inline bool InterpolationCurve<T>::setIterationCount(const int c)
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
template <class T>
inline DataPoint<T>* InterpolationCurve<T>::findDataPoint(const ama::Rectangle<T>& ra)
{
	DataPointVect<T>& dpv = dataPointVect;
	auto it(std::find_if(dpv.begin(), dpv.end(), [this, &ra](const DataPoint<T>& dp)
	{
		return isPointInRectangle(dp, ra);
	}));
	return it != dpv.end() ? &(*it) : nullptr;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template <class T>
inline void InterpolationCurve<T>::addDataPoint(const Point<T>& ndp, const bool c)
{
	DataPointVect<T>& dpv = dataPointVect;
	if (dpv.empty())
	{
		dpv.emplace_back(ndp, c);
	}
	else
	{
		auto it(std::find_if(dpv.cbegin(), dpv.cend(), [&ndp](const DataPoint<T>& dp)
		{
			return ndp.x < dp.x;
		}));
		dpv.emplace(it, ndp, c);
		initControlPointVect();
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template <class T>
inline void InterpolationCurve<T>::removeDataPoint(const DataPoint<T>* const p_dp)
{
	DataPointVect<T>& dpv = dataPointVect;
	std::erase_if(dpv, [&p_dp](const DataPoint<T>& dp)
	{
		return p_dp == &dp;
	});
	initControlPointVect();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template <class T>
inline void InterpolationCurve<T>::limitDataPointPosition(DataPoint<T>* const p_dp)
{
	DataPointVect<T>& dpv = dataPointVect;
	if (p_dp && dpv.size() > 1)
	{
		T px = dpv.front().x;
		for (auto it(std::next(dpv.begin())); it != dpv.end(); ++it)
		{
			if (&(*it) == p_dp)
			{
				if (it->x < px) it->x = px;
				break;
			}
			px = it->x;
		}
		T nx = dpv.back().x;
		for (auto it(std::next(dpv.rbegin())); it != dpv.rend(); ++it)
		{
			if (&(*it) == p_dp)
			{
				if (it->x > nx) it->x = nx;
				break;
			}
			nx = it->x;
		}
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template<class T>
inline void InterpolationCurve<T>::initControlPointVect()
{
	const DataPointVect<T>& dpv = dataPointVect;
	ControlPointVect<T>& cpv = controlPointVect;
	if (dpv.size() > 1)
	{
		// First initizialization
		cpv.resize(dpv.size() * 3 - 2);
		cpv[0] = dpv[0];
		for (size_t i = 1; i < dpv.size(); ++i)
		{
			cpv[i * 3 - 2] = (dpv[i] - dpv[i - 1]) * T(1.0 / 3.0) + dpv[i - 1];
			cpv[i * 3 - 1] = (dpv[i] - dpv[i - 1]) * T(2.0 / 3.0) + dpv[i - 1];
			cpv[i * 3 - 0] = dpv[i];
		}
		// Iterative alignment
		switch (type)
		{
			case Type::cubicSpline: initCubicSpline(); break;
			case Type::finiteSpline: initFiniteSpline(); break;
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
inline T InterpolationCurve<T>::calcValue(const T x) const
{
	const ControlPointVect<T>& cpv = controlPointVect;
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
} //namespace ama
  //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#endif // AMA_INTERPOLATION_CURVE_H
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
