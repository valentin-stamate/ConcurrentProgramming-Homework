import express = require("express");
import morgan from 'morgan';
import helmet from 'helmet';
import cors from 'cors';
import {initSocketIO} from "./socket/socket";

const app = express();
const port = 8080;
const host = `http://localhost:${port}`

initSocketIO(app)
    .catch(err => {
        console.log('Error initializing SocketIO')
    });

/************************************************************************************
 *                              Basic Express Middlewares
 ***********************************************************************************/

app.set('json spaces', 4);
app.use(express.json());
app.use(express.urlencoded({extended: true}));

// Handle logs in console during development
app.use(morgan('dev'));
app.use(cors({origin: '*'}));

// Handle security and origin in production
if (process.env.NODE_ENV === 'production') {
    app.use(helmet());
}

/************************************************************************************
 *                               Register all REST routes
 ***********************************************************************************/

app.get('/', (req, res) => {
    res.end('Hello word!')
});

app.listen(port, () => {
    console.log(`Server started at ${host}`);
});