#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QFile>
#include <QString>


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
    void acceptConnection();
    void readClient();
    void on_pushButton_startlisten_clicked();

    void on_pushButton_clicked();

    void on_lineEdit_localPort_returnPressed();

private:
    Ui::MainWindow *ui;

    QTcpServer *server;
    QTcpSocket *receivedSocket;
    QFile *newFile;

    QByteArray inBlock;
    QString fileName;
    qint64 totalSize;  //总共需要发送的文件大小（文件内容&文件名信息）
    qint64 byteReceived;  //已经发送的大小

    QString localIp;
    QString savePath;
    bool findNoExits;
    int localPort;

};
#endif // MAINWINDOW_H
