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
    var client = dgram.createSocket('udp6');

    client.send(message, 0, message.length, BIOTZ_UDP_PORT, BIOTZ_ROUTER_HOST, function(err, bytes) {
        if (err)
        {
            console.log('Error:', err);
        }
        client.close();
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
brokerUdpListener.on('message', function (message, remote) {
    if (message.length > 0)
    {
        biotzData = JSON.parse(message);
    }
});


// Listen for and act on Broker HTTP Requests
var restify = require('restify');

function getRoot(req, res, next) {
    res.setHeader('Content-Type', 'text/plain');
    res.send("Biotz Broker v0.0\n" + 
        "eg http://" + BROKER_HOST + ":" + BROKER_HTTP_PORT + "/\n" +
        "   http://" + BROKER_HOST + ":" + BROKER_HTTP_PORT + "/biotz\n" +
        "   http://" + BROKER_HOST + ":" + BROKER_HTTP_PORT + "/biotz/count\n" +
        "   http://" + BROKER_HOST + ":" + BROKER_HTTP_PORT + "/biotz/addresses\n" +
        "   http://" + BROKER_HOST + ":" + BROKER_HTTP_PORT + "/biotz/addresses/XXXXXX\n" +
        "   http://" + BROKER_HOST + ":" + BROKER_HTTP_PORT + "/biotz/addresses/XXXXXX/w\n" +
        "   http://" + BROKER_HOST + ":" + BROKER_HTTP_PORT + "/biotz/addresses/XXXXXX/x\n" +
        "   http://" + BROKER_HOST + ":" + BROKER_HTTP_PORT + "/biotz/addresses/XXXXXX/y\n" +
        "   http://" + BROKER_HOST + ":" + BROKER_HTTP_PORT + "/biotz/addresses/XXXXXX/z\n" 
    );
    next();
}

function getBiotData(req, res, next) {
    sendBiotzRouterMessage();
    res.header("Access-Control-Allow-Origin", "*"); 
    res.header("Access-Control-Allow-Headers", "X-Requested-With");
    res.setHeader('Content-Type', 'application/json');

    res.send(biotzData);
    next();
}

function getBiot(req, res, next) {

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

function getBiotQuality(req, res, next) {

    var address = req.params['address'];
    var quality = req.params['quality'];
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
brokerListener.get('/biotz', getBiotData);
brokerListener.get('/biotz/count', getBiotCount);
brokerListener.get('/biotz/addresses', getBiotz);
brokerListener.get('/biotz/addresses/:address', getBiot);
brokerListener.get('/biotz/addresses/:address/:quality', getBiotQuality);

brokerListener.listen(BROKER_HTTP_PORT, BROKER_HOST, function() {
    console.log('Broker %s listening for HTTP requests at port:%s', brokerListener.name, brokerListener.url);
    console.log('eg: http://%s:%s/biotz', brokerListener.name, brokerListener.url);
    brokerUdpListener.bind(BIOTZ_UDP_PORT, UDP_LOCAL_HOST);
});






