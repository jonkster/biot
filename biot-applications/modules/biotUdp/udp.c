#include <inttypes.h>
#include <xtimer.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "net/af.h"
#include "net/conn/udp.h"
#include "net/ipv6.h"

#include <unistd.h>
#include "msg.h"
#include "board.h"
#include "reboot.h"
#include "../identify/biotIdentify.h"
#include "../imu/imu.h"

#include "udp.h"

#define XSTR(x) STR(x)
#define STR(x) #x


#define MUDP_Q_SZ           (64)
#define SERVER_BUFFER_SIZE  (256)
#define UDP_PORT            (8888)

extern uint32_t getCurrentTime(void);
extern void setCurrentTime(uint32_t t);

static int serverSocket = -1;
struct sockaddr_in6 serverSocketAddr;

// define some variables globally here to keep them off the stack
static char serverBuffer[SERVER_BUFFER_SIZE];
static char gpTextBuf[30];
static msg_t msg_q[MUDP_Q_SZ];
static char json[MAX_MESSAGE_LENGTH];
static char srcAdd[IPV6_ADDR_MAX_STR_LEN];
static char selfAdd[IPV6_ADDR_MAX_STR_LEN];
struct in6_addr srcSocketAddr; 

uint32_t messagesRelayed = 0;
uint32_t startTime = 0;

#ifdef MAX_NODES
    char nodeData[MAX_NODES][IPV6_ADDR_MAX_STR_LEN];
#endif

char dataDestAdd[IPV6_ADDR_MAX_STR_LEN];

bool relayDataToBroker(char * destAdd, char *srcAdd, char *type, char *val);

bool setupUdpServer(void)
{
    printf("starting udp server...");
    if (serverSocket >= 0)
    {
        puts("closing open socket");
        close(serverSocket);
    }
    serverSocket = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);

    serverSocketAddr.sin6_family = AF_INET6;
    memset(&serverSocketAddr.sin6_addr, 0, sizeof(serverSocketAddr.sin6_addr));
    serverSocketAddr.sin6_port = htons(UDP_PORT);

    if (serverSocket < 0)
    {
        puts("error initializing socket");
        serverSocket = -1;
        return false;
    }

    if (bind(serverSocket, (struct sockaddr *)&serverSocketAddr, sizeof(serverSocketAddr)) < 0)
    {
        serverSocket = -1;
        puts("error binding socket");
        return false;
    }

    strcpy(dataDestAdd, "");

    puts(" OK");
    return true;
}

void actOnLedCommandMessage(char *data)
{
    if (strcmp(data, "0"))
    {
        LED0_OFF;
    }
    else if (strcmp(data, "1"))
    {
        LED0_ON;
    }
    else if (strcmp(data, "2"))
    {
        LED0_ON;
    }
    else if (strcmp(data, "3"))
    {
        identifyYourself(selfAdd);
    }
}

void actOnTimCommandMessage(char *data)
{
    uint32_t t = atoi(data);
    setCurrentTime(t);
}

void actOnDofCommandMessage(char *data)
{
    if (data[0] == '0')
        setGyroUse(false);
    else
        setGyroUse(true);

    if (data[1] == '0')
        setAccelUse(false);
    else
        setAccelUse(true);

    if (data[2] == '0')
        setCompassUse(false);
    else
        setCompassUse(true);
}

void actOnCavCommandMessage(char *data)
{
    int16_t cal[6];
    sscanf(data, "%"SCNd16":%"SCNd16":%"SCNd16":%"SCNd16":%"SCNd16":%"SCNd16, &cal[0], &cal[1], &cal[2], &cal[3], &cal[4], &cal[5]);
    setMagCalibration(cal);
    forceReorientation();
}

void actOnMcmCommandMessage(char *data)
{
    if (strcmp(data, "0"))
    {
        autoCalibrate = false;
    }
    else if (strcmp(data, "1"))
    {
        autoCalibrate = true;
    }
    else
    {
        int16_t cal[] = {0, 0, 0, 0, 0, 0};
        if (strcmp(data, "2"))
        {
            autoCalibrate = true;
            setMagCalibration(cal);
            forceReorientation();
        }
        else if (strcmp(data, "3"))
        {
            autoCalibrate = false;
            setMagCalibration(cal);
            forceReorientation();
        }
        else
        {
            printf("Error: unable to parse calibration mode: %s\n", data);
        }
    }
}

void actOnDupCommandMessage(char *data)
{
    uint32_t t = atoi(data);
    dupInterval = t;
}

void actOnRebCommandMessage(char *data)
{
    reboot();
}

void actOnSynCommandMessage(char *data)
{
    syncKnown();
}

void actOnOrientDataMessage(char *data)
{
    relayMessage("do", data, dataDestAdd);
}
void actOnCalibrDataMessage(char *data)
{
    relayMessage("dc", data, dataDestAdd);
}
void actOnStatusDataMessage(char *data)
{
    relayMessage("ds", data, dataDestAdd);
}

void relayMessage(char *cmd, char *data, char *address)
{
    char msg[30];
    sprintf(msg, "%s#%s", cmd, data);
    udpSend(address, msg);
}

void actOnUdpRequests(int res, char *srcAdd, char* selfAdd)
{
    printf("%s: %d: %s\n", srcAdd, strlen(serverBuffer), serverBuffer);

    // extract components of command
    char *cmd = NULL;
    char *data = NULL;
    char *address = NULL;
    char *p = strtok(serverBuffer, "#");
    if (p)
    {
        cmd = strdup(p);
        p = strtok(serverBuffer, "#");
        if (p)
        {
            data = strdup(p);
            p = strtok(serverBuffer, "#");
            if (p)
            {
                address = strdup(p);
                relayMessage(cmd, data, address);
                return;
            }
        }
    }
    if (cmd == NULL)
        return;

    if (strcmp(cmd, "cled") == 0)
    {
        actOnLedCommandMessage(data);
    }
    else if (strcmp(cmd, "ctim") == 0)
    {
        actOnTimCommandMessage(data);
    }
    else if (strcmp(cmd, "cdof") == 0)
    {
        actOnDofCommandMessage(data);
    }
    else if (strcmp(cmd, "ccav") == 0)
    {
        actOnCavCommandMessage(data);
    }
    else if (strcmp(cmd, "cmcm") == 0)
    {
        actOnMcmCommandMessage(data);
    }
    else if (strcmp(cmd, "cdup") == 0)
    {
        actOnDupCommandMessage(data);
    }
    else if (strcmp(cmd, "creb") == 0)
    {
        actOnRebCommandMessage(data);
    }
    else if (strcmp(cmd, "csyn") == 0)
    {
        actOnSynCommandMessage(data);
    }
    else if (strcmp(cmd, "do") == 0)
    {
        actOnOrientDataMessage(data);
    }
    else if (strcmp(cmd, "dc") == 0)
    {
        actOnCalibrDataMessage(data);
    }
    else if (strcmp(cmd, "ds") == 0)
    {
        actOnStatusDataMessage(data);
    }
    else
    {
        printf("rx unknown udp msg from %s : %s\n", srcAdd, serverBuffer);
    }
}

/*
void actOnUdpRequestsOLD(int res, char *srcAdd, char* selfAdd)
{
    //printf("%s: %d: %s\n", srcAdd, strlen(serverBuffer), serverBuffer);

    if (strcmp(serverBuffer, "on") == 0)
    {
        LED0_ON;
    }
    else if (strcmp(serverBuffer, "off") == 0)
    {
        LED0_OFF;
    }
    else if (strcmp(serverBuffer, "identify") == 0)
    {
        identifyYourself(selfAdd);
    }
    else if (strncmp(serverBuffer, "nudge:", 6) == 0)
    {
        udpSend(serverBuffer+6, "identify");
    }
    else if (strcmp(serverBuffer, "sync") == 0)
    {
#ifdef MAX_NODES
        syncKnown();
#endif
    }
    else if (strncmp(serverBuffer, "set-sens:", 9) == 0)
    {
        // Message will be in form:
        //  set-sens:GAC#ADDRESS (bounce message)
        // or:
        //  set-sens:GAC (direct message)
        // where G = gyroscope on/off [1|0]
        // where A = accelerometer on/off [1|0]
        // where C = magnetometer on/off [1|0]
        // eg:
        //  set-sens:010#affe::584b:3763:a0ca:19b6
        // or
        //  set-sens:111
        // if no '#ADDRESS' (ie a 'direct message') then set this nodes sensors,
        // if '#ADDRESS' (ie a 'bounce message') then send a 'direct message' to the given address
        char *msg = serverBuffer+9;
        const char *delim = "#";
        char *addr;
        char *data;

        data = strsep(&msg, delim);
        if (msg == NULL)
        {
            if (strlen(data) != 3)
            {
                printf("malformed GAC: '%s'\n", data);
                return;
            }

            if (data[0] == '0')
                setGyroUse(false);
            else
                setGyroUse(true);

            if (data[1] == '0')
                setAccelUse(false);
            else
                setAccelUse(true);

            if (data[2] == '0')
                setCompassUse(false);
            else
                setCompassUse(true);

            return;
        }
        addr = strsep(&msg, delim);
        if (msg != NULL)
        {
            printf("malformed message: %s\n", serverBuffer);
            return;
        }
        sprintf(gpTextBuf, "set-sens:%s", data);
        udpSend(addr, gpTextBuf);
    }
    else if (strcmp(serverBuffer, "get-data") == 0)
    {
        // record the sender as a destination for imu data
        strcpy(dataDestAdd, srcAdd);
    }
    else if (strncmp(serverBuffer, "data:", 5) == 0)
    {
        // pass node's data to the current data destination
        relayDataToBroker(dataDestAdd, srcAdd, "dat", serverBuffer+5);
    }
    else if (strncmp(serverBuffer, "calib:", 6) == 0)
    {
        // pass node's calibration to the current data destination
        relayDataToBroker(dataDestAdd, srcAdd, "cal", serverBuffer+6);
    }
    else if (strcmp(serverBuffer, "get-cal") == 0)
    {
        // record the sender as a destination for calibration data
        strcpy(dataDestAdd, srcAdd);
    }
    else if (strncmp(serverBuffer, "set-cal:", 8) == 0)
    {
        // relay calibration data to addressed node.
        // Message will be in form: set-cal:DATA#ADDRESS eg:
        //  set-cal:-89:-86:-82:88:3:54#affe::584b:3763:a0ca:19b6
        char *msg = serverBuffer+8;
        const char *delim = "#";
        char *addr;
        char *data;

        data = strsep(&msg, delim);
        if (msg == NULL)
        {
            printf("malformed message: %s\n", serverBuffer);
            return;
        }
        addr = strsep(&msg, delim);
        if (msg != NULL)
        {
            printf("malformed message: %s\n", serverBuffer);
            return;
        }
        sprintf(gpTextBuf, "up-cal:%s", data);
        udpSend(addr, gpTextBuf);
    }
    else if (strncmp(serverBuffer, "up-cal:", 7) == 0)
    {
        // update the nodes calibration.  Format is like:
        //  up-cal:-89:-86:-82:88:3:54
        char *data = serverBuffer + 7;
        int16_t cal[6];
        sscanf(data, "%"SCNd16":%"SCNd16":%"SCNd16":%"SCNd16":%"SCNd16":%"SCNd16, &cal[0], &cal[1], &cal[2], &cal[3], &cal[4], &cal[5]);
        setMagCalibration(cal);
        forceReorientation();
    }
    else if (strcmp(serverBuffer, "cal-please") == 0)
    {
#ifdef MAX_NODES
        sprintf(gpTextBuf, "{\"t\":\"getc\",\"s\":\"%s\"}", srcAdd);
        udpSend("affe::1", gpTextBuf);
#endif
    }
    else if (strcmp(serverBuffer, "time-please") == 0)
    {
        sprintf(gpTextBuf, "ts:%lu", getCurrentTime());
        udpSend(srcAdd, gpTextBuf);
    }
    else if (strncmp(serverBuffer, "ts:", 3) == 0)
    {
        if (srcAdd != selfAdd)
        {
            uint32_t t = atoi(serverBuffer+3);
            setCurrentTime(t);
        }
    }
    else
    {
        printf("rx unknown udp msg from %s : %s\n", srcAdd, serverBuffer);
    }
}
*/

double getMessageRate(void)
{
    uint32_t dt = xtimer_now() - startTime;
    return (1000000 * messagesRelayed)/dt;
}

int udpSend(char *addrStr, char *data)
{
    size_t dataLen = strlen(data);
    if (dataLen > 0)
    {
        struct sockaddr_in6 dst;
        dst.sin6_family = AF_INET6;
        if (inet_pton(AF_INET6, addrStr, &dst.sin6_addr) != 1) {
            puts("Error: unable to parse destination address");
            printf("sending: %s msg: %s\n", addrStr, data);
            return 1;
        }
        dst.sin6_port = htons(UDP_PORT);

        int s;
        s = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
        if (s < 0) {
            puts("error initializing socket");
            return 1;
        }

        if (sendto(s, data, dataLen, 0, (struct sockaddr *)&dst, sizeof(dst)) < 0) {
            puts("error: could not send message");
            close(s);
            return 1;
        }
        //printf("Success: send %u byte(s) to %s:%u\n", (unsigned)dataLen, addrStr, UDP_PORT);
        close(s);
        /*if (! streq(dataDestAdd, addr))
        {
            messagesRelayed++;
        }*/
    }
    else
    {
        puts("trying to send empty value via udp!! Send discarded.");
    }

    return 0;
}

void udpGetRequestAndAct(void)
{
    memset(serverBuffer, 0, SERVER_BUFFER_SIZE);

    //uint32_t waitTimer = xtimer_now();
    struct sockaddr_in6 src;
    socklen_t srcLen = sizeof(struct sockaddr_in6);
    // this blocks :( no non blocking recvfrom in RIOT OS yet
    int res = recvfrom(serverSocket,
            serverBuffer,
            sizeof(serverBuffer),
            0,
            (struct sockaddr *)&src,
            &srcLen);
    //printf("w%lu\n", xtimer_now() - waitTimer);

    // get strings represnting source and server ipv6 addresses
    srcSocketAddr = src.sin6_addr; 
    inet_ntop(AF_INET6, &(srcSocketAddr.s6_addr), srcAdd, IPV6_ADDR_MAX_STR_LEN);
    inet_ntop(AF_INET6, &(serverSocketAddr.sin6_addr), selfAdd, IPV6_ADDR_MAX_STR_LEN);

    if (res < 0)
    {
        printf("Error on RX %d:%s rx from: %s (%s)\n", errno, strerror(errno), srcAdd, serverBuffer);
        xtimer_usleep(100);
        return;
    }
    else if (res == 0)
    {
        puts("Peer did shut down");
        return;
    }
    else if (res >= SERVER_BUFFER_SIZE)
    {
        puts("OVERFLOW!");
        return;
    }
    actOnUdpRequests(res, srcAdd, selfAdd);
}


void *udpServer(void *arg)
{
    initUdp();
    if (setupUdpServer())
    {
        for(;;)
        {
            udpGetRequestAndAct();
        }
    }
    else
    {
        return NULL;
    }
}

bool relayDataToBroker(char * destAdd, char *srcAdd, char *type, char *val)
{
    if (strlen(destAdd) > 0)
    {
        sprintf(json, "{\"t\":\"%s\",\"s\":\"%s\",\"v\":\"%s\"}", type, srcAdd, val);
        udpSend(destAdd, json);
        return true;
    }
    return false;
}

void syncKnown(void)
{
#ifdef MAX_NODES
    sprintf(gpTextBuf, "ctim#%lu", getCurrentTime());
    // this dodgy - rather than use unicast address should send specifically to
    // known nodes
    udpSend("ff02::1", gpTextBuf);
    return;
#endif
}

void initUdp(void)
{
#ifdef MAX_NODES
    for (uint8_t i = 0; i < MAX_NODES; i++)
    {
        memset(nodeData[i], 0, IPV6_ADDR_MAX_STR_LEN);
    }
    msg_init_queue(msg_q, MUDP_Q_SZ);
#endif
    startTime = xtimer_now();
}

int udp_cmd(int argc, char **argv)
{
    if (argc == 3) {
        return udpSend(argv[1], argv[2]);
    }

    printf("usage: %s <IPv6-address> <message>\n", argv[0]);
    return 1;
}

