import QtQuick 2.2
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2

ColumnLayout {

    GroupBox {
        title: "Camera Parameters"
        Layout.fillWidth: true

        GridLayout {
            anchors.fill: parent
            columns: 2

            Text { text: "Lens radius"; color: "gray"; Layout.alignment: Qt.AlignRight }
            SpinBox { value: renderer.cameraParams.perspectiveCameraParams.lensRadius; decimals: 5; minimumValue: 0; maximumValue: 999999; stepSize: 0.1;
                onValueChanged: renderer.cameraParams.perspectiveCameraParams.lensRadius = value; }

            Text { text: "Focal distance"; color: "gray"; Layout.alignment: Qt.AlignRight }
            SpinBox { value: renderer.cameraParams.perspectiveCameraParams.focalDistance; decimals: 5; minimumValue: 0; maximumValue: 999999; stepSize: 0.1;
                onValueChanged: renderer.cameraParams.perspectiveCameraParams.focalDistance = value; }

            Text { text: "X view angle"; color: "gray"; Layout.alignment: Qt.AlignRight }
            SpinBox { value: renderer.cameraParams.perspectiveCameraParams.xViewAngle; decimals: 5; minimumValue: 0; maximumValue: 999999; stepSize: 0.1;
                onValueChanged: renderer.cameraParams.perspectiveCameraParams.xViewAngle = value; }
        }
    }
}
