import {Routes} from '@angular/router';
import {AboutComponent} from './components/about/about.component';
import {NodesComponent} from './components/nodes/nodes.component';
import {AssembliesComponent} from './components/assemblies/assemblies.component';

export const rootRouterConfig: Routes = [
  {path: '', redirectTo: 'about', pathMatch: 'full'},
  {path: 'about', component: AboutComponent},
  {path: 'nodes', component: NodesComponent},
  {path: 'assemblies', component: AssembliesComponent}
];

