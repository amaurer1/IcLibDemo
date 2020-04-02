//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Function.h
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Copyright (c) 2020 Adrian Maurer. All rights reserved.
// Distributed under the MIT software license (http://www.opensource.org/licenses/mit-license.php).
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#pragma once
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template <class T>
inline T roundTo(const float v)
{
	return T(v > 0.0F ? v + 0.5F : v - 0.5F);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template <class T>
inline T roundTo(const double v)
{
	return T(v > 0.0 ? v + 0.5 : v - 0.5);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template <class T>
inline T limit(const T v, const T l1, const T l2)
{
	if (v < l1) return l1;
	if (v > l2) return l2;
	return v;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template <class I, class F>
inline I findLastIf(I b, I e, F f)
{
	auto it(e);
	while (it != b)
	{
		--it;
		if (f(*it)) return it;
	}
	return e;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------