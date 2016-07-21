#include <inttypes.h>
#include <xtimer.h>
//#include "random.h"
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
#include "udp.h"
#include "../identify/biotIdentify.h"
#include "../dataCache/dataCache.h"
#include "../imu/imu.h"

#define XSTR(x) STR(x)
#define STR(x) #x


#define MUDP_Q_SZ           (32)
#define SERVER_BUFFER_SIZE  (128)
#define UDP_PORT            (8888)

extern uint32_t getCurrentTime(void);
extern void setCurrentTime(uint32_t t);

static int server_socket = -1;
static char serverBuffer[SERVER_BUFFER_SIZE];
static msg_t msg_q[MUDP_Q_SZ];

hash_t *nodeData;
hash_t *nodeCalibration;

uint8_t errs = 0;

static void *udp_server_loop(void)
{
    struct sockaddr_in6 server_addr;

    server_socket = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);

    server_addr.sin6_family = AF_INET6;
    memset(&server_addr.sin6_addr, 0, sizeof(server_addr.sin6_addr));
    server_addr.sin6_port = htons(UDP_PORT);

    if (server_socket < 0)
    {
        puts("error initializing socket");
        server_socket = 0;
        return NULL;
    }

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        server_socket = -1;
        puts("error binding socket");
        return NULL;
    }

    int res;
    struct sockaddr_in6 src;
    socklen_t srcLen = sizeof(struct sockaddr_in6);
    while (1)
    {
        if (errs > 10)
        {
            puts("too many errors in udp system, panic!");
            return NULL;
        }
        memset(serverBuffer, 0, SERVER_BUFFER_SIZE);
        res = recvfrom(server_socket, serverBuffer, sizeof(serverBuffer), 0, (struct sockaddr *)&src, &srcLen);

            struct in6_addr src_addr = src.sin6_addr; 
            char srcAdd[IPV6_ADDR_MAX_STR_LEN];
            char selfAdd[IPV6_ADDR_MAX_STR_LEN];
            inet_ntop(AF_INET6, &(src_addr.s6_addr), srcAdd, IPV6_ADDR_MAX_STR_LEN);
            inet_ntop(AF_INET6, &(server_addr.sin6_addr), selfAdd, IPV6_ADDR_MAX_STR_LEN);

        if (res < 0)
        {
            //errs++;
            puts("Error on receive");
            printf("error %d:%s.  Msg l:%d from: %s\n", errno, strerror(errno), srcLen, srcAdd);
            xtimer_sleep(2);
            // what now????
        }
        else if (res == 0)
        {
            puts("Peer did shut down");
        }
        else
        {

            //uint32_t rand = random_uint32();
            /*if (! strncmp(serverBuffer, "data:", 4) == 0)*/
              //printf("msg: %s from %s\n", serverBuffer, srcAdd);
            //if (true) { printf("msg: %s from %s\n", serverBuffer, srcAdd); } else
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
                udp_send(serverBuffer+6, "identify");
            }
            else if (strcmp(serverBuffer, "sync") == 0)
            {
                char ts[15];
                udp_send("ff02::1", ts);
            }
            else if (strcmp(serverBuffer, "get-data") == 0)
            {
                // send a structure containing the cache of node orientations (typically sent to a processing application)
                char *json = nodeDataJson();
                //printf("sending:%s\n", json);
                udp_send(srcAdd, json);
                free(json);
            }
            else if (strncmp(serverBuffer, "data:", 5) == 0)
            {
                // update the cache of all the node data values (this is stored
                // by the router).  The data will be periodically fired from
                // nodes to the router
                setValue(nodeData, srcAdd, serverBuffer+5);
            }
            else if (strncmp(serverBuffer, "calib:", 6) == 0)
            {
                // update the cache of all the node calibration values (this is
                // stored by the router).  The data will be periodically fired from
                // nodes to the router
                setValue(nodeCalibration, srcAdd, serverBuffer+6);
            }
            else if (strcmp(serverBuffer, "get-cal") == 0)
            {
                // send a structure containing the cache of node calibration parameters (typically sent to a processing application)
                char *json = nodeCalibrationJson();
                udp_send(srcAdd, json);
                free(json);
            }
            else if (strcmp(serverBuffer, "time-please") == 0)
            {
                char ts[25];
                udp_send(srcAdd, ts);
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
        thread_yield();
    }
    return NULL;
}

int udp_send(char *addrStr, char *data)
{
    //printf("sending: %s msg: %s\n", addrStr, data);
    struct sockaddr_in6 src, dst;
    size_t data_len = strlen(data);
    int s;
    src.sin6_family = AF_INET6;
    dst.sin6_family = AF_INET6;
    memset(&src.sin6_addr, 0, sizeof(src.sin6_addr));
    /* parse destination address */
    if (inet_pton(AF_INET6, addrStr, &dst.sin6_addr) != 1) {
        puts("Error: unable to parse destination address");
        return 1;
    }

    dst.sin6_port = htons(UDP_PORT);
    src.sin6_port = htons(UDP_PORT);

    s = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);

    if (s < 0) {
        puts("error initializing socket");
        return 1;
    }

    if (sendto(s, data, data_len, 0, (struct sockaddr *)&dst, sizeof(dst)) < 0) {
        puts("error: could not send message");
        close(s);
        return 1;
    }

    //printf("Success: send %u byte(s) to %s:%u\n", (unsigned)data_len, addrStr, UDP_PORT);

    close(s);

    return 0;
}


/*
 * trampoline for udp_server_loop()
 */
void *udp_server(void *arg)
{
    (void) arg;
    //random_init(12345);
#ifdef MAX_NODES
    nodeData = newHash(MAX_NODES);
    nodeCalibration = newHash(MAX_NODES);
#endif
    msg_init_queue(msg_q, MUDP_Q_SZ);
    udp_server_loop();

    /* never reached */
    return NULL;
}

int udp_cmd(int argc, char **argv)
{
    if (argc == 3) {
        return udp_send(argv[1], argv[2]);
    }

    printf("usage: %s <IPv6-address> <message>\n", argv[0]);
    return 1;
}

char *nodeCalibrationJson(void)
{
    uint16_t count = nodeCalibration->currentSize;
    char json[MAX_MESSAGE_LENGTH];
    sprintf(json, "{\"c\":%d,\"n\":[", count);
    for (uint8_t i = 0; i < count; i++)
    {
        if (i != 0)
        {
            // add leading comma if not first line
            strncat(json, ",", 1);
        }
        char *key = nodeCalibration->keySet[i];
        char jsonBit[MAX_MESSAGE_LENGTH/10];
        sprintf(jsonBit, "{\"a\":\"%s\",\"v\":\"%s\"}", key, getValue(nodeCalibration, key));
        strncat(json, jsonBit, MAX_MESSAGE_LENGTH - strlen(json) - 1);
    }
    strncat(json, "]}", MAX_MESSAGE_LENGTH - strlen(json) - 1);
    return strdup(json);
}

void dumpNodeCalibration(void)
{
    uint16_t count = nodeCalibration->currentSize;
    printf("hash has %d values\n", count);
    for (uint8_t i = 0; i < count; i++)
    {
        char *key = nodeCalibration->keySet[i];
        printf("key:'%s' = '%s'\n", key, getValue(nodeCalibration, key));
    }
}


char *nodeDataJson(void)
{
    uint16_t count = nodeData->currentSize;
    char json[MAX_BIG_MESSAGE_LENGTH];
    sprintf(json, "{\"c\":%d,\"n\":[", count);
    for (uint8_t i = 0; i < count; i++)
    {
        if (i != 0)
        {
            // add leading comma if not first line
            strncat(json, ",", 1);
        }
        char *key = nodeData->keySet[i];
        char jsonBit[MAX_BIG_MESSAGE_LENGTH/10];
        sprintf(jsonBit, "{\"a\":\"%s\",\"v\":\"%s\"}", key, getValue(nodeData, key));
        strncat(json, jsonBit, MAX_BIG_MESSAGE_LENGTH - strlen(json) - 1);
    }
    strncat(json, "]}", MAX_BIG_MESSAGE_LENGTH - strlen(json) - 1);
    return strdup(json);
}

void dumpNodeData(void)
{
    uint16_t count = nodeData->currentSize;
    printf("hash has %d values\n", count);
    for (uint8_t i = 0; i < count; i++)
    {
        char *key = nodeData->keySet[i];
        printf("key:'%s' = '%s'\n", key, getValue(nodeData, key));
    }
}

