'use strict';

window.$ = window.jQuery = require('jquery');
var ipc = require('ipc');
var fs = require('fs');
var Ractive = require('ractive');
var _ = require('lodash');

var nodeAPI = require('../../NodeAPI');

var mainTemplate = fs.readFileSync(__dirname + '/main.tmpl').toString();

var initParamsFromSchema = function(paramsSchema) {
  var initParamsRec = function(params, paramsSchema) {
    _.each(_.keys(paramsSchema), function(key) {
      if (_.isString(paramsSchema[key].type)) {
        params[key] = {};
        params[key].type = paramsSchema[key].type;
        if (!_.isUndefined(paramsSchema[key].defaultValue)) {
          params[key].value = paramsSchema[key].defaultValue;
        }
      } else {
        params[key] = {};
        initParamsRec(params[key], paramsSchema[key]);
      }
    });
  };

  var params = {};
  initParamsRec(params, paramsSchema);
  return params;
};

var ractive = new Ractive({
  el: "#container",
  template: mainTemplate,
  data: {
    file: "",
    imageData: "",
    logs: "",
    allRenderers: nodeAPI.allRenderers,
    renderer: nodeAPI.allRenderers[0],
    rendererParams: initParamsFromSchema(nodeAPI.rendererParamsSchema[nodeAPI.allRenderers[0]]),
    cameraParams: {
      width: 640,
      height: 480,
      fov: 60,
      lensRadius: 0,
      focalDistance: 0
    },

    scene: null,

    readingScene: false,
    rendering: false
  }
});

ractive.on('changeRenderer', function() {
  var renderer = ractive.get('renderer');
  ractive.set('rendererParams', initParamsFromSchema(nodeAPI.rendererParamsSchema[renderer]));
});

var logMessage = function(level, message) {
  var logs = ractive.get('logs');

  var str = level + ": " + message;
  logs += logs.length > 0 ? '\n' + str : str;
  ractive.set('logs', logs);
};

nodeAPI.setLog(logMessage);

var prerenderCamera;

ractive.on('openFile', function() {
  if (ractive.get('readingScene')) {
    logMessage("Error", "The scene reading is already in progress.");
    return;
  }

  var files = ractive.get('file');
  if (files && files.length == 1) {
    logMessage("Info", "Reading file " + files[0].path);
    ractive.set('readingScene', true);

    // Reset any previously read scene and camera.
    ractive.set('scene', null);
    stopPrerender();

    nodeAPI.createScene(files[0].path, function(error, data) {
      if (error) {
        logMessage("Error", error);
      } else if (data) {
        // Clear previously renderer image (if any).
        ractive.set('imageData', "");

        logMessage("Info", "The scene file has been opened successfully.");
        ractive.set('scene', data.scene);

        var cameraParams = data.cameras[0].getCameraParams();
        ractive.set('cameraParams', cameraParams);

        var sceneObjects = data.scene.getSceneObjects();
        prerenderCamera = startPrerender(sceneObjects, cameraParams);
      }

      ractive.set('readingScene', false);
    });
  }
});

var renderer;

var purifyParams = function(params) {
  var purifyParamsRec = function(purified, params) {
    _.each(_.keys(params), function(key) {
      if (_.isString(params[key].type)) {
        if (!_.isUndefined(params[key].value)) {
          purified[key] = params[key].value;
        }
      } else {
        purified[key] = {};
        purifyParamsRec(purified[key], params[key]);
      }
    });
  };

  var purified = {};
  purifyParamsRec(purified, params);
  return purified;
};

ractive.on("startRendering", function() {
  if (!ractive.get('scene')) {
    logMessage("Error", "Nothing to render. Select a scene file first.");
    return;
  }

  if (ractive.get('rendering')) {
    logMessage("Error", "The rendering is already in progress.");
    return;
  }

  var cameraPosition = prerenderCamera();
  stopPrerender();

  ractive.set('imageData', "");
  if (renderer) {
    renderer.stop();
  }
  renderer = nodeAPI.createRenderer(ractive.get('renderer'), purifyParams(ractive.get('rendererParams')));
  if (!renderer) {
    logMessage("Error", "Can't initialize the renderer.");
    return;
  }

  var prepareImageData = function(pngBuffer) {
    return 'data:image/png;base64,' + pngBuffer.toString('base64');
  };

  var imageUpdateCallback = function(pngBuffer) {
    ractive.set('imageData', prepareImageData(pngBuffer));
  };
  var doneCallback = function(error, pngBuffer) {
    if (error) {
      logMessage("Error", error);
    } else if (pngBuffer) {
      ractive.set('imageData', prepareImageData(pngBuffer));
    }
    ractive.set('rendering', false);
  };

  ractive.set('rendering', true);

  var cameraParams = ractive.get('cameraParams');
  cameraParams.position = cameraPosition.position;
  cameraParams.lookAt = cameraPosition.lookAt;
  cameraParams.up = cameraPosition.up;
  var camera = nodeAPI.createCamera(cameraParams);

  renderer.render(ractive.get('scene'), camera, doneCallback, imageUpdateCallback);
});

ractive.on("stopRendering", function() {
  if (renderer) {
    renderer.stop();
    ractive.set('rendering', false);
  }
});
