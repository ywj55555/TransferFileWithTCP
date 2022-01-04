#include<iostream>
#include <QString>
#include <QtNetwork/QHostInfo>
#include <QtNetwork/QNetworkInterface>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "winsock2.h"

#pragma comment(lib,"ws2_32.lib")

using std::cout;
using std::endl;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    WSADATA wsaData;
    char name[155];
    char *ip;
    PHOSTENT hostinfo;
    if ( WSAStartup( MAKEWORD(2,0), &wsaData ) == 0 )
    {
      if( gethostname ( name, sizeof(name)) == 0)
            {
                if((hostinfo = gethostbyname(name)) != NULL)
                 {
                    ip = inet_ntoa (*(struct in_addr *)*hostinfo->h_addr_list);
                     printf("当前服务器IP:%s\n", ip);
                }
             }
             WSACleanup( );
        }
}

int MainWindow::init_client_socket(){
    WORD	wVersionRequested;
    WSADATA wsaData;
    int		err,iLen;
    wVersionRequested	=	MAKEWORD(2,2);//create 16bit data

    //(1)Load WinSock
    err	=	WSAStartup(wVersionRequested,&wsaData);	//load win socket
    if(err!=0){
        std::cout<<"Load WinSock Failed!" << std::endl;
        return -1;
    }

    //(2)create socket
    SOCKET sockClt = socket(AF_INET,SOCK_STREAM,0);
    if(sockClt == INVALID_SOCKET){
        std::cout<<"socket() fail:"<<WSAGetLastError()<<std::endl;
        return -2;
    }

    //(3)IP
    SOCKADDR_IN addrSrv;
    addrSrv.sin_family = AF_INET;
    addrSrv.sin_addr.s_addr = inet_addr(DEFAULT_SERVER_ADDR);
    addrSrv.sin_port = htons(DEFAULT_PORT);

    //(5)connect
    err = ::connect(sockClt,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));

    if(err == INVALID_SOCKET){
        std::cout<<"connect() fail "<<WSAGetLastError()<<std::endl;
        return -1;
    }
    cout<<"connect successfully!!!"<<endl;
//    int height = 2040;
//    int width = 2040;
//    int channels = 1;
//    int size = height * width * channels;
//    ushort* recvBuf = new ushort[size];

//    while (true){
//        iLen = ::recv(sockClt,(char*)recvBuf, size * sizeof(ushort),0);
//        if (iLen == 0){
//            break;
//        }else if (iLen == SOCKET_ERROR){
//            std::cout<<"recv() fail:"<<WSAGetLastError()<< std::endl;
//            break;
//        }

//    }

    closesocket(sockClt);
    WSACleanup();
    //system("PAUSE");
    return 0;
}

void MainWindow::on_pushButton_2_clicked()
{
    int res = init_client_socket();
    if(res==0)cout<<"connetc successfully"<<endl;
    else cout<<"failled!!"<<endl;
}

void MainWindow::on_pushButton_3_clicked()
{
    // 使用Qt的网络包
    // 获取ip地址
        // 循环遍历获取所有的ip地址
        foreach(const QHostAddress& hostAddress,QNetworkInterface::allAddresses())
        //  这里我们判断ip地址是否有效kdlsk;jfg
        {
            if ( hostAddress != QHostAddress::LocalHost && hostAddress.toIPv4Address() )
                localIp = hostAddress.toString().toStdString();
            //  这里我们就直接返回本机ip地址
            //localIp = QHostAddress(QHostAddress::LocalHost).toString().toStdString();
        }
        cout<<localIp<<endl;


}
