import QtQuick 2.2
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2

ColumnLayout {

    GroupBox {
        title: "Photon Shooting Parameters"
        Layout.fillWidth: true

        GridLayout {
            anchors.fill: parent
            columns: 2

            Text { text: "Photon paths (x10^6)"; color: "gray"; Layout.alignment: Qt.AlignRight }
            SpinBox { value: renderer.renderParams.photonMapParams.photonPaths; minimumValue: 1; maximumValue: 999999; onValueChanged: renderer.renderParams.photonMapParams.photonPaths = value; }

            Text { text: "Max caustic photons (x10^6)"; color: "gray"; Layout.alignment: Qt.AlignRight }
            SpinBox { value: renderer.renderParams.photonMapParams.maxCausticPhotons; minimumValue: 0; maximumValue: 40; onValueChanged: renderer.renderParams.photonMapParams.maxCausticPhotons = value; }

            Text { text: "Max direct photons (x10^6)"; color: "gray"; Layout.alignment: Qt.AlignRight }
            SpinBox { value: renderer.renderParams.photonMapParams.maxDirectPhotons; minimumValue: 0; maximumValue: 40; onValueChanged: renderer.renderParams.photonMapParams.maxDirectPhotons = value; }

            Text { text: "Max indirect photons (x10^6)"; color: "gray"; Layout.alignment: Qt.AlignRight }
            SpinBox { value: renderer.renderParams.photonMapParams.maxIndirectPhotons; minimumValue: 0; maximumValue: 40; onValueChanged: renderer.renderParams.photonMapParams.maxIndirectPhotons = value; }
       }

    }

    GroupBox {
        title: "Rendering Parameters"
        Layout.fillWidth: true

        GridLayout {
            anchors.fill: parent
            columns: 2

            Text { text: "Samples per pixel"; color: "gray"; Layout.alignment: Qt.AlignRight }
            SpinBox { value: renderer.renderParams.photonMapParams.samplesPerPixel; minimumValue: 1; maximumValue: 999999; onValueChanged: renderer.renderParams.photonMapParams.samplesPerPixel = value; }

            Text { text: "Direct light samples"; color: "gray"; Layout.alignment: Qt.AlignRight }
            SpinBox { value: renderer.renderParams.photonMapParams.directLightSamplesNum; minimumValue: 1; maximumValue: 999999; onValueChanged: renderer.renderParams.photonMapParams.directLightSamplesNum = value; }

            Text { text: "Final gather samples"; color: "gray"; Layout.alignment: Qt.AlignRight }
            SpinBox { value: renderer.renderParams.photonMapParams.finalGatherSamples; minimumValue: 1; maximumValue: 999999; onValueChanged: renderer.renderParams.photonMapParams.finalGatherSamples = value; }

            Text { text: "Caustic photons lookup number"; color: "gray"; Layout.alignment: Qt.AlignRight }
            SpinBox { value: renderer.renderParams.photonMapParams.causticLookupPhotonsNum; minimumValue: 1; maximumValue: 999999; onValueChanged: renderer.renderParams.photonMapParams.causticLookupPhotonsNum = value; }

            Text { text: "Caustic lookup dist"; color: "gray"; Layout.alignment: Qt.AlignRight }
            SpinBox { value: renderer.renderParams.photonMapParams.causticLookupDist; decimals: 4; minimumValue: 0; maximumValue: 999999; onValueChanged: renderer.renderParams.photonMapParams.causticLookupDist = value; }

            Text { text: "Max specular depth"; color: "gray"; Layout.alignment: Qt.AlignRight }
            SpinBox { value: renderer.renderParams.photonMapParams.maxSpecularDepth; minimumValue: 1; maximumValue: 999999; onValueChanged: renderer.renderParams.photonMapParams.maxSpecularDepth = value; }

            Text { text: "Media step size"; color: "gray"; Layout.alignment: Qt.AlignRight }
            SpinBox { value: renderer.renderParams.photonMapParams.mediaStepSize; decimals: 4; minimumValue: 0; maximumValue: 999999; onValueChanged: renderer.renderParams.photonMapParams.mediaStepSize = value; }
        }

    }


}
