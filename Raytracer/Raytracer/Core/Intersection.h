#ifndef INTERSECTION_H
#define INTERSECTION_H

#include <Common/Common.h>
#include <Math/Geometry.h>

class Intersection
  {
  public:
    Intersection();
    Intersection(const Primitive &i_primitive, const DifferentialGeometry &i_dg);

  private:
    Primitive m_primitive;
    DifferentialGeometry m_dg;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

inline Intersection::Intersection()
  {
  }

inline Intersection::Intersection(const Primitive &i_primitive, const DifferentialGeometry &i_dg):
m_primitive(i_primitive), m_dg(i_dg)
  {
  }

#endif // INTERSECTION_H