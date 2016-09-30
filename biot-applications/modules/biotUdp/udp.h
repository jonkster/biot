#ifndef UDP_COMMON_H
#define UDP_COMMON_H

#include "../imu/imu.h"

#define MAX_MESSAGE_LENGTH 80

typedef struct nodeDataStructure {
    uint32_t timeStamp;
    double w;    
    double x;    
    double y;    
    double z;    
} nodeData_t;

#ifdef __cplusplus
extern "C" {
#endif

extern bool biotMsgSilent;

//void *udp_server(void *);

// message Handling routines
void actOnLedCommandMessage(char *data);
void actOnTimCommandMessage(char *data);
void actOnDofCommandMessage(char *data);
void actOnCavCommandMessage(char *data);
void actOnMcmCommandMessage(char *data);
void actOnDupCommandMessage(char *data);
void actOnRebCommandMessage(char *data);
void actOnSynCommandMessage(char *data);
void actOnOrientDataMessage(char *data, char *srcAdd);
void actOnCalibrDataMessage(char *data, char *srcAdd);
void actOnStatusDataMessage(char *data, char *srcAdd);
void relayMessage(char *cmd, char *data, char *address);

void registerNode(char *addr);

int udp_cmd(int argc, char **argv);

int udpSend(char *addr_str, char *data);

void initUdp(void);

void sendData(char *address, nodeData_t data);
void sendCalibration(char *address, int16_t *cal);
void translateStatus(imuStatus_t *status, char* buffer);
void sendStatus(char *address, imuStatus_t *status);

bool setupUdpServer(void);

void syncKnown(void);

void udpGetRequestAndAct(void);

void *udpServer(void *arg);

#ifdef __cplusplus
}
#endif
#endif /* UDP_COMMON_H */
/** @} */
