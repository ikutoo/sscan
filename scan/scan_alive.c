#include "scan_alive.h"
#include "utilities.h"

extern int errno;

extern int sockfd;
extern struct sockaddr_in addr;
extern char straddr[128];
char sendbuf[2048];
char recvbuf[2048];
int sendnum = 0;
int recvnum = 0;
int datalen = 30;
const int pkt_num = 3;
int endflag = 0;

/*construct the icmp packet and send it*/
void send_icmp()
{
    int i;
    for (i = 0; i < pkt_num; ++i)
    {
        memset(sendbuf, 0, sizeof(sendbuf));

        struct icmp* icmp = (struct icmp*)sendbuf;
        icmp->icmp_type = ICMP_ECHO;
        icmp->icmp_code = 0;
        icmp->icmp_cksum = 0;
        icmp->icmp_id = getpid();
        icmp->icmp_seq = ++sendnum;
        gettimeofday((struct timeval*)icmp->icmp_data, NULL);
        int len = 8 + datalen;
        icmp->icmp_cksum = my_cksum((unsigned short*)icmp, len);
        int retval = sendto(sockfd, sendbuf, len, 0, (struct sockaddr*)&addr, sizeof(addr));
        if(retval == -1)
        {
            perror("sendto()");
            exit(-1);
        }
        else
        {
            char buf[64];
            sprintf(buf,"发送 icmp 请求 到 %s(%d) 字节\n", straddr, len);
            log_append(buf);
        }
        sleep(1);
    }
    endflag = 1;
}

/*analysis the icmp packet and print it in screen*/

void recv_icmp()
{
    struct timeval timeout = {0, 0};
    static int index=-1;
    fd_set sockset;

    for(; !endflag;)
    {
        FD_ZERO(&sockset);
        FD_SET(sockfd, &sockset);
        memset(recvbuf, 0, sizeof(recvbuf));

        int ret = select(sockfd+1, &sockset, NULL, NULL, &timeout);
        if (0 > ret)
        {
            exit(-1);
            break; //select 错误，退出程序
        }
        else if(0 == ret)
        {
            continue;
        }
        else
        {
            if(FD_ISSET(sockfd, &sockset)) // 测试sock是否可读，即是否网络上有数据
            {
                if(-1 == recvfrom(sockfd, recvbuf, sizeof(recvbuf), 0, 0, 0))
                {
                    if(errno == EINTR)continue;
                    else
                    {
                        perror("recvfrom()");
                        exit(-1);
                    }
                }
                else
                {
                    struct ip *ip = (struct ip*)recvbuf;
                    if(ip->ip_src.s_addr != addr.sin_addr.s_addr)
                    {
                        continue;
                    }
                    struct icmp *icmp = (struct icmp*)(recvbuf + ((ip->ip_hl)<<2));
                    if(icmp->icmp_id != getpid())
                    {
                        continue;
                    }
                    if (index==icmp->icmp_seq)
                    {
                        continue;
                    }
                    index = icmp->icmp_seq;
                    recvnum++;
                }
            } //end if break
        }
    }
}

BOOL scan_alive()
{
    pthread_t thread;
    pthread_create(&thread,NULL, (void *)(&send_icmp), NULL);
    recv_icmp();

    if (recvnum > 0)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

