#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QFile>
#include <string>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_sendfile_clicked();

    void on_pushButton_openfile_clicked();
    void send();  //传送文件头信息
    void goOnSend(qint64);  //传送文件内容

    void on_lineEdit_dstIP_returnPressed();

    void on_lineEdit_sendSpeed_returnPressed();

//    void on_lineEdit_dstPort_cursorPositionChanged(int arg1, int arg2);

    void on_lineEdit_dstPort_returnPressed();

private:
    Ui::MainWindow *ui;
    QTcpSocket *tcpClient ;
    QFile *localFile;
    QString fileName;  //文件名

    QByteArray outBlock;  //分次传
    qint64 loadSize;  //每次发送数据的大小
    qint64 byteToWrite;  //剩余数据大小
    qint64 totalSize;  //文件总大小
    QString dspIP;
    int dstPort;

    int sendTimes;  //用来标记是否为第一次发送，第一次以后连接信号触发，后面的则手动调用
    int sendSpeed;
};
#endif // MAINWINDOW_H
