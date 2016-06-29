/*
 * Copyright (C) 2015 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Showing minimum memory footprint of gnrc network stack
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 *
 * @}
 */

#include <stdio.h>
#include <thread.h>
#include <xtimer.h>

#include "msg.h"
#include "net/ipv6/addr.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/ipv6/netif.h"
#include "shell.h"

#define PRIO    (THREAD_PRIORITY_MAIN + 1)
static char housekeeping_stack[THREAD_STACKSIZE_DEFAULT];
static char display_stack[THREAD_STACKSIZE_DEFAULT];

/* ########################################################################## */
extern  void *display_handler(void *arg);
extern int oledCmd(int argc, char **argv);
extern int oledClearAll(void);
extern void oledWriteText(const char *string);

static const shell_command_t shell_commands[] = {
    /* Add a new shell commands here */
    { NULL, NULL, NULL }
};



/* set interval to 1 second */
#define INTERVAL (1000000U)
void *housekeeping_handler(void *arg)
{
   int factor = 1; 
   int i = 0;
    while(1)
    {
        uint32_t last_wakeup = xtimer_now();
        thread_yield();
        xtimer_usleep_until(&last_wakeup, INTERVAL/(2*factor));
        LED0_OFF;
        thread_yield();
        xtimer_usleep_until(&last_wakeup, INTERVAL/factor);
        LED0_ON;

        char st[10];
        sprintf(st, "%d", i++);
        oledClearAll();
        oledWriteText(st);
    }
}

int main(void)
{
    kernel_pid_t ifs[GNRC_NETIF_NUMOF];

    puts("Biotz Shell+UDP+OLED experiment\n");

    thread_create(housekeeping_stack, sizeof(housekeeping_stack), PRIO, THREAD_CREATE_STACKTEST, housekeeping_handler,
                  NULL, "housekeeping");

    thread_create(display_stack, sizeof(display_stack), PRIO, THREAD_CREATE_STACKTEST, display_handler,
                  NULL, "display");


    /* get the first IPv6 interface and prints its address */
    size_t numof = gnrc_netif_get(ifs);
    if (numof > 0) {
        gnrc_ipv6_netif_t *entry = gnrc_ipv6_netif_get(ifs[0]);
        for (int i = 0; i < GNRC_IPV6_NETIF_ADDR_NUMOF; i++) {
            if ((ipv6_addr_is_link_local(&entry->addrs[i].addr)) && !(entry->addrs[i].flags & GNRC_IPV6_NETIF_ADDR_FLAGS_NON_UNICAST)) {
                char ipv6_addr[IPV6_ADDR_MAX_STR_LEN];
                ipv6_addr_to_str(ipv6_addr, &entry->addrs[i].addr, IPV6_ADDR_MAX_STR_LEN);
                printf("My address is %s\n", ipv6_addr);
            }
        }
    }

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* never reached */
    return 0;
}
