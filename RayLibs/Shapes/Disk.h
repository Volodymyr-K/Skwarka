#ifndef DISK_H
#define DISK_H

#include "Shape.h"
#include <Math/Geometry.h>
#include <Math/Transform.h>

/**
* %Disk implementation of the Shape interface.
* The class has no methods to set (outer) disk radius, instead it provides a method to set an arbitrary transformation.
* The class provides methods to set inner radius of the disk (which result in a ring-shaped disk) and maximum phi angle (2*PI by default).
* If no transformation is set, it will create a disk centered at the origin with a unit radius and normal aligned with Z axis.
* The number of subdivisions is configurable.
*/
class Disk: public Shape
  {
  public:
    Disk();

    /**
    * Sets number of disk subdivisions.
    * The final number of triangles in the mesh is equal to i_subdivisions if inner radius is equal to zero and 2*i_subdivisions otherwise.
    * If the method is not called, the default number of subdivisions is 360.
    * @param i_subdivisions Number of subdivisions. Must be equal or greater than 3.
    */
    void SetSubdivisions(size_t i_subdivisions);

    /**
    * Sets inner radius of the disk.
    * If the method is not called, the default value is 0.
    * @param i_inner_radius Inner radius. Must be equal or greater than zero.
    */
    void SetInnerRadius(double i_inner_radius);

    /**
    * Sets maximum value of the phi angle of the disk.
    * The triangles will be created only for the range of the phi angles equal to [0;i_max_phi].
    * If the method is not called, the default value is 2*M_PI.
    */
    void SetMaxPhi(double i_max_phi);

    /**
    * Sets transformation to be applied to the disk.
    * If the method is not called, the default transformation is the identity one which results
    * in a disk centered at the origin with a unit radius and normal aligned with Z axis.
    */
    void SetTransformation(const Transform &i_transform);

    /**
    * Builds disk triangle mesh.
    * @return Smart pointer to the built mesh. Never NULL.
    */
    virtual intrusive_ptr<TriangleMesh> BuildMesh();

  private:
    /**
    * Helper method that builds disk with zero inner radius.
    */
    intrusive_ptr<TriangleMesh> _BuildFullMesh();

    /**
    * Helper method that builds disk with non-zero inner radius.
    */
    intrusive_ptr<TriangleMesh> _BuildPartialMesh();

  private:
    size_t m_subdivisions;
    Transform m_transform;

    double m_inner_radius, m_max_phi;
  };


#endif // DISK_H