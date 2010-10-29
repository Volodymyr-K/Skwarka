#include "PbrtRenderOptions.h"
#include <Raytracer/VolumeRegions/AggregateVolumeRegion.h>
#include "Factories/FilmFilterFactory.h"
#include "Factories/FilmFactory.h"
#include "Factories/CameraFactory.h"
#include "Factories/LightSourceFactory.h"
#include "PbrtUtils.h"

namespace PbrtImport
  {

  RenderOptions::RenderOptions(intrusive_ptr<Log> ip_log): mp_log(ip_log)
    {
    FilterName = "box";
    FilmName = "image";
    CameraName = "perspective";
    currentInstance = NULL;
    }

  intrusive_ptr<const Scene> RenderOptions::MakeScene()
    {
    intrusive_ptr<const VolumeRegion> p_volumeRegion;
    if (volumeRegions.size() == 0)
      p_volumeRegion = NULL;
    else if (volumeRegions.size() == 1)
      p_volumeRegion = volumeRegions[0];
    else
      p_volumeRegion.reset( new AggregateVolumeRegion(volumeRegions) );

    // Compute the world bounds for the construction of the light sources.
    BBox3D_d world_bounds;
    for(size_t i=0;i<primitives.size();++i)
      world_bounds.Unite( primitives[i]->GetTriangleMesh()->GetBounds() );
    if (p_volumeRegion)
      world_bounds.Unite( p_volumeRegion->GetBounds() );

    // Create the "delayed" lights.
    PbrtImport::LightSourceFactory light_source_factory(mp_log);
    for(size_t i=0;i<m_delayed_light_names.size();++i)
      {
      intrusive_ptr<const DeltaLightSource> p_delta_light =
        light_source_factory.CreateDeltaLight(m_delayed_light_names[i], m_delayed_light_transforms[i], m_delayed_light_params[i], world_bounds);
      intrusive_ptr<const InfiniteLightSource> p_infinite_light =
        light_source_factory.CreateInfiniteLight(m_delayed_light_names[i], m_delayed_light_transforms[i], m_delayed_light_params[i], world_bounds);

      if (p_delta_light == NULL && p_infinite_light == NULL)
        PbrtImport::Utils::LogError(mp_log, std::string("Light type \"") + m_delayed_light_names[i] + std::string("\" unknown or not supported."));
      else
        {
        if (p_delta_light)
          lights.m_delta_light_sources.push_back(p_delta_light);

        if (p_infinite_light)
          lights.m_infinite_light_sources.push_back(p_infinite_light);
        }
      }

    intrusive_ptr<const Scene> p_scene( new Scene(primitives, p_volumeRegion, lights) );

    primitives.clear();
    lights.m_area_light_sources.clear();
    lights.m_delta_light_sources.clear();
    lights.m_infinite_light_sources.clear();
    volumeRegions.clear();

    m_delayed_light_names.clear();
    m_delayed_light_transforms.clear();
    m_delayed_light_params.clear();

    return p_scene;
    }

  intrusive_ptr<const Camera> RenderOptions::MakeCamera() const
    {
    PbrtImport::FilmFilterFactory film_filter_factory(mp_log);
    PbrtImport::FilmFactory film_factory(mp_log);
    PbrtImport::CameraFactory camera_factory(mp_log);

    intrusive_ptr<const FilmFilter> p_filter = film_filter_factory.CreateFilmFilter(FilterName, FilterParams);
    if (!p_filter)
      {
      PbrtImport::Utils::LogError(mp_log, "Unable to create film filter.");
      return NULL;
      }

    intrusive_ptr<Film> p_film = film_factory.CreateFilm(FilmName, FilmParams, p_filter);
    if (!p_film)
      {
      PbrtImport::Utils::LogError(mp_log, "Unable to create film.");
      return NULL;
      }

    intrusive_ptr<const Camera> p_camera = camera_factory.CreateCamera(CameraName, CameraParams, CameraToWorld, p_film);
    if (!p_camera)
      PbrtImport::Utils::LogError(mp_log, "Unable to create camera.");

    return p_camera;
    }

  };