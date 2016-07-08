import {Component} from '@angular/core';

import {Observable} from 'rxjs/Observable';

import {Biotz} from '../../services/biotz';

@Component({
  selector: 'home',
  pipes: [],
  providers: [ Biotz ],
  directives: [],
  styleUrls: ['./home.css'],
  templateUrl: './home.html'
})
export class Home {
    private biotzData:any = {};
    private monitoring:boolean = true;
    private counter:integer = 0;

    constructor(public biotz:Biotz) {}

    ngOnInit() {
        this.updateData();
    }

    getData() {
        this.biotz.getData()
            .subscribe(rawData => {
                this.biotzData = rawData;
                this.biotzData = {
                    'count': rawData.c,
                    'nodes': []
                };
                for (var i = 0; i < rawData.n.length; i++) {
                    var addr = rawData.n[i].a;
                    var dataSt = rawData.n[i].v.split(/:/);
                    this.biotzData.nodes.push({
                        'address': addr,
                        'time': dataSt[0]/1000,
                        'w': dataSt[1],
                        'x': dataSt[2],
                        'y': dataSt[3],
                        'z': dataSt[4]
                    });
                }
            });
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
        if (this.counter % 10 == 0)
            this.getData();

        if (this.monitoring) {
            this.counter++;
            requestAnimationFrame(
                () => this.updateData()
            );
        }
    }

}
