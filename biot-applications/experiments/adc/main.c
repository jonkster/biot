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
#include "periph/gpio.h"
#include "net/gnrc/ipv6/nc.h"
#include "net/gnrc/ipv6/netif.h"

#define PRIO    (THREAD_PRIORITY_MAIN - 1)
#define Q_SZ    (8)
#define COAP_CONTENTTYPE_APPLICATION_JSON 60
static msg_t msg_q[Q_SZ];
bool led_status = false;
static char coap_stack[THREAD_STACKSIZE_MAIN];
//static char udp_stack[THREAD_STACKSIZE_DEFAULT-100];
uint8_t response[MAX_RESPONSE_LEN] = { 0 };

static const shell_command_t shell_commands[];

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

static int handle_get_neighbours(coap_rw_buffer_t *scratch, const coap_packet_t *inpkt,
                                 coap_packet_t *outpkt, uint8_t id_hi, uint8_t id_lo)
{
    char neighs[MAX_RESPONSE_LEN];
    strcpy(neighs, "{");
    int n = 0;

    // get neighbours that are managed by this node
    gnrc_ipv6_nc_t *entry;
    for (entry = gnrc_ipv6_nc_get_next(NULL); entry != NULL; entry = gnrc_ipv6_nc_get_next(entry))
    {
        char buf[40];
        ipv6_addr_to_str(buf, &entry->ipv6_addr, sizeof(buf));
        if ((entry->flags & GNRC_IPV6_NC_STATE_MASK) != GNRC_IPV6_NC_STATE_UNMANAGED)
        {
            if (n > 0)
                strcat(neighs, ",");
            strcat(neighs, "\"");

            strcat(neighs, buf);
            strcat(neighs, "\"");
            n++;
        }
        else
        {
        }
    }
    // add this node's address
    if (n > 0)
        strcat(neighs, ",");
    strcat(neighs, "\"affe::2\"");
    strcat(neighs, "}\n");
    int len = strlen(neighs);
    memcpy(response, neighs, len);

    return coap_make_response(scratch, outpkt, (const uint8_t *)response, len, id_hi, id_lo,
                              &inpkt->tok, COAP_RSPCODE_CONTENT, COAP_CONTENTTYPE_APPLICATION_JSON);
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

static const coap_endpoint_path_t path_neighbours =
        { 2, { "net", "neigh" } };

/* ########################################################################## */


const coap_endpoint_t endpoints[] =
{
    { COAP_METHOD_GET,	handle_get_well_known_core, &path_well_known_core, "ct=40" },

    { COAP_METHOD_GET,	handle_get_riot_board, &path_riot_board, "ct=0" },

    { COAP_METHOD_GET,	handle_get_led_status, &path_led_status, "ct=0" },

    { COAP_METHOD_GET,	handle_get_voltage, &path_voltage, "ct=0" },

    { COAP_METHOD_GET,	handle_get_neighbours, &path_neighbours, "ct=50" },

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

extern void batch(const shell_command_t *command_list, char *line);

bool isRoot = false;
void btnCallback(void* arg)
{
    if (! isRoot)
    {
        isRoot = true;
        LED_RGB_R_ON;
        LED0_ON;
        batch(shell_commands, "ifconfig 6 add affe::2");
        batch(shell_commands, "rpl root 1 affe::2");
        batch(shell_commands, "ifconfig 7 add affe::3");
        batch(shell_commands, "ncache add 7 affe::1");
        LED_RGB_OFF;
        LED0_ON;
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


int main(void)
{
    msg_init_queue(msg_q, Q_SZ);

    puts("Type 'help' for a list of available commands");

    LED0_OFF;
    LED1_ON;
    LED_RGB_OFF;

    printf("Biotz\n");
    batch(shell_commands, "rpl init 7");
    gpio_init_int(BUTTON_GPIO, GPIO_IN_PU, GPIO_RISING, (gpio_cb_t)btnCallback, NULL);

    thread_create(coap_stack, sizeof(coap_stack), PRIO, THREAD_CREATE_STACKTEST, microcoap_server,
                  NULL, "coap");
    /*thread_create(udp_stack, sizeof(udp_stack), PRIO, THREAD_CREATE_STACKTEST, udp_server,
                  NULL, "udp");*/



    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* never reached */
    return 0;
}
