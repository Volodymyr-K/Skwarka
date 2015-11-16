'use strict';

function initStats() {
    var stats = new Stats();
    stats.setMode(0); // 0: fps, 1: ms

    // Align top-left
    stats.domElement.style.position = 'absolute';
    stats.domElement.style.left = '0px';
    stats.domElement.style.top = '0px';

    document.getElementById("Stats-output").appendChild(stats.domElement);

    return stats;
}

var prerenderRunning = false;

function startPrerender(sceneObjects, cameraProps) {
    // create a scene, that will hold all our elements such as objects, cameras and lights.
    var scene = new THREE.Scene();

    // three.js works with vertical FOVs, so we need to convert from horizontal to vertical
    var halfFovTan = Math.tan( THREE.Math.degToRad( cameraProps.fov ) * 0.5 );
    var verticalFov = THREE.Math.radToDeg( 2 * Math.atan( halfFovTan * cameraProps.height / cameraProps.width ) );

    // create a camera, which defines where we're looking at.
    var camera = new THREE.PerspectiveCamera(verticalFov, cameraProps.width / cameraProps.height, 0.01, 1000);

    // create a render and set the size
    var renderer = new THREE.WebGLRenderer();

    // add the output of the renderer to the html element
    document.getElementById("WebGL-output").appendChild(renderer.domElement);

    renderer.setClearColor(new THREE.Color(0xEEEEEE, 1.0));
    renderer.setSize(cameraProps.width, cameraProps.height);
    renderer.shadowMap.enabled = true;

    var trackballControls = new THREE.TrackballControls(camera, renderer.domElement);
    trackballControls.dynamicDampingFactor=0.45;

    trackballControls.position0.set(cameraProps.origin[0], cameraProps.origin[1], cameraProps.origin[2]);
    trackballControls.up0.set(cameraProps.up[0], cameraProps.up[1], cameraProps.up[2]);
    trackballControls.target0.set(
        cameraProps.origin[0] + cameraProps.direction[0],
        cameraProps.origin[1] + cameraProps.direction[1],
        cameraProps.origin[2] + cameraProps.direction[2]);
    trackballControls.reset();

    var hemiLight = new THREE.HemisphereLight(0xffffff, 0x444444, 0.6);
    hemiLight.position.set(0, 0, 1);
    scene.add(hemiLight);

    var geometries = [];
    _.each(sceneObjects.triangleMeshes, function(triangleMesh) {
        var vertices = _.map(triangleMesh.vertices, function(vertex) {
            return new THREE.Vector3(vertex[0], vertex[1], vertex[2]);
        });

        var faces = _.map(triangleMesh.triangles, function(triangle) {
            return new THREE.Face3(triangle[0], triangle[1], triangle[2]);
        });

        var geom = new THREE.Geometry();
        geom.vertices = vertices;
        geom.faces = faces;
        geom.computeFaceNormals();
        geometries.push(geom);
    });

    _.each(sceneObjects.primitives, function(primitive) {
        var material = new THREE.MeshLambertMaterial({color: 0xaaaaaa});
        material.side = THREE.DoubleSide;

        var mesh = new THREE.Mesh(geometries[primitive.meshIndex], material);
        mesh.castShadow = true;
        mesh.receiveShadow = true;

        var matrix = new THREE.Matrix4();
        matrix.set.apply(matrix, primitive.transformationMatrix);
        mesh.matrix = matrix;
        mesh.matrixAutoUpdate = false;

        scene.add(mesh);
    });

    var stats = initStats();
    var clock = new THREE.Clock();

    prerenderRunning = true;
    render();
    
    function render() {
        if (prerenderRunning) {
            stats.update();
            var delta = clock.getDelta();
            trackballControls.update(delta);

            renderer.render(scene, camera);

            // render using requestAnimationFrame
            requestAnimationFrame(render);
        }
    }
}

function stopPrerender() {
    prerenderRunning = false;

    var statsEl = document.getElementById("Stats-output");
    while (statsEl.hasChildNodes()) {
        statsEl.removeChild(statsEl.lastChild);
    }

    var webGLEl = document.getElementById("WebGL-output");
    while (webGLEl.hasChildNodes()) {
        webGLEl.removeChild(webGLEl.lastChild);
    }
}