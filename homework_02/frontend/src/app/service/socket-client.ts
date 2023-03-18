import { Injectable } from '@angular/core';
import {Socket} from "ngx-socket-io";
import {Events} from "./events";
import {Message} from "../interfaces/interfaces";

@Injectable({
  providedIn: 'root'
})
export class SocketClient {
  updateMessagesEvent = this.socket.fromEvent<Message[]>(Events.UPDATE_MESSAGES);

  constructor(private socket: Socket) { }

  sendMessage(message: Message) {
    this.socket.emit(Events.POST_MESSAGE, message);
  }

  sendGetMessages() {
    this.socket.emit(Events.GET_MESSAGES);
  }

}
