import { Injectable } from '@angular/core';
import { Http, URLSearchParams } from '@angular/http';
import 'rxjs/add/operator/map';

@Injectable()
export class Biotz {
  constructor(private http: Http) {}

  getCalibration(addr) {
    var result = this.makeBrokerRequest('biotz/addresses/' + addr + '/calibration');
    return result;
  }


  getData() {
    var result = this.makeBrokerRequest('biotz');
    return result;
  }

  identify(addr) {
    var result = this.makeBrokerRequest('biotz/addresses/' + addr + '/identify');
    return result;
  }

  private makeBrokerRequest(path: string) {
    var url = "http://localhost:8889/" + path ;

    return this.http.get(url)
      .map((response) => response.json());
  }

  synchronise() {
    var result = this.makeBrokerRequest('biotz/synchronise');
    return result;
  }
}
