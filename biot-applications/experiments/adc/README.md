*This is a very experimental setup.  Code is ugly.*

# Summary

Rough code for 6lowPAN nodes.

Uses RIOT-OS

Nodes are (currently) Atmel ATSAMR21ZLL-EK boards.

Nodes return (via coap) sensor data (currently reports ADC value).

After flashing, need to use RIOT shell on nodes to set up the system (see
below).

test from PC using:
    coap-client "coap://[affe::2]/adc/voltage"
    (or any ipv6 address of the participating nodes)
    

currently includes RIOT forked with hacks so it can use the samr21zll-ek board
and provide ADC capability for samd21 cpu.


# Environment:

A DODAG of nodes that can talk to each other.  The root of the DODAG
is also to be an edge router than can talk to a standard IPv6 network and acts
as a bridge to the 6LowPAN network, allowing communication between the outside
and any node in the DODAG.

# Assumptions:

1. we have 2 or more 6LowPAN nodes running the biot-node RIOT-OS application

2. we have a PC that can run a SLIP network interface

3. One of the nodes has a SLIP network interface that is wired to the PC (eg
    using FTDI port on the node and USB on the PC).  This will be the root
    node.

## Suppose:

We want the PC to have an IP address of affe::1 and the 6lowPAN mesh to have
addresses in the affe:: range

The root node will be given an address affe::2 for the 6lowPAN interface.

The root node wired interface will be given an address affe::3.

Assume each node has a 6lowPAN network interface #7.  The root node wired
interface reports as #8. (Other nodes may also have a #8 interface but this is
not used here).

Assume the PC FTDI device connects to /dev/ttyUSB0

```
+------------------+---------------------------------------------------+
| Outside World    |                  DODAG                            |
|     PC           |     Root Node                  Other Nodes        |
|                  |                                                   |
| tun0 <-----SLIP--|---> #8 <--> #7 <------6lowPAN-----> #7 <--etc---> |
| affe::1          |   affe::3  affe::2                affe:*          |
|                  |                                                   |
+------------------+---------------------------------------------------+
```

The nodes should 'auto' setup on power up except for the root node which
requires its SW0 switch to be pressed.  When pressed, all powered nodes in
range will join the DODAG with the selected node as the root.

# Setup steps:

*NB some of these steps should be done by the node code itself:
step 1.i by every node on startup, steps 1.ii, 1.iii, 2.i and 2.ii after the
node SW0 switch is pressed.  This means all that is required is for the nodes
to be powered, the edge node to have its button pressed and the SLIP interface
(step 3) to be made manually.*



1. Set up rpl network interfaces
  1. for every node (including root), join the wireless interface as a member of the DODAG:
    ```
        rpl init 7
    ```
  2. give the root node's wireless interface an address (NB only the root node):
    ```
        ifconfig 7 add affe::2
    ```
  3. on the root node, set it as the DODAG root
    ```
        rpl root 1 affe::2
    ```
  4. after this, check the root node 6lowPAN interface now has an address of
       affe::2 and that every other node now has an address in the affe:: range.  

2. Configure edge router setup
  1. set the root node set the *wired* interface address:
    ```
        ifconfig 8 add affe::3
    ```
  2. from the root node, inform the neighbourhood cache to know about the PC
    ```
        ncache add 8 affe::1
    ```

3. Configure the SLIP network interface on the PC
    ```
    sudo ./tunslip6 affe::1/64 -t tun0 -s /dev/ttyUSB0 -B115200 &
    ```


4. Tests:
  1. On a non root node, get its address (eg might be affe::5847:2c7d:4b62:c2a)
  2. on root node:
    ```
    ping6 affe::1
    ping6 affe::5847:2c7d:4b62:c2a (or other non root node address)
    ```
  3. on non root
    ```
    ping6 affe::1
    ping6 affe::2
    ping6 affe::3
    ```
  4. on PC
    ```
    ping6 affe::2
    ping6 affe::3
    ping6 affe::5847:2c7d:4b62:c2a (or other non root node address)
    ```