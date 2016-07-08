Experimental code for a sensor network using ATMEL atsamr21g microcontrollers.

#About Biotz

Biotz nodes are small devices that gather and relay data about where they
are physically located (eg their orientation in space, current movement etc).
A group of Biotz nodes can be joined into a network and can provide information
about complex movements of an assembly of parts.

For example, a human body may be
considered as an assembly of various limbs. If a number of Biotz nodes are
attached to locations around the body, the movement of the limbs can be
monitored or recorded.

The prototype Biotz nodes are currently implemented using ATSAMR21ZLL-EK and ATSAMR21-XPRO boards, if the system appears viable the plan being to then build purpose designed Biotz devices using ATMEL atsamr21ga microcontrollers.

The nodes run a forked version of RIOT OS (see https://github.com/RIOT-OS/RIOT for the 'real' RIOT not my hacked up one). 
My forked version is at https://github.com/jonkster/RIOT and is forked to allow me to support the (currently) unsupported ATSAMR21ZLL-EK boards and also some customised bits I have added to the ATSAMR21-XPRO boards.

They communicate using 6lowPAN with an edge router (currently an ATSAMR21-XPRO board with a SLIP network interface to the wider network).  The edge router is the root of a number of Biotz sensor devices.

This stuff is very experimental and a work in progress.  Code quality (my code anyway) ranges from crap to even worse.  You have been warned. :)

Current Structure
```
├── biot-applications
│   ├── biot - the sensor node code for the ZLL-EK and XPRO boards
│   ├── router - the edge router code
│   ├── modules - components common to the router and sensor nodes
│   └── utils - some stuff to assist flashing and working with the boards (cruddy but handy)
├── biot-broker - a nodejs based web service run on the PC that provides information on the Biotz network to other applications
└── biot-interface - a browser based user interface to the Biotz network



