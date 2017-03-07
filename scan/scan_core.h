#ifndef SCAN_CORE_H
#define SCAN_CORE_H

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

int scan_cgi(char* ip_addr);
int scan_ftp(char* ip_addr);
int scan_smtp(char* ip_addr);


#endif // SCAN_CORE_H
