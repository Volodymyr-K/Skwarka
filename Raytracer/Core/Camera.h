#ifndef CAMERA_H
#define CAMERA_H

#include <Common\Common.h">
#include <Math\Geometry.h>
#include "Film.h"

class Camera
  {
  public:
    Camera(const Transform &i_world2cam, shared_ptr<Film> ip_film);

  private:
  };

#endif // CAMERA_H