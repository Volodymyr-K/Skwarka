#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <limits>

#define FLT_INF       (std::numeric_limits<float>::infinity())
#define FLT_EPS       (std::numeric_limits<float>::epsilon())  /* smallest such that 1.0+FLT_EPSILON != 1.0 */

#define DBL_INF       (std::numeric_limits<double>::infinity())
#define DBL_EPS       (std::numeric_limits<double>::epsilon())  /* smallest such that 1.0+DBL_EPSILON != 1.0 */
#define DBL_3D_EPS    (1e-9) /* an empirical epsilon value for 3D vector and point operations */

#ifdef M_PI
#undef M_PI
#undef M_PI_2
#undef M_PI_3
#undef M_PI_4
#undef M_PI_6
#endif

#define M_PI          3.14159265358979323846
#define	M_PI_2		  	1.57079632679489661923
#define	M_PI_3		  	1.04719755119659774615
#define	M_PI_4		  	0.78539816339744830911
#define	M_PI_6		  	0.52359877559829887308
#define INV_PI        0.31830988618379067154
#define INV_2PI       0.15915494309189533577

#endif // CONSTANTS_H