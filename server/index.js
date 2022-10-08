"use strict";

const WebSocket = require('ws');
const Fs = require('fs');
const https = require('https');
const express = require('express');

require('dotenv').config({ path: __dirname + '\\.env' });

const port = 8089;

const app = express();
const server = https.createServer({
    key: Fs.readFileSync(`${__dirname}/sslcert/key.pem`),
    cert: Fs.readFileSync(`${__dirname}/sslcert/cert.pem`)
}, app);

const wss = new WebSocket.Server({ server:server });


wss.on('connection', (ws) => {
    console.log('connected');
    ws.on('message', (msg) => {
        console.log(`msg${msg}`);
        ws.send(`ya got it: ${msg}`);
    })
})

app.use(express.static(`${__dirname}/public`));
server.listen(port, () => { console.log(`server is up on port ${port}!`); })