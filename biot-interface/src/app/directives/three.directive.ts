import { Directive, ElementRef } from '@angular/core';

import * as THREE from 'three';

@Directive({
    selector: '[myThreeD]'
})
export class ThreeDirective {

    //Text2D = THREE_Text.Text2D;
    //SpriteText2D = THREE_Text.SpriteText2D;
    //textAlign = THREE_Text.textAlign;

    cursor = undefined;
    scene = undefined;
    camera = undefined;
    renderer = undefined;
    grid = undefined;
    pointLight = undefined;
    hemiLight = undefined;
    clearColour = 'white';
    oldMidx = 0;
    sizeX = 1900;
    sizeY = 256;
    zoomFactor = 3;
    lightTarget = undefined;


    constructor(private el: ElementRef) {
	el.nativeElement.style.backgroundColor = this.clearColour;
	this.init(el);
	this.anim();
    }


    addNode(name, x, y, z, colour) {

	// make box
	var geometry = new THREE.BoxGeometry(20, 25, 10);
	var material = new THREE.MeshLambertMaterial( { color: colour} );

	var node = new THREE.Mesh(geometry, material);
	node.position.x = x;
	node.position.y = y;
	node.position.z = z;

	var cGeometry = new THREE.CylinderGeometry( 20, 20, 10, 50 );
	var cMaterial = new THREE.MeshLambertMaterial({color: 0xa0a0a0});
	var cylinder = new THREE.Mesh( cGeometry, cMaterial );
	cylinder.rotateX(Math.PI/2);
	cylinder.position.x = -10;
	cylinder.position.z = -5;
	cylinder.castShadow = true;
	cylinder.receiveShadow = true;
	node.add(cylinder);

	var worldAxis = this.addWorldAxis(x, y, z, 100, 1, 0.35);
	worldAxis.name = 'world-axis-' + name;
	worldAxis.translateX(x);
	worldAxis.castShadow = true;
	this.scene.add(worldAxis);

	var localAxis = this.addWorldAxis(x, y, z, 40, 5, 0);
	localAxis.castShadow = true;
	node.add(localAxis);

	node.name = 'biot-node-' + name;
	node.castShadow = true;
	node.receiveShadow = true;
	this.scene.add(node);

	 var midx = this.getCentreOfNodes();
	 this.camera.position.x = midx;
	 this.pointLight.position.set(midx, 0, 400);
	 this.lightTarget.position.set(midx, 0, 0);

    }

    addWorldAxis(x, y, z, length, width, brightness) {
	var group = new THREE.Object3D();

	var r = new THREE.Color(0.5, 0.1, 0.1).offsetHSL(0, 0, brightness);
	var g = new THREE.Color(0.1, 0.5, 0.1).offsetHSL(0, 0, brightness);
	var b = new THREE.Color(0.1, 0.1, 0.5).offsetHSL(0, 0, brightness);

	var lmaterial = new THREE.LineBasicMaterial( {color: r.getHex(), linewidth: width});
	var lgeometry = new THREE.Geometry();
	lgeometry.vertices.push(new THREE.Vector3(0, 0, 0));
	lgeometry.vertices.push(new THREE.Vector3(length, 0, 0));
	var line = new THREE.Line(lgeometry, lmaterial);
	group.add(line);
	line.castShadow = true;
	line.receiveShadow = true;

	lmaterial = new THREE.LineBasicMaterial( {color: g.getHex(), linewidth: width} );
	lgeometry = new THREE.Geometry();
	lgeometry.vertices.push(new THREE.Vector3(0, 0, 0));
	lgeometry.vertices.push(new THREE.Vector3(0, length, 0));
	line = new THREE.Line(lgeometry, lmaterial);
	group.add(line);
	line.castShadow = true;
	line.receiveShadow = true;

	lmaterial = new THREE.LineBasicMaterial( {color: b.getHex(), linewidth: width} );
	lgeometry = new THREE.Geometry();
	lgeometry.vertices.push(new THREE.Vector3(0, 0, 0));
	lgeometry.vertices.push(new THREE.Vector3(0, 0, length));
	line = new THREE.Line(lgeometry, lmaterial);
	group.add(line);
	line.castShadow = true;
	line.receiveShadow = true;

	return group;
    }

    anim() {
	requestAnimationFrame(()=> {
	    this.anim()
	});

	this.renderer.render( this.scene, this.camera );
    }


    getCentreOfNodes() {
	var xmax = -1000;
	var xmin = 1000;
	this.scene.traverse(function(item) {
	    if (item.name.match(/biot-node/)) {
		if (item.position.x > xmax) {
		    xmax = item.position.x;
		}
		if (item.position.x < xmin) {
		    xmin = item.position.x;
		}
	    }
	});
	var midx = xmin + (xmax - xmin)/2;
	console.log(midx);
	return midx;
    }


    init(el) {

	this.scene = new THREE.Scene();

	//this.camera = new THREE.OrthographicCamera( -800, 800, 200, -200, 0.1, 1000);
	this.camera = new THREE.PerspectiveCamera( 45, this.sizeX/this.sizeY, 0.1, 10000 );
	this.scene.add(this.camera);
	this.camera.up.set( 0, 0, 1 )
	this.camera.position.z = 200;
	this.camera.position.y = -250;
	this.camera.position.x = 0;
	this.camera.lookAt( this.scene.position );

	/*var axisHelper = new THREE.AxisHelper( 125 );
	 this.scene.add( axisHelper );*/

	 this.setupLighting();
	 this.makeFloor();

	 this.renderer = new THREE.WebGLRenderer();
	 this.renderer.preserveDrawingBuffer = true ;
	 this.renderer.setClearColor(this.clearColour, 1);
	 this.renderer.setSize( this.sizeX, this.sizeY );
	 this.renderer.shadowMapEnabled = true;
	 el.nativeElement.appendChild( this.renderer.domElement );

    }

    focusNode(addr) {
	this.oldMidx = this.camera.position.x;
	this.camera.position.x = this.getCentreOfNodes();
	this.camera.zoom = this.zoomFactor;
	var node = this.scene.getObjectByName('biot-node-' + addr);
	this.camera.lookAt( node.position );
	this.camera.updateProjectionMatrix();
    }

    makeFloor() {

	var floorMaterial = new THREE.MeshStandardMaterial( { color: 0xddffdd, roughness: 0.8, metalness: 0.1 } );
	var floorGeometry = new THREE.PlaneGeometry(2000, 500, 1, 1);
	var floor = new THREE.Mesh(floorGeometry, floorMaterial);
	floor.position.y = 0;
	floor.position.x = 300;
	floor.position.z = -50;
	floor.receiveShadow = true;
	this.scene.add(floor);
    }


    moveNode(addr, q) {
	var node = this.scene.getObjectByName('biot-node-' + addr);
	node.setRotationFromQuaternion(new THREE.Quaternion(q.x, q.y, q.z, q.w).normalize());
	node.updateMatrix();
    }

    removeNode(name) {
	var node = this.scene.getObjectByName('biot-node-' + name);
	this.scene.remove(node);
	node = this.scene.getObjectByName('world-axis-' + name);
	this.scene.remove(node);
    }

    setupLighting() {
	var ambientLight = new THREE.AmbientLight(0xddddff, 0.3);
	this.scene.add(ambientLight);

	this.lightTarget = new THREE.Object3D();
	this.lightTarget.position.set(0, 0, 0);
	this.scene.add(this.lightTarget);
	this.hemiLight = new THREE.HemisphereLight(0xddddff, 0xddffdd, 0.6);
	this.hemiLight.position.set(0, 0, 400);
	this.hemiLight.castShadow = true;
	this.scene.add(this.hemiLight);
	this.hemiLight.target = this.lightTarget;

	this.pointLight = new THREE.SpotLight(0x7f7f7f, 0.7);
	this.pointLight.position.set(0, 0, 400);
	this.pointLight.castShadow = true;
	this.pointLight.shadow.mapSize.width = 640;
	this.pointLight.shadow.mapSize.height = 640;
	this.pointLight.shadow.camera.near = 100;
	this.pointLight.shadow.camera.far = 1000;
	this.scene.add(this.pointLight);
	this.pointLight.target = this.lightTarget;
    }

    unFocusNode() {
	this.camera.position.x = this.oldMidx;
	this.camera.zoom = 1;
	this.camera.lookAt(
	    new THREE.Vector3( this.oldMidx, 0, 0 )
	);
	this.camera.updateProjectionMatrix();
    }



}


