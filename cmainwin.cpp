#include "cmainwin.h"
#include "ui_cmainwin.h"
#include <QDebug>
#include <QFile>
#include <QProcess>
extern "C"
{
#include "scan/scan_alive.h"
#include "scan/scan_port.h"
#include "scan/scan_sysInfo.h"
#include "scan/scan_core.h"
#include "scan/utilities.h"
#include <unistd.h>
}
int sockfd;
struct sockaddr_in addr;
char straddr[128];

CMainWin::CMainWin(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CMainWin)
{
    ui->setupUi(this);
    scan_result = ui->textBrowser;
    m_bScanAlive = false;
    m_bScanPort = false;
    m_bScanFtp = false;
    m_bScanCgi = false;
    m_bScanSmtp = false;
}

CMainWin::~CMainWin()
{
    delete ui;
}

void CMainWin::mScan(const char*  hostname)
{
    log_clear();

    int port_start;
    int port_end;

    port_start = DEFALUT_START_PORT;
    port_end = DEFALUT_END_PORT;

    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if(sockfd == -1)
    {
        perror("socket()");
        return ;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    int retval = inet_pton(AF_INET, hostname, &addr.sin_addr);
    if(retval == -1 || retval == 0)
    {
        struct hostent* host = gethostbyname(hostname);
        if(host == NULL)
        {
            fprintf(stderr, "gethostbyname(%s):%s\n",hostname, strerror(errno));
            //close(sockfd);
            exit(-1);
        }

        if(host->h_addr_list != NULL && *(host->h_addr_list) != NULL)
        {
            strncpy((char*)&addr.sin_addr, *(host->h_addr_list), 4);
            inet_ntop(AF_INET, *(host->h_addr_list), straddr, sizeof(straddr));
        }
    }
    else
    {
        strcpy(straddr, hostname);
    }

    if (m_bScanAlive)
    {

        log_append("开始目标主机存活性扫描...\n");
        mUpdateBrowser();
        BOOL alive = scan_alive();
        if (!alive)
        {
            log_append("目标主机死亡!\n");
        }
        else
        {
            log_append("目标主机存活!\n");
        }
        mUpdateBrowser();
    }

    if (m_bScanPort)
    {
        log_append("开始目标主机开放端口扫描...\n");
        mUpdateBrowser();
        scan_port(straddr, port_start, port_end);
        mUpdateBrowser();
    }

    //    if (m_bScanSysInfo)
    //    {
    //        log_append("开始检测目标主机操作系统版本...\n");
    //        mUpdateBrowser();
    //        scan_sysInfo(straddr);
    //        mUpdateBrowser();
    //    }

	    if (m_bScanCgi)
	    {
	        log_append("开始CGI漏洞扫描...\n");
	        mUpdateBrowser();
	        if (0 != scan_cgi(straddr))
	        {
	            log_append("CGI漏洞扫描失败！\n");
	        }
	        mUpdateBrowser();
	    }

	    if (m_bScanFtp)
	    {
	        log_append("开始FTP漏洞扫描...\n");
	        mUpdateBrowser();
	        if (0 != scan_ftp(straddr))
	        {
	            log_append("FTP漏洞扫描失败！\n");
	        }
	        mUpdateBrowser();
	    }

	    if (m_bScanSmtp)
	    {
	        log_append("开始SMTP漏洞扫描...\n");
	        mUpdateBrowser();
	        if (0 != scan_smtp(straddr))
	        {
	            log_append("SMTP漏洞扫描失败！\n");
	        }
	        mUpdateBrowser();
	    }

	    //close(sockfd);
	}

void CMainWin::mUpdateBrowser()
{
    scan_result->setText(g_buf);
    scan_result->repaint();
}

void CMainWin::on_pushButton_start_scan_clicked()
{
    QString hostname = ui->lineEdit_ip->text();
    mScan(hostname.toStdString().c_str());
}

void CMainWin::on_pushButton_stop_scan_clicked()
{

}

void CMainWin::on_pushButton_report_clicked()
{
    QString str = g_buf;
    QFile file("report.txt");
    if (!file.open(QIODevice::WriteOnly|QIODevice::Text)) {

        return ;
    }
    QTextStream out(&file);
    out<<str<<endl;
    out.flush();
    file.close();
    QProcess::execute("gedit report.txt");
}

void CMainWin::on_radioButton_alive_clicked(bool checked)
{
    m_bScanAlive = checked;
}

void CMainWin::on_radioButton_port_clicked(bool checked)
{
    m_bScanPort = checked;
}

void CMainWin::on_radioButton_ftp_clicked(bool checked)
{
    m_bScanFtp = checked;
}

void CMainWin::on_radioButton_cgi_clicked(bool checked)
{
    m_bScanCgi = checked;
}

void CMainWin::on_checkBox_smtp_clicked(bool checked)
{
    m_bScanSmtp = checked;
}
