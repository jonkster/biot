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

var messagesFromRouter = 0;


var startTime = 0;

// Listen to Biotz Router device
var brokerUdpListener = dgram.createSocket('udp6');
brokerUdpListener.on('listening', function () {
    var address = brokerUdpListener.address();
    console.log('UDP Server listening on ' + address.address + ":" + address.port);
    startTime = new Date();
    sendBiotzRouterMessage();
});

var biotzData = {};
var biotzCal = {};
var nodeStatus = {};

var realNodes = {};
var dummyNodes = {};
var dummyTime = 0;

// received an update message - store info
brokerUdpListener.on('message', function (message, remote) {
    messagesFromRouter++;

    if (message.length > 0)
    {
        try {
            var jResponse = JSON.parse(message);
            addNodeData(jResponse);
        }
        catch(e) {
            console.log(e, "unrecognised broker message:", message.toString());
        }
    }
});

// Listen for and act on Broker HTTP Requests
var restify = require('restify');
var brokerListener = restify.createServer();
brokerListener.use(restify.bodyParser());

brokerListener.pre(function(req, res, next) {
    //console.log("REQ:", req.url);
    res.header("Access-Control-Allow-Origin", "*"); 
    res.header("Access-Control-Allow-Headers", "X-Requested-With");
    res.setHeader('Content-Type', 'application/json');
    next();
});
brokerListener.get('/', getRoot);

brokerListener.get('/biotz', getAllBiotData);
brokerListener.get('/biotz/count', getBiotCount);
brokerListener.get('/biotz/status', getBiotzStatus);
brokerListener.get('/biotz/synchronise', biotSync);
brokerListener.get('/biotz/addresses', getBiotz);
brokerListener.put('/biotz/addnode/:address', addDummyNode);
brokerListener.put('/biotz/dropnodes', dropDummyNodes);

brokerListener.get('/biotz/addresses/:address', getBiotFull);
brokerListener.get('/biotz/addresses/:address/data', getBiotData);
brokerListener.get('/biotz/addresses/:address/identify', biotIdentify);
brokerListener.get('/biotz/addresses/:address/calibration', getBiotCalibration);
brokerListener.get('/biotz/addresses/:address/sensors', getBiotSensors);
brokerListener.get('/biotz/addresses/:address/status', getBiotStatus);
brokerListener.get('/biotz/addresses/:address/:quality', getBiotQuality);

brokerListener.put('/biotz/addresses/:address/calibration/:data', putBiotCalibration);
brokerListener.put('/biotz/addresses/:address/sensors/:data', putBiotSensors);

brokerListener.get('/data/assembly', getCachedAssemblies);
brokerListener.get('/data/assembly/:name', getCachedAssembly);
brokerListener.post('/data/assembly/:name', postCachedAssembly);

brokerListener.get('/data/addresses', getCachedAddresses);
brokerListener.get('/data/addresses/:address/calibration', getCachedCalibration);

brokerListener.put('/data/addresses/:address/calibration/:data', putCachedCalibration);

brokerListener.get('/system/mrate', getSystemMessageRate);


brokerListener.listen(BROKER_HTTP_PORT, BROKER_HOST, function() {
    console.log('Broker %s listening for HTTP requests at port:%s', brokerListener.name, brokerListener.url);
    console.log('eg: http://%s:%s/biotz', brokerListener.name, brokerListener.url);
    brokerUdpListener.bind(BIOTZ_UDP_PORT, UDP_LOCAL_HOST);
});

function addDummyNode(req, res, next) {
    var address = req.params['address'];
    nodeStatus[address] = {
        'status': 'dummy',
        'ts': 0
    }
    dummyNodes[address] = true;
    console.log("adding dummy node:", address);
    res.send('OK');
    next();
}


function addNodeData(jResponse) {
    /* expecting jResponse to be in form:
     * json: { t: 'dat',
         *   s: 'affe::585a:6b64:95b5:846',
         *   v: '190150:0.828802:-0.104520:-0.535533:-0.123965' }
     */
    var address = jResponse.s;

    nodeStatus[address] = {
        'ts' : new Date(),
        'status' : 'active'
    }

    if (jResponse['t'] == 'dat') {
        biotzData[address] = jResponse.v;
        realNodes[address] = true;
    }
    else if (jResponse['t'] == 'cal'){
        biotzCal[address] = jResponse.v;
    }
    else if (jResponse['t'] == 'getc'){
        console.log("calibration cache of:", address);
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

function dropDummyNodes(req, res, next) {
    var dAddresses = Object.keys(dummyNodes);
    for (var i = 0; i < dAddresses.length; i++) {
        var address = dAddresses[i];
        delete biotzData[address];
        delete biotzCal[address];
        delete nodeStatus[address];
        delete dummyNodes[address];
    }
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

    var dAddresses = Object.keys(dummyNodes);
    for (var i = 0; i < dAddresses.length; i++) {
        var address = dAddresses[i];
        if (realNodes[address]) {
            // a real node with this address exists - drop dummy node in that
            // case and use the real one.
            delete dummyNodes[address];
            break;
        }
        if (biotzData[address] == undefined) {
            var w = 0.793339;
            var x = 0.540891;
            var y = 0.167431;
            var z = 0.223644;
            biotzData[address] = 0 + ":" + w + ":" + x + ":" + y + ":" + z;
        }
        var q = biotzData[address].split(':');
        var t = parseInt(q[0]);
        var w = parseFloat(q[1]);
        var x = parseFloat(q[2]);
        var y = parseFloat(q[3]);
        var z = parseFloat(q[4]);
        w += 0.1 * (0.5 - Math.random());
        var norm = Math.sqrt(w * w + x * x + y * y + z * z);
        w = w/norm;
        x = x/norm;
        y = y/norm;
        z = z/norm;
        t++;

        nodeStatus[address] = {
            'status' : 'dummy',
            'ts': t
        }

        biotzData[address] = t + ":" + w + ":" + x + ":" + y + ":" + z;
        biotzCal[address] = "-360:-350:-728:394:461:56";
        var nodeValue = biotzData[address];
        nodes.push({
            "a" : address,
            "v" : nodeValue
        });
        c++;
        messagesFromRouter++;
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
    res.send(value);
    next();
}

function getBiotCalibration(req, res, next) {

    var address = req.params['address'];

    sendBiotzRouterMessage();
    var value = biotzCal[address];
    if (value === undefined) {
        res.send(404, value);
    } else {
        res.send(200, value);
    }
    next();
}

function getBiotCount(req, res, next) {

    sendBiotzRouterMessage();
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
    res.send(value);
    next();
}

function getBiotSensors(req, res, next) {

    var address = req.params['address'];

    res.send(501, 'not implemented yet');
    next();
}


function getBiotz(req, res, next) {

    sendBiotzRouterMessage();
    res.send(Object.keys(biotzData));
    next();
}

function getBiotStatus(req, res, next) {
    var address = req.params['address'];
    var now = new Date();
    if (nodeStatus[address].status !== 'dummy') {
        if (nodeStatus[address] !== undefined) {

            var timeDiff = (now - nodeStatus[address].ts) / 1000; // as seconds

                if (timeDiff > 20) {
                    nodeStatus[address] = undefined;
                } else if (timeDiff > 10) {
                    nodeStatus[address].status = 'lost';
                } else if (timeDiff > 5) {
                    nodeStatus[address].status = 'inactive';
                } else {
                    nodeStatus[address].status = 'active';
                }
        }
    }
    if (nodeStatus[address] === undefined) {
        biotzData[address] = undefined;
    }

    if (nodeStatus[address] === undefined) {
        res.send(404, 'does not exist');
    }
    else {
        res.send(200, nodeStatus[address]);
    }
    next();
}

function getBiotzStatus(req, res, next) {
    var addresses = Object.keys(nodeStatus);
    var now = new Date();
    for (var i = 0; i < addresses.length; i++) {
        var address = addresses[i];
        var timeDiff = (now - nodeStatus[address].ts) / 1000; // as seconds

        if (timeDiff > 20) {
            nodeStatus[address] = undefined;
        } else if (timeDiff > 10) {
            nodeStatus[address].status = 'lost';
        } else if (timeDiff > 5) {
            nodeStatus[address].status = 'inactive';
        } else {
            nodeStatus[address].status = 'active';
        }
    }
    res.send(nodeStatus);
    next();
}

function getCachedAddresses(req, res, next) {

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

function getCachedAssemblies(req, res, next) {

    var path = dataPath + '/assembly/';

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

function getCachedAssembly(req, res, next) {
    var name = req.params['name'];
    var path = dataPath + '/assembly/' + name;

    var fs = require('fs');
    fs.readFile(path, function(err, data) {
        data = JSON.parse(data);
        if (err) {
            console.log(err, 'reading file:', path);
            res.send(404, data);
        } else {
            res.send(200, data);
        }
        next();
    });
}

function getCachedCalibration(req, res, next) {
    var address = req.params['address'];
    var path = dataPath + '/addresses/' + address + '/calibration.json';

    var fs = require('fs');
    fs.readFile(path, function(err, data) {
        data = JSON.parse(data);
        if (err) {
            console.log(err, 'reading file:', path);
            res.send(404, data);
        } else {
            res.send(200, data);
        }
        next();
    });
}

function getSystemMessageRate(req, res, next) {
    var now = new Date();
    var messageRate = messagesFromRouter/(now - startTime);
    res.send(200, messageRate);
    next();
}

function putBiotCalibration(req, res, next) {

    var address = req.params['address'];
    var data = req.params['data'];

    var message = new Buffer('set-cal:' + data + '#' + address);
    var client = dgram.createSocket('udp6');

    client.send(message, 0, message.length, BIOTZ_UDP_PORT, BIOTZ_ROUTER_HOST, function(err, bytes) {
        console.log('put cal', address, data, err);
        if (err) {
            console.log('Error:', err);
            res.send(500, err);
        } else {
            res.send(200, 'OK');
        }
        client.close();
        next();
    });
}

function putBiotSensors(req, res, next) {

    var address = req.params['address'];
    var data = req.params['data'];

    var message = new Buffer('set-sens:' + data + '#' + address);
    var client = dgram.createSocket('udp6');

    client.send(message, 0, message.length, BIOTZ_UDP_PORT, BIOTZ_ROUTER_HOST, function(err, bytes) {
        if (err) {
            console.log('Error:', err);
            res.send(500, err);
        } else {
            res.send(200, 'OK');
        }
        client.close();
        next();
    });
}

function postCachedAssembly(req, res, next) {
    var name = req.params['name'];
    var data = req.body;
    var path = dataPath + '/assembly/';


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
    path += '/' + name;
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

function putCachedCalibration(req, res, next) {
    var address = req.params['address'];
    var data = req.params['data'];
    var path = dataPath + '/addresses/' + address;


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


var xx = 0;
// send message to Biotz Router device
function sendBiotzRouterMessage() {
    if (xx++ % 100 == 0) {
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
}


