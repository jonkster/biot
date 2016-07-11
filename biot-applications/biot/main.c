#include <random.h>
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
#include "../modules/sendData/sendData.h"

#define PRIO    (THREAD_PRIORITY_MAIN + 1)
static char housekeeping_stack[THREAD_STACKSIZE_DEFAULT];
#if !defined NOOLED
static char display_stack[THREAD_STACKSIZE_DEFAULT];
#endif
static char udp_stack[THREAD_STACKSIZE_DEFAULT];

char dodagRoot[IPV6_ADDR_MAX_STR_LEN];
char dodagParent[IPV6_ADDR_MAX_STR_LEN];

extern void batch(const shell_command_t *command_list, char *line);
extern int udp_send(char *addr_str, char *data);
extern uint32_t getCurrentTime(void);
extern bool isTimeSet(void);
bool hasTimeChanged(void);
extern void timeInit(void);
extern void sendData(char *address, nodeData_t data);
/* ########################################################################## */

int identify_cmd(int argc, char **argv)
{
    identifyYourself();
    return 0;
}

bool knowsRoot(void)
{
    return (strlen(dodagRoot) > 0);
}


int findParent(void)
{
    if (gnrc_rpl_instances[0].state == 0) {
        puts("no parents...");
        return 1;
    }

    gnrc_rpl_dodag_t *dodag = &gnrc_rpl_instances[0].dodag;
    gnrc_rpl_parent_t *parents = dodag->parents;

    ipv6_addr_to_str(dodagParent, &parents->addr, sizeof(dodagParent));
    printf("parent: %s\n", dodagParent);
    //udp_send(dodagRoot, "nudge");
    return 0;
}

int findRoot(void)
{
    if (gnrc_rpl_instances[0].state == 0) {
        puts("no dodag...");
        return 1;
    }

    gnrc_rpl_dodag_t *dodag = &gnrc_rpl_instances[0].dodag;
    ipv6_addr_to_str(dodagRoot, &dodag->dodag_id, sizeof(dodagRoot));
    printf("dodag: %s\n", dodagRoot);
    findParent();
    //udp_send(dodagRoot, "nudge");
    return 0;
}

void sendNodeData(uint32_t ts)
{
    if (strlen(dodagRoot) > 0)
    {
        nodeData_t data;
        data.timeStamp = ts;
        data.w = random_uint32();
        data.x = random_uint32();
        data.y = random_uint32();
        data.z = random_uint32();
        thread_yield();
        sendData(dodagRoot, data);
    }
}


int sendTimeRequest(void)
{
    if (findParent() == 0)
    {
        udp_send(dodagParent, "time-please");
        return 0;
    }
    return 1;

    /*if (gnrc_rpl_instances[0].state == 0) {
        return 1;
    }

    gnrc_rpl_dodag_t *dodag = &gnrc_rpl_instances[0].dodag;
    ipv6_addr_to_str(dodagRoot, &dodag->dodag_id, sizeof(dodagRoot));
    udp_send(dodagRoot, "time-please");
    return 0;*/
}


int callRoot_cmd(int argc, char **argv)
{
    if (findRoot() == 0)
    {
        findParent();
        return 0;
    }
    puts("I have no DODAG");
    return 1;
}

int callTime_cmd(int argc, char **argv)
{
    if (sendTimeRequest() == 0)
    {
        return 0;
    }
    puts("I have no DODAG");
    return 1;
}

static const shell_command_t shell_commands[] = {
    /* Add a new shell commands here */
    { "identify", "visually identify board", identify_cmd },
    { "callRoot", "contact root node", callRoot_cmd },
    { "timeAsk", "ask for current net time", callTime_cmd },
    { "udp", "send a message: udp <IPv6-address> <message>", udp_cmd },
    { NULL, NULL, NULL }
};



/* set interval to 1 second */
#define INTERVAL (1000000U)
void *housekeeping_handler(void *arg)
{
    int counter = 5;
    uint32_t lastSecs = 0;
    while(1)
    {
        uint32_t secs = getCurrentTime()/1500000;
        uint32_t mSecs = getCurrentTime()/1500;
        if (mSecs % 50 == 0)
        {
            sendNodeData(mSecs);
            thread_yield();
        }

        if (secs != lastSecs)
        {
            if (secs % 2 == 0)
            {
                LED0_OFF;
                thread_yield();
            }
            else
            {
                LED0_ON;
                thread_yield();
            }
            if (knowsRoot())
            {
                if (counter++ > 30)
                {
                    /*
#if !defined NOOLED
                    oledPrint(1, "syncing" );
#endif
                    sendTimeRequest();
                    counter = 0;*/
                }
                else if (hasTimeChanged())
                {
#if !defined NOOLED
                    oledPrint(1, "changed");
#endif
                }
                else
                {
#if !defined NOOLED
                    oledPrint(1, "Node Member" );
#endif
                }
#if !defined NOOLED
                char st[12];
                sprintf(st, "T=%lu", secs);
                oledPrint(2, st);
#endif
            }
            else
            {
                findRoot();
                if (knowsRoot())
                {
#if !defined NOOLED
                    oledPrint(1, "Node Member" );
#endif
                    sendTimeRequest();
                }
                else
                {
#if !defined NOOLED
                    oledPrint(1, "orphan..." );
#endif
                    //batch(shell_commands, "rpl init 6");
                }
            }
        }
        lastSecs = secs;
        thread_yield();
    }
}

int main(void)
{
    puts("Biotz Node\n");
    LED0_OFF;


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

    timeInit();
    sendTimeRequest();

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* never reached */
    return 0;
}
