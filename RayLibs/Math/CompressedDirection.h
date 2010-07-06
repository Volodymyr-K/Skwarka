#ifndef COMPRESSED_DIRECTION_H
#define COMPRESSED_DIRECTION_H

#include <Common/Common.h>
#include "Vector3D.h"

/**
* Compact direction representation that fits in only 2 bytes.
* The representation is based on the method described by Raphael Baptista at http://www.gamedev.net/reference/articles/article1252.asp
*
* The general idea is that we reserve 3 bits (out of 16) for signs. The remaining 13 bits are used to quantize directions in one octant (where X,Y,Z > 0).
* Directions (which can be considered as points on a unit sphere centered at origin) are projected onto triangle with veretices at (1,0,0), (0,1,0) and (0,0,1).
* A point on the triangle is defined by two coordinates, say X' and Y'.
* The coordinates are defined as follows: X' = X / (X+Y+Z) and Y' = Y / (X+Y+Z) where X,Y,Z are coordinates of the input (normalized) vector in octant X,Y,Z > 0.
* X' and Y' are both in range [0;1]. X' and Y' are quantized into 128 equal regions. Although naive implementation will require 7+7 bits to pack X' and Y'
* it is easy to see that since X'+Y' is also in range [0;1] all possible (X',Y') points lie under the diagonal of [0..127]x[0..127] domain.
* So, 13 bits are just enough to represent X' and Y' both quantized into 128 regions.
*
* The class uses statically precomputed Vector3D_d values for all possible compressed directions, so that the reverse conversion is just as simple as a table lookup.
*/
class CompressedDirection
  {
  public:
    CompressedDirection();

    /**
    * Creates CompressedDirection from the specified Vector3D.
    */
    template<typename T>
    explicit CompressedDirection(Vector3D<T> i_vector);

    /**
    * Converts the compressed direction back to Vector3D instance.
    */
    template<typename T>
    Vector3D<T> ToVector3D() const;

    /**
    * Returns ID of the compressed direction which can be used as an unique ID for the direction.
    */
    unsigned short GetID() const;

    /**
    * Returns CompressedDirection for the specified ID.
    */
    static CompressedDirection FromID(unsigned short i_id);

  private:
    static const unsigned int X_SIGN_MASK = 1<<13;
    static const unsigned int Y_SIGN_MASK = 1<<14;
    static const unsigned int Z_SIGN_MASK = 1<<15;
    static const unsigned int SIGNS_MASK = (1<<15) | (1<<14) | (1<<13);

    static const unsigned int X_BITS_MASK = (1<<6) | (1<<7) | (1<<8) | (1<<9) | (1<<10) | (1<<11) | (1<<12);
    static const unsigned int Y_BITS_MASK = (1<<0) | (1<<1) | (1<<2) | (1<<3) | (1<<4) | (1<<5);

  private:
    // This is a friend class that initializes static m_vectors array during application startup.
    friend class CompressedDirection_StaticInitializer;

    // Needed for the boost serialization framework.
    friend class boost::serialization::access;

    /**
    * Serializes CompressedDirection to/from the specified Archive. This method is used by the boost serialization framework.
    */
    template<typename Archive>
    void serialize(Archive &i_ar, const unsigned int i_version);

  private:
    unsigned short m_data;

    /**
    * Static array used for lookup.
    */
    static Vector3D_d m_vectors[1<<16];
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

inline CompressedDirection::CompressedDirection(): m_data(0)
  {
  }

template<typename T>
CompressedDirection::CompressedDirection(Vector3D<T> i_vector): m_data(0)
  {
  ASSERT(i_vector.IsNormalized());

  // Set sign bits.
  if (i_vector[0]<0) {m_data |= X_SIGN_MASK;i_vector[0]=-i_vector[0];}
  if (i_vector[1]<0) {m_data |= Y_SIGN_MASK;i_vector[1]=-i_vector[1];}
  if (i_vector[2]<0) {m_data |= Z_SIGN_MASK;i_vector[2]=-i_vector[2];}

  // 126.999 is used as a value slightly less than 127.0 so that if the input vector is equal to (1,0,0) for example
  // the quantized X' value will be rounded down to 126, not to 127.
  // This is so because we don't use values on the diagonal.
  double scale = 126.999 / (i_vector[0]+i_vector[1]+i_vector[2]);
  unsigned char x = (unsigned char) (i_vector[0] * scale);
  unsigned char y = (unsigned char) (i_vector[1] * scale);
  ASSERT(x>=0 && x<127);
  ASSERT(y>=0 && y<127);
  ASSERT(x+y<127);

  if (y>=64)
    {
    x = 127-x;
    y = 127-y;
    }

  m_data |= (x<<6) | y;
  }

template<typename T>
Vector3D<T> CompressedDirection::ToVector3D() const
  {
  return Convert<T>(m_vectors[m_data]);
  }

inline unsigned short CompressedDirection::GetID() const
  {
  return m_data;
  }

inline CompressedDirection CompressedDirection::FromID(unsigned short i_id)
  {
  CompressedDirection ret;
  ret.m_data = i_id;
  return ret;
  }

template<typename Archive>
void CompressedDirection::serialize(Archive &i_ar, const unsigned int i_version)
  {
  i_ar & m_data;
  }

// Don't store class info for CompressedDirection.
BOOST_CLASS_IMPLEMENTATION(CompressedDirection, boost::serialization::object_serializable)

#endif // COMPRESSED_DIRECTION_H
