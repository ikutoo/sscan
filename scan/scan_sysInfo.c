#include "scan_sysInfo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>
#include <netinet/ip_icmp.h>
#include <unistd.h>
#include "utilities.h"
#define MAX_SIZE 1024

struct osMatchTTL
{
    int ttl;
    char os[16];
};

struct osMatchTTL osLookUp[] = {{64,"linux"},
    {128,"windows"},
    {255,"unix"}
};

int getTTL(char* ip_addr)
{
    int size = 50 * 1024;
    int addr_len;
    int sockfd,netaddr;
    char send_buf[MAX_SIZE];
    char recv_buf[MAX_SIZE];
    struct protoent *protocol;
    struct hostent *host;
    struct sockaddr_in peer_addr;
    struct ip *ip;
    struct icmp *icmp;

    memset(send_buf,0,sizeof(send_buf));
    addr_len = sizeof(struct sockaddr);

    //获取icmp的信息
    if((protocol = getprotobyname("icmp")) == NULL)
    {
        perror("Fail to getprotobyname");
        exit(EXIT_FAILURE);
    }

    //创建原始套接字
    if((sockfd = socket(AF_INET,SOCK_RAW,protocol->p_proto)) < 0)
    {
        perror("Fail to socket");
        exit(EXIT_FAILURE);
    }


    /*
     扩大套接子接收缓冲区到50k，这样做主要为了减少接收缓冲区溢出的可能性
     若无影中ping一个广播地址或多播地址，将会引来大量应答
     */
    if(setsockopt(sockfd,SOL_SOCKET,SO_RCVBUF,&size,sizeof(size)) < 0)
    {
        perror("Fail to setsockopt");
        exit(EXIT_FAILURE);
    }

    //填充对方的地址
    bzero(&peer_addr,sizeof(peer_addr));
    peer_addr.sin_family = AF_INET;
    peer_addr.sin_addr.s_addr = inet_addr(ip_addr);



    icmp = (struct icmp *)send_buf;
    icmp->icmp_type = ICMP_ECHO;
    icmp->icmp_code = 0;
    icmp->icmp_cksum = 0;
    icmp->icmp_id = htons(getpid());
    icmp->icmp_seq = htons(12345);
    icmp->icmp_cksum = my_cksum((unsigned short *)icmp,64);


    //发送包文和接收报文
    if(sendto(sockfd,send_buf,64,0,(void*)&peer_addr,sizeof(struct sockaddr)) < 0)
    {
        fprintf(stderr,"Fail to sendto");
        return -1;
    }
    while(1)
    {
        if(recvfrom(sockfd,recv_buf,sizeof(recv_buf),0,NULL,NULL) < 0)
        {
            fprintf(stderr,"Fail to recvfrom");
            return -1;
        }

        ip = (struct ip *)recv_buf;
        icmp = (struct icmp *)(recv_buf + (ip->ip_hl << 2));

        if((ntohs(icmp->icmp_id) == getpid())&&(icmp->icmp_type == ICMP_ECHOREPLY))
        {
            shutdown(sockfd,SHUT_RDWR);
            return ip->ip_ttl;
        }
    }

    return 0;
}

void scan_sysInfo(char* ip_addr)
{
    int ttl;
    int osCount;
    int i;

    osCount = sizeof(osLookUp)/sizeof(struct osMatchTTL);
    ttl = getTTL(ip_addr);
    for(i = 0; i<osCount; i++)
    {
        if(ttl==osLookUp[i].ttl)
        {
            char buf[64];
            sprintf(buf,"operate system type is :%s\n",osLookUp[i].os);
            log_append(buf);
            break;
        }
    }
}
