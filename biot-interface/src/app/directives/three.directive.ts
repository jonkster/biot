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
    clearColour = 'black';
    sizeX = 1900;
    sizeY = 256;


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
        node.name = name;
        node.position.x = x;
        node.position.y = y;
        node.position.z = z;

        var cGeometry = new THREE.CylinderGeometry( 20, 20, 10, 50 );
        var cMaterial = new THREE.MeshLambertMaterial({color: 0xa0a0a0});
        var cylinder = new THREE.Mesh( cGeometry, cMaterial );
        cylinder.rotateX(Math.PI/2);
        cylinder.position.x = -10;
        cylinder.position.z = -5;
        node.add(cylinder);

        // make axis indicator
        var lmaterial = new THREE.LineBasicMaterial( {color: 0xff0000});
        var lgeometry = new THREE.Geometry();
        lgeometry.vertices.push(new THREE.Vector3(0, 0, 0));
        lgeometry.vertices.push(new THREE.Vector3(60, 0, 0));
        var line = new THREE.Line(lgeometry, lmaterial);
        node.add(line);
        lmaterial = new THREE.LineBasicMaterial( {color: 0x00ff00});
        lgeometry = new THREE.Geometry();
        lgeometry.vertices.push(new THREE.Vector3(0, 0, 0));
        lgeometry.vertices.push(new THREE.Vector3(0, 60, 0));
        line = new THREE.Line(lgeometry, lmaterial);
        node.add(line);
        lmaterial = new THREE.LineBasicMaterial( {color: 0x0000ff});
        lgeometry = new THREE.Geometry();
        lgeometry.vertices.push(new THREE.Vector3(0, 0, 0));
        lgeometry.vertices.push(new THREE.Vector3(0, 0, 60));
        line = new THREE.Line(lgeometry, lmaterial);
        node.add(line);

        this.scene.add(node);
	var labelName = name.replace(/:/g, ":\n");
        var label = this.createLabel(labelName, x, y, z+10, 20, "white", "black", 3);
        this.scene.add(label);
    }

    anim() {
        requestAnimationFrame(()=> {
            this.anim()
        });

        this.renderer.render( this.scene, this.camera );
    }

    createLabel(text, x, y, z, size, color, backGroundColor, backgroundMargin) {
	var canvas = document.createElement("canvas");

	var context = canvas.getContext("2d");
	var font = size + "pt Courier";
        context.font = font;
	var textWidth = context.measureText(text).width;

	if (! backgroundMargin)
		backgroundMargin = 50;
	canvas.width = textWidth + backgroundMargin;
	canvas.height = size + backgroundMargin;

	context = canvas.getContext("2d");
        context.font = font;
	if (backGroundColor) {
	    context.fillStyle = backGroundColor;
	    context.fillRect(
                canvas.width / 2 - textWidth / 2 - backgroundMargin / 2,
                canvas.height / 2 - size / 2 - +backgroundMargin / 2,
                textWidth + backgroundMargin,
                size + backgroundMargin);
	}
	context.textAlign = "center";
	context.textBaseline = "middle";
	context.fillStyle = color;
	context.fillText(text, canvas.width / 2, canvas.height / 2);

	var texture = new THREE.Texture(canvas);
	texture.needsUpdate = true;
	var material = new THREE.MeshBasicMaterial({
	    map : texture
	});

	var mesh = new THREE.Mesh(new THREE.PlaneGeometry(canvas.width, canvas.height), material);
	mesh.position.x = x;
	mesh.position.y = y;
	mesh.position.z = z;
        mesh.lookAt(this.camera.position());

	return mesh;
    }


    init(el) {

        this.scene = new THREE.Scene();

        this.camera = new THREE.PerspectiveCamera( 45, this.sizeX/this.sizeY, 0.1, 10000 );
        this.scene.add(this.camera);
        this.camera.up.set( 0, 0, 1 )
        this.camera.position.z = 0;
        this.camera.position.y = -250;
        this.camera.position.x = 0;
        console.log(this.camera);
        this.camera.lookAt( new THREE.Vector3(0, 0, 0) );

        var axisHelper = new THREE.AxisHelper( 125 );
        this.scene.add( axisHelper );

        var ambientLight = new THREE.AmbientLight(0xddddff, 0.6);
        this.scene.add(ambientLight);
        //var hemiLight = new THREE.HemisphereLight(0xddddff, 0xddffdd, 0.6);
        //hemiLight.position.set(0, 100, 0);
        //this.scene.add(hemiLight);
        var pointLight = new THREE.PointLight(0x7f7f7f, 1, 0);
        pointLight.position.set(100, 100, 0);
        this.scene.add(pointLight);


        this.renderer = new THREE.WebGLRenderer();
        this.renderer.preserveDrawingBuffer = true ;
        this.renderer.setClearColor(this.clearColour, 1);
        this.renderer.setSize( this.sizeX, this.sizeY );
        el.nativeElement.appendChild( this.renderer.domElement );

    }

    moveNode(addr, q) {
        var node = this.scene.getObjectByName(addr);
        node.setRotationFromQuaternion(new THREE.Quaternion(q.x, q.y, q.z, q.w).normalize());
        node.updateMatrix();
    }


}


