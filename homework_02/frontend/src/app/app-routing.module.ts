import { NgModule } from '@angular/core';
import { RouterModule, Routes } from '@angular/router';
import {SocketIoConfig, SocketIoModule} from "ngx-socket-io";

const routes: Routes = [];

const config: SocketIoConfig = {
  url: 'http://18.184.112.27:8081',
  options: {},
};

@NgModule({
  imports: [
    RouterModule.forRoot(routes),
    SocketIoModule.forRoot(config)],
  exports: [RouterModule]
})
export class AppRoutingModule { }
