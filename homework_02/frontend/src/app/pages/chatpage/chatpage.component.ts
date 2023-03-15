import {Component, OnInit} from '@angular/core';
import {SocketClient} from "../../service/socket-client";

@Component({
  selector: 'app-chatpage',
  templateUrl: './chatpage.component.html',
  styleUrls: ['./chatpage.component.scss']
})
export class ChatPageComponent implements OnInit {

  constructor(private socket: SocketClient) {
  }

  ngOnInit(): void {
    this.socket.sendMessage();
    console.log('Sending message')
  }

}
