import QtQuick 2.2
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2

ColumnLayout {

    GroupBox {
        title: "Rendering Parameters"
        Layout.fillWidth: true

        GridLayout {
            anchors.fill: parent
            columns: 2

            Text { text: "Samples per pixel"; color: "gray"; Layout.alignment: Qt.AlignRight }
            SpinBox { value: renderer.renderParams.directLightParams.samplesPerPixel; minimumValue: 1; maximumValue: 999999; onValueChanged: renderer.renderParams.directLightParams.samplesPerPixel = value; }

            Text { text: "Direct light samples"; color: "gray"; Layout.alignment: Qt.AlignRight }
            SpinBox { value: renderer.renderParams.directLightParams.directLightSamplesNum; minimumValue: 1; maximumValue: 999999; onValueChanged: renderer.renderParams.directLightParams.directLightSamplesNum = value; }

            Text { text: "Max specular depth"; color: "gray"; Layout.alignment: Qt.AlignRight }
            SpinBox { value: renderer.renderParams.directLightParams.maxSpecularDepth; minimumValue: 1; maximumValue: 999999; onValueChanged: renderer.renderParams.directLightParams.maxSpecularDepth = value; }

            Text { text: "Media step size"; color: "gray"; Layout.alignment: Qt.AlignRight }
            SpinBox { value: renderer.renderParams.directLightParams.mediaStepSize; decimals: 4; minimumValue: 0; maximumValue: 999999; onValueChanged: renderer.renderParams.directLightParams.mediaStepSize = value; }
        }

    }


}
