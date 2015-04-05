import QtQuick 2.2
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import org.skwarka 1.0
import "../forms"

Item {

    ColumnLayout {
        id: column

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: column.spacing

        GroupBox {
            title: "Renderer"
            Layout.fillWidth: true

            ComboBox {
                id: rendererType
                anchors.fill: parent
                editable: false
                model: ListModel {
                    ListElement { text: "Direct light" }
                    ListElement { text: "Photon maps" }
                }
                currentIndex: renderer.renderParams.rendererTypeIndex
                onCurrentIndexChanged: renderer.renderParams.rendererTypeIndex = currentIndex
            }
        }

        DirectLightForm {
            visible: rendererType.currentIndex == 0
        }

        PhotonMapForm {
            visible: rendererType.currentIndex == 1
        }

    }
}
