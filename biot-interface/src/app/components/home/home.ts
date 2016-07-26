import {Component, ViewChildren} from '@angular/core';
import {Observable} from 'rxjs/Observable';


import {ThreeDirective} from '../../directives/three.directive';
import {Biotz} from '../../services/biotz';

@Component({
  selector: 'home',
  pipes: [],
  providers: [ Biotz ],
  directives: [ ThreeDirective ],
  styleUrls: ['./home.css'],
  templateUrl: './home.html'
})
export class Home {
    @ViewChildren(ThreeDirective) threeDirective;
    private biotzData:any = {};
    private biotzCalibration:any = {};
    private biotzStatus:any = {};
    private savedCalibrations:any = {};
    private monitoring:boolean = true;
    private counter:number = 0;
    private nodes: any = {};
    private threeD: any = {};

    constructor(public biotz:Biotz) {}

    ngOnInit() {
        this.updateData();
        if (Object.keys(this.savedCalibrations).length == 0) {
            this.readStoredCalibrations();
        }
    }

    ngAfterViewInit() {
        this.threeD = this.threeDirective.first;
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

    getData() {
        this.biotz.getData()
            .subscribe(rawData => {
                //this.biotzData = rawData;
                this.biotzData = {
                    'count': 0,
                    'nodes': []
                };
                var colours = [
                    0xff0000, 0x00ff00, 0x0000ff, 0xff00ff, 0x00ffff, 0xffff00
                ];
                var nodesUpdated = [];
                for (var i = 0; i < rawData.n.length; i++) {
                    var addr = rawData.n[i].a;
                    if (rawData.n[i].v !== undefined) {
                        var dataSt = rawData.n[i].v.split(/:/);
                        var q = {
                            'w': dataSt[1],
                            'x': dataSt[2],
                            'y': dataSt[3],
                            'z': dataSt[4]
                        };
                        var idx = this.biotzData.nodes.length;
                        var colour = colours[idx % colours.length];
                        var colourSt = colour.toString(16);
                        while (colourSt.length < 6)
                        colourSt = '00' + colourSt;

                        var cal = this.savedCalibrations[addr];
                        if (cal === undefined) 
                            cal = '';

                        var nStat = this.biotzStatus[addr];
                        if (nStat === undefined) {
                            nStat = 'unknown';
                        } else {
                            nStat = nStat.status;
                        }


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
                                this.threeD.addNode(addr, (i*200)-600, 0, 0, colour);
                            }
                            this.nodes[addr] = q;
                        this.threeD.moveNode(addr, q);
                        nodesUpdated[addr] = true;
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

    sendCalibrations() {
         this.biotzCalibration = this.savedCalibrations;
    }

    synchronise() {
        this.biotz.synchronise().subscribe();;
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
