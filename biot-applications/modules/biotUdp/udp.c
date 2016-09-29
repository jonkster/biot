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
static char gpTextBuf[40];
static msg_t msg_q[MUDP_Q_SZ];
static char srcAdd[IPV6_ADDR_MAX_STR_LEN];
static char selfAdd[IPV6_ADDR_MAX_STR_LEN];
struct in6_addr srcSocketAddr; 

uint32_t startTime = 0;
bool biotMsgSilent = false;

#ifdef MAX_NODES
    char nodeData[MAX_NODES][IPV6_ADDR_MAX_STR_LEN];
#endif

char dataDestAdd[IPV6_ADDR_MAX_STR_LEN];

//bool relayDataToBroker(char * destAdd, char *srcAdd, char *type, char *val);

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
    relayMessage("do", data, "affe::1");
}
void actOnCalibrDataMessage(char *data)
{
    relayMessage("dc", data, "affe::1");
}
void actOnStatusDataMessage(char *data)
{
    relayMessage("ds", data, "affe::1");
}

void relayMessage(char *cmd, char *data, char *address)
{
    char msg[MAX_MESSAGE_LENGTH];
    //printf("relaying cmd:%s with data:%s to:%s\n", cmd, data, address);
    sprintf(msg, "%s#%s", cmd, data);
    udpSend(address, msg);
    return;
}

void actOnUdpRequests(int res, char *srcAdd, char* selfAdd)
{
    if (biotMsgSilent)
    {
        return;
    }
//printf("from:%s: len:%d: msg:%s\n", srcAdd, strlen(serverBuffer), serverBuffer);

    // extract components of command
    char *cmd = NULL;
    char *data = NULL;
    char *address = NULL;
    char *p = strtok(serverBuffer, "#");
    if (p > 0)
    {
        cmd = strdup(p);
//printf("got cmd:%s\n", cmd);
        p = strtok(NULL, "#");
        if (p)
        {
            data = strdup(p);
//printf("got data:%s\n", data);
            p = strtok(NULL, "#");
            if (p)
            {
                address = strdup(p);
//printf("got add:%s\n", address);
                relayMessage(cmd, data, address);
                free(cmd);
                free(data);
                free(address);
                return;
            }
        }
    }

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
    free(cmd);
    free(data);
}

int udpSend(char *addrStr, char *data)
{
//    printf("sending udp:%s\n", data);
    size_t dataLen = strlen(data);
    if (dataLen > 0)
    {
        if (dataLen > MAX_MESSAGE_LENGTH)
        {
            printf("message too long: %i > %i   '%s'\n", dataLen, MAX_MESSAGE_LENGTH, data);
            return 1;
        }
        struct sockaddr_in6 dst;
        dst.sin6_family = AF_INET6;
        if (inet_pton(AF_INET6, addrStr, &dst.sin6_addr) != 1) {
            puts("Error: unable to parse destination address");
            printf("sending to add: %s   msg: %s\n", addrStr, data);
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
        close(s);
 //       printf("Success: send %u byte(s) to %s:%u\n", (unsigned)dataLen, addrStr, UDP_PORT);
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

    struct sockaddr_in6 src;
    socklen_t srcLen = sizeof(struct sockaddr_in6);
    // this blocks :( no non blocking recvfrom in RIOT OS yet
    int res = recvfrom(serverSocket,
            serverBuffer,
            sizeof(serverBuffer),
            0,
            (struct sockaddr *)&src,
            &srcLen);

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

