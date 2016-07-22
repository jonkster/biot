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

        var worldAxis = this.addWorldAxis(x, y, z, 100, 1, 0.35);
        worldAxis.translateX(x);
        this.scene.add(worldAxis);

        var localAxis = this.addWorldAxis(x, y, z, 40, 5, 0);
        node.add(localAxis);

        this.scene.add(node);
	/*var labelName = name.replace(/:/g, ":\n");
        var label = this.createLabel(labelName, x, y, z, 20, "white", "black", 3);
        this.scene.add(label);*/
    }

    addWorldAxis(x, y, z, length, width, brightness) {
        var group = new THREE.Object3D();

        var r = new THREE.Color(0.5, 0.1, 0.1).offsetHSL(0, 0, brightness);
        var g = new THREE.Color(0.1, 0.5, 0.1).offsetHSL(0, 0, brightness);
        var b = new THREE.Color(0.1, 0.1, 0.5).offsetHSL(0, 0, brightness);
        console.log(r.getHSL(), g.getHSL(), b.getHSL());

        var lmaterial = new THREE.LineBasicMaterial( {color: r.getHex(), linewidth: width});
        var lgeometry = new THREE.Geometry();
        lgeometry.vertices.push(new THREE.Vector3(0, 0, 0));
        lgeometry.vertices.push(new THREE.Vector3(length, 0, 0));
        var line = new THREE.Line(lgeometry, lmaterial);
        group.add(line);

        lmaterial = new THREE.LineBasicMaterial( {color: g.getHex(), linewidth: width} );
        lgeometry = new THREE.Geometry();
        lgeometry.vertices.push(new THREE.Vector3(0, 0, 0));
        lgeometry.vertices.push(new THREE.Vector3(0, length, 0));
        line = new THREE.Line(lgeometry, lmaterial);
        group.add(line);

        lmaterial = new THREE.LineBasicMaterial( {color: b.getHex(), linewidth: width} );
        lgeometry = new THREE.Geometry();
        lgeometry.vertices.push(new THREE.Vector3(0, 0, 0));
        lgeometry.vertices.push(new THREE.Vector3(0, 0, length));
        line = new THREE.Line(lgeometry, lmaterial);
        group.add(line);

        return group;
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

        this.camera = new THREE.OrthographicCamera( -800, 800, 200, -200, 0.1, 1000);
        //this.camera = new THREE.PerspectiveCamera( 45, this.sizeX/this.sizeY, 0.1, 10000 );
        this.scene.add(this.camera);
        this.camera.up.set( 0, 0, 1 )
        this.camera.position.z = 200;
        this.camera.position.y = -250;
        this.camera.position.x = 30;
        this.camera.lookAt( this.scene.position );

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

    removeNode(name) {
        var node = this.scene.getObjectByName(name);
        this.scene.remove(node);
    }



}


