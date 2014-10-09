#ifndef SHAPE_H
#define SHAPE_H

#include <Common/Common.h>
#include <Raytracer/Core/TriangleMesh.h>

/**
* Interface class for all shapes that can be triangulated into a TriangleMesh.
* This is a base class with the only pure virtual method to build the triangle mesh.
* @sa TriangleMesh
*/
class Shape
  {
  public:

    /**
    * Builds triangle mesh.
    * @return Smart pointer to the built mesh or a NULL pointer if the mesh can not be built (e.g. not all required parameters are set yet).
    */
    virtual intrusive_ptr<TriangleMesh> BuildMesh() = 0;

    virtual ~Shape();

  protected:
    Shape();

  private:
    // Not implemented, Shape is not a value type.
    Shape(const Shape&);
    Shape &operator=(const Shape&);
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

inline Shape::Shape()
  {
  }

inline Shape::~Shape()
  {
  }

#endif // SHAPE_H