import {Component, ViewChildren} from '@angular/core';
import {Observable} from 'rxjs/Observable';


import {ThreeDirective} from '../../directives/three.directive';
import {Biotz} from '../../services/biotz';

@Component({
  selector: 'nodes',
  pipes: [],
  providers: [ Biotz ],
  directives: [ ThreeDirective ],
  styleUrls: ['./nodes.css'],
  templateUrl: './nodes.html'
})
export class Nodes {
    @ViewChildren(ThreeDirective) threeDirective;
    private biotzData:any = {};
    private biotzCalibration:any = {};
    private detectedAddresses:any = {};
    private biotzStatus:any = {};
    private nodeColours:any = {};
    private savedCalibrations:any = {};
    private monitoring:boolean = true;
    private counter:number = 0;
    private nodes: any = {};
    private threeD: any = {};
    private showOnlyAddress: any = {};
    private wantAll: boolean = true;

    constructor(public biotz:Biotz) {}

    ngOnInit() {
        this.updateData();
        if (! this.savedCalibrationsExist()) {
            this.readStoredCalibrations();
        }
    }

    ngAfterViewInit() {
        this.threeD = this.threeDirective.first;
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
        this.biotzStatus[addr] = undefined;
        this.detectedAddresses[addr] = undefined;
        this.nodes[addr] = undefined;
        this.threeD.removeNode(addr);
    }

    getNodeCalibration(addr) {
        this.biotz.getCalibration(addr)
            .subscribe(rawData => {
                this.biotzCalibration[addr] = rawData;
            },
            error => {
                console.error("ERROR getting calibration!", error);
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
                                    this.threeD.addNode(addr, i*200, 0, 0, parseInt(colourSt, 16));
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
                        this.threeD.removeNode(name);
                        this.nodes[name] = undefined;
                    }
                }
            },
            error => {
                console.error("Error updating data!", error);
            });
    }

    identify(addr) {
        this.biotz.identify(addr).subscribe();
    }


    readStoredCalibrations() {
        var knownAddresses = [];
        this.biotz.getCachedCalibrationAddresses()
            .subscribe(addresses => {
                knownAddresses = addresses;
                for (var i = 0; i < knownAddresses.length; i++) {
                    var addr = knownAddresses[i];
                    (function(a, obj) {
                        obj.biotz.getCachedCalibration(a).subscribe( res => {
                            obj.savedCalibrations[a] = res;
                        });
                    })(addr, this);
                }
            });
    }

    getNodeColour(addr) {
        var colours = [
            0xff0000, 0x00ff00, 0x0000ff, 0xff00ff, 0x00ffff, 0xffff00
        ];
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

    saveCalibrations() {
         var addresses = Object.keys(this.biotzCalibration);
         for (var i = 0; i < addresses.length; i++) {
             var address = addresses[i];
             (function(a, obj) {
                var cal = obj.biotzCalibration[a];
                 obj.biotz.putCachedCalibration(a, cal)
                     .subscribe(res => {
                         obj.savedCalibrations[a] = obj.biotzCalibration[a];
                     });
             })(address, this)
         }

    }

    savedCalibrationsExist() {
        return (Object.keys(this.savedCalibrations).length > 0);
    }

    sendCalibrations() {
         this.biotzCalibration = this.savedCalibrations;
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
        if (this.counter % 2 == 0)
            this.getData();
        else if (this.counter % 21 == 0) {
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
