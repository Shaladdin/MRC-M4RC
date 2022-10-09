"use strict";

const WebSocket = require('ws');
const Fs = require('fs');
const http = require('http');
const express = require('express');

require('dotenv').config({ path: __dirname + '\\.env' });

const port = 8089;

const app = express();
const server = http.createServer(app);

const wss = new WebSocket.Server({ server: server });

wss.on('connection', (ws) => {
    console.log('connected');
    ws.on('message', (msg) => {
        console.log(`msg${msg}`);
        ws.send(`ya got it: ${msg}`);
    })
})

app.use(express.static(`${__dirname}/public`));
server.listen(port, () => { console.log(`server is up on port ${port}!`); })