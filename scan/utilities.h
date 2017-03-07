#ifndef UTILITIES_H_INCLUDED
#define UTILITIES_H_INCLUDED

/*compute checksum*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/time.h>
#include<pthread.h>


#define DEFALUT_START_PORT 0
#define DEFALUT_END_PORT 1023
#define BUF_SIZE 10240
char g_buf[BUF_SIZE];

unsigned short my_cksum(unsigned short *data, int len);

void log_append(char* str);
void log_clear();
#endif // UTILITIES_H_INCLUDED
