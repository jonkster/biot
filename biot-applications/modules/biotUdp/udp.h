#ifndef UDP_COMMON_H
#define UDP_COMMON_H

#define MAX_BIG_MESSAGE_LENGTH 512
#define MAX_MESSAGE_LENGTH 768

#ifdef __cplusplus
extern "C" {
#endif

void *udp_server(void *);

int udp_cmd(int argc, char **argv);

int udpSend(char *addr_str, char *data);

void dumpNodeData(void);

char *nodeCalibrationJson(void);

void dumpNodeCalibration(void);

char *nodeDataJson(void);

void syncKnown(void);

#ifdef __cplusplus
}
#endif
#endif /* UDP_COMMON_H */
/** @} */
