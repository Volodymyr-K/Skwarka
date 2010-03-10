#include "CompressedDirection.h"

Vector3D_d CompressedDirection::m_vectors[1<<16];

class CompressedDirection_StaticInitializer
  {
  public:
    CompressedDirection_StaticInitializer()
      {
      for(size_t i=0;i<(1<<16);++i)
        {
        unsigned char x = (i&CompressedDirection::X_BITS_MASK)>>6;
        unsigned char y = (i&CompressedDirection::Y_BITS_MASK);

        if (x+y >= 127)
          {
          x = 127 - x;
          y = 127 - y;
          } 

        Vector3D_d direction(x+0.5, y+0.5, 126-x-y);
        direction.Normalize();

        if (i&CompressedDirection::X_SIGN_MASK) direction[0] = -direction[0];
        if (i&CompressedDirection::Y_SIGN_MASK) direction[1] = -direction[1];
        if (i&CompressedDirection::Z_SIGN_MASK) direction[2] = -direction[2];

        CompressedDirection::m_vectors[i] = direction;
        }
      }
  };

CompressedDirection_StaticInitializer global_compressed_direction_static_initializer;