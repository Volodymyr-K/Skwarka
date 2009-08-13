#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <limits>

#define FLT_INF       (std::numeric_limits<float>::infinity())
#define FLT_EPS       (std::numeric_limits<float>::epsilon())  /* smallest such that 1.0+FLT_EPSILON != 1.0 */

#define DBL_INF       (std::numeric_limits<double>::infinity())
#define DBL_EPS       (std::numeric_limits<double>::epsilon())  /* smallest such that 1.0+DBL_EPSILON != 1.0 */

#ifdef M_PI
#undef M_PI
#undef M_PI_2
#undef M_PI_3
#undef M_PI_4
#undef M_PI_6
#endif

#define M_PI          3.14159265358979323846f
#define	M_PI_2		  	1.57079632679489661923f
#define	M_PI_3		  	1.04719755119659774615f
#define	M_PI_4		  	0.78539816339744830911f
#define	M_PI_6		  	0.52359877559829887308f
#define INV_PI        0.31830988618379067154f
#define INV_2PI       0.15915494309189533577f

#endif // CONSTANTS_H