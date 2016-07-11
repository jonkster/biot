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
    clearColour = 'white';
    sizeX = 1900;
    sizeY = 256;


    constructor(private el: ElementRef) {
        el.nativeElement.style.backgroundColor = '#ffffff';
        this.init(el);
        this.anim();
    }

    addNode(name, x, y, z, colour) {
        var material = new THREE.MeshLambertMaterial( { color: colour, wireframe: false } );
        var geometry = new THREE.BoxGeometry(6, 50, 4);
        var node = new THREE.Mesh(geometry, material);
        node.name = name;
        node.position.x = x;
        node.position.y = y;
        node.position.z = z;
        this.scene.add(node);
	var labelName = name.replace(/:/g, ":\n");
        var label = this.createLabel(labelName, x*2, -80, -100, 15, "white", "black", 10);
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
	    context.fillRect(canvas.width / 2 - textWidth / 2 - backgroundMargin / 2,
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

	return mesh;
    }


    init(el) {

        this.scene = new THREE.Scene();

        this.camera = new THREE.PerspectiveCamera( 75, this.sizeX/this.sizeY, 1, 10000 );
        this.camera.position.z = 100;
        this.camera.lookAt = new THREE.Vector3(0, 0, 0);

        var hemiLight = new THREE.HemisphereLight(0xddddff, 0xddffdd, 0.6);
        hemiLight.position.set(0, 500, 0);
        this.scene.add(hemiLight);
        var pointLight = new THREE.PointLight(0xccffcc);
        hemiLight.position.set(0, 500, 0);
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


