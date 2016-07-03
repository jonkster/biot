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
#include "net/gnrc/rpl/dodag.h"
#include "shell.h"
#include "../modules/ssd1306/ssd1306.h"
#include "../modules/biotUdp/udp.h"
#include "../modules/identify/biotIdentify.h"

#define PRIO    (THREAD_PRIORITY_MAIN + 1)
static char housekeeping_stack[THREAD_STACKSIZE_DEFAULT];
#if !defined NOOLED
static char display_stack[THREAD_STACKSIZE_DEFAULT];
#endif
static char udp_stack[THREAD_STACKSIZE_DEFAULT];

char dodagRoot[IPV6_ADDR_MAX_STR_LEN];

extern void batch(const shell_command_t *command_list, char *line);
extern int udp_send(char *addr_str, char *data);
/* ########################################################################## */

int identify_cmd(int argc, char **argv)
{
    identifyYourself();
    return 0;
}

int nudgeRoot(void)
{
    if (gnrc_rpl_instances[0].state == 0) {
        return 1;
    }

    gnrc_rpl_dodag_t *dodag = &gnrc_rpl_instances[0].dodag;
    ipv6_addr_to_str(dodagRoot, &dodag->dodag_id, sizeof(dodagRoot));
    udp_send(dodagRoot, "nudge");
    return 0;
}

int callRoot_cmd(int argc, char **argv)
{
    if (nudgeRoot() == 0)
    {
        printf("DODAG root: %s\n", dodagRoot);
        return 0;
    }
    puts("I have no DODAG");
    return 1;
}

static const shell_command_t shell_commands[] = {
    /* Add a new shell commands here */
    { "identify", "visually identify board", identify_cmd },
    { "callRoot", "contact root node", callRoot_cmd },
    { "udp", "send a message: udp <IPv6-address> <message>", udp_cmd },
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

        if (nudgeRoot() == 0)
        {
            char msg[IPV6_ADDR_MAX_STR_LEN+10];
            sprintf(msg, "DODAG:%s", dodagRoot);
#if !defined NOOLED
            oledPrint(1, "dodag");
#endif
        }
        else
        {
#if !defined NOOLED
            oledPrint(1, "orphan...");
#endif
        }
        char st[10];
        sprintf(st, "%d", i++);
#if !defined NOOLED
        oledPrint(2, st);
#endif
    }
}

int main(void)
{
//    kernel_pid_t ifs[GNRC_NETIF_NUMOF];

    puts("Biotz Node\n");


#if !defined NOOLED
    thread_create(display_stack, sizeof(display_stack), PRIO, THREAD_CREATE_STACKTEST, (thread_task_func_t) display_handler,
                  NULL, "display");
#endif

    thread_create(housekeeping_stack, sizeof(housekeeping_stack), PRIO, THREAD_CREATE_STACKTEST, housekeeping_handler,
                  NULL, "housekeeping");

    thread_create(udp_stack, sizeof(udp_stack), PRIO, THREAD_CREATE_STACKTEST, udp_server,
                  NULL, "udp");

    batch(shell_commands, "rpl init 6");

    identifyYourself();

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* never reached */
    return 0;
}
