import {NgModule} from '@angular/core'
import {RouterModule} from "@angular/router";
import {rootRouterConfig} from "./app.routes";
import {AppComponent} from "./app.component";
import {FormsModule} from "@angular/forms";
import {BrowserModule} from "@angular/platform-browser";
import {HttpModule} from "@angular/http";
import {LocationStrategy, HashLocationStrategy} from '@angular/common';

import {AboutComponent} from './components/about/about.component';
import {NodesComponent} from './components/nodes/nodes.component';
import {AssembliesComponent} from './components/assemblies/assemblies.component';
import {DialogComponent} from './components/dialog/dialog.component';
import {DropDownComponent} from './components/dropdown/dropdown.component';
import {ThreeDirective} from './directives/three.directive';

@NgModule({
  declarations: [AppComponent, AboutComponent, NodesComponent, AssembliesComponent, DialogComponent, DropDownComponent, ThreeDirective ],
  imports     : [BrowserModule, FormsModule, HttpModule, RouterModule.forRoot(rootRouterConfig)],
  bootstrap   : [AppComponent]
})
export class AppModule {

}
