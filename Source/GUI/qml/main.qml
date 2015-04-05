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

import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.1
import QtQuick.Window 2.1
import org.skwarka 1.0
import "tabs"

ApplicationWindow {
    visible: true
    width: 920
    height: 700
    minimumWidth: 300
    minimumHeight: 300

    title: "Skwarka Renderer" + (renderer.fileName === null || renderer.fileName === "" ? "" : " - " + renderer.fileName)

    MessageDialog {
        id: aboutBox
        title: "About Text"
        text: "This is Skwarka Renderer"
        icon: StandardIcon.Information
    }

    FileDialog {
        id: fileDialog
        nameFilters: ["PBRT scene files (*.pbrt)"]
        onAccepted: renderer.fileUrl = fileUrl;
    }

    Action {
        id: fileOpenAction
        iconSource: "images/fileopen.png"
        text: "Open"
        onTriggered: fileDialog.open()
    }

    Action {
        id: renderStartAction
        iconSource: "images/start.png"
        text: "Start rendering"
        onTriggered: renderer.renderStart()
        enabled: !renderer.rendering && renderer.sceneLoaded
    }

    Action {
        id: renderStopAction
        iconSource: "images/stop.png"
        text: "Stop rendering"
        onTriggered: renderer.renderStop()
        enabled: renderer.rendering
    }


    menuBar: MenuBar {
        Menu {
            title: "&File"
            MenuItem { action: fileOpenAction }
            MenuItem { text: "Quit"; onTriggered: Qt.quit() }
        }
        Menu {
            title: "&Render"
            MenuItem { action: renderStartAction }
            MenuItem { action: renderStopAction }
        }
        Menu {
            title: "&Help"
            MenuItem { text: "About..." ; onTriggered: aboutBox.open() }
        }
    }

    toolBar: ToolBar {
        id: mainToolBar
        width: parent.width
        RowLayout {
            anchors.fill: parent
            spacing: 0
            ToolButton { action: fileOpenAction }

            ToolBarSeparator {}

            ToolButton { action: renderStartAction }
            ToolButton { action: renderStopAction }

            ToolBarSeparator {}

            Item { Layout.fillWidth: true }
        }
    }

    SplitView {
        orientation: Qt.Horizontal
        anchors.fill: parent

        SplitView {
            orientation: Qt.Vertical
            Layout.fillWidth: true

            RenderCanvas {
                id: renderCanvas
                Layout.fillHeight: true
                Layout.alignment: Qt.AlignTop
            }

            TextArea {
                id: logArea
                readOnly: true
                Layout.minimumHeight: 50
            }
        }

        TabView {
            id: tabView
            tabPosition: Qt.TopEdge

            Layout.minimumWidth: 50
            Layout.alignment: Qt.AlignTop
            width: 260

            Tab {
                title: "Renderer"
                RendererTab {}
            }

            Tab {
                title: "Camera"
                CameraTab {}
            }
        }
    }

    RenderHandler {
        id: renderer
        canvas: renderCanvas
        logItem: logArea
    }

}
