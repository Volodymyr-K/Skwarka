#ifndef LIGHTS_SAMPLING_STRATEGY_H
#define LIGHTS_SAMPLING_STRATEGY_H

#include <Common/Common.h>
#include <Math/Geometry.h>

/**
* An abstract base class defining the contract for direct lights sampling.
* The class has two methods that given a point in the scene return CDF for the lights contributing to the direct illumination at that point.
* The CDF defines the distributions of samples needed for a Monte Carlo integration to estimate the direct lighting from infinity lights and area lights.
* Delta lights are not sampled and therefore the CDF for them is not returned.
*/
class LightsSamplingStrategy: public ReferenceCounted
  {
  public:
    /**
    * Returns CDF for the infinity and area lights at the specified 3D point being shaded.
    * Provided output array should have enough space to fit as many values as there are infinity and area lights in the scene.
    * The CDF values will be ordered in the o_lights_CDF array so that the CDF values corresponding to the infinity lights are written before
    * the CDF values corresponding to the area lights. Infinity and area lights are ordered with respect to the order defined by LightSources class.
    * @param i_point 3D point being shaded.
    * @param[out] o_lights_CDF Output array where the CDF values will be written to. Should have enough space to fit as many values as there are infinity and area lights in the scene.
    * The CDF values will be in non-descending order and will all be in [0;1] range.
    */
    virtual void GetLightsCDF(const Point3D_d &i_point, double *o_lights_CDF) = 0;

    /**
    * Returns CDF for the infinity and area lights at the specified 3D point being shaded and for the specified surface normal.
    * Provided output array should have enough space to fit as many values as there are infinity and area lights in the scene.
    * The CDF values will be ordered in the o_lights_CDF array so that the CDF values corresponding to the infinity lights are written before
    * the CDF values corresponding to the area lights. Infinity and area lights are ordered with respect to the order defined by LightSources class.
    * The default implementation calls GetLightsCDF() method without the normal information.
    * @param i_point 3D point being shaded.
    * @param i_normal Surface normal at the specified point. Should be normalized.
    * @param[out] o_lights_CDF Output array where the CDF values will be written to. Should have enough space to fit as many values as there are infinity and area lights in the scene.
    * The CDF values will be in non-descending order and will all be in [0;1] range.
    */
    virtual void GetLightsCDF(const Point3D_d &i_point, const Vector3D_d &i_normal, double *o_lights_CDF);

    virtual ~LightsSamplingStrategy();
 
  protected:
    LightsSamplingStrategy();

  private:
    // Not implemented, not a value type.
    LightsSamplingStrategy(const LightsSamplingStrategy&);
    LightsSamplingStrategy &operator=(const LightsSamplingStrategy&);
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

inline LightsSamplingStrategy::LightsSamplingStrategy()
  {
  }

inline LightsSamplingStrategy::~LightsSamplingStrategy()
  {
  }

inline void LightsSamplingStrategy::GetLightsCDF(const Point3D_d &i_point, const Vector3D_d &i_normal, double *o_lights_CDF)
  {
  ASSERT(i_normal.IsNormalized());
  GetLightsCDF(i_point, o_lights_CDF);
  }

#endif // LIGHTS_SAMPLING_STRATEGY_H