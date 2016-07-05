/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 * @author      Cenk Gündoğan <cnkgndgn@gmail.com>
 */

#include <inttypes.h>
#include <xtimer.h>
#include "random.h"
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
#include "udp.h"
#include "board.h"
#include "../modules/identify/biotIdentify.h"

#define XSTR(x) STR(x)
#define STR(x) #x

#if (defined NOOLED)
#pragma message "Assuming no OLED available therefore samr21-xpro board"
    #define LED1_OFF do {} while(0);
    #define LED_RGB_OFF do {} while(0);
    #define LED_RGB_R_ON do {} while(0);
    #define LED_RGB_G_ON do {} while(0);
    #define LED_RGB_B_ON do {} while(0);
#else
#pragma message "Assuming OLED available and samr21-zllk board"
    #include "../modules/ssd1306/ssd1306.h"
#endif





#define MUDP_Q_SZ           (8)
#define SERVER_BUFFER_SIZE  (128)
#define UDP_PORT            (8888)

extern uint32_t getCurrentTime(void);
extern void setCurrentTime(uint32_t t);

static int server_socket = -1;
static char server_buffer[SERVER_BUFFER_SIZE];
static msg_t msg_q[MUDP_Q_SZ];
static int udp_send_jk(struct in6_addr destAdd, char *data);

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
    socklen_t src_len = sizeof(struct sockaddr_in6);
    while (1)
    {
        memset(server_buffer, 0, SERVER_BUFFER_SIZE);
        res = recvfrom(server_socket, server_buffer, sizeof(server_buffer), 0, (struct sockaddr *)&src, &src_len);

        struct in6_addr src_addr = src.sin6_addr; 

        if (res < 0)
        {
            puts("Error on receive");
        }
        else if (res == 0)
        {
            puts("Peer did shut down");
        }
        else
        {
            uint32_t rand = random_uint32();
            char srcAdd[IPV6_ADDR_MAX_STR_LEN];
            inet_ntop(AF_INET6, &(src_addr.s6_addr), srcAdd, IPV6_ADDR_MAX_STR_LEN);
            //printf("rx from %s %s\n", srcAdd, server_buffer);
            if (strcmp(server_buffer, "on") == 0)
            {
                LED0_ON;
            }
            else if (strcmp(server_buffer, "identify") == 0)
            {
                identifyYourself();
            }
            else if (strncmp(server_buffer, "data:", 5) == 0)
            {
                //printf("data from %s = %s\n", srcAdd, server_buffer+5);
            }
            else if (strcmp(server_buffer, "time-please") == 0)
            {
                char ts[25];
                sprintf(ts, "ts:%lu", getCurrentTime());
                udp_send(srcAdd, ts);
            }
            else if (strncmp(server_buffer, "ts:", 3) == 0)
            {
                uint32_t t = atoi(server_buffer+3);

                printf("time received from %s : %s -> %lu\n", srcAdd, server_buffer, t);
                setCurrentTime(t);
            }
            else if (strcmp(server_buffer, "off") == 0)
            {
                LED0_OFF;
                LED1_OFF;
                LED_RGB_OFF;
                xtimer_usleep(100000);
                if (rand > 2000000000 )
                {
#if !defined NOOLED
                    oledPrint(1, "got off sending red");
#endif
                    udp_send_jk(src_addr, "red");
                }
                else
                {
#if !defined NOOLED
                    oledPrint(1, "got off sending off");
#endif
                    udp_send_jk(src_addr, "off");
                }
            }
            else if (strcmp(server_buffer, "red") == 0)
            {
                LED_RGB_OFF;
                LED_RGB_R_ON;
                xtimer_usleep(100000);
                if (rand > 2000000000)
                {
#if !defined NOOLED
                    oledPrint(1, "got red sending green");
#endif
                    udp_send_jk(src_addr, "green");
                }
                else
                {
#if !defined NOOLED
                    oledPrint(1, "got red sending red");
#endif
                    udp_send_jk(src_addr, "red");
                }
            }
            else if (strcmp(server_buffer, "green") == 0)
            {
                LED_RGB_OFF;
                LED_RGB_G_ON;
                xtimer_usleep(100000);
                if (rand > 2000000000)
                {
#if !defined NOOLED
                    oledPrint(1, "got green sending blue");
#endif
                    udp_send_jk(src_addr, "blue");
                }
                else
                {
#if !defined NOOLED
                    oledPrint(1, "got green sending green");
#endif
                    udp_send_jk(src_addr, "green");
                }
            }
            else if (strcmp(server_buffer, "blue") == 0)
            {
                LED_RGB_OFF;
                LED_RGB_B_ON;
                xtimer_usleep(100000);
                if (rand > 2000000000)
                {
#if !defined NOOLED
                    oledPrint(1, "got blue sending off");
#endif
                    udp_send_jk(src_addr, "off");
                }
                else
                {
#if !defined NOOLED
                    oledPrint(1, "got blue sending blue");
#endif
                    udp_send_jk(src_addr, "blue");
                }
            }
        }
    }
    return NULL;
}

int udp_send(char *addr_str, char *data)
{
    struct sockaddr_in6 src, dst;
    size_t data_len = strlen(data);
    int s;
    src.sin6_family = AF_INET6;
    dst.sin6_family = AF_INET6;
    memset(&src.sin6_addr, 0, sizeof(src.sin6_addr));
    /* parse destination address */
    if (inet_pton(AF_INET6, addr_str, &dst.sin6_addr) != 1) {
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

    //printf("Success: send %u byte(s) to %s:%u\n", (unsigned)data_len, addr_str, UDP_PORT);

    close(s);

    return 0;
}

static int udp_send_jk(struct in6_addr destAdd, char *data)
{
    struct sockaddr_in6 src, dst;
    size_t data_len = strlen(data);
    src.sin6_family = AF_INET6;
    dst.sin6_family = AF_INET6;
    memset(&src.sin6_addr, 0, sizeof(src.sin6_addr));
    dst.sin6_addr = destAdd;
    /* parse destination address */

    dst.sin6_port = htons(UDP_PORT);
    src.sin6_port = htons(UDP_PORT);

    int s = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);

    if (s < 0) {
        puts("error initializing socket");
        return 1;
    }

    if (sendto(s, data, data_len, 0, (struct sockaddr *)&dst, sizeof(dst)) < 0) {
        puts("error: could not send message");
        close(s);
        return 1;
    }

    printf("Success: sent %u byte(s)\n", (unsigned)data_len);

    close(s);

    return 0;
}


/*
 * trampoline for udp_server_loop()
 */
void *udp_server(void *arg)
{
    (void) arg;
    msg_init_queue(msg_q, MUDP_Q_SZ);
    random_init(12345);

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

/** @} */
