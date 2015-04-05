/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Quick Controls module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "RenderHandler.h"

#include <QtCore/QFileInfo>
#include <QThread>

#include "WindowLog.h"
#include "RenderWorkers/PhotonMapRenderWorker.h"
#include "RenderWorkers/DirectLightRenderWorker.h"

#include <Common/Log.h>
#include <CADImport/SceneImporters/PbrtSceneImporter.h>
#include <Raytracer/Cameras/PerspectiveCamera.h>
#include <Raytracer/Films/ImageFilm.h>
#include <Raytracer/FilmFilters/MitchellFilter.h>
#include <Math/Transform.h>
#include <iostream>

RenderHandler::RenderHandler():
  m_rendering(false)
  {
  }

void RenderHandler::setCanvas(RenderCanvas *ip_canvas)
  {
  mp_canvas = ip_canvas;
  if (!mp_canvas)
    return;

  connect(mp_canvas, SIGNAL(imageDataUpdated()), mp_canvas, SLOT(update()));
  }

RenderCanvas *RenderHandler::getCanvas() const
  {
  return mp_canvas;
  }

void RenderHandler::setLogItem(QQuickItem *ip_log_item)
  {
  mp_log_item = ip_log_item;
  if (!mp_log_item)
    return;

  mp_log.reset( new WindowLog(mp_log_item, Log::INFO_LEVEL) );
  }

QQuickItem *RenderHandler::getLogItem() const
  {
  return mp_log_item;
  }

void RenderHandler::setFileUrl(const QUrl &arg)
  {
  if (m_fileUrl != arg)
    {
    m_fileUrl = arg;
    QString fileName = QQmlFile::urlToLocalFileOrQrc(arg);
    if (QFile::exists(fileName))
      {
      PbrtSceneImporter importer(fileName.toStdString(), mp_log);

      auto p_scene = importer.GetScene();
      if (p_scene && importer.GetCameras().size()>0)
        {
        mp_scene = p_scene;
        //mp_pbrt_camera = importer.GetCameras()[0];
        setCameraParameters(importer.GetCameras()[0]);
        mp_log->LogMessage(Log::INFO_LEVEL, "Scene loaded successfully.");

        clearCanvas();
        emit cameraChanged();
        emit sceneLoadedChanged();
        emit fileChanged();
        }
      else
        mp_log->LogMessage(Log::ERROR_LEVEL, "Scene loading failed.");
      }
    }
  }

void RenderHandler::clearCanvas()
  {
  mp_canvas->setImageData(0, 0, std::vector<unsigned char>());
  }

void RenderHandler::setCameraParameters(intrusive_ptr<const Camera> ip_camera)
  {
  const PerspectiveCamera *p_perspective_camera = dynamic_cast<const PerspectiveCamera*>(ip_camera.get());
  if (p_perspective_camera)
    {
    m_camera_params.setCameraTypeIndex(0);
    PerspectiveCameraParams *p_params = m_camera_params.getPerspectiveCameraParams();

    p_params->setLensRadius(p_perspective_camera->GetLensRadius());
    p_params->setFocalDistance(p_perspective_camera->GetFocalDistance());
    p_params->setXViewAngle(p_perspective_camera->GetXViewAngle());

    Transform trans = p_perspective_camera->GetCamera2WorldTransform();

    m_camera_params.setWidth( p_perspective_camera->GetFilm()->GetXResolution() );
    m_camera_params.setHeight( p_perspective_camera->GetFilm()->GetYResolution() );
    m_camera_params.setPosition( trans(Point3D_d(0.0,0.0,0.0)) );
    m_camera_params.setDirection( trans(Vector3D_d(0.0,0.0,1.0)).Normalized() );
    m_camera_params.setUp( trans(Vector3D_d(0.0,-1.0,0.0)).Normalized() );
    }
  else
    mp_log->LogMessage(Log::ERROR_LEVEL, "Cannot identify camera.");
  }

intrusive_ptr<const Camera> RenderHandler::constructCamera()
  {
  if (m_camera_params.getCameraTypeIndex()==0)
    {
    /*
    mp_log->LogMessage(Log::INFO_LEVEL, std::to_string(origin[0]));
    mp_log->LogMessage(Log::INFO_LEVEL, std::to_string(origin[1]));
    mp_log->LogMessage(Log::INFO_LEVEL, std::to_string(origin[2]));
    mp_log->LogMessage(Log::INFO_LEVEL, std::to_string(dir[0]));
    mp_log->LogMessage(Log::INFO_LEVEL, std::to_string(dir[1]));
    mp_log->LogMessage(Log::INFO_LEVEL, std::to_string(dir[2]));
    mp_log->LogMessage(Log::INFO_LEVEL, std::to_string(up[0]));
    mp_log->LogMessage(Log::INFO_LEVEL, std::to_string(up[1]));
    mp_log->LogMessage(Log::INFO_LEVEL, std::to_string(up[2]));
    */

    double lens_radius = m_camera_params.getPerspectiveCameraParams()->getLensRadius();
    double focal_dist = m_camera_params.getPerspectiveCameraParams()->getFocalDistance();
    double x_view_angle = m_camera_params.getPerspectiveCameraParams()->getXViewAngle();

    Transform c2w = MakeLookAt(m_camera_params.getPosition(), m_camera_params.getDirection().Normalized(), m_camera_params.getUp().Normalized()).Inverted();
    intrusive_ptr<const FilmFilter> p_filter( new MitchellFilter(2.0, 2.0, 1.0/3.0, 1.0/3.0) );
    intrusive_ptr<Film> p_film( new ImageFilm(m_camera_params.getWidth(), m_camera_params.getHeight(), p_filter) );
    intrusive_ptr<const Camera> p_camera( new PerspectiveCamera(c2w, p_film, lens_radius, focal_dist, x_view_angle) );

    return p_camera;
    }
  else
    mp_log->LogMessage(Log::ERROR_LEVEL, "Cannot identify camera.");
  }

void RenderHandler::renderStart()
  {
  mp_log->LogMessage(Log::INFO_LEVEL, "Starting the render process.");

  intrusive_ptr<const Camera> p_camera = constructCamera();

  if (m_render_params.getRendererTypeIndex()==0)
    mp_worker = new DirectLightRenderWorker(mp_canvas, mp_scene, p_camera, mp_log, m_render_params.getDirectLightParams());
  else
    mp_worker = new PhotonMapRenderWorker(mp_canvas, mp_scene, p_camera, mp_log, m_render_params.getPhotonMapParams());

  QThread *p_thread = new QThread;
  mp_worker->moveToThread(p_thread);

  connect(p_thread, &QThread::started, [&] {this->m_rendering = true; emit renderingChanged();});
  connect(mp_worker, &RenderWorker::finished, [&] {this->m_rendering = false; emit renderingChanged();});

  connect(p_thread, SIGNAL(started()), mp_worker, SLOT(process()));
  connect(mp_worker, SIGNAL(finished()), p_thread, SLOT(quit()));
  connect(mp_worker, SIGNAL(finished()), mp_worker, SLOT(deleteLater()));
  connect(p_thread, SIGNAL(finished()), p_thread, SLOT(deleteLater()));

  clearCanvas();
  p_thread->start();
  }

void RenderHandler::renderStop()
  {
  if (m_rendering)
    mp_worker->stop();
  }
