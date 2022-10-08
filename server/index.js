"use strict";

require('dotenv').config({ path: __dirname + '\\.env' });
require('dotenv').config({ path: __dirname + '\\sslcert\\.env' });


const Fs = require('fs');
const express = require('express');
const https = require('http');
const port = 3000;


const app = express();
const server = https.createServer({
    ssl: true,
    key: Fs.readFileSync(`${__dirname}${process.env.KEY}`),
    cert: Fs.readFileSync(`${__dirname}${process.env.CERT}`)
}, app);

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