import { Injectable } from '@angular/core';
import {Socket} from "ngx-socket-io";
import {Events} from "./events";

@Injectable({
  providedIn: 'root'
})
export class SocketClient {

  constructor(private socket: Socket) { }

  sendMessage() {
    this.socket.emit(Events.MESSAGE, 'Ana are mere');
  }

}
