#ifndef CYLINDER_H
#define CYLINDER_H

#include "Shape.h"
#include <Math/Geometry.h>
#include <Math/Transform.h>

/**
* %Cylinder implementation of the Shape interface.
* The class has no methods to set cylinder radius or height, instead it provides a method to set an arbitrary transformation.
* If no transformation is set, it will create a cylinder aligned with Z axis and with a unit radius with Z coordinate in [0;1] range.
* The number of subdivisions is configurable.
*/
class Cylinder: public Shape
  {
  public:
    Cylinder();

    /**
    * Sets number of cylinder subdivisions.
    * The final number of triangles in the mesh is equal to 2*i_subdivisions;
    * If the method is not called, the default number of subdivisions is 360.
    */
    void SetSubdivisions(size_t i_subdivisions);

    /**
    * Sets range of the phi angles of the cylinder.
    * If the method is not called, the default range is the entire [0;2*M_PI]
    */
    void SetPhiRange(double i_min_phi, double i_max_phi);

    /**
    * Sets transformation to be applied to the cylinder.
    * If the method is not called, the default transformation is the identity one which results
    * in a cylinder aligned with Z axis and with a unit radius with Z coordinate in [0;1] range.
    */
    void SetTransformation(const Transform &i_transform);

    /**
    * Builds cylinder triangle mesh.
    * @return Smart pointer to the built mesh. Never NULL.
    */
    virtual intrusive_ptr<TriangleMesh> BuildMesh();

  private:
    size_t m_subdivisions;
    Transform m_transform;

    double m_min_phi, m_max_phi;
  };


#endif // CYLINDER_H