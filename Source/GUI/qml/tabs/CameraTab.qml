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
            title: "Film Parameters"
            Layout.fillWidth: true

            GridLayout {
                anchors.fill: parent
                columns: 2

                Text { text: "Width"; color: "gray"; Layout.alignment: Qt.AlignRight }
                SpinBox { value: renderer.cameraParams.width; minimumValue: 1; maximumValue: 10000;
                    onValueChanged: renderer.cameraParams.width = value; }

                Text { text: "Height"; color: "gray"; Layout.alignment: Qt.AlignRight }
                SpinBox { value: renderer.cameraParams.height; minimumValue: 1; maximumValue: 10000;
                    onValueChanged: renderer.cameraParams.height = value; }
            }
        }

        GroupBox {
            title: "Camera Position"
            Layout.fillWidth: true

            GridLayout {
                anchors.fill: parent
                columns: 2

                Text { text: "x"; color: "gray"; Layout.alignment: Qt.AlignRight }
                SpinBox { value: renderer.cameraParams.positionX; decimals: 5; minimumValue: -999999; maximumValue: 999999; stepSize: 0.1;
                    onValueChanged: renderer.cameraParams.positionX = value; }

                Text { text: "y"; color: "gray"; Layout.alignment: Qt.AlignRight }
                SpinBox { value: renderer.cameraParams.positionY; decimals: 5; minimumValue: -999999; maximumValue: 999999; stepSize: 0.1;
                    onValueChanged: renderer.cameraParams.positionY = value; }

                Text { text: "z"; color: "gray"; Layout.alignment: Qt.AlignRight }
                SpinBox { value: renderer.cameraParams.positionZ; decimals: 5; minimumValue: -999999; maximumValue: 999999; stepSize: 0.1;
                    onValueChanged: renderer.cameraParams.positionZ = value; }
            }
        }

        GroupBox {
            title: "Camera Viewing Direction"
            Layout.fillWidth: true

            GridLayout {
                anchors.fill: parent
                columns: 2

                Text { text: "x"; color: "gray"; Layout.alignment: Qt.AlignRight }
                SpinBox { value: renderer.cameraParams.directionX; decimals: 5; minimumValue: -999999; maximumValue: 999999; stepSize: 0.1;
                    onValueChanged: renderer.cameraParams.directionX = value; }

                Text { text: "y"; color: "gray"; Layout.alignment: Qt.AlignRight }
                SpinBox { value: renderer.cameraParams.directionY; decimals: 5; minimumValue: -999999; maximumValue: 999999; stepSize: 0.1;
                    onValueChanged: renderer.cameraParams.directionY = value; }

                Text { text: "z"; color: "gray"; Layout.alignment: Qt.AlignRight }
                SpinBox { value: renderer.cameraParams.directionZ; decimals: 5; minimumValue: -999999; maximumValue: 999999; stepSize: 0.1;
                    onValueChanged: renderer.cameraParams.directionZ = value; }
            }
        }

        GroupBox {
            title: "Camera Up Direction"
            Layout.fillWidth: true

            GridLayout {
                anchors.fill: parent
                columns: 2

                Text { text: "x"; color: "gray"; Layout.alignment: Qt.AlignRight }
                SpinBox { value: renderer.cameraParams.upX; decimals: 5; minimumValue: -999999; maximumValue: 999999; stepSize: 0.1;
                    onValueChanged: renderer.cameraParams.upX = value; }

                Text { text: "y"; color: "gray"; Layout.alignment: Qt.AlignRight }
                SpinBox { value: renderer.cameraParams.upY; decimals: 5; minimumValue: -999999; maximumValue: 999999; stepSize: 0.1;
                    onValueChanged: renderer.cameraParams.upY = value; }

                Text { text: "z"; color: "gray"; Layout.alignment: Qt.AlignRight }
                SpinBox { value: renderer.cameraParams.upZ; decimals: 5; minimumValue: -999999; maximumValue: 999999; stepSize: 0.1;
                    onValueChanged: renderer.cameraParams.upZ = value; }
            }
        }

        GroupBox {
            title: "Camera"
            Layout.fillWidth: true

            ComboBox {
                id: cameraType
                anchors.fill: parent
                editable: false
                model: ListModel {
                    ListElement { text: "Perspective camera" }
                }
                currentIndex: renderer.cameraParams.cameraTypeIndex
                onCurrentIndexChanged: renderer.cameraParams.cameraTypeIndex = currentIndex
            }
        }

        PerspectiveCameraForm {
            visible: cameraType.currentIndex == 0
        }
    }
}
