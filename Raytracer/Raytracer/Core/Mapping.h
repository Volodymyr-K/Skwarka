#ifndef MAPPING_H
#define MAPPING_H

#include <Common/Common.h>
#include <Math/Geometry.h>
#include "DifferentialGeometry.h"

/**
* An abstract base class defining the contract for mapping surface point described by a DifferentialGeometry object to a 2D point.
* @sa Mapping3D
*/
class Mapping2D : public ReferenceCounted
  {
  public:
    /**
    * Maps DifferentialGeometry to a 2D point.
    * @param i_dg DifferentialGeometry object describing the surface point.
    * @param[out] o_point Mapped 2D point.
    * @param[out] o_dp_dx Mapped X screen-space differential.
    * @param[out] o_dp_dy Mapped Y screen-space differential.
    */
    virtual void Map(const DifferentialGeometry &i_dg, Point2D_d &o_point, Vector2D_d &o_dp_dx, Vector2D_d &o_dp_dy) const = 0;

    virtual ~Mapping2D();

  protected:
    Mapping2D();
  };

/**
* An abstract base class defining the contract for mapping surface point described by a DifferentialGeometry object to a 3D point.
* @sa Mapping2D
*/
class Mapping3D : public ReferenceCounted
  {
  public:
    /**
    * Maps DifferentialGeometry to a 3D point.
    * @param i_dg DifferentialGeometry object describing the surface point.
    * @param[out] o_point Mapped 3D point.
    * @param[out] o_dp_dx Mapped X screen-space differential.
    * @param[out] o_dp_dy Mapped Y screen-space differential.
    */
    virtual void Map(const DifferentialGeometry &i_dg, Point3D_d &o_point, Vector3D_d &o_dp_dx, Vector3D_d &o_dp_dy) const = 0;

    virtual ~Mapping3D();

  protected:
    Mapping3D();
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

inline Mapping2D::Mapping2D()
  {
  }

inline Mapping2D::~Mapping2D()
  {
  }

inline Mapping3D::Mapping3D()
  {
  }

inline Mapping3D::~Mapping3D()
  {
  }

#endif // MAPPING_H