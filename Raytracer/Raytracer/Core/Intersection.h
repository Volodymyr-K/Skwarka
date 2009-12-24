#ifndef INTERSECTION_H
#define INTERSECTION_H

#include <Common/Common.h>
#include "DifferentialGeometry.h"
#include "Primitive.h"

/**
* The structure holds the data describing the complete local surface properties at an intersection point.
* The data includes the pointer to the primitive, differential geometry object and the index of the intersected mesh triangle.
*/
struct Intersection
  {
  /**
  * Pointer to the intersected primitive.
  */
  const Primitive *mp_primitive;

  /**
  * Describes local surface geometry at the intersection point.
  */
  DifferentialGeometry m_dg;

  /**
  * Index of the intersected mesh triangle.
  */
  size_t m_triangle_index;

  /**
  * Specific parameter of ray-triangle intersection test.
  * This value is used to determine epsilon value for the outgoing rays to avoid self-intersection with the same triangle.
  */
  Vector3D_d m_cross;

  /**
  * Specific parameter of ray-triangle intersection test.
  * This value is used to determine epsilon value for the outgoing rays to avoid self-intersection with the same triangle.
  */
  double m_dot;
  };

#endif // INTERSECTION_H