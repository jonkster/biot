import { Injectable } from '@angular/core';
import { Http, URLSearchParams } from '@angular/http';
import 'rxjs/add/operator/map';

@Injectable()
export class Biotz {
  constructor(private http: Http) {}

  getData() {
    var result = this.makeBrokerRequest('biotz');
    return result;
  }

  private makeBrokerRequest(path: string) {
    var url = "http://localhost:8889/" + path ;

    return this.http.get(url)
      .map((response) => response.json());
  }
}
