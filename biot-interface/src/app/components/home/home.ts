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
    private monitoring:boolean = true;
    private counter:number = 0;
    private nodes: any = {};
    private threeD: any = {};

    constructor(public biotz:Biotz) {}

    ngOnInit() {
        this.updateData();
    }

    ngAfterViewInit() {
        this.threeD = this.threeDirective.first;
    }

    getData() {
        this.biotz.getData()
            .subscribe(rawData => {
                this.biotzData = rawData;
                this.biotzData = {
                    'count': rawData.c,
                    'nodes': []
                };
                var colours = [
                    0xff0000, 0x00ff00, 0x0000ff, 0xff00ff, 0x00ffff, 0xffff00
                ];
                for (var i = 0; i < rawData.n.length; i++) {
                    var addr = rawData.n[i].a;
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
                    this.biotzData.nodes.push({
                        'address': addr,
                        'colour': colourSt,
                        'time': dataSt[0]/1000,
                        'w': q.w,
                        'x': q.x,
                        'y': q.y,
                        'z': q.z
                    });

                    if (this.nodes[addr] === undefined)
                    {
                        this.nodes[addr] = {};
                        this.threeD.addNode(addr, (i*200)-600, 0, 0, colour);
                    }
                    this.nodes[addr] = q;
                    this.threeD.moveNode(addr, q);
                }
            });
    }

    identify(addr) {
        this.biotz.identify(addr).subscribe();;
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
        if (this.counter % 5 == 0)
            this.getData();

        if (this.monitoring) {
            this.counter++;
            requestAnimationFrame(
                () => this.updateData()
            );
        }
    }

}
