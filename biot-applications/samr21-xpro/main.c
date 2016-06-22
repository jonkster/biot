/*
 * Copyright (C) 2015 Cenk Gündoğan <cnkgndgn@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @brief       Introductory example for the IEEE WF-IOT 2015 Tutorial
 * @author      Cenk Gündoğan <cnkgndgn@gmail.com>
 */

#include <stdio.h>
#include "shell.h"
#include "msg.h"
#include <stdbool.h>
#include <string.h>
#include "board.h"
#include "thread.h"
#include "udp_common.h"
#include "periph/gpio.h"
#include "net/gnrc/ipv6/nc.h"
#include "net/gnrc/ipv6/netif.h"

#define PRIO    (THREAD_PRIORITY_MAIN - 1)
#define Q_SZ    (8)
static msg_t msg_q[Q_SZ];
bool led_status = false;
static char udp_stack[THREAD_STACKSIZE_DEFAULT];
static char housekeeping_stack[THREAD_STACKSIZE_DEFAULT];

static const shell_command_t shell_commands[];


/* Add the shell command function here ###################################### */

int led_control(int argc, char **argv)
{
    if (argc == 2) {
        if (strcmp(argv[1], "on") == 0) {
            led_status = true;
            LED0_ON;
            //LED1_OFF;
            return 0;
        }
        else if (strcmp(argv[1], "off") == 0) {
            led_status = false;
            LED0_OFF;
            //LED1_ON;
            //LED_RGB_OFF;
            return 0;
        }
        else if (strcmp(argv[1], "red") == 0) {
            led_status = false;
            LED0_ON;
            //LED_RGB_OFF;
            //LED_RGB_R_ON;
            return 0;
        }
        else if (strcmp(argv[1], "green") == 0) {
            led_status = false;
            LED0_ON;
            //LED_RGB_OFF;
            //LED_RGB_G_ON;
            return 0;
        }
        else if (strcmp(argv[1], "blue") == 0) {
            led_status = false;
            LED0_ON;
            //LED_RGB_OFF;
            //LED_RGB_B_ON;
            return 0;
        }
    }

    return -1;
}

extern void batch(const shell_command_t *command_list, char *line);

bool isRoot = false;
bool isRootPending = false;
void btnCallback(void* arg)
{
    if (! isRoot)
    {
        isRoot = true;
        //LED_RGB_R_ON;
        LED0_ON;
        isRootPending = true;
    }
}


/* ########################################################################## */
extern int adc_cmd(int argc, char **argv);

static const shell_command_t shell_commands[] = {

/* Add a new shell command here ############################################# */

    { "led", "use 'led on' to turn the LED on and 'led off' to turn the LED off", led_control },

    { "udp", "send a message: udp <IPv6-address> <message>", udp_cmd },

    { "adc", "read value of ADC", adc_cmd },

    /* ########################################################################## */
    { NULL, NULL, NULL }
};


void setRoot(void)
{
    isRootPending = false;
    // set up rpl root
    batch(shell_commands, "ifconfig 6 add affe::2");
    batch(shell_commands, "rpl root 1 affe::2");
    // add wired interface
    batch(shell_commands, "ifconfig 7 add affe::3");
    batch(shell_commands, "ncache add 7 affe::1");
    //LED_RGB_OFF;
    LED0_ON;
}

/* set interval to 1 second */
#define INTERVAL (1000000U)
void *housekeeping_handler(void *arg)
{
   int factor = 1; 
    while(1)
    {
        if (isRootPending)
        {
            setRoot();
            factor = 5;
        }
        uint32_t last_wakeup = xtimer_now();
        xtimer_usleep_until(&last_wakeup, INTERVAL/(2*factor));
        LED0_OFF;
        xtimer_usleep_until(&last_wakeup, INTERVAL/factor);
        LED0_ON;
    }
}


int main(void)
{
    msg_init_queue(msg_q, Q_SZ);

    puts("Type 'help' for a list of available commands");

    LED0_OFF;
    //LED1_ON;
    //LED_RGB_OFF;

    printf("Biotz\n");
    batch(shell_commands, "rpl init 7");
    gpio_init_int(BUTTON_GPIO, GPIO_IN_PU, GPIO_RISING, (gpio_cb_t)btnCallback, NULL);

    thread_create(udp_stack, sizeof(udp_stack), PRIO, THREAD_CREATE_STACKTEST, udp_server,
                  NULL, "udp");

    thread_create(housekeeping_stack, sizeof(housekeeping_stack), THREAD_PRIORITY_MAIN - 3, THREAD_CREATE_STACKTEST, housekeeping_handler,
                  NULL, "housekeeping");


    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* never reached */
    return 0;
}
