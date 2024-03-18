#include "tcpclient.h"
#include <QApplication>

//#include "opewidget.h"
// #include "online.h"
//#include "friend.h"
// #include <QMessageBox>
// #include<QDebug>



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // TcpClient w;
    // w.show();
    TcpClient::getInstance().show();
    // int ret=QMessageBox::information(NULL,"添加好友","66666",QMessageBox::Yes,QMessageBox::No);
    // if(ret==QMessageBox::Yes)
    // {
    //     qDebug()<<"yes";
    // }
    // else if (ret==QMessageBox::No)
    // {
    //     qDebug()<<"no";
    // }
    // OpeWidget w;
    // w.show();

    // Online w;
    // w.show();

    // Friend w;
    // w.show();
    return a.exec();
}
