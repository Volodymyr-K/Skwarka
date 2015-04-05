#include "RenderWorker.h"

RenderWorker::RenderWorker(RenderCanvas *ip_canvas, intrusive_ptr<const Scene> ip_scene, intrusive_ptr<const Camera> ip_camera, intrusive_ptr<Log> ip_log):
  mp_canvas(ip_canvas), mp_scene(ip_scene), mp_camera(ip_camera), mp_log(ip_log)
  {
  ASSERT(ip_canvas);
  ASSERT(ip_scene);
  ASSERT(ip_camera);
  ASSERT(ip_log);
  }

RenderWorker::~RenderWorker()
  {
  }

intrusive_ptr<const Scene> RenderWorker::getScene() const
  {
  return mp_scene;
  }

intrusive_ptr<const Camera> RenderWorker::getCamera() const
  {
  return mp_camera;
  }

intrusive_ptr<Log> RenderWorker::getLog() const
  {
  return mp_log;
  }

RenderCanvas *RenderWorker::getCanvas() const
  {
  return mp_canvas;
  }
