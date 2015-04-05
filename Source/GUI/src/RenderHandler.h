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

#ifndef RENDER_HANDLER_H
#define RENDER_HANDLER_H

#include <qqmlfile.h>
#include <QObject>
#include <QQuickItem>
#include <QtQuick/QQuickPaintedItem>

#include "RenderCanvas.h"
#include "RenderWorker.h"
#include "Params/RenderParams.h"
#include "Params/CameraParams.h"

#include <Common/Common.h>
#include <Raytracer/Core/Scene.h>
#include <Raytracer/Core/Camera.h>

class RenderHandler : public QObject
{
    Q_OBJECT

    Q_PROPERTY(RenderCanvas *canvas READ getCanvas WRITE setCanvas)
    Q_PROPERTY(QQuickItem *logItem READ getLogItem WRITE setLogItem)
    Q_PROPERTY(QUrl fileUrl READ getFileUrl WRITE setFileUrl)
    Q_PROPERTY(QString fileName READ getFileName NOTIFY fileChanged)
    Q_PROPERTY(RenderParams *renderParams READ getRenderParams CONSTANT)
    Q_PROPERTY(CameraParams *cameraParams READ getCameraParams CONSTANT)

    Q_PROPERTY(bool rendering MEMBER m_rendering NOTIFY renderingChanged)
    Q_PROPERTY(bool sceneLoaded READ isSceneLoaded NOTIFY sceneLoadedChanged)

public:
    RenderHandler();

    RenderCanvas *getCanvas() const;

    void setCanvas(RenderCanvas *ip_canvas);

    QQuickItem *getLogItem() const;

    void setLogItem(QQuickItem *ip_log_item);

    QUrl getFileUrl() const { return m_fileUrl; }

    QString getFileName() const { return m_fileUrl.fileName(); }

    RenderParams *getRenderParams() { return &m_render_params; }

    CameraParams *getCameraParams() { return &m_camera_params; }

    bool isSceneLoaded() const { return mp_scene != NULL; }

public Q_SLOTS:
    void setFileUrl(const QUrl &arg);

    void renderStart();
    void renderStop();

Q_SIGNALS:
    void fileChanged();
    void cameraChanged();
    void sceneLoadedChanged();
    void renderingChanged();

private:
    void clearCanvas();
    void setCameraParameters(intrusive_ptr<const Camera> ip_camera);
    intrusive_ptr<const Camera> constructCamera();

    RenderCanvas *mp_canvas;
    QQuickItem *mp_log_item;

    intrusive_ptr<Log> mp_log;
    QUrl m_fileUrl;

    intrusive_ptr<const Scene> mp_scene;
    //intrusive_ptr<const Camera> mp_pbrt_camera;

    RenderParams m_render_params;
    CameraParams m_camera_params;

    RenderWorker *mp_worker;

    bool m_rendering;
};

#endif // RENDER_HANDLER_H
