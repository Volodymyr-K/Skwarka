#ifndef RENDER_WORKER_H
#define RENDER_WORKER_H

#include <QObject>

#include <Common/Common.h>
#include <Raytracer/Core/Scene.h>
#include <Raytracer/Core/Camera.h>

#include "RenderCanvas.h"

class RenderWorker : public QObject
  {
  Q_OBJECT

  protected:
    RenderWorker(RenderCanvas *ip_canvas, intrusive_ptr<const Scene> ip_scene, intrusive_ptr<const Camera> ip_camera, intrusive_ptr<Log> ip_log);

    intrusive_ptr<const Scene> getScene() const;

    intrusive_ptr<const Camera> getCamera() const;

    intrusive_ptr<Log> getLog() const;

    RenderCanvas *getCanvas() const;

  public:
    virtual ~RenderWorker();

  public slots:
    virtual void process() = 0;
    virtual void stop() = 0;

  signals:
    void finished();

  private:
    intrusive_ptr<const Scene> mp_scene;
    intrusive_ptr<const Camera> mp_camera;
    intrusive_ptr<Log> mp_log;

    RenderCanvas *mp_canvas;
};

#endif // RENDER_WORKER_H
