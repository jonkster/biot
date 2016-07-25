#!/usr/bin/node

// this application acts as in interface between HTTP requests for data/control
// and a Biotz router (that communicates via UDP)

var BIOTZ_UDP_PORT = 8888;
var UDP_LOCAL_HOST = 'affe::1';
var BIOTZ_ROUTER_HOST = 'affe::3';

var BROKER_HTTP_PORT = 8889; 
var BROKER_HOST = 'localhost'; 

var dgram = require('dgram');

var dataPath = './data';


// Listen to Biotz Router device
var brokerUdpListener = dgram.createSocket('udp6');
brokerUdpListener.on('listening', function () {
    var address = brokerUdpListener.address();
    console.log('UDP Server listening on ' + address.address + ":" + address.port);
    sendBiotzRouterMessage();
});

var biotzData = {};
var biotzCal = {};
// received an update message - store info
brokerUdpListener.on('message', function (message, remote) {
    if (message.length > 0)
    {
        try {
            var jResponse = JSON.parse(message);
            //console.log('json:', jResponse);
            addNodeData(jResponse);
        }
        catch(e) {
            console.log(e, "unrecognised broker message:", message);
        }
    }
});

// Listen for and act on Broker HTTP Requests
var restify = require('restify');
var brokerListener = restify.createServer();
brokerListener.get('/', getRoot);
brokerListener.get('/biotz', getAllBiotData);
brokerListener.get('/biotz/count', getBiotCount);
brokerListener.get('/biotz/synchronise', biotSync);
brokerListener.get('/biotz/addresses', getBiotz);
brokerListener.get('/biotz/addresses/:address', getBiotFull);
brokerListener.get('/biotz/addresses/:address', getBiotFull);
brokerListener.get('/biotz/addresses/:address/data', getBiotData);
brokerListener.get('/biotz/addresses/:address/identify', biotIdentify);
brokerListener.get('/biotz/addresses/:address/calibration', getBiotCalibration);
brokerListener.get('/biotz/addresses/:address/:quality', getBiotQuality);
brokerListener.put('/biotz/addresses/:address/calibration', putBiotCalibration);

brokerListener.get('/data/addresses', getCachedAddresses);
brokerListener.get('/data/addresses/:address/calibration', getCachedCalibration);
brokerListener.put('/data/addresses/:address/calibration/:data', putCachedCalibration);

brokerListener.listen(BROKER_HTTP_PORT, BROKER_HOST, function() {
    console.log('Broker %s listening for HTTP requests at port:%s', brokerListener.name, brokerListener.url);
    console.log('eg: http://%s:%s/biotz', brokerListener.name, brokerListener.url);
    brokerUdpListener.bind(BIOTZ_UDP_PORT, UDP_LOCAL_HOST);
});


function addNodeData(jResponse) {
    /* expecting jResponse to be in form:
     * json: { t: 'dat',
         *   s: 'affe::585a:6b64:95b5:846',
         *   v: '190150:0.828802:-0.104520:-0.535533:-0.123965' }
     */
    if (jResponse['t'] == 'dat') {
        biotzData[jResponse.s] = jResponse.v;
    }
    else if (jResponse['t'] == 'cal'){
        biotzCal[jResponse.s] = jResponse.v;
    }
    else{
        console.log("unknown response type", jResponse['t']);
    }
}



function biotIdentify(req, res, next) {
    var address = req.params['address'];
    var message = new Buffer('nudge:' + address);
    var client = dgram.createSocket('udp6');

    client.send(message, 0, message.length, BIOTZ_UDP_PORT, BIOTZ_ROUTER_HOST, function(err, bytes) {
        if (err)
        {
            console.log('Error:', err);
        }
        client.close();
    });
    res.send('OK');
    next();
}

function biotSync(req, res, next) {
    var message = new Buffer('sync');
    var client = dgram.createSocket('udp6');

    client.send(message, 0, message.length, BIOTZ_UDP_PORT, BIOTZ_ROUTER_HOST, function(err, bytes) {
        if (err)
        {
            console.log('Error:', err);
        }
        client.close();
    });
    res.send('OK');
    next();
}

function getRoot(req, res, next) {
    res.setHeader('Content-Type', 'text/plain');
    res.send("Biotz Broker v0.0\n" + 
        "eg http://" + BROKER_HOST + ":" + BROKER_HTTP_PORT + "/\n" +
        "   http://" + BROKER_HOST + ":" + BROKER_HTTP_PORT + "/biotz\n" +
        "   http://" + BROKER_HOST + ":" + BROKER_HTTP_PORT + "/biotz/count\n" +
        "   http://" + BROKER_HOST + ":" + BROKER_HTTP_PORT + "/biotz/synchronise\n" +
        "   http://" + BROKER_HOST + ":" + BROKER_HTTP_PORT + "/biotz/addresses\n" +
        "   http://" + BROKER_HOST + ":" + BROKER_HTTP_PORT + "/biotz/addresses/XXXXXX\n" +
        "   http://" + BROKER_HOST + ":" + BROKER_HTTP_PORT + "/biotz/addresses/XXXXXX/calibration\n" +
        "   http://" + BROKER_HOST + ":" + BROKER_HTTP_PORT + "/biotz/addresses/XXXXXX/identify\n" +
        "   http://" + BROKER_HOST + ":" + BROKER_HTTP_PORT + "/biotz/addresses/XXXXXX/data\n" +
        "   http://" + BROKER_HOST + ":" + BROKER_HTTP_PORT + "/biotz/addresses/XXXXXX/w\n" +
        "   http://" + BROKER_HOST + ":" + BROKER_HTTP_PORT + "/biotz/addresses/XXXXXX/x\n" +
        "   http://" + BROKER_HOST + ":" + BROKER_HTTP_PORT + "/biotz/addresses/XXXXXX/y\n" +
        "   http://" + BROKER_HOST + ":" + BROKER_HTTP_PORT + "/biotz/addresses/XXXXXX/z\n" 
    );
    next();
}

function getAllBiotData(req, res, next) {
    sendBiotzRouterMessage();
    res.header("Access-Control-Allow-Origin", "*"); 
    res.header("Access-Control-Allow-Headers", "X-Requested-With");
    res.setHeader('Content-Type', 'application/json');

    var addresses = Object.keys(biotzData);
    var c =addresses.length;

    var nodes = [];
    for (var i = 0; i < c; i++) {
        var address = addresses[i];
        var nodeValue = biotzData[address];
        nodes.push({
            "a" : address,
            "v" : nodeValue
        });
    }

    var value = {
        "c": c,
        "n" : nodes
    }

    res.send(value);
    next();
}

function getBiotData(req, res, next) {

    var address = req.params['address'];
    var value = biotzData[address];

    // trigger update request for next time
    sendBiotzRouterMessage();
    res.header("Access-Control-Allow-Origin", "*"); 
    res.header("Access-Control-Allow-Headers", "X-Requested-With");
    res.setHeader('Content-Type', 'application/json');
    res.send(value);
    next();
}

function getBiotCalibration(req, res, next) {

    var address = req.params['address'];

    sendBiotzRouterMessage();
    res.header("Access-Control-Allow-Origin", "*"); 
    res.header("Access-Control-Allow-Headers", "X-Requested-With");
    res.setHeader('Content-Type', 'application/json');
    var value = biotzCal[address];
    res.send(value);
    next();
}

function getBiotCount(req, res, next) {

    sendBiotzRouterMessage();
    res.header("Access-Control-Allow-Origin", "*"); 
    res.header("Access-Control-Allow-Headers", "X-Requested-With");
    res.setHeader('Content-Type', 'application/json');
    var addresses = Object.keys(biotzData);
    res.send(addresses.length);
    next();
}

function getBiotFull(req, res, next) {

    var address = req.params['address'];
    if (! address) {
        // url entered with trailing slash
        getBiotz(req, res, next);
        return;
    }

    var data = biotzData[address];
    var cal = biotzCal[address];
    var value = {
        'data': data,
        'calibration': cal,
    }


    // trigger update request for next time
    sendBiotzRouterMessage();
    res.header("Access-Control-Allow-Origin", "*"); 
    res.header("Access-Control-Allow-Headers", "X-Requested-With");
    res.setHeader('Content-Type', 'application/json');
    res.send(value);
    next();
}


function getBiotQuality(req, res, next) {

    var address = req.params['address'];
    var quality = req.params['quality'];
    if (! quality) {
        // url entered with trailing slash
        getBiotFull(req, res, next);
        return;
    }

    var nodeData = biotzData[address];
    var value = "";
    var vs = nodeData.split(':');
    switch(quality) {
        case 'time': value = vs[0];
            break;
        case 'w': value = vs[1];
            break;
        case 'x': value = vs[2];
            break;
        case 'y': value = vs[3];
            break;
        case 'z': value = vs[4];
            break;
    }
    sendBiotzRouterMessage();
    res.header("Access-Control-Allow-Origin", "*"); 
    res.header("Access-Control-Allow-Headers", "X-Requested-With");
    res.setHeader('Content-Type', 'application/json');
    res.send(value);
    next();
}

function getBiotz(req, res, next) {

    sendBiotzRouterMessage();
    res.header("Access-Control-Allow-Origin", "*"); 
    res.header("Access-Control-Allow-Headers", "X-Requested-With");
    res.setHeader('Content-Type', 'application/json');
    res.send(Object.keys(biotzData));
    next();
}

function getCachedAddresses(req, res, next) {

    res.header("Access-Control-Allow-Origin", "*"); 
    res.header("Access-Control-Allow-Headers", "X-Requested-With");
    res.setHeader('Content-Type', 'application/json');
    var path = dataPath + '/addresses/';

    var fs = require('fs');
    fs.readdir(path, function(err, files) {
        if (err) {
            res.send(500, 'fail');
        } else {
            res.send(200, files);
        }
        next();
    });
}

function getCachedCalibration(req, res, next) {
    var address = req.params['address'];
    var path = dataPath + '/addresses/' + address + '/calibration.json';
    console.log(path);

    var fs = require('fs');
    fs.readFile(path, function(err, data) {
        data = JSON.parse(data);
        console.log(data);
        res.header("Access-Control-Allow-Origin", "*"); 
        res.header("Access-Control-Allow-Headers", "X-Requested-With");
        res.setHeader('Content-Type', 'application/json');
        if (err) {
            console.log(err, 'reading file:', path);
            res.send(404, data);
        } else {
            res.send(200, data);
        }
        next();
    });
}

function putBiotCalibration(req, res, next) {
    console.log("cannot send calibrations to biot node yet!");
}

function putCachedCalibration(req, res, next) {
    var address = req.params['address'];
    var data = req.params['data'];
    var path = dataPath + '/addresses/' + address;

    res.header("Access-Control-Allow-Origin", "*"); 
    res.header("Access-Control-Allow-Headers", "X-Requested-With");
    res.setHeader('Content-Type', 'application/json');

    var fs = require('fs');

    if (! fs.existsSync(path)) {
        var mkdirp = require('mkdirp');
        var dir = mkdirp.sync(path);
        if (! dir) {
            res.send(500, 'failed to create resource');
            next();
            return;
        }
    }
    path += '/calibration.json';
    fs.writeFile(path, JSON.stringify(data), function(err) {
        if (err) {
            res.send(500, 'OK');
            console.log(fErr, 'writing file:', path);
        } else {
            res.send('OK');
        }
        next();
    });

}


// send message to Biotz Router device
function sendBiotzRouterMessage() {
    var message = new Buffer('get-data');
    var client1 = dgram.createSocket('udp6');

    // ask for update on node data knowledge
    client1.send(message, 0, message.length, BIOTZ_UDP_PORT, BIOTZ_ROUTER_HOST, function(err, bytes) {
        if (err)
        {
            console.log('Error:', err);
        }
        client1.close();
    });

    // ask for update on node calibration knowledge
    message = new Buffer('get-cal');
    client2 = dgram.createSocket('udp6');
    client2.send(message, 0, message.length, BIOTZ_UDP_PORT, BIOTZ_ROUTER_HOST, function(err, bytes) {
        if (err)
        {
            console.log('Error:', err);
        }
        client2.close();
    });
}


