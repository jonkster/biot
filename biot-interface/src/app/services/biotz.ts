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

  getCachedCalibrationAddresses() {
      var result = this.makeBrokerRequest('data/addresses');
      return result;
  }

  getCachedCalibration(addr) {
      var path = 'data/addresses/' + addr + '/calibration';
      var result = this.makeBrokerRequest(path);
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

  putCachedCalibration(addr, data: string) {
      var path = 'data/addresses/' + addr + '/calibration/' + data;
      var url = "http://localhost:8889/" + path ;
      return this.http.put(url, 'where_is_this?')
          .map((response) => response.json());
  }

  synchronise() {
    var result = this.makeBrokerRequest('biotz/synchronise');
    return result;
  }
}
