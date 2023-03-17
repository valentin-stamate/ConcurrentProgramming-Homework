import {Component, ElementRef, OnInit, ViewChild} from '@angular/core';
import {SocketClient} from "../../service/socket-client";
import {Message} from "../../interfaces/interfaces";

const messages: Message[] = [
  {
    username: 'alinus',
    message: 'Lorem ipsum dolor sit amed',
    date: new Date(),
  },
  {
    username: 'valstam',
    message: 'Lorem ipsum dolor sit amed',
    date: new Date(),
  },
  {
    username: 'claurentiu',
    message: 'Lorem ipsum dolor sit amed',
    date: new Date(),
  },
];

@Component({
  selector: 'app-chatpage',
  templateUrl: './chatpage.component.html',
  styleUrls: ['./chatpage.component.scss']
})
export class ChatPageComponent implements OnInit {

  messages: Message[] = [];
  username: string = 'valstam';
  @ViewChild('messageContainer') messageContainer: ElementRef = {} as ElementRef;

  constructor(private socket: SocketClient) { }

  ngOnInit(): void {
    this.socket.sendMessage();
    this.fetchMessages();
  }

  onSendMessage(event: Event, form: HTMLFormElement, input: HTMLInputElement) {
    event.preventDefault();

    const messageObject: Message = {
      username: this.username,
      message: input.value,
      date: new Date(),
    }

    if (messageObject.message === '') {
      return;
    }

    messages.push(messageObject);
    this.fetchMessages();
  }

  fetchMessages() {
    this.messages = JSON.parse(JSON.stringify(messages));
    this.messages.reverse();
  }

}
