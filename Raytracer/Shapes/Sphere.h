#ifndef SPHERE_H
#define SPHERE_H

#include "Shape.h"
#include <Math\Geometry.h>

class Sphere: public BaseShape
  {
  public:
    Sphere();

    shared_ptr<TriangleMesh> BuildMesh();

    ~Sphere();
  private:
    static const int DEFAULT_SUBDIVISIONS = 3;
  };

#endif // SPHERE_H