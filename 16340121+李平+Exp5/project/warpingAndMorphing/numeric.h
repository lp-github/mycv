#pragma once


#include <cmath>
#include <limits>

/**
 * @brief use of machine epsilon to compare floating-point values for equality
 * http://en.cppreference.com/w/cpp/types/numeric_limits/epsilon
 */


/*bool    almost_equal(double x, double y, int ulp=2)
{
	
    // the machine epsilon has to be scaled to the magnitude of the values used
    // and multiplied by the desired precision in ULPs (units in the last place)
    return std::abs(x-y) <= std::numeric_limits<double>::epsilon() * std::abs(x+y) * ulp
    // unless the result is subnormal
        || std::abs(x-y) < std::numeric_limits<double>::min();
}
*/
inline bool almost_equal(double x, double y, int ulp = 2)
{
	// the machine epsilon has to be scaled to the magnitude of the values used
	// and multiplied by the desired precision in ULPs (units in the last place)
	return std::fabs(x - y) <= std::numeric_limits<double>::epsilon() * std::fabs(x + y) * ulp
		// unless the result is subnormal
		|| std::fabs(x - y) < std::numeric_limits<double>::min();
}
/*template<class T>
T half(T x){}*/


/*template <>
float half(float x){return 0.5f * x;}*/

//template <>
//double half(double x){return 0.5 * x;}

