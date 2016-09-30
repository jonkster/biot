#!/usr/bin/node

// this application acts as in interface between HTTP requests for data/control
// and a Biotz router (that communicates via UDP)
//
var VERSION = "0.0.2";

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
});

var biotzData = {};
var biotzCal = {};
var biotzStatus = {};
var nodeStatus = {};

var realNodes = {};
var dummyNodes = {};
var dummyTime = 0;

// received an update message - store info
brokerUdpListener.on('message', function (message, remote) {
    messagesFromRouter++;

    if (message.length > 0)
    {
        addNodeData(message.toString());
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

//brokerListener.put('/biotz/addnode/:address', addDummyNode);
//brokerListener.put('/biotz/dropnodes', dropDummyNodes);

brokerListener.get('/biotz/addresses/:address', getBiotFull);
brokerListener.get('/biotz/addresses/:address/data', getBiotData);
brokerListener.get('/biotz/addresses/:address/calibration', getBiotCalibration);
brokerListener.put('/biotz/addresses/:address/calibration', putBiotCalibration);
brokerListener.get('/biotz/addresses/:address/status', getBiotStatus);
brokerListener.get('/biotz/addresses/:address/interval', getBiotInterval);
brokerListener.put('/biotz/addresses/:address/interval', putBiotInterval);
brokerListener.get('/biotz/addresses/:address/auto', getBiotAuto);
brokerListener.put('/biotz/addresses/:address/auto', putBiotAuto);
brokerListener.get('/biotz/addresses/:address/dof', getBiotDof);
brokerListener.put('/biotz/addresses/:address/dof', putBiotDof);
brokerListener.get('/biotz/addresses/:address/led', getBiotLed);
brokerListener.put('/biotz/addresses/:address/led', putBiotLed);
brokerListener.get('/biotz/addresses/:address/alive', getBiotAlive);
brokerListener.get('/biotz/addresses/:address/alive/ts', getBiotAliveTs);
brokerListener.get('/biotz/addresses/:address/alive/status', getBiotAliveStatus);


brokerListener.get('/data', getData);
brokerListener.get('/data/:category', getDataCategories);
brokerListener.get('/data/:category/:name', getDataValue);
brokerListener.put('/data/:category/:name', putDataValue);
brokerListener.del('/data/:category/:name', deleteDataValue);



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


function addNodeData(message) {
    /*
     * cmd#data#addr
     */

    var bits = message.split('#');
    var address = bits[2];

    nodeStatus[address] = {
        'ts' : new Date(),
        'status' : 'active'
    }

    if (bits[0] == 'do') {
        biotzData[address] = bits[1];
        realNodes[address] = true;
    }
    else if (bits[0] == 'dc'){
        biotzCal[address] = bits[1];
    }
    else if (bits[0] == 'ds'){
        biotzStatus[address] = bits[1];
    }
    else{
        console.log("unknown message type", bits[0]);
    }
}



function biotIdentify(req, res, next) {
    var address = req.params['address'];
    var message = new Buffer('cled#3#' + address);
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
    var message = new Buffer('csyn##');
    var client = dgram.createSocket('udp6');

    client.send(message, 0, message.length, BIOTZ_UDP_PORT, BIOTZ_ROUTER_HOST, function(err, bytes) {
        if (err)
        {
            console.log('Error:', err);
        }
        client.close();
    });
    res.send(200, 'OK');
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
    /*
     * method: '/'
     */
    res.send({
        "title": "Biotz Broker REST API",
        "description": "interface to a network of Biot Orientation Sensors",
        "version": VERSION,
        "links": [
            "http://" + BROKER_HOST + ":" + BROKER_HTTP_PORT + "/",
            "http://" + BROKER_HOST + ":" + BROKER_HTTP_PORT + "/biotz",
            "http://" + BROKER_HOST + ":" + BROKER_HTTP_PORT + "/data"

        ]
    });
    next();
}

function getAllBiotData(req, res, next) {
    /*
     * method: '/biotz'
     */

    var addresses = Object.keys(biotzData);
    var value = {
        "count": addresses.length,
        "addresses": addresses
    }
    res.send(value);
    next();
}

function getBiotData(req, res, next) {

    var address = req.params['address'];
    var value = biotzData[address];

    res.send(200, value);
    next();
}

function getBiotCalibration(req, res, next) {

    var address = req.params['address'];

    var value = biotzCal[address];
    if (value === undefined) {
        res.send(404, value);
    } else {
        res.send(200, value);
    }
    next();
}

function getBiotCount(req, res, next) {

    var addresses = Object.keys(biotzData);
    res.send(200, addresses.length);
    next();
}

function getBiotFull(req, res, next) {

    var address = req.params['address'];
    if (! address) {
        // url entered with trailing slash
        getBiotz(req, res, next);
        return;
    }

    var bits = biotzStatus[address].split(":");

    var data = biotzData[address];
    var cal = biotzCal[address];
    var value = {
        'data': data,
        'calibration': cal,
        "status": biotzStatus[address],
        "interval": bits[1],
        "auto": bits[2],
        "dof": bits[0],
        "led": "?"
    }

    res.send(200, value);
    next();
}

function getBiotAuto(req, res, next) {

    var address = req.params['address'];
    if (! address) {
        // url entered with trailing slash
        getBiotz(req, res, next);
        return;
    }
    var bits = biotzStatus[address].split(":");

    var value = bits[2];

    res.send(200, value);
    next();
}

function getBiotDof(req, res, next) {

    var address = req.params['address'];
    if (! address) {
        // url entered with trailing slash
        getBiotz(req, res, next);
        return;
    }
    var bits = biotzStatus[address].split(":");

    var value = bits[0];

    res.send(200, value);
    next();
}

function getBiotLed(req, res, next) {

    var address = req.params['address'];
    if (! address) {
        // url entered with trailing slash
        getBiotz(req, res, next);
        return;
    }
    var bits = biotzStatus[address].split(":");
    // missing this info

    var value = "?";

    res.send(200, value);
    next();
}

function getBiotInterval(req, res, next) {

    var address = req.params['address'];
    if (! address) {
        // url entered with trailing slash
        getBiotz(req, res, next);
        return;
    }
    var bits = biotzStatus[address].split(":");

    var value = bits[1];

    res.send(200, value);
    next();
}

function getBiotStatus(req, res, next) {

    var address = req.params['address'];
    if (! address) {
        // url entered with trailing slash
        getBiotz(req, res, next);
        return;
    }

    var value = biotzStatus[address];

    res.send(200, value);
    next();
}

function getBiotz(req, res, next) {

    res.send(Object.keys(biotzData));
    next();
}

function getBiotAlive(req, res, next) {
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

function getBiotAliveStatus(req, res, next) {
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
        res.send(200, nodeStatus[address].status);
    }
    next();
}

function getBiotAliveTs(req, res, next) {
    var address = req.params['address'];
    if (nodeStatus[address] === undefined) {
        res.send(404, 'does not exist');
    }
    else {
        res.send(200, nodeStatus[address].ts);
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

function getDataCategories(req, res, next) {

    var category = req.params['category'];
    var path = dataPath + '/' + category + '/';

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

function getData(req, res, next) {

    var path = dataPath + '/';

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

function getDataValue(req, res, next) {
    var category = req.params['category'];
    var name = req.params['name'];
    var path = dataPath + '/' + category + '/' + name;

    var fs = require('fs');
    fs.readFile(path, function(err, data) {
        if (err) {
            res.send(404, err);
        } else {
            data = JSON.parse(data);
            res.send(200, data);
        }
        next();
    });
}

function deleteDataValue(req, res, next) {
    var category = req.context['category'];
    var name = req.context['name'];
    var path = dataPath + '/' + category + '/' + name;

    var fs = require('fs');
    fs.unlink(path, function(err) {
        if (err) {
            console.log(err, 'deleting file:', path);
            res.send(500, err);
        } else {
            console.log('deleted file:', path);
            res.send(200, "OK");
        }
    });
}


function getSystemMessageRate(req, res, next) {
    var now = new Date();
    var messageRate = messagesFromRouter/(now - startTime);
    res.send(200, messageRate);
    next();
}

function putBiotAuto(req, res, next) {

    var address = req.context['address'];
    var data = req.body;

    var message = new Buffer('cmcm#' + data + '#' + address);
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

function putBiotCalibration(req, res, next) {

    var address = req.context['address'];
    var data = req.body;

    var message = new Buffer('ccav#' + data + '#' + address);
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

function putBiotInterval(req, res, next) {

    var address = req.context['address'];
    var data = req.body;

    var message = new Buffer('cdup#' + data + '#' + address);
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

function putBiotDof(req, res, next) {

    var address = req.context['address'];
    var data = req.body;

    var message = new Buffer('cdof#' + data + '#' + address);
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

function putBiotLed(req, res, next) {

    var address = req.context['address'];
    var data = req.body;

    var message = new Buffer('cled#' + data + '#' + address);
    console.log("sending", message);
    var client = dgram.createSocket('udp6');

    client.send(message, 0, message.length, BIOTZ_UDP_PORT, BIOTZ_ROUTER_HOST, function(err, bytes) {
        if (err) {
            console.log('Error:', err);
            res.send(500, err);
        } else {
            console.log('put led', message);
            res.send(200, 'OK');
        }
        client.close();
        next();
    });
}


function putDataValue(req, res, next) {
    var category = req.context['category'];
    var name = req.context['name'];
    var data = req.body;
    var path = dataPath + '/' + category + '/';

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





