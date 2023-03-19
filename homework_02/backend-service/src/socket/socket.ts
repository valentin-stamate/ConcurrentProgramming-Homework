import {Express} from 'express'
import http from 'http'
import { Server } from 'socket.io'
import {Events} from "./events";
import {PersistenceService} from "../service/persistence.service";

export async function initSocketIO(app: Express) {
    const httpServer = http.createServer(app);
    const io = new Server(httpServer, {
        cors: {
            origin: "*",
            methods: ["GET", "POST"],
            allowedHeaders: ["*"],
            credentials: true,
        }
    });

    io.on('connection', socket => {
        console.log('User connected');

        socket.on(Events.POST_MESSAGE, async (data) => {
            await PersistenceService.addMessage(data);

            const newMessages = await PersistenceService.getAllMessages();
            socket.broadcast.emit(Events.UPDATE_MESSAGES, newMessages);
        });

        socket.on(Events.GET_MESSAGES, async () => {
            const messages = await PersistenceService.getAllMessages();

            socket.emit(Events.UPDATE_MESSAGES, messages);
        });
    });

    httpServer.listen(8081, () => {
        console.log('Running Socket server at port 8081');
    });
}