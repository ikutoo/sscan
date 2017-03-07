#ifndef CMAINWIN_H
#define CMAINWIN_H

#include <QMainWindow>
#include <QTextBrowser>
namespace Ui {
class CMainWin;
}

class CMainWin : public QMainWindow
{
    Q_OBJECT

public:
    explicit CMainWin(QWidget *parent = 0);
    ~CMainWin();
    QTextBrowser* scan_result;
private slots:
    void on_pushButton_start_scan_clicked();

    void on_pushButton_stop_scan_clicked();

    void on_pushButton_report_clicked();

    void on_radioButton_alive_clicked(bool checked);

    void on_radioButton_port_clicked(bool checked);

    void on_radioButton_ftp_clicked(bool checked);

    void on_radioButton_cgi_clicked(bool checked);

    void on_checkBox_smtp_clicked(bool checked);

private:
    Ui::CMainWin *ui;
    void mScan(const char* hostname);
    void mUpdateBrowser();

    bool m_bScanAlive ;
    bool m_bScanPort ;
    bool m_bScanFtp ;
    bool m_bScanCgi ;
    bool m_bScanSmtp ;
};

#endif // CMAINWIN_H
