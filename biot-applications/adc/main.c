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
#include <coap.h>
#include "board.h"
#include "thread.h"
#include "coap_common.h"
#include "udp_common.h"

#define PRIO    (THREAD_PRIORITY_MAIN - 1)
#define Q_SZ    (8)
static msg_t msg_q[Q_SZ];
bool led_status = false;
static char coap_stack[THREAD_STACKSIZE_MAIN];
static char udp_stack[THREAD_STACKSIZE_DEFAULT];

uint8_t response[MAX_RESPONSE_LEN] = { 0 };

static int handle_get_riot_board(coap_rw_buffer_t *scratch, const coap_packet_t *inpkt,
                                 coap_packet_t *outpkt, uint8_t id_hi, uint8_t id_lo)
{
    const char *riot_name = RIOT_BOARD;
    int len = strlen(RIOT_BOARD);

    memcpy(response, riot_name, len);

    return coap_make_response(scratch, outpkt, (const uint8_t *)response, len, id_hi, id_lo,
                              &inpkt->tok, COAP_RSPCODE_CONTENT, COAP_CONTENTTYPE_TEXT_PLAIN);
}

/* add a new callback function here ######################################### */

static int handle_get_led_status(coap_rw_buffer_t *scratch, const coap_packet_t *inpkt,
                                 coap_packet_t *outpkt, uint8_t id_hi, uint8_t id_lo)
{
    char *led_msg = NULL;;
    int len = 0;

    if (led_status) {
        led_msg = "LED is ON";
    }
    else {
        led_msg = "LED is OFF";
    }

    len = strlen(led_msg);
    memcpy(response, led_msg, len);

    return coap_make_response(scratch, outpkt, (const uint8_t *)response, len, id_hi, id_lo,
                              &inpkt->tok, COAP_RSPCODE_CONTENT, COAP_CONTENTTYPE_TEXT_PLAIN);
}

extern int getVoltage(void);
static int handle_get_voltage(coap_rw_buffer_t *scratch, const coap_packet_t *inpkt,
                                 coap_packet_t *outpkt, uint8_t id_hi, uint8_t id_lo)
{
    int v = getVoltage();
    char volts[20];
    sprintf(volts, "%dmV", v);
    int len = strlen(volts);
    memcpy(response, volts, len);
    return coap_make_response(scratch, outpkt, (const uint8_t *)response, len, id_hi, id_lo,
                              &inpkt->tok, COAP_RSPCODE_CONTENT, COAP_CONTENTTYPE_TEXT_PLAIN);
}

/* ########################################################################## */

static const coap_endpoint_path_t path_well_known_core =
        { 2, { ".well-known", "core" } };

static const coap_endpoint_path_t path_riot_board =
        { 2, { "riot", "board" } };

static const coap_endpoint_path_t path_led_status =
        { 2, { "led", "status" } };

static const coap_endpoint_path_t path_voltage =
        { 2, { "adc", "voltage" } };

/* ########################################################################## */


const coap_endpoint_t endpoints[] =
{
    { COAP_METHOD_GET,	handle_get_well_known_core, &path_well_known_core, "ct=40" },
    { COAP_METHOD_GET,	handle_get_riot_board, &path_riot_board, "ct=0" },


    { COAP_METHOD_GET,	handle_get_led_status, &path_led_status, "ct=0" },
    { COAP_METHOD_GET,	handle_get_voltage, &path_voltage, "ct=0" },

    /* marks the end of the endpoints array: */
    { (coap_method_t)0, NULL, NULL, NULL }
};

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

int main(void)
{
    msg_init_queue(msg_q, Q_SZ);

    puts("Type 'help' for a list of available commands");

    LED0_OFF;
    LED1_ON;
    LED_RGB_OFF;

    thread_create(coap_stack, sizeof(coap_stack), PRIO, THREAD_CREATE_STACKTEST, microcoap_server,
                  NULL, "coap");
    thread_create(udp_stack, sizeof(udp_stack), PRIO, THREAD_CREATE_STACKTEST, udp_server,
                  NULL, "udp");

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* never reached */
    return 0;
}
