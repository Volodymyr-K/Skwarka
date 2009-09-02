#ifndef SPHERE_H
#define SPHERE_H

#include "Shape.h"
#include <Math/Geometry.h>

/**
* Sphere implementation of the Shape interface.
* The triangulation method starts from a tetrahedron and then iteratively subdivides each face into four new triangles.
* The number of subdivision iterations is configurable.
*/
class Sphere: public BaseShape
  {
  public:
    Sphere();

    shared_ptr<TriangleMesh> BuildMesh();

    ~Sphere();

  private:
    /**
    * Helper method to all the parameters.
    * @return true if all the parameters have been read successfully.
    */
    bool _GetParameters();

  private:
    /**
    * An inner struct for all the parameters Sphere needs.
    */
    struct Parameters
      {
      Parameters();

      Point3D_f m_center;
      float m_radius;
      int m_subdivisions;
      };

  private:
    Parameters m_params;
  };


#endif // SPHERE_H