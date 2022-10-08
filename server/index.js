const express = require('express');


const port1 = 3000, port2 = 4000;

const app = express();
const WebSocket = require('ws');
const { WebSocketServer } = WebSocket;
const wss = new WebSocketServer({ port: port2 });

wss.on('connection', (ws) => {
    console.log('connected');
    ws.on('message', (msg) => {
        console.log(`msg${msg}`);
        ws.send(`ya got it: ${msg}`);
    })
})

app.use(express.static(`${__dirname}/public`));

app.listen(port1, () => { console.log(`app is up on port ${port1}!`); })