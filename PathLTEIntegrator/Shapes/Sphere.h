#ifndef SPHERE_H
#define SPHERE_H

#include "Shape.h"
#include <Math/Geometry.h>

/**
* %Sphere implementation of the Shape interface.
* The triangulation method starts from a tetrahedron and then iteratively subdivides each face into four new triangles.
* The number of subdivision iterations is configurable.
* 
* The parameters for the sphere are the following:
*   - %Sphere center.
*     - Parameter name: "center"
*     - Parameter type: Point3D_f
*     - Parameter restrictions: none
*     - Required.
*   - %Sphere radius.
*     - Parameter name: "radius"
*     - Parameter type: float
*     - Parameter restrictions: should be greater than zero
*     - Required.
*   - Number of subdivisions.
*     - Parameter name: "subdivisions"
*     - Parameter type: int
*     - Parameter restrictions: should be greater or equal than zero
*     - Optional, default value is 3.
*/
class Sphere: public BaseShape
  {
  public:
    Sphere();

    intrusive_ptr<TriangleMesh> BuildMesh();

    ~Sphere();

  private:
    /**
    * An inner struct for all the parameters Sphere has.
    */
    struct Parameters;

  private:
    /**
    * Helper method to read all the parameters.
    * @return true if all the parameters have been read successfully.
    */
    bool _GetParameters(Sphere::Parameters &o_params);
  };


#endif // SPHERE_H