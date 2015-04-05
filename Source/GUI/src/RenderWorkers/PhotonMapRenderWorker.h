#ifndef PHOTON_MAP_RENDER_WORKER_H
#define PHOTON_MAP_RENDER_WORKER_H

#include <QObject>

#include <Raytracer/Core/Renderer.h>
#include "../RenderWorker.h"
#include "../Params/PhotonMapParams.h"

class PhotonMapRenderWorker : public RenderWorker
  {
  Q_OBJECT

  public:
    PhotonMapRenderWorker(RenderCanvas *ip_canvas, intrusive_ptr<const Scene> ip_scene, intrusive_ptr<const Camera> ip_camera, intrusive_ptr<Log> ip_log,
                          PhotonMapParams *ip_params);

  public slots:
    virtual void process();
    virtual void stop();

  private:
    intrusive_ptr<Renderer> mp_renderer;
    PhotonMapParams *mp_params;
};

#endif // PHOTON_MAP_RENDER_WORKER_H

