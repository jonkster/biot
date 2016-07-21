/*
 * Copyright (C) 2016 Jon Kelly
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */


#include <stdio.h>
#include <thread.h>
#include <xtimer.h>

#include "msg.h"
#include "net/ipv6/addr.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/ipv6/netif.h"
#include "net/gnrc/rpl/dodag.h"
#include "periph/gpio.h"
#include "shell.h"
#include "../modules/identify/biotIdentify.h"
#include "../modules/sendData/sendData.h"
#include "../modules/dataCache/dataCache.h"
#include "../modules/biotUdp/udp.h"



#define PRIO    (THREAD_PRIORITY_MAIN + 1)
static char housekeeping_stack[THREAD_STACKSIZE_DEFAULT];
static char udp_stack[THREAD_STACKSIZE_DEFAULT + 1024];

char dodagRoot[] = "affe::2";

extern void batch(const shell_command_t *command_list, char *line);
extern int udp_send(char *addr_str, char *data);
extern uint32_t getCurrentTime(void);
extern bool isTimeSet(void);
bool hasTimeChanged(void);
extern void timeInit(void);
extern void sendData(char *address, nodeData_t data);
extern void setCurrentTime(uint32_t t);
/* ########################################################################## */

void sync(void)
{
    char ts[15];
    sprintf(ts, "ts:%lu", getCurrentTime());
    udp_send("ff02::1", ts);
}

int sync_cmd(int argc, char **argv)
{
    sync();
    return 0;
}

int time_cmd(int argc, char **argv)
{
    printf("time: %lu\n", getCurrentTime());
    return 0;
}

int resetTime_cmd(int argc, char **argv)
{
    setCurrentTime(0);
    sync();
    return 0;
}

int identify_cmd(int argc, char **argv)
{
    identifyYourself("");
    return 0;
}

int showNode_cmd(int argc, char **argv)
{
    dumpNodeData();
    return 0;
}

int showCalibration_cmd(int argc, char **argv)
{
    dumpNodeCalibration();
    return 0;
}




bool isRoot = false;
bool isRootPending = false;
void btnCallback(void* arg)
{
    if (! isRoot)
    {
        isRoot = true;
        LED0_ON;
        isRootPending = true;
    }
}




static const shell_command_t shell_commands[] = {
    /* Add a new shell commands here */
    { "identify", "visually identify board", identify_cmd },

    { "sync", "synchronise time across nodes", sync_cmd },

    { "time", "show net time", time_cmd },

    { "retime", "reset net time", resetTime_cmd },

    { "data", "show current node data", showNode_cmd },

    { "cal", "show current node calibration", showCalibration_cmd },

    { "udp", "send a message: udp <IPv6-address> <message>", udp_cmd },

    { NULL, NULL, NULL }
};


void setRoot(void)
{
    isRootPending = false;
    // set up rpl root
    puts("setting as root node");
    batch(shell_commands, "ifconfig 6 add affe::2");
    batch(shell_commands, "rpl root 1 affe::2");

    // add wired interface
    puts("setting as border router");
    batch(shell_commands, "ifconfig 7 add affe::3");
    // assume outside world is at affe::1
    batch(shell_commands, "ncache add 7 affe::1");
    LED0_ON;
}


/* set interval to 1 second */
#define INTERVAL (1000000U)
void *housekeeping_handler(void *arg)
{
    int counter = 0;
    uint32_t lastSecs = 0;
    while(1)
    {
        uint32_t secs = getCurrentTime()/1500000;
        if (secs != lastSecs)
        {
            if (isRootPending)
            {
                setRoot();
            }

            if (isRoot && counter++ > 30)
            {
                sync();
                counter = 0;
            }

            if (secs % 2 == 0)
            {
                LED0_ON;
                thread_yield();
            }
            else
            {
                LED0_OFF;
                thread_yield();
            }
        }
        lastSecs = secs;
        thread_yield();
    }
}

int main(void)
{
    puts("Biotz Root Node\n");
    LED0_OFF;

    thread_create(housekeeping_stack, sizeof(housekeeping_stack), PRIO, THREAD_CREATE_STACKTEST, housekeeping_handler, NULL, "housekeeping");

    thread_create(udp_stack, sizeof(udp_stack), PRIO, THREAD_CREATE_STACKTEST, udp_server, NULL, "udp");


    batch(shell_commands, "rpl init 6");
    setRoot();
    gpio_init_int(BUTTON_GPIO, GPIO_IN_PU, GPIO_RISING, (gpio_cb_t)btnCallback, NULL);

    identifyYourself("");

    timeInit();
    sync();


    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* never reached */
    return 0;
}
