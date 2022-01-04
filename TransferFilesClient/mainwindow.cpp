#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QHostAddress>
#include <QMessageBox>
#include <iostream>

#pragma execution_character_set("utf-8")

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    tcpClient = new QTcpSocket(this);
    sendTimes = 0;
    //tcpClient = NULL ;
    localFile= NULL;
    fileName = "";  //文件名
    dspIP = "127.0.0.1";
    dstPort = 8000;
    sendSpeed = 100;
    ui->lineEdit_sendSpeed->setText(QString::number(sendSpeed));
    ui->lineEdit_dstPort->setText(QString::number(dstPort));
    connect(tcpClient, SIGNAL(connected()), this, SLOT(send()));  //当连接成功时，就开始传送文件
    connect(tcpClient, SIGNAL(bytesWritten(qint64)), this, SLOT(goOnSend(qint64)));

}

MainWindow::~MainWindow()
{
    delete localFile;
    delete tcpClient;
    delete ui;
}


void MainWindow::on_pushButton_sendfile_clicked()
{
    if(sendTimes == 0)  //只有第一次发送的时候，是发生在连接产生信号connect时
    {
        tcpClient->connectToHost(QHostAddress(dspIP), dstPort);
        if (!tcpClient->waitForConnected(5000))  // 连接成功则进入if{}
        {
            //ui->sendStatusLabel->setText(tr("连接失败，请确认目的主机IP地址和端口号"));
            QMessageBox::critical(this, tr("连接失败：") ,tr("请确认目的主机IP地址和端口号，且服务器已开始监听！"), QMessageBox::Ok | QMessageBox::Default , QMessageBox::Cancel | QMessageBox::Escape , 0 );
            return;
        }
        else{
            sendTimes = 1;
        }
    }
    else send();  //第一次发送的时候是由connectToHost出发connect信号才能调用send，第二次之后就需要调用send了
    ui->sendStatusLabel->setText(tr("正在发送文件 %1").arg(fileName));

}

void MainWindow::on_pushButton_openfile_clicked()
{
    ui->sendStatusLabel->setText(tr("正在打开文件..."));
    ui->sendProgressBar->setValue(0);  //非第一次发送

    loadSize = 0;
    byteToWrite = 0;
    totalSize = 0;
    outBlock.clear();

    fileName = QFileDialog::getOpenFileName(this);
    localFile = new QFile(fileName);
    localFile->open(QFile::ReadOnly);
    //std::cout<<"wenjiandaxiao:"<<localFile->size()<<std::endl;
    ui->sendStatusLabel->setText(tr("已打开文件 %1").arg(fileName));

}
void MainWindow::send()  //发送文件头信息
{
    byteToWrite = localFile->size();  //剩余数据的大小
    totalSize = localFile->size();

    loadSize = sendSpeed*1024;  //每次发送数据的大小 单位KB

    QDataStream out(&outBlock, QIODevice::WriteOnly);
    QString currentFileName = fileName.right(fileName.size() - fileName.lastIndexOf('/')-1);

    out<<qint64(0)<<qint64(0)<<currentFileName;

    totalSize += outBlock.size();  //总大小为文件大小加上文件名等信息大小
    byteToWrite += outBlock.size();

    out.device()->seek(0);  //回到字节流起点来写好前面连个qint64，分别为总大小和文件名等信息大小
    out<<totalSize<<qint64(outBlock.size());
    //总大小（整个包的大小）-头信息大小（两个qint64和一个文件名）
    tcpClient->write(outBlock);  //将读到的文件发送到套接字
    //之后触发bytesWritten信号，然后触发goOnSend，正式开始发送文件内容！！！

    //ui->progressLabel->show();
    ui->sendProgressBar->setMaximum(totalSize);
    ui->sendProgressBar->setValue(totalSize - byteToWrite);
}

void MainWindow::goOnSend(qint64 numBytes)  //开始发送文件内容
{
    byteToWrite -= numBytes;  //剩余数据大小
    outBlock = localFile->read(qMin(byteToWrite, loadSize));
    tcpClient->write(outBlock);

    ui->sendProgressBar->setMaximum(totalSize);
    ui->sendProgressBar->setValue(totalSize - byteToWrite);

    if(byteToWrite == 0)  //发送完毕
        ui->sendStatusLabel->setText(tr("%1发送完毕!").arg(fileName));
}

void MainWindow::on_lineEdit_dstIP_returnPressed()
{
    dspIP = ui->lineEdit_dstIP->text();
    //std::cout<<dspIP.toStdString()<<std::endl;
}

void MainWindow::on_lineEdit_sendSpeed_returnPressed()
{
    sendSpeed = ui->lineEdit_sendSpeed->text().toInt();
}

//void MainWindow::on_lineEdit_dstPort_cursorPositionChanged(int arg1, int arg2)
//{
//    dstPort = ui->lineEdit_dstPort->text().toInt();
//}

void MainWindow::on_lineEdit_dstPort_returnPressed()
{
    dstPort = ui->lineEdit_dstPort->text().toInt();
}
