/*
 * Copyright (C) 2016 Jon Kelly
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */


#ifndef UDP_COMMON_H
#define UDP_COMMON_H


#ifdef __cplusplus
extern "C" {
#endif

void *udp_server(void *);
int udp_cmd(int argc, char **argv);
int udp_send(char *addr_str, char *data);
void dumpNodeData(void);

#ifdef __cplusplus
}
#endif
#endif /* UDP_COMMON_H */
/** @} */
