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
#include <thread.h>
#include "board.h"
#include "thread.h"
#include "udp_common.h"
#include "ssd1306.h"
#include "periph/gpio.h"
#include "net/gnrc/ipv6/nc.h"
#include "net/gnrc/ipv6/netif.h"

#define PRIO    (THREAD_PRIORITY_MAIN + 1)
#define Q_SZ    (8)
static msg_t msg_q[Q_SZ];
bool led_status = false;

static char udp_stack[THREAD_STACKSIZE_DEFAULT];
static char housekeeping_stack[THREAD_STACKSIZE_DEFAULT];
static char display_stack[THREAD_STACKSIZE_DEFAULT];

static const shell_command_t shell_commands[];

extern  void *display_handler(void *arg);

/* Add the shell command function here ###################################### */

int led_control(int argc, char **argv)
{
    if (argc == 2) {
        if (strcmp(argv[1], "on") == 0) {
            led_status = true;
            LED0_ON;
            LED1_OFF;
            return 0;
        }
        else if (strcmp(argv[1], "off") == 0) {
            led_status = false;
            LED0_OFF;
            LED1_ON;
            LED_RGB_OFF;
            return 0;
        }
        else if (strcmp(argv[1], "red") == 0) {
            led_status = false;
            LED_RGB_OFF;
            LED_RGB_R_ON;
            return 0;
        }
        else if (strcmp(argv[1], "green") == 0) {
            led_status = false;
            LED_RGB_OFF;
            LED_RGB_G_ON;
            return 0;
        }
        else if (strcmp(argv[1], "blue") == 0) {
            led_status = false;
            LED_RGB_OFF;
            LED_RGB_B_ON;
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
        LED_RGB_R_ON;
        LED0_ON;
        isRootPending = true;
    }
}


/* ########################################################################## */
extern int adc_cmd(int argc, char **argv);
extern int getVoltage(void);
extern int oled_cmd(int argc, char **argv);

static const shell_command_t shell_commands[] = {

/* Add a new shell command here ############################################# */

    { "led", "use 'led on' to turn the LED on and 'led off' to turn the LED off", led_control },

    { "udp", "send a message: udp <IPv6-address> <message>", udp_cmd },

    { "adc", "read value of ADC", adc_cmd },

    { "oled", "test oled display", oled_cmd },

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
    LED_RGB_OFF;
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
        thread_yield();
        xtimer_usleep_until(&last_wakeup, INTERVAL/(2*factor));
        LED0_OFF;
        thread_yield();
        xtimer_usleep_until(&last_wakeup, INTERVAL/factor);
        LED0_ON;
    //    batch(shell_commands, "ps");
    }
}


int main(void)
{
    msg_init_queue(msg_q, Q_SZ);

    puts("Type 'help' for a list of available commands");

    LED0_OFF;
    LED1_ON;
    LED_RGB_OFF;

    puts("Biotz\n");
    batch(shell_commands, "rpl init 7");
    gpio_init_int(BUTTON_GPIO, GPIO_IN_PU, GPIO_RISING, (gpio_cb_t)btnCallback, NULL);

    puts("make houeskeeping\n");

    kernel_pid_t hkpid = thread_create(housekeeping_stack, sizeof(housekeeping_stack), PRIO + 3, THREAD_CREATE_SLEEPING, housekeeping_handler,
                  NULL, "housekeeping");

    puts("make display\n");
    kernel_pid_t dhpid = thread_create(display_stack, sizeof(display_stack), PRIO + 1, THREAD_CREATE_SLEEPING, display_handler,
                  NULL, "display");

    puts("make udp\n");
    kernel_pid_t udpid = thread_create(udp_stack, sizeof(udp_stack), PRIO + 1, THREAD_CREATE_SLEEPING, udp_server,
                  NULL, "udp");

    thread_wakeup(hkpid);
    thread_wakeup(dhpid);
    thread_wakeup(udpid);

    puts("make shell\n");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* never reached */
    return 0;
}
