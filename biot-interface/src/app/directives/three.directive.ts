import { Directive, ElementRef, EventEmitter, HostListener, Input, Output } from '@angular/core';

import * as THREE from 'three';

@Directive({
    selector: '[myThreeD]',
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
    mouse = undefined;
    pointLight = undefined;
    hemiLight = undefined;
    raycaster = undefined;
    clearColour = '#d0d0d0';
    currentLimb = undefined;
    oldMidx = 0;
    selectedLimb = undefined;
    zoomFactor = 3;
    lightTarget = undefined;
    element = undefined;
    zoom = 1;

    @Input() sizeY: number = 300;
    @Input() sizeX: number = 1024;

    @Output() onLimbClicked = new EventEmitter<string>();

    @HostListener('mousemove', ['$event'])
      onMousemove(event) { this.mouseMoveHandler(event); };

    @HostListener('mousedown', ['$event'])
      onMousedown(event) { this.mouseDownHandler(event); };

    @HostListener('mouseup', ['$event'])
      onMouseup(event) { this.mouseUpHandler(event); };

    @HostListener('mousewheel', ['$event'])
      onMousewheel(event) { this.mouseWheelHandler(event); };



    constructor(private el: ElementRef) {
        this.element = el;
    }

    ngOnInit() {
	this.init();
	this.anim();
    }

    addNode(parentNodeName, name, x, y, z, colour, showLimb) {

        // make box
        var geometry = new THREE.BoxGeometry(20, 25, 10);
        var material = new THREE.MeshLambertMaterial( { color: colour} );

        var node = new THREE.Mesh(geometry, material);
        node.position.x = x;
        node.position.y = y;
        node.position.z = z;

        var limbRadius = 10;
        var limbLength = 100;
        if (showLimb) {
            var limbGeometry = new THREE.CylinderGeometry( limbRadius, limbRadius, limbLength, 50 );
            var matrix = new THREE.Matrix4();
            // shift so hinge point is at end of limb not middle
            matrix  = new THREE.Matrix4().makeTranslation(0, -limbLength/2, 0 );
            limbGeometry.applyMatrix( matrix );


            /* Make an ENVELOPE for the Limb */

            var limbMaterial = new THREE.MeshPhongMaterial({
                'transparent': true,
                'opacity': 0.3,
                'color': colour,
                'specular': 0xffffff,
                'shininess': 10
            });

            var limb = new THREE.Mesh( limbGeometry, limbMaterial );
            limb.rotateZ(Math.PI/2);
            limb.position.z = 0;
            limb.castShadow = true;
            limb.receiveShadow = true;
            limb.name = 'limb-envelope-' + name;
            node.add(limb);
        }


        var localAxis = this.addWorldAxis(0, 0, 0, 40, 2, 0.35);
        localAxis.castShadow = true;
        node.add(localAxis);

        node.name = 'biot-node-' + name;
        node.castShadow = true;
        node.receiveShadow = true;
        node.userData = {
            'parent': null,
            'address': name,
            'limbLength': limbLength,
            'limbRadius': limbRadius,
            'defaultX' : x,
            'defaultY' : y,
            'defaultZ' : z
        };
        if (parentNodeName == null) {
            this.scene.add(node);
        }
        else {
            var pnode = this.scene.getObjectByName('biot-node-' + parentNodeName);
            pnode.add(node);
        }

        var midx = this.getCentreOfNodes();
        this.camera.position.x = midx;
        this.pointLight.position.set(midx, 0, 400);
        this.lightTarget.position.set(midx, 0, 0);

    }

    addParent(address, parentAddress) {
        var node = this.scene.getObjectByName('biot-node-' + address);
        var pNode = this.scene.getObjectByName('biot-node-' + parentAddress);
        if (node && pNode) {
            if (this.noLoops(node, pNode)) {
                this.removeNode(node);
                node.userData['parent'] = parentAddress;
                node.position.x = 1.1 * pNode.userData['limbLength'];
                node.position.y = 0;
                node.position.z = 0;
                /*node.position.y = pNode.position.y;
                node.position.z = pNode.position.z;*/
                node.userData['defaultX'] = node.position.x;
                node.userData['defaultY'] = node.position.y;
                node.userData['defaultZ'] = node.position.z;
                pNode.add(node);
            } else {
                console.log('cannot set parent due to loop');
            }
        } else if (node) {
            this.removeNode(node);
            node.userData['parent'] = null;
	    node.position.x = node.userData['defaultX'];
	    node.position.y = node.userData['defaultY'];
	    node.position.z = node.userData['defaultZ'];
            this.scene.add(node);
        }
        else {
            console.log("cannot add parent to node: parent=" + parentAddress + ", node=" + address);
        }
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

        group.position.x = x;

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
	return midx;
    }


    init() {

        this.scene = new THREE.Scene();

        this.camera = new THREE.OrthographicCamera( -800, 800, 200, -200, 0.1, 1000);
        //this.camera = new THREE.PerspectiveCamera( 45, this.sizeX/this.sizeY, 0.1, 10000 );
        this.scene.add(this.camera);
        this.camera.up.set( 0, 0, 1 )
        this.camera.position.z = 200;
        this.camera.position.y = -250;
        this.camera.position.x = 0;
        this.camera.lookAt( this.scene.position );


        var worldAxis = this.addWorldAxis(-500, 0, 0, 100, 1, 0.35);
        worldAxis.name = 'world-axis-' + name;
        worldAxis.castShadow = true;
        this.scene.add(worldAxis);

        this.setupLighting();
        this.makeFloor();

        this.renderer = new THREE.WebGLRenderer();
        this.renderer.preserveDrawingBuffer = true ;
        this.renderer.setClearColor(this.clearColour, 1);
        this.renderer.setSize( this.sizeX, this.sizeY );
        this.renderer.shadowMap.enabled = true;
        this.element.nativeElement.appendChild( this.renderer.domElement );

        this.raycaster = new THREE.Raycaster();
        this.mouse = new THREE.Vector2();

    }


    focusNode(addr) {
	this.oldMidx = this.camera.position.x;
	this.camera.position.x = this.getCentreOfNodes();
	this.camera.zoom = this.zoomFactor;
	var node = this.scene.getObjectByName('biot-node-' + addr);
	this.camera.lookAt( node.position );
	this.camera.updateProjectionMatrix();
    }

    getLimbLength(addr) {
	var node = this.scene.getObjectByName('biot-node-' + addr);
        if (node) {
            return node.userData['limbLength'];
        }
        return '';
    }

    getParentAddress(addr) {
	var node = this.scene.getObjectByName('biot-node-' + addr);
        if (node) {
            return node.userData['parent'];
        }
        return '';
    }

    getRootLimb(limb) {
        var parentAddr = limb.userData['parent'];
        if (! parentAddr)
            return limb;
	var parentNode = this.scene.getObjectByName('biot-node-' + parentAddr);
        if (! parentNode)
            return limb;
        else
            return this.getRootLimb(parentNode);
    }

    makeFloor() {

	var floorMaterial = new THREE.MeshStandardMaterial( { color: 0xddffdd, roughness: 0.8, metalness: 0.1 } );
	var floorGeometry = new THREE.PlaneGeometry(2000, 500, 1, 1);
	var floor = new THREE.Mesh(floorGeometry, floorMaterial);
	floor.position.y = 0;
	floor.position.x = 300;
	floor.position.z = -50;
	floor.receiveShadow = true;
        floor.name = 'scene-floor';
        floor.visible = false;
	this.scene.add(floor);
    }


    moveNode(addr, q) {
	var node = this.scene.getObjectByName('biot-node-' + addr);
        node.updateMatrix();
        var tq = new THREE.Quaternion(q.x, q.y, q.z, q.w).normalize();
        var pAddr = node.userData['parent'];

        if (pAddr != null) {
            // undo parent rotation
	    var pNode = this.scene.getObjectByName('biot-node-' + pAddr);
            if (! pNode) {
                console.log("Cannot find parent!");
            } else {
	        pNode.updateMatrix();
                var newQ = pNode.quaternion.clone();
                newQ.inverse().normalize();
                newQ.multiply(tq).normalize();
                node.setRotationFromQuaternion(newQ);
            }
        } else {
            node.setRotationFromQuaternion(tq);
        }
        node.updateMatrix();
    }

    mouseDownHandler(event) {
        this.mouse.x = -1 + 2 * (event.offsetX / event.srcElement.width);
        this.mouse.y = 1 - 2 * (event.offsetY / event.srcElement.height);

        this.raycaster.setFromCamera(this.mouse, this.camera);
        var limbs = [];
        this.scene.traverse(function(ob) {
            if (ob.name.match(/biot-node/))
            limbs.push(ob);
        });
        var intersects = this.raycaster.intersectObjects(limbs);

        if (intersects.length > 0) {
            for (var i = 0; i < intersects.length; i++) {
                var limb = intersects[i].object;
                if (limb.userData.address) {
                    this.selectedLimb = limb;            
                    this.currentLimb = limb;            
                    this.selectedLimb.userData['currentX'] = this.selectedLimb.position.x;
                    this.selectedLimb.userData['currentY'] = this.selectedLimb.position.y;
                    this.selectedLimb.userData['currentZ'] = this.selectedLimb.position.z;
                    break;
                }
            }
        }
    }

    mouseMoveHandler(event) {

        if (this.selectedLimb) {
            this.mouse.x = -1 + 2 * (event.offsetX / event.srcElement.width);
            this.mouse.y = 1 - 2 * (event.offsetY / event.srcElement.height);

            if (this.selectedLimb) {
                this.raycaster.setFromCamera(this.mouse, this.camera);
                var plane = new THREE.Plane(new THREE.Vector3(0, 1, 0), this.selectedLimb.position.y);
                var hitPoint = this.raycaster.ray.intersectPlane(plane);
                if (hitPoint) {
                    this.selectedLimb.position.x = hitPoint.x;
                    this.selectedLimb.position.y = hitPoint.y;
                    this.selectedLimb.position.z = hitPoint.z;
                } else {
                    this.selectedLimb = undefined;
                }

            }
        }
    }

    mouseUpHandler(event) {
        if (this.selectedLimb) {
            if ((this.selectedLimb.position.x == this.selectedLimb.userData['currentX']) && (this.selectedLimb.position.y == this.selectedLimb.userData['currentY']) && (this.selectedLimb.position.z == this.selectedLimb.userData['currentZ'])) {
                this.openNodeDialog(this.selectedLimb);
            } else if (this.selectedLimb.userData['parent']) {
                this.selectedLimb.position.x = this.selectedLimb.userData['currentX'];
                this.selectedLimb.position.y = this.selectedLimb.userData['currentY'];
                this.selectedLimb.position.z = this.selectedLimb.userData['currentZ'];
            }
            this.selectedLimb = undefined;
        }
    }

    mouseWheelHandler(event) {
        var amount = event.wheelDelta;
        var delta = 0.1;
        if (amount < 0)
            delta = -0.1;
        this.camera.zoom += delta;
        this.camera.updateProjectionMatrix();
    }

    noLoops(node, pNode): boolean {
        if (node.userData['address'] == pNode.userData['address'])
            return false;
        while (pNode) {
            var pNodeAdd = pNode.userData['parent'];
	    pNode = this.scene.getObjectByName('biot-node-' + pNodeAdd);
            if (pNode) {
                if (node.userData['address'] == pNode.userData['address'])
                    return false;
            }
        }
        return true;
    }

    openNodeDialog(limb) {
        this.onLimbClicked.emit(limb.userData['address']);
    }

    removeNode(name) {
	var node = this.scene.getObjectByName('biot-node-' + name);
	this.scene.remove(node);
    }

    setupLighting() {
	var ambientLight = new THREE.AmbientLight(0xddddff, 0.3);
	this.scene.add(ambientLight);

        // where lighting aims
	this.lightTarget = new THREE.Object3D();
	this.lightTarget.position.set(0, 0, 0);
	this.scene.add(this.lightTarget);

	this.hemiLight = new THREE.HemisphereLight(0xddddff, 0xddffdd, 0.6);
	this.hemiLight.position.set(0, 0, 400);
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

    setCurrentLimb(addr) {
	var node = this.scene.getObjectByName('biot-node-' + addr);
        this.currentLimb = node;            
    }

    setFloorVisibility(show) {
        var floor = this.scene.getObjectByName('scene-floor');
        floor.visible = show;
    }

    setLimbLength(addr, len) {
        var node = this.scene.getObjectByName('biot-node-' + addr);
        if (node) {
            node.userData['limbLength'] = len;
            var adjusted = false;
            node.traverse(function(nodeChild){
                if (nodeChild.name == 'limb-envelope-' + addr) {
                    nodeChild.scale.y = len/100;
                } else if (nodeChild.userData['parent'] && ! adjusted) {
                    if (nodeChild.name != node.name) {
                        nodeChild.position.x = 1.1 * len;
                        nodeChild.position.y = 0;
                        nodeChild.position.z = 0;
                        adjusted = true;
                    }
                }
            });
        }
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


