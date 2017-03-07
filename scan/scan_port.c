#include "scan_port.h"

int thread_num;
int seg_len = 20;

struct port_segment
{
    char* ip_addr;
    int min_port;
    int max_port;
};

int checkConnect(char * addr, int port)
{
    struct sockaddr_in server;
    int msockfd;
    int ret;

    if (-1 == (msockfd = socket(AF_INET, SOCK_STREAM, 0)))
    {
        close(msockfd);
        return -1;
    }
    memset(&server, 0, sizeof(struct sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(addr);
    server.sin_port = htons(port);
    if (0 > (ret = connect(msockfd, (struct sockaddr*)&server, sizeof(struct sockaddr))))
    {
        close(msockfd);
        return 0;
    }
    close(msockfd);
    return 1;
}

void *scan( void *arg )
{
    int i;
    struct port_segment* seg=(struct port_segment*)arg;
    char * ip_addr=seg->ip_addr;
    int start_port=seg->min_port;
    int end_port=seg->max_port;
    struct servent * pServent;

    for (i = start_port; i <= end_port; i++)
    {
        if (1 != checkConnect(ip_addr, i))
        {
            continue;
        }
        pServent = getservbyport(htons(i), NULL);
        if (NULL == pServent)
        {
            char buf[64];
            sprintf(buf,"port:%d\t unknow \n", i);
            log_append(buf);
        }
        else
        {
            char buf[64];
            sprintf(buf,"port:%d\t name:%s \n", i, pServent->s_name);
            log_append(buf);
        }
    }
}

void scan_port(char* ip_addr, int port_start, int port_end)
{
    int i;
    pthread_t * thread;
    struct port_segment * port;

    thread_num = (port_end - port_start) / seg_len + 1;
    if (thread_num > 100)
    {
        thread_num = 100;
        seg_len = (port_end - port_start) / thread_num + 1;
    }

    thread = (pthread_t *)malloc( thread_num * sizeof(pthread_t) );
    port = (struct port_segment *)malloc( thread_num * sizeof(struct port_segment));

    for (i = 0; i < thread_num; ++i )
    {
        port[i].ip_addr = ip_addr;
        port[i].min_port = i * seg_len + port_start;
        if (i == (thread_num - 1) )
            port[i].max_port = port_end;
        else
            port[i].max_port = port[i].min_port + seg_len - 1;

        if (pthread_create(&thread[i], NULL, scan, (void *)&port[i]) != 0 )
        {
            printf("pthread_create failed\n");
            free(thread);
            exit(-2);
        }
    }
    for (i = 0; i < thread_num; ++i )
    {
        pthread_join(thread[i],NULL);
    }
}
