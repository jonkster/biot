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
#include "../modules/position/position.h"
#include "../modules/imu/imu.h"

#define PRIO    (THREAD_PRIORITY_MAIN + 1)
static char housekeeping_stack[THREAD_STACKSIZE_DEFAULT+1024];
static char udp_stack[THREAD_STACKSIZE_DEFAULT + 1024];

char dodagRoot[IPV6_ADDR_MAX_STR_LEN];
char dodagParent[IPV6_ADDR_MAX_STR_LEN];

extern void batch(const shell_command_t *command_list, char *line);
extern int udp_send(char *addr_str, char *data);
extern uint32_t getCurrentTime(void);
//extern bool isTimeSet(void);
//bool hasTimeChanged(void);
extern void timeInit(void);
extern void sendData(char *address, nodeData_t data);
bool imuReady = false;
myQuat_t currentPosition;
/* ########################################################################## */

mpu9150_t imuDev;

int identify_cmd(int argc, char **argv)
{
    identifyYourself("");
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
    return 0;
}

int findRoot(void)
{
    puts("where is root?");
    if (gnrc_rpl_instances[0].state == 0) {
        return 1;
    }

    gnrc_rpl_dodag_t *dodag = &gnrc_rpl_instances[0].dodag;
    ipv6_addr_to_str(dodagRoot, &dodag->dodag_id, sizeof(dodagRoot));
    printf("dodag: %s\n", dodagRoot);
    findParent();
    return 0;
}

void updatePosition(void)
{
    currentPosition = getPosition(imuDev);
}

void sendNodeCalibration(void)
{
    int16_t *data = getMagCalibration();
    if (strlen(dodagRoot) > 0)
    {
        sendCalibration(dodagRoot, data);
    }
    else
    {
        sendCalibration("affe::2", data);
    }
}


void sendNodeData(uint32_t ts)
{
    nodeData_t data;
    data.timeStamp = ts;
    data.w = currentPosition.w;
    data.x = currentPosition.x;
    data.y = currentPosition.y;
    data.z = currentPosition.z;
    thread_yield();
    if (strlen(dodagRoot) > 0)
    {
        sendData(dodagRoot, data);
    }
    else
    {
        sendData("affe::2", data);
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

void initIMU(void)
{
    if (initialiseIMU(&imuDev))
    {
        displayConfiguration(imuDev);
    }
    else
    {
        puts("could not initialise IMU device");
    }
}

int imu_cmd(int argc, char **argv)
{
    imuData_t imuData;
    if (getIMUData(imuDev, &imuData))
    {
        displayData(imuData);
        return 0;
    }
    else
    {
        puts("could not read IMU device");
        return 1;
    }
}

int mag_cmd(int argc, char **argv)
{
    displayCorrections();
    return 0;
}



int quat_cmd(int argc, char **argv)
{
    imuData_t imuData;
    if (getIMUData(imuDev, &imuData))
    {
        double x = (double)imuData.accel.x_axis/1024;
        double y = (double)imuData.accel.y_axis/1024;
        double z = (double)imuData.accel.z_axis/1024;

        double from[3] = {0, 1, 0};
        double to[3] = {x, y, z};
	myQuat_t q = quatFrom2Vecs(from, to);
	dumpQuat(q);
        return 0;
    }
    else
    {
        puts("could not read IMU device");
        return 1;
    }
}

static const shell_command_t shell_commands[] = {
    /* Add a new shell commands here */
    { "identify", "visually identify board", identify_cmd },
    { "callRoot", "contact root node", callRoot_cmd },
    { "timeAsk", "ask for current net time", callTime_cmd },
    { "udp", "send a message: udp <IPv6-address> <message>", udp_cmd },
    { "imu", "get IMU position data", imu_cmd },
    { "quat", "get position quaternion", quat_cmd },
    { "mag", "display compass correction data", mag_cmd },
    { NULL, NULL, NULL }
};



/* set interval to 1 second */
#define INTERVAL (1000000U)
void *housekeeping_handler(void *arg)
{
    uint32_t lastSecs = 0;
    while(1)
    {
        if (! imuReady)
        {
            initIMU();
            imuReady = true;
        }

        uint32_t secs = getCurrentTime()/1500000;
        uint32_t mSecs = getCurrentTime()/1500;
        if (mSecs % 50 == 0)
        {
            updatePosition();
            sendNodeData(mSecs);
        }

        if (secs != lastSecs)
        {
            if (secs % 2 == 0)
            {
                LED0_OFF;
                sendNodeCalibration();
            }
            else
            {
                LED0_ON;
            }
            
            if (! knowsRoot())
            {
                findRoot();
                if (knowsRoot())
                {
                    sendTimeRequest();
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

    makeIdentityQuat(&currentPosition);

    thread_create(housekeeping_stack, sizeof(housekeeping_stack), PRIO, THREAD_CREATE_STACKTEST, housekeeping_handler,
                  NULL, "housekeeping");

    thread_create(udp_stack, sizeof(udp_stack), PRIO, THREAD_CREATE_STACKTEST, udp_server,
                  NULL, "udp");

    batch(shell_commands, "rpl init 6");

    identifyYourself("");

    timeInit();
    sendTimeRequest();


    puts("starting shell");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* never reached */
    return 0;
}
