#ifndef TEXTURE_H
#define TEXTURE_H

#include <Common/Common.h>
#include "DifferentialGeometry.h"

/**
* Base abstract class defining the contract for evaluating a value for a surface point.
* The only method returns the value corresponding to the specified DifferentialGeometry and triangle index.
* The template parameter corresponds to the value type.
*/
template<typename T>
class Texture : public ReferenceCounted
  {
  public:
    /**
    * Returns the value corresponding to the specified DifferentialGeometry and triangle index.
    */
    virtual T Evaluate(const DifferentialGeometry &i_dg, size_t i_triangle_index) const = 0;

    virtual ~Texture();

  protected:
    Texture();

  private:
    // Not implemented, not a value type.
    Texture(const Texture&);
    Texture &operator=(const Texture&);
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
Texture<T>::Texture()
  {
  }

template<typename T>
Texture<T>::~Texture()
  {
  }

/**
* Serializes Texture to/from the specified Archive. This method is used by the boost serialization framework.
*/
template<typename T, class Archive>
void serialize(Archive &i_ar, Texture<T> &i_texture, const unsigned int i_version)
  {
  // Nothing to do here, everything must be serialized by the derived classes.

  // Just call the serialization for the base ReferenceCounted class.
  i_ar & boost::serialization::base_object<ReferenceCounted>(i_texture);
  }

#endif // TEXTURE_H