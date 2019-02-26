#pragma once




#include <iostream>
#include <cmath>
#include "numeric.h"

class Vector2
{
	public:
		//
		// Constructors
		//

		Vector2():x(0), y(0){}

		Vector2(double _x, double _y): x(_x), y(_y){}

		Vector2(const Vector2 &v): x(v.x), y(v.y){}

		//
		// Operations
		//
		double dist2(const Vector2 &v) const
		{
			double dx = x - v.x;
			double dy = y - v.y;
			return dx * dx + dy * dy;
		}

		double dist(const Vector2 &v) const
		{
			return sqrt(dist2(v));
		}

		double norm2() const
		{
			return x * x + y * y;
		}

		double x;
		double y;

};


//double Vector2::dist(const Vector2 &v) const { return hypot(x - v.x, y - v.y);}


inline std::ostream &operator << (std::ostream &str, Vector2 const &point)
{
	return str << "Point x: " << point.x << " y: " << point.y;
}


inline bool operator == (const Vector2& v1, const Vector2& v2)
{
	return (v1.x == v2.x) && (v1.y == v2.y);
}



 inline bool   almost_equal(const Vector2& v1, const Vector2& v2, int ulp=2)
{
	return almost_equal(v1.x, v2.x, ulp) && almost_equal(v1.y, v2.y, ulp);
}
 inline double half(double x) 
 { 
	 return 0.5 * x; 
 }
