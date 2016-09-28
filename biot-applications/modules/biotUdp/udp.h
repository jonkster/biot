#ifndef UDP_COMMON_H
#define UDP_COMMON_H

#define MAX_BIG_MESSAGE_LENGTH 768
#define MAX_MESSAGE_LENGTH 512

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
void actOnOrientDataMessage(char *data);
void actOnCalibrDataMessage(char *data);
void actOnStatusDataMessage(char *data);
void relayMessage(char *cmd, char *data, char *address);

int udp_cmd(int argc, char **argv);

int udpSend(char *addr_str, char *data);

void initUdp(void);

bool setupUdpServer(void);

void syncKnown(void);

void udpGetRequestAndAct(void);

void *udpServer(void *arg);

#ifdef __cplusplus
}
#endif
#endif /* UDP_COMMON_H */
/** @} */
