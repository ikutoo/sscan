#ifndef SCAN_PORT_H_INCLUDED
#define SCAN_PORT_H_INCLUDED

#include<stdlib.h>
#include<stdio.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<unistd.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<pthread.h>


#define DEFALUT_START_PORT 0
#define DEFALUT_END_PORT 1023

void scan_port(char* ip_addr, int port_start, int port_end);

#endif // SCAN_PORT_H_INCLUDED
