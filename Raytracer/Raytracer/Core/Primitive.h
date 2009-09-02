#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include <Common/Common.h>
#include "TriangleMesh.h"
#include "Primitive.h"
#include "DifferentialGeometry.h"

class Primitive
  {
  public:
    Primitive();
    Primitive(shared_ptr<TriangleMesh> ip_mesh);

  private:
    shared_ptr<TriangleMesh> mp_mesh;
    // NYI: material
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

inline Primitive::Primitive()
  {
  }

inline Primitive::Primitive(shared_ptr<TriangleMesh> ip_mesh):
mp_mesh(ip_mesh)
  {
  }

#endif // PRIMITIVE_H