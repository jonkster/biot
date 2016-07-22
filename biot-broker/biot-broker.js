#!/usr/bin/node

// this application acts as in interface between HTTP requests for data/control
// and a Biotz router (that communicates via UDP)

var BIOTZ_UDP_PORT = 8888;
var UDP_LOCAL_HOST = 'affe::1';
var BIOTZ_ROUTER_HOST = 'affe::3';

var BROKER_HTTP_PORT = 8889; 
var BROKER_HOST = 'localhost'; 

var dgram = require('dgram');

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
        var jResponse = JSON.parse(message);
        if (jResponse['t'] == 'dat')
            biotzData = jResponse;
        else if (jResponse['t'] == 'cal')
            biotzCal = jResponse;
        else
            console.log("unknown response type");
    }
});


// Listen for and act on Broker HTTP Requests
var restify = require('restify');

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

    res.send(biotzData);
    next();
}

function getBiotData(req, res, next) {

    var address = req.params['address'];
    var nodes = biotzData.n;
    var value = "";
    for (var i = 0; i < nodes.length; i++) {
        var node = nodes[i];
        if (node.a === address) {
            value = node.v;
            break;
        }
    }
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
    var nodes = biotzCal.n;
    var value = "";
    for (var i = 0; i < nodes.length; i++) {
        var node = nodes[i];
        if (node.a === address) {
            value = node.v;
            break;
        }
    }
    // trigger update request for next time
    sendBiotzRouterMessage();
    res.header("Access-Control-Allow-Origin", "*"); 
    res.header("Access-Control-Allow-Headers", "X-Requested-With");
    res.setHeader('Content-Type', 'application/json');
    res.send(value);
    next();
}

function getBiotCount(req, res, next) {

    var addresses = [];
    res.header("Access-Control-Allow-Origin", "*"); 
    res.header("Access-Control-Allow-Headers", "X-Requested-With");
    res.setHeader('Content-Type', 'application/json');
    res.send(biotzData.c);
    sendBiotzRouterMessage();
    next();
}

function getBiotFull(req, res, next) {

    var address = req.params['address'];
    if (! address) {
        // url entered with trailing slash
        getBiotz(req, res, next);
        return;
    }
    var data = "";
    var nodes = biotzData.n;
    for (var i = 0; i < nodes.length; i++) {
        var node = nodes[i];
        if (node.a === address) {
            data = node.v;
            break;
        }
    }

    var cal = "";
    var nodes = biotzCal.n;
    for (var i = 0; i < nodes.length; i++) {
        var node = nodes[i];
        if (node.a === address) {
            cal = node.v;
            break;
        }
    }
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
    var nodes = biotzData.n;
    var value = "";
    for (var i = 0; i < nodes.length; i++) {
        var node = nodes[i];
        if (node.a === address) {
            var vs = node.v.split(':');
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
            break;
        }
    }
    sendBiotzRouterMessage();
    res.header("Access-Control-Allow-Origin", "*"); 
    res.header("Access-Control-Allow-Headers", "X-Requested-With");
    res.setHeader('Content-Type', 'application/json');
    res.send(value);
    next();
}

function getBiotz(req, res, next) {

    var addresses = [];
    if (biotzData.c > 0)
    {
        var nodes = biotzData.n;
        for (var i = 0; i < nodes.length; i++) {
            var node = nodes[i];
            addresses.push(node.a);
        }
    }
    sendBiotzRouterMessage();
    res.header("Access-Control-Allow-Origin", "*"); 
    res.header("Access-Control-Allow-Headers", "X-Requested-With");
    res.setHeader('Content-Type', 'application/json');
    res.send(addresses);
    next();
}

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

brokerListener.listen(BROKER_HTTP_PORT, BROKER_HOST, function() {
    console.log('Broker %s listening for HTTP requests at port:%s', brokerListener.name, brokerListener.url);
    console.log('eg: http://%s:%s/biotz', brokerListener.name, brokerListener.url);
    brokerUdpListener.bind(BIOTZ_UDP_PORT, UDP_LOCAL_HOST);
});






