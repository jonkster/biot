import { Injectable } from '@angular/core';
import { Http, Response, URLSearchParams } from '@angular/http';

import {Observable} from 'rxjs/Observable';

import 'rxjs/add/operator/catch';
import 'rxjs/add/operator/map';
import 'rxjs/add/observable/throw';


@Injectable()
export class Biotz {
  constructor(private http: Http) {}

  private extractWSData (res: Response) {
      let body = res.json();
      return body || {};
  }

  addDummyNode(addr) {
      var path = 'biotz/addnode/' + addr;
      var url = "http://localhost:8889/" + path;
      return this.http.put(url, '')
          .map((response) => response.json());
  }

  dropDummyNodes() {
      var path = 'biotz/dropnodes';
      var url = "http://localhost:8889/" + path;
      return this.http.put(url, '')
          .map((response) => response.json());
  }

  getCalibration(addr) {
    var result = this.makeBrokerRequest('biotz/addresses/' + addr + '/calibration');
    return result;
  }

  getCachedAssemblies() {
      var path = 'data/assembly';
      var result = this.makeBrokerRequest(path);
      return result;
  }

  getCachedAssembly(name) {
      var path = 'data/assembly/' + name;
      var result = this.makeBrokerRequest(path);
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

  getStatus(addr) {
    var result = this.makeBrokerRequest('biotz/addresses/' + addr + '/status');
    return result;
  }

  getSystemMessageRate() {
    var result = this.makeBrokerRequest('system/mrate');
    return result;
  }

  private handleError (error: any) {
      let errMsg = (error.message) ? error.message :
          error.status ? `${error.status} - ${error.statusText}` : 'Server error';
      console.log("Error in broker request", errMsg);
      return Observable.throw(errMsg);
  }


  identify(addr) {
    var result = this.makeBrokerRequest('biotz/addresses/' + addr + '/identify');
    return result;
  }

  private makeBrokerRequest(path: string) {
    var url = "http://localhost:8889/" + path ;

    return this.http.get(url)
      //.map((response) => response.json())
      .map(this.extractWSData)
      .catch(this.handleError);
  }

  postAssemblyToCache(name, data: string) {
      var path = 'data/assembly/' + name;
      var url = "http://localhost:8889/" + path ;
      return this.http.post(url, data)
          .map((response) => response.json());
  }

  putCalibrationsToCache(addr, data: string) {
      var path = 'data/addresses/' + addr + '/calibration/' + data;
      var url = "http://localhost:8889/" + path ;
      return this.http.put(url, 'where_is_this?')
          .map((response) => response.json());
  }

  putCalibrationToNode(addr, data: string) {
      var path = 'biotz/addresses/' + addr + '/calibration/' + data;
      var url = "http://localhost:8889/" + path ;
      return this.http.put(url, 'where_is_this?')
          .map((response) => response.json());
  }

  putBiotzSensors(addr:string, g:boolean, a:boolean, c:boolean) {

      var data = '';
      if (g) {
          data += '1';
      } else {
          data += '0';
      } if (a) {
          data += '1';
      } else {
          data += '0';
      } if (c) {
          data += '1';
      } else {
          data += '0';
      }

      var path = 'biotz/addresses/' + addr + '/sensors/' + data;
      var url = "http://localhost:8889/" + path ;
      return this.http.put(url, 'where_is_this?')
          .map((response) => response.json());
  }

  resetCalibrationOnNode(addr) {
      var path = 'biotz/addresses/' + addr + '/calibration/0:0:0:0:0:0';
      var url = "http://localhost:8889/" + path ;
      return this.http.put(url, 'where_is_this?')
          .map((response) => response.json());
  }

  synchronise() {
    var result = this.makeBrokerRequest('biotz/synchronise');
    return result;
  }
}
