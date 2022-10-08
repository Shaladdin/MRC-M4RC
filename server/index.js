const express = require('express');
const http = require('http');
const port = 3000;

const app = express();
const server = http.createServer(app);

const WebSocket = require('ws');
const { WebSocketServer } = WebSocket;
const wss = new WebSocketServer({ server: server });

wss.on('connection', (ws) => {
    console.log('connected');
    ws.on('message', (msg) => {
        console.log(`msg${msg}`);
        ws.send(`ya got it: ${msg}`);
    })
})

app.use(express.static(`${__dirname}/public`));

server.listen(port, () => { console.log(`server is up on port ${port}!`); })