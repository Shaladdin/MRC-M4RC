"use strict";

const WebSocket = require('ws');
const http = require('http');
const express = require('express');
const nedb = require('nedb');

require('dotenv').config({ path: __dirname + '\\.env' });

const port = 8080;
const app = express();
const server = http.createServer(app);
const wss = new WebSocket.Server({ server: server });
const db = {
    sensor: new nedb({ filename: `${__dirname}/db/sensors.db`, autoload: true })
}


for (const [key, value] of Object.entries(db)) {
    value.loadDatabase();
    value.persistence.setAutocompactionInterval(60);
}



const { stringify, parse: jsonify } = JSON;
const deserializeError = stringify({
    msg: "error",
    err: "input is not valid json, json only"
})

const getAllData = async () => {
    const output = {};
    for (const [key, value] of Object.entries(device)) {
        output[key] = await value.getData();
    }
    return output;
}
const updateStream = async () => {
    const { users } = usersModule;
    const data = await getAllData();
    for (const [key, value] of Object.entries(users))
        if (value.mode.currentMode === 'sensorStream')
            value.ws.send(stringify(data));
}
const updateDevice = async (device, data) => {
    db.sensor.update({ robot: device }, { $set: { data: data } }, { upsert: true });
}

const device = {
    smartHome: {
        ws: undefined,
        run: async () => {
            const { smartHome } = device;
            const { ws, data } = smartHome;
            // allow stream for sending sensors data
            ws.allowStream = true;

            ws.on('message', (msg) => {
                let res = objify(msg);
                if (res === false) { ws.send(deserializeError); return; };
                // if stream
                if (res.type === 'stream') {
                    const { suhu, humidity, gas, api, orang } = res.data;
                    if (res.stream === 'sensors') {
                        data.suhu = suhu; data.api = api; data.humidity = humidity; data.gas = gas; data.orang = orang;
                        updateDevice("smartHome", data);
                        updateStream();
                        return;
                    }
                    ws.send(stringify({
                        msg: "error",
                        err: "no stream found"
                    }));
                }
                else if (res.type === 'notify') {
                    if (res.notify === 'security') {
                        const { users } = usersModule;
                        for (const [key, value] of Object.entries(users))
                            value.ws.send(stringify({msg:'security alert'}));
                        return
                    }
                }
            })
        },
        data: {
            suhu: undefined,
            humidity: undefined,
            api: false,
            orang: undefined,
            security: false,
            gas: {
                co: undefined,
                lpg: undefined,
                smoke: undefined
            },
        },
        mode: 'automatic',
        getData: async () => {
            const { smartHome } = device;
            const { ws, data } = smartHome;
            if (ws !== undefined && ws.allowStream === true) return device.smartHome.data
            // set up for empty data
            var dbRead = await db.read(db.sensor, { robot: "smartHome" })
            var { data: dbData } = dbRead;
            data.suhu = dbData.suhu; data.humidity = dbData.humidity; data.api = dbData.api; data.gas = dbData.gas; data.orang = dbData.orang;

            return device.smartHome.data
        },
    },
    roboBin: {
        ws: undefined,
        run: async () => {
            const { roboBin } = device;
            const { ws, data } = roboBin;
            // allow stream for sending sensors data
            ws.allowStream = true;
            ws.on('message', (msg) => {
                let res = objify(msg);
                if (res === false) { ws.send(deserializeError); return; };
                if (res.type === 'stream') {
                    if (res.stream === 'sensors') {
                        const { metal, nonMetal, } = res.data;
                        data.metal = metal; data.nonMetal = nonMetal;
                        db.sensor.update({ robot: "roboBin" }, { $set: { data: data } }, { upsert: true });
                        updateStream();
                        return;
                    }
                    ws.send(stringify({
                        msg: "error",
                        err: "no stream found"
                    }));
                }
            })
        },
        data: {
            metal: undefined,
            nonMetal: undefined,
        },
        setting: {
            metal: {
                empty: 26.0,
                full: 4.5
            },
            nonMetal: {
                empty: 26.0,
                full: 4.5
            }
        },
        getData: async () => {
            const { setting, data } = device.roboBin;
            const out = { metal: undefined, nonMetal: undefined };

            // set up for empty data
            var dbRead = await db.read(db.sensor, { robot: "roboBin" })
            var { data: dbData } = dbRead;
            data.metal = dbData.metal; data.nonMetal = dbData.nonMetal;

            for (const [key, value] of Object.entries(setting)) {
                out[key] = map(data[key], value.empty, value.full, 0, 100);
            }
            return out;
        }
    }
};
// update device
function updateWs() {
    usersModule.update();
    for (const [key, value] of Object.entries(device)) {
        if (value.ws != undefined && (value.ws.readyState != WebSocket.OPEN && value.ws.readyState != WebSocket.CONNECTING)) {
            delete device[key].ws;
            device[key].ws = undefined;
        }
    }
}

const usersModule = {
    user: class {
        constructor(ws) {
            // const ws = WebSocket;

            this.ws = ws;
            this.id = ws.device;

            // modes code(switch)
            const mode = {
                currentMode: "init",
                script: {
                    sensorStream: () => {
                        ws.log("entering stream");
                        ws.send(stringify({
                            msg: "change mode response",
                            mode: "sensorStream"
                        }))
                    }
                },
            };
            this.mode = mode;
            //Switch the mode 
            ws.on('message', (msg) => {
                let res = objify(msg);
                ws.log('incoming: ' + msg);
                if (res.msg === "change mode") {
                    mode.currentMode = res.mode;
                    mode.script[res.mode]();
                    return;
                }
                if (res.msg === "device data req")
                    updateStream();

                else if (res.msg === 'security mode') {
                    const { smartHome } = device;
                    smartHome.data.security = res.mode;

                    updateDevice("smartHome", smartHome.data);

                    if (smartHome.ws === undefined) return;
                    smartHome.ws.send(stringify({
                        msg: 'security mode',
                        details: res.mode ? "0" : "1"
                    }))
                }
            })
        }
    },
    users: {},
    update: () => {
        for (const [key, value] of Object.entries(usersModule.users))
            if (value.ws.readyState != WebSocket.OPEN && value.ws.readyState != WebSocket.CONNECTING)
                delete usersModule.users[key];
    },
}

// depricated:
// // if all device is connected
// var alreadyActive = false;
// function activateSystem() {
//     if (alreadyActive) return;
//     for (const [key, value] of Object.entries(device))
//         if (value.ws === undefined)
//             return;
//     console.log('\nactivating the whole system:');
//     for (const [key, value] of Object.entries(device)) {
//         value.run(value.ws);
//         console.log(`${key} has been activated`);
//     }
//     console.log('all system activated\n');
//     alreadyActive = true;
// }


// for identify
wss.on('connection', (ws) => {
    updateWs();
    console.log(`connection attemp been made`);
    ws.send(stringify({ msg: "identity requested" }));

    // ws variables
    // identification needed before progressing
    ws.allowed = false;
    ws.allowStream = false;
    ws.device = "unknown";
    // send log
    ws.log = (logout) => { console.log(`${ws.device}: ${logout}`) };
    const { log } = ws;
    // code for starting connection and handle web client
    ws.on('message', (msg) => {
        // update stuffs
        updateWs();

        // turn it into json
        let res = objify(msg);

        // if its not json
        if (res === false) {
            if (!ws.allowStream) {
                ws.send(deserializeError);
                log(`not a valid json, input is: ${msg}`);
                return;
            }
            // stream code here
            return;
        }

        // client connect request
        if (res.msg === "connectClient") {
            if (alreadyConnected(ws)) return;
            ws.device = `client:${getUniqueID()}`
            usersModule.users[ws.device] = new usersModule.user(ws);
            ws.allowed = true;

            ws.send(stringify({
                msg: "connected",
                details: `connected sucsesfully as ${ws.device}`
            }))
            log('connected sucsesfully');
            return;
        }

        // identify request
        if (res.msg === 'identity') {
            // if already connected
            if (alreadyConnected(ws)) return;
            // check if device type legit
            if (!Object.keys(device).includes(res.device)) {
                ws.send(stringify({
                    msg: "error",
                    err: "access denied, device type not found"
                }));
                log('denied, device not found');
                return;
            }
            // check if other device already connected as ws.device
            if (device[res.device].ws !== undefined && res.device != 'user') {
                const { ws } = device[res.device];
                ws.send(stringify({
                    msg: "error",
                    err: "smart home connected as other device"
                }));
                ws.terminate();
            }
            // allow and connected
            ws.device = res.device;
            device[ws.device].ws = ws;
            ws.allowed = true;
            ws.send(stringify({
                msg: "connected",
                details: `connected sucsesfully as ${ws.device}`
            }))
            log('device connected');
            // run local script
            device[ws.device].run();
            return;
        }

        // codes below only accesseble when identified
        if (!ws.allowed) {
            // send error that the request didn't macht any code or keyword
            ws.send(stringify({
                msg: "error",
                err: "unknown request"
            }))
            log(`uknown request, request is: ${msg}`);
            return;
        }

    });
    ws.on('close', () => { updateWs(); ws.log('disconnected') });
})


// common function
// turn string into json SAFELY 
function objify(str) {
    try {
        return jsonify(String(str));
    } catch (e) {
        return false;
    }
}

// make id for websocket
function getUniqueID() {
    function s4() {
        return Math.floor((1 + Math.random()) * 0x10000).toString(16).substring(1);
    }
    return s4() + s4() + '-' + s4();
};

// check if device already connected
function alreadyConnected(ws) {
    if (ws.device != 'unknown') {
        ws.send(stringify({
            msg: "error",
            err: `acces denied, this device already connected as ${ws.device}`
        }));
        ws.log(`relogin attemp, denied`);
        return true;
    }
    return false;
}

// map data
const map = (x, inMin, inMax, outMin, outMax) => (x - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;

// read database
db.read = (database, query, multiple = false) => new Promise((res, rej) => {
    database.find(query, (err, doc) => {
        if (err) { rej(err); return };
        res(multiple ? doc : doc[0]);
    })
})


// init
app.use(express.static(`${__dirname}/public`));
server.listen(port, () => { console.log(`server is up on port ${port}!`); })
// request example:
/*

    {
        "msg": "connectClient"
    }
    
    {
        "msg":"change mode",
        "mode": "sensorStream"
    }

    {
        "msg":"security mode",
        "mode":true
    }



    {
        "msg":"identity",
        "device":"smartHome"
    }
    
    {
        "type": "stream",
        "stream": "sensors",
        "data": {
            "suhu": 27.79999924,
            "humidity": 37,
            "api":false,
            "orang":1,
            "gas": {
                "co": 0.004595334,
                "lpg": 0.008170504,
                "smoke": 0.009170504
            }
        }
    }
    
    {
        "type":"notify",
        "notify":"security"
    }



    {
        "msg":"identity",
        "device":"roboBin"
    }

    {
        "type": "stream",
        "stream": "sensors",
        "data": {
            "metal": 69,
            "nonMetal": 42
        }
    }

*/