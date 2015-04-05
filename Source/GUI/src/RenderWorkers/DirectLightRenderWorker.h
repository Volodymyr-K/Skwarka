#ifndef DIRECT_LIGHT_RENDER_WORKER_H
#define DIRECT_LIGHT_RENDER_WORKER_H

#include <QObject>

#include <Raytracer/Core/Renderer.h>
#include "../RenderWorker.h"
#include "../Params/DirectLightParams.h"

class DirectLightRenderWorker : public RenderWorker
  {
  Q_OBJECT

  public:
    DirectLightRenderWorker(RenderCanvas *ip_canvas, intrusive_ptr<const Scene> ip_scene, intrusive_ptr<const Camera> ip_camera, intrusive_ptr<Log> ip_log,
                            DirectLightParams *ip_params);

  public slots:
    virtual void process();
    virtual void stop();

  private:
    intrusive_ptr<Renderer> mp_renderer;
    DirectLightParams *mp_params;
};

#endif // DIRECT_LIGHT_RENDER_WORKER_H

