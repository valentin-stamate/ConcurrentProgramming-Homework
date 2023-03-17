import express, {Express} from 'express'
import http from 'http'
import { Server } from 'socket.io'
import {Events} from "./events";

export async function initSocketIO(app: Express) {
    const httpServer = http.createServer(app);
    const io = new Server(httpServer, {
        cors: {
            origin: "http://localhost:4200",
            methods: ["GET", "POST"],
            allowedHeaders: ["*"],
            credentials: true
        }
    });

    io.on('connection', socket => {
        console.log('User connected');

        socket.on(Events.MESSAGE, (data) => {
            console.log(data);
        });
    });

    httpServer.listen(8081, () => {
        console.log('Running Socket server at port 8081');
    });
}