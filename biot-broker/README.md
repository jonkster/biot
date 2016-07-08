#Biot Broker

This application is an HTTP REST web service providing resources that reflect
the state of a Biotz Network.

The service listens for HTTP resource requests and returns what information it
currently knows about the Biot network.

(It gathers the data on the Biot network using UDP communication with the Biot
Edge Router).

Example resources it can provide are:
```
http://localhost:8889/
http://localhost:8889/biotz
http://localhost:8889/biotz/count
http://localhost:8889/biotz/addresses
http://localhost:8889/biotz/addresses/XXXXXX
http://localhost:8889/biotz/addresses/XXXXXX/w
http://localhost:8889/biotz/addresses/XXXXXX/x
http://localhost:8889/biotz/addresses/XXXXXX/y
http://localhost:8889/biotz/addresses/XXXXXX/z
```
Where XXXXXX = the IPv6 address of a participating Biot node.
Participating addresses can be obtained from the:
```
    http://localhost:8889/biotz/addresses
```
resource.

##Using

To Install:
```
npm install

```

You then need to configure the parameters in the broker script to reflect the
location of the Biot router and make sure a suitable network interface is
available to communicate with it.  In the experimental setup this is currently
a SLIP connection to the edge router that needs to be set up using the script
in the biot project utils directory.

To Run:
Once the connection exists:

```
npm start

```


