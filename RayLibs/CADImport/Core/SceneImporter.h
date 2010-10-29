#ifndef SCENE_IMPORTER_H
#define SCENE_IMPORTER_H

#include <Common/Common.h>
#include <Raytracer/Core/Scene.h>
#include <Raytracer/Core/Camera.h>
#include <vector>

class SceneImporter
  {
  public:
    virtual intrusive_ptr<const Scene> GetScene() const = 0;
    virtual std::vector<intrusive_ptr<const Camera> > GetCameras() const = 0;
  };

#endif // SCENE_IMPORTER_H
