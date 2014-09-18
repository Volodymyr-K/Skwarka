#ifndef PBRT_CAMERA_FACTORY
#define PBRT_CAMERA_FACTORY

#include <Common/Common.h>
#include "../PbrtParamSet.h"
#include <Raytracer/Cameras/PerspectiveCamera.h>
#include <Raytracer/Films/ImageFilm.h>
#include <Math/Transform.h>
#include <Math/MathRoutines.h>
#include "../PbrtUtils.h"

namespace PbrtImport
  {

  class CameraFactory
    {
    public:
      CameraFactory(intrusive_ptr<Log> ip_log): mp_log(ip_log) {}

      intrusive_ptr<const Camera> CreateCamera(const std::string &i_name, const ParamSet &i_params, const Transform &i_camera_to_world, intrusive_ptr<Film> ip_film) const
        {
        Transform camera_to_world = i_camera_to_world;

        if (i_name == "perspective")
          return _CreatePerspectiveCamera(i_params, camera_to_world, ip_film);
        else if (i_name == "orthographic")
          return _CreateOrthographicCamera(i_params, camera_to_world, ip_film);
        else if (i_name == "environment")
          return _CreateEnvironmentCamera(i_params, camera_to_world, ip_film);
        else
          PbrtImport::Utils::LogError(mp_log, std::string("Camera \"") + i_name + std::string("\" unknown."));

        return NULL;
        }

    private:
      intrusive_ptr<const Camera> _CreatePerspectiveCamera(const ParamSet &i_params, const Transform &i_transform, intrusive_ptr<Film> ip_film) const
        {
        float lensradius = i_params.FindOneFloat("lensradius", 0.f);
        float focaldistance = i_params.FindOneFloat("focaldistance", 1e30f);
        float fov = i_params.FindOneFloat("fov", 90.);
        float halffov = i_params.FindOneFloat("halffov", -1.f);
        if (halffov > 0.f)
          // hack for structure synth, which exports half of the full fov
          fov = 2.f * halffov;

        double frame = i_params.FindOneFloat("frameaspectratio", ip_film->GetXResolution()/(float)ip_film->GetYResolution());
        double half_fov_tan = tan(MathRoutines::DegreesToRadians(fov)/2.0);
        double new_fov = atan(half_fov_tan * std::max(1.0,frame))*2.0;
        return new PerspectiveCamera(i_transform, ip_film, lensradius, focaldistance, new_fov);
        }

      intrusive_ptr<const Camera> _CreateOrthographicCamera(const ParamSet &i_params, const Transform &i_transform, intrusive_ptr<Film> ip_film) const
        {
        PbrtImport::Utils::LogError(mp_log, "Orthographic camera is not supported");
        return NULL;
        }

      intrusive_ptr<const Camera> _CreateEnvironmentCamera(const ParamSet &i_params, const Transform &i_transform, intrusive_ptr<Film> ip_film) const
        {
        PbrtImport::Utils::LogError(mp_log, "Environment camera is not supported");
        return NULL;
        }

    private:
      intrusive_ptr<Log> mp_log;
    };

  };

#endif // PBRT_CAMERA_FACTORY
