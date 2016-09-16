import {Component, ViewChild, ViewChildren} from '@angular/core';
import {Observable} from 'rxjs/Observable';

import {ThreeDirective} from '../../directives/three.directive';
import {DialogComponent} from '../dialog/dialog.component';
import {DropDownComponent} from '../dropdown/dropdown.component';
import {Biotz} from '../../services/biotz';

@Component({
  selector: 'assemblies',
  pipes: [],
  providers: [ Biotz ],
  directives: [ ThreeDirective, DropDownComponent, DialogComponent ],
  styleUrls: ['./assemblies.css', '../dialog/dialog.component.css' ],
  templateUrl: './assemblies.html'
})
export class Assemblies {
    @ViewChildren(ThreeDirective) threeDirective;
    @ViewChildren(DialogComponent) dialogDirectives;

    private accel:boolean = true;
    private assemblyNames:any = [];
    private biotzCalibration:any = {};
    private biotzData:any = {};
    private biotzStatus:any = {};
    private compass:boolean = true;
    private counter:number = 0;
    private currentAssembly:any = {};
    private detectedAddresses:any = {};
    private limbDialog;
    private parentsNeedUpdating: boolean = false;
    private assemblyDialog;
    private assemblyLoadDialog;
    private dummyNodeCount = 0;
    private gyro:boolean = true;
    private monitoring:boolean = true;
    private nodeColours:any = {};
    private nodes: any = {};
    private savedCalibrations:any = {};
    private showOnlyAddress: any = {};
    private systemMessageRate: number = 0;
    private threeD: any = {};
    private throttle:boolean = false;
    private wantAll: boolean = true;
    private wantedAssembly: string = '';

    //private parents:Array<string> = [];

    constructor(public biotz:Biotz) {}

    ngOnInit() {
        this.updateData();
        if (! this.savedCalibrationsExist()) {
            this.readSavedCalibrations();
        }
    }

    ngAfterViewInit() {
        this.threeD = this.threeDirective.first;
        this.threeD.setFloorVisibility(false);
        var dialogs = {};
        this.dialogDirectives.forEach(function(dialog) {
            dialogs[dialog.name] = dialog;
        });
        this.limbDialog = dialogs['limb-dialog'];
        this.assemblyDialog = dialogs['assembly-dialog'];
        this.assemblyLoadDialog = dialogs['assembly-load-dialog'];
    }

    addDummyNode(addr) {
        if (! addr)
            addr = 'affe::1234:' + this.dummyNodeCount++;
        this.biotz.addDummyNode(addr).subscribe();
    }

    canShow(addr) {
        if (Object.keys(this.showOnlyAddress).length === 0)
            return true;
        else
            return this.showOnlyAddress[addr];
    }

    dropNode(addr) {
        var nodes = this.biotzData.nodes;
        var activeNodes = [];
        for (var i = 0; i < nodes.length; i++) {
            var node = nodes[i];
            if (node.address !== addr) {
                activeNodes.push(node);
            }
        }
        this.biotzData = {
            'count' : activeNodes.length,
            'nodes': activeNodes
        }
        delete this.biotzStatus[addr];
        delete this.detectedAddresses[addr];
        delete this.nodes[addr];
        this.threeD.removeNode(addr);
    }

    dropDummyNodes() {
        var monitoringStatus = this.monitoring;
        this.monitoring = false;
        this.biotz.dropDummyNodes().subscribe();
        var nodes = this.biotzData.nodes;
        for (var i = 0; i < nodes.length; i++) {
            var node = nodes[i];
            this.dropNode(node.address);
        }
        this.dummyNodeCount = 0;
        this.monitoring = monitoringStatus;
    }

    getNodeCalibration(addr) {
        this.biotz.getCalibration(addr)
            .subscribe(rawData => {
                this.biotzCalibration[addr] = rawData;
            },
            error => {
                console.error("ERROR getting calibration. Has broker died?", error);
            });
    }

    getNodeStatus(addr) {
        this.biotz.getStatus(addr)
            .subscribe(rawData => {
                this.biotzStatus[addr] = rawData;
            },
            error => {
                console.error("ERROR getting status!", error);
                this.dropNode(addr);
            });
    }

    getDetectedAddresses() {
        return Object.keys(this.detectedAddresses);
    }

    getOtherAddresses(thisAddr) {

        var others = ['no parent'];
        var addresses = this.getDetectedAddresses();
        for (var i = 0; i < addresses.length; i++) {
            var addr = addresses[i];
            if (addr != thisAddr) {
                others.push(addr);
            }
        }
        return others;
    }

    getCurrentLimbAddress() {
        if (this.threeD.currentLimb)
            return this.threeD.currentLimb.userData['address'];
        else
            return '';
    }

    getVisibleAddresses() {
        var vis = [];
        var addresses = this.getDetectedAddresses();
        for (var i = 0; i < addresses.length; i++) {
            var addr = addresses[i];
            if (this.canShow(addr)) {
                vis.push(addr);
            }
        }
        return vis;
    }

    getBiotData(addr) {
        var nodes = this.biotzData.nodes;
        for (var i = 0; i < this.biotzData.count; i++) {
            var node = nodes[i];
            if (node.address === addr) {
                return node;
            }
        }
        return {};
    }

    getBiotProperty(addr, quality) {
        var biot = this.getBiotData(addr);
        if (biot !== undefined) {
            return biot[quality];
        } else {
            return '';
        }
    }

    getData() {
        if (this.throttle) {
            return;
        }
        this.throttle = true;
        this.biotz.getData()
            .subscribe(rawData => {
                this.biotzData = {
                    'count': 0,
                    'nodes': []
                };
                var nodesUpdated = [];
                for (var i = 0; i < rawData.n.length; i++) {
                    var addr = rawData.n[i].a;
                    if (rawData.n[i].v !== undefined) {
                        this.detectedAddresses[addr] = true;
                        if (this.canShow(addr)) {
                            var dataSt = rawData.n[i].v.split(/:/);
                            var q = {
                                'w': dataSt[1],
                                'x': dataSt[2],
                                'y': dataSt[3],
                                'z': dataSt[4]
                            };

                            var cal = this.savedCalibrations[addr];
                            if (cal === undefined) 
                                cal = '';

                            var nStat = this.biotzStatus[addr];
                            if (nStat === undefined) {
                                nStat = 'unknown';
                            } else {
                                if (nStat.status)
                                    nStat = nStat.status;
                            }

                            var colourSt = this.getNodeColour(addr);

                            this.biotzData.nodes.push({
                                'address': addr,
                                'colour': colourSt,
                                'time': dataSt[0]/1000,
                                'w': q.w,
                                'x': q.x,
                                'y': q.y,
                                'z': q.z,
                                'status': nStat,
                                'calibration' : this.biotzCalibration[addr],
                                'savedCals' : cal
                            });

                            if (this.nodes[addr] === undefined)
                                {
                                    this.nodes[addr] = {};
                                    this.threeD.addNode(null, addr, i*200, 0, 0, parseInt(colourSt, 16), 'limb-' + addr, 100, true);
                                    var cal = this.savedCalibrations[addr];
                                    this.biotz.putCalibrationToNode(addr, cal)
                                }
                                this.nodes[addr] = q;
                            this.threeD.moveNode(addr, q);
                            nodesUpdated[addr] = true;
                        }
                    }
                }
                this.biotzData.count = this.biotzData.nodes.length;

                // clean up expired nodes
                var addressesKnown = Object.keys(this.nodes);
                for (var i = 0; i < addressesKnown.length; i++) {
                    var name = addressesKnown[i];
                    if (! nodesUpdated[name]) 
                    {
                        delete this.detectedAddresses[name];
                        delete this.nodes[name];
                        this.threeD.removeNode(name);
                    }
                }
                this.throttle = false;
                if (this.parentsNeedUpdating) {
                    this.setParentsOfCurrentAssembly();
                }
            },
            error => {
                console.error("Error updating data, has broker died?", error);
            });
    }

    getMessageRate() {
        this.biotz.getSystemMessageRate().subscribe( res => {
            this.systemMessageRate = 1000 * res;
        });
    }

    identify(addr) {
        this.biotz.identify(addr).subscribe();
    }

    loadAssembly(name) {
        this.biotz.getCachedAssembly(name)
            .subscribe(res => {
                this.currentAssembly = {};
                this.currentAssembly = JSON.parse(res);
                var savedAddresses = Object.keys(this.currentAssembly);
                var colourSt = this.getNodeColour(addr);
                for (var i = 0; i < savedAddresses.length; i++) {
                    var addr = savedAddresses[i];
                    var node = this.currentAssembly[addr];
                    if (! this.detectedAddresses[addr]) {
                        this.addDummyNode(addr);
                        this.detectedAddresses[addr] = true;
                    }
                    this.setLimbLength(addr, node['limbLength']);
                    this.setLimbName(addr, node['limbName']);
                }

                // dummy nodes will be registered at this point by broker but
                // the drawing stuff won't know about them until it next reads
                // data from broker in the getData method so you can't assemble
                // the nodes yet.
              
                this.parentsNeedUpdating = true;
            });
    };

    getCurrentLimbName() {
        if (this.threeD.getLimbName)
            return this.threeD.getLimbName(this.getCurrentLimbAddress());
        return '';
    }

    setLimbName(addr, name) {
        this.threeD.setLimbName(addr, name);
    }

    getLimbName(addr) {
        if (this.threeD.getLimbName)
            return this.threeD.getLimbName(addr);
        return '';
    }

    setParentsOfCurrentAssembly() {
        var ok = true;
        var addresses = Object.keys(this.currentAssembly);
        for (var i = 0; i < addresses.length; i++) {
            var addr = addresses[i];
            var node = this.currentAssembly[addr];
            this.setLimbLength(addr, node['limbLength']);
            this.setLimbName(addr, node['limbName']);
            if (node['parent'] != 'none') {
                if (! this.setParent(addr, node['parent'])) {
                    ok = false;
                }
            }
        }
        this.parentsNeedUpdating = ! ok;
        return ok;
    }

    loadSavedAssembly() {
        this.biotz.getCachedAssemblies()
            .subscribe(res => {
                this.assemblyNames = res;
                this.assemblyLoadDialog.openDialog('load');
            });
    };


    setLimbLength(addr, len) {
        this.threeD.setLimbLength(addr, len);
    }

    saveAssembly(name) {
        this.currentAssembly = {};
        var addresses = this.getDetectedAddresses();
        for (var i = 0; i < addresses.length; i++) {
            var addr = addresses[i];
            var parentAddr = this.getParentAddress(addr);
            this.currentAssembly[addr] = {
                'parent': parentAddr,
                'limbLength': this.getLimbLength(addr),
                'limbName': this.getLimbName(addr)
            }
        }
        this.biotz.postAssemblyToCache(name, JSON.stringify(this.currentAssembly))
            .subscribe(res => {
                console.log(addr, "assembly data should now be saved to cache");
            });
    }

    saveCurrentAssembly() {
        this.assemblyDialog.openDialog('save');
    };

    setSensors(g, a, c) {
        this.gyro = g;
        this.accel = a;
        this.compass = c;
        var addresses = Object.keys(this.savedCalibrations);
        for (var i = 0; i < addresses.length; i++) {
            var address = addresses[i];
            this.biotz.putBiotzSensors(address, this.gyro, this.accel, this.compass).subscribe();
        }
    }

    // normally only needed on start up, reads data from cache on PC
    readSavedCalibration(address) {
        this.biotz.getCachedCalibration(address).subscribe( res => {
            this.savedCalibrations[address] = res;
        });
    }

    // normally only needed on start up, reads data from cache on PC
    readSavedCalibrations() {
        var knownAddresses = [];
        this.biotz.getCachedCalibrationAddresses()
            .subscribe(addresses => {
                knownAddresses = addresses;
                for (var i = 0; i < knownAddresses.length; i++) {
                    var addr = knownAddresses[i];
                    (function(a, obj) {
                        obj.readSavedCalibration(a);
                    })(addr, this);
                }
            });
    }

    getNodeColour(addr) {
        var colours = [
            0xff0000, 0x00ff00, 0x0000ff, 0xff00ff, 0x00ffff, 0xffff00
        ];
        if (! addr || addr == 'none') 
            return '7f7f7f';
        if (this.nodeColours[addr] === undefined) {
            var idx = Object.keys(this.nodeColours).length;
            var colour = colours[idx % colours.length];
            var colourSt = colour.toString(16);
            while (colourSt.length < 6) {
                colourSt = '00' + colourSt;
            }
            this.nodeColours[addr] = colourSt;
        }
        return this.nodeColours[addr];
    }

    getParentAddress(addr) {
        var pAddr = this.threeD.getParentAddress(addr);
        if (pAddr)
            return pAddr;
        else
            return 'none'
    }

    getCurrentLimbLength() {
        return this.getLimbLength(this.getCurrentLimbAddress());
    }


    getLimbLength(addr) {
        if (addr) {
            return this.threeD.getLimbLength(addr);
        } else {
            return '??';
        }

    }

    resetCalibration(addr) {
        this.biotz.resetCalibrationOnNode(addr).subscribe( res => {
            console.log('reset', res);
        });
    }


    saveCalibration(addr, cal) {
        this.biotz.putCalibrationsToCache(addr, cal)
            .subscribe(res => {
                console.log(addr, "measured calibrations should now be saved to cache");
                this.savedCalibrations[addr] = this.biotzCalibration[addr];
            });
    }

    // store currently measured calibrations in cache for later use
    saveCalibrations() {
         var addresses = Object.keys(this.biotzCalibration);
         for (var i = 0; i < addresses.length; i++) {
             var address = addresses[i];
             (function(a, obj) {
                var cal = obj.biotzCalibration[a];
                obj.saveCalibration(a, cal);
             })(address, this)
         }

    }

    savedCalibrationsExist() {
        return (Object.keys(this.savedCalibrations).length > 0);
    }

    sendCalibration(addr, cal) {
        this.biotz.putCalibrationToNode(addr, cal)
            .subscribe(res => {
                console.log(addr, "node should now have previously cached calibration", cal);
            });
    }

    // send cached calibrations to nodes
    sendCalibrations() {
         var addresses = Object.keys(this.savedCalibrations);
         for (var i = 0; i < addresses.length; i++) {
             var address = addresses[i];
             (function(a, obj) {
                var cal = obj.savedCalibrations[a];
                 obj.biotz.putCalibrationToNode(a, cal)
                     .subscribe(res => {
                         console.log(address, "node should now have previously cached calibrations");
                         //obj.savedCalibrations[a] = obj.biotzCalibration[a];
                     });
             })(address, this)
         }

    }

    setParent(child, mother) {
        return this.threeD.addParent(child, mother);
    }

    synchronise() {
        this.biotz.synchronise().subscribe();;
    }

    showAll(value) {
        var addresses = this.getDetectedAddresses();
        for (var i = 0; i < addresses.length; i++) {
            var addr = addresses[i];
            this.showOnlyAddress[addr] = value;
        }
        if (value) {
            this.wantAll = true;
            this.threeD.unFocusNode();
        }
    }

    setWantedAssembly(name) {
        this.wantedAssembly = name;
    }

    selectLimb(addr) {
        this.threeD.setCurrentLimb(addr);
        this.limbDialog.openDialog(addr);
    }

    showOne(addr, value) {
        this.showOnlyAddress[addr] = value;
    }

    toggleShowAll(addr) {
        if (this.wantAll) {
            this.showAll(false);
            this.showOne(addr, true);
            this.wantAll = false;
            this.threeD.focusNode(addr);
        } else {
            this.showAll(true);
            this.wantAll = true;
            this.threeD.unFocusNode();
        }
    }

    toggleMonitoring() {
        this.monitoring = ! this.monitoring;
        if (this.monitoring) {
            this.updateData();
        } else {
            this.counter = 0;
        }
    }

    updateData() {
        this.getData();
        if (this.counter % 21 == 0) {
            this.getMessageRate();
            for (var i = 0; i < this.biotzData.count; i++) {
                var addr = this.biotzData.nodes[i].address;
                this.getNodeCalibration(addr);
                this.getNodeStatus(addr);
            }
        }

        if (this.monitoring) {
            this.counter++;
            requestAnimationFrame(
                () => this.updateData()
            );
        }
    }

}
