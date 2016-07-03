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
#include <xtimer.h>
#include "board.h"
#include "thread.h"
#include "ssd1306.h"
#include "periph/gpio.h"

#define PRIO    (THREAD_PRIORITY_MAIN + 1)
#define Q_SZ    (8)
static msg_t msg_q[Q_SZ];
bool led_status = false;

static char housekeeping_stack[THREAD_STACKSIZE_DEFAULT];

static const shell_command_t shell_commands[];

extern  void *display_handler(void *arg);
static char display_stack[THREAD_STACKSIZE_DEFAULT];

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

void buttonPressedCallback(void* arg)
{
    puts("button pushed");
}


/* ########################################################################## */
extern int oledCmd(int argc, char **argv);
extern int oledClearAll(void);
extern void oledWriteText(const char *string);

static const shell_command_t shell_commands[] = {

/* Add a new shell command here ############################################# */

    { "led", "use 'led on' to turn the LED on and 'led off' to turn the LED off", led_control },

    { "oled", "test oled display", oledCmd },

    /* ########################################################################## */
    { NULL, NULL, NULL }
};


/* set interval to 1 second */
#define INTERVAL (1000000U)
void *housekeeping_handler(void *arg)
{
   int factor = 1; 
   uint16_t i = 0;
    while(1)
    {
        uint32_t last_wakeup = xtimer_now();
        thread_yield();
        xtimer_usleep_until(&last_wakeup, INTERVAL/(2*factor));
        LED0_OFF;
        //LED_RGB_OFF;
        thread_yield();
        xtimer_usleep_until(&last_wakeup, INTERVAL/factor);
        LED0_ON;
        //LED_RGB_R_ON;
        char st[10];
        sprintf(st, "%d", i++);
        oledClearAll();
        oledWriteText(st);
    }
}


int main(void)
{
    msg_init_queue(msg_q, Q_SZ);

    puts("Type 'help' for a list of available commands");

    LED0_OFF;
    LED1_ON;
    LED_RGB_OFF;

    puts("Biotz OLED experiment\n");
    gpio_init_int(BUTTON_GPIO, GPIO_IN_PU, GPIO_RISING, (gpio_cb_t)buttonPressedCallback, NULL);

    kernel_pid_t hkpid = thread_create(housekeeping_stack, sizeof(housekeeping_stack), PRIO, THREAD_CREATE_SLEEPING, housekeeping_handler,
                  NULL, "housekeeping");

    kernel_pid_t dhpid = thread_create(display_stack, sizeof(display_stack), PRIO, THREAD_CREATE_SLEEPING, display_handler,
                  NULL, "display");

    thread_wakeup(hkpid);
    thread_wakeup(dhpid);

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* never reached */
    return 0;
}
