var nodeAPI = require('../../Binaries/Release/NodeAPI');
//var nodeAPI = require('../../Binaries/Debug/NodeAPId');

nodeAPI.allRenderers = ["Photon Mapping"];

nodeAPI.rendererParamsSchema = {
  "Photon Mapping": {
    "photonMapParams": {
      directLightSamples: {
        type: "int",
        defaultValue: 16
      },
      gatherSamplesNum: {
        type: "int",
        defaultValue: 16
      },
      causticLookupPhotonsNum: {
        type: "int",
        defaultValue: 100
      },
      maxCausticLookupDist: {
        type: "double",
        defaultValue: 0.05
      },
      maxSpecularDepth: {
        type: "int",
        defaultValue: 10
      },
      mediaStepSize:  {
        type: "double",
        defaultValue: 0.01
      }
    },
    "samplesPerPixel": {
      type: "int",
      defaultValue: 1
    },
    "photonsMillions": {
      type: "int",
      defaultValue: 1
    },
    "refreshPeriod": {
      type: "double",
      defaultValue: 2.0
    }
  }
};

module.exports = nodeAPI;
