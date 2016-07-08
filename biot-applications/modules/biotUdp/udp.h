#ifndef UDP_COMMON_H
#define UDP_COMMON_H

#define MAX_MESSAGE_LENGTH 1024

#ifdef __cplusplus
extern "C" {
#endif

void *udp_server(void *);
int udp_cmd(int argc, char **argv);
int udp_send(char *addr_str, char *data);
void dumpNodeData(void);
char *nodeDataJson(void);

#ifdef __cplusplus
}
#endif
#endif /* UDP_COMMON_H */
/** @} */
