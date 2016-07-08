import { Injectable } from '@angular/core';
import { Http, URLSearchParams } from '@angular/http';
import 'rxjs/add/operator/map';

@Injectable()
export class Github {
  constructor(private http: Http) {}

  getData(org: string) {
    var biotData = this.makeUdpRequest('get-data');
    console.log(biotData);
    return biotData;
  }

  private makeRequest(path: string) {
    let params = new URLSearchParams();
    //params.set('per_page', '100');

    let url = `http://localhost/${ path }`;

    return this.http.get(url, {search: params})
      .map((res) => res.json());
  }
}
