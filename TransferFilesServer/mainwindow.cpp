#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtNetwork/QNetworkInterface>
#include <QFileDialog>
#include <QMessageBox>
//#include<iostream>

#pragma execution_character_set("utf-8")

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    server = NULL ;
    receivedSocket = NULL;
    newFile = NULL;
    savePath = "D:";
    findNoExits = false;
    localPort = 8000;
    foreach(const QHostAddress& hostAddress,QNetworkInterface::allAddresses())
    //  这里我们判断ip地址是否有效kdlsk;jfg
    {
        if ( hostAddress != QHostAddress::LocalHost && hostAddress.toIPv4Address() )
            localIp = hostAddress.toString();
    }
    ui->label_localIP->setText(localIp);
    ui->lineEdit_localPort->setText(QString::number(localPort));
    //label_localIP
    //ui->progressLabel->hide();
    //QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
}

MainWindow::~MainWindow()
{
    delete server;
    delete newFile;
    delete ui;
}

void MainWindow::acceptConnection()
{
    ui->receivedStatusLabel->setText(tr("已连接，正在准备接收文件!"));

    receivedSocket = server->nextPendingConnection();
    connect(receivedSocket, SIGNAL(readyRead()), this, SLOT(readClient()));
}

void MainWindow::readClient()
{
    ui->receivedStatusLabel->setText(tr("正在接收文件..."));

    if(byteReceived == 0)  //才刚开始接收数据，此数据为文件信息
    {
        ui->receivedProgressBar->setValue(0);
        QDataStream in(receivedSocket);
        in>>totalSize>>byteReceived>>fileName;
        fileName = savePath+"/" + fileName;
        QFileInfo file(fileName);
        if(file.exists()){
            QMessageBox::critical(this, tr("请注意：") ,tr("当前路径下已有同名文件，请重新选择保存文件路径！"), QMessageBox::Ok | QMessageBox::Default , QMessageBox::Cancel | QMessageBox::Escape , 0 );
            findNoExits = true;
//            this->~MainWindow();
//            exit(0);
//            return;
        }else{
            newFile = new QFile(fileName);
            newFile->open(QFile::WriteOnly);
//            fileExits = true;
        }

    }
    else  //正式读取文件内容
    {
        //如果没有创建文件，则在这里阻塞 主线程会不会被阻塞？？
        //判断是否为NULL
        if(newFile == NULL){
            //_sleep(1);
//            QThread::sleep
            return;//等待文件创建后重新调用
        }
        inBlock = receivedSocket->readAll();

        byteReceived += inBlock.size();
        newFile->write(inBlock);
        newFile->flush();
    }

    //ui->progressLabel->show();
    ui->receivedProgressBar->setMaximum(totalSize);
    ui->receivedProgressBar->setValue(byteReceived);

    if(byteReceived == totalSize)
    {
        ui->receivedStatusLabel->setText(tr("%1接收完成").arg(fileName));

        inBlock.clear();
        byteReceived = 0;
        totalSize = 0;
        newFile->close();
        newFile = NULL;
        findNoExits = false;

    }
}

void MainWindow::on_pushButton_startlisten_clicked()
{
    totalSize = 0;
    byteReceived = 0;

    server = new QTcpServer(this);
    //server->listen(QHostAddress("172.19.198.43"), 10000);
    server->listen(QHostAddress::Any, localPort);

    connect(server, SIGNAL(newConnection()), this, SLOT(acceptConnection()));

    ui->receivedProgressBar->setValue(0);
    ui->receivedStatusLabel->setText(tr("开始监听..."));
    ui->pushButton_startlisten->setEnabled(false);
}

void MainWindow::on_pushButton_clicked()
{
    savePath = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "D:/", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    ui->label_savepath->setText(savePath);
    if(findNoExits&&byteReceived!=0)
    {   //要更改fileName！！！
        fileName =savePath +"/" + fileName.right(fileName.size() - fileName.lastIndexOf('/')-1);
        QFileInfo file(fileName);
        if(file.exists()){
            QMessageBox::critical(this, tr("请注意：") ,tr("当前路径下已有同名文件，请重启，并选择新的保存文件路径！"), QMessageBox::Ok | QMessageBox::Default , QMessageBox::Cancel | QMessageBox::Escape , 0 );
            return;
        }
        newFile = new QFile(fileName);
        newFile->open(QFile::WriteOnly);
        readClient();

    }
}

void MainWindow::on_lineEdit_localPort_returnPressed()
{
    localPort = ui->lineEdit_localPort->text().toInt();
}
