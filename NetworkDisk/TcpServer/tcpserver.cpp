#include "tcpserver.h"
#include "ui_tcpserver.h"
#include <QByteArray>
#include <QDebug>
#include <QMessageBox>
#include <QHostAddress>
#include <QFile>

TcpServer::TcpServer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TcpServer)
{
    ui->setupUi(this);

    loadConfig();
    MyTcpServer::getInstance().listen(QHostAddress(m_strIP),m_usPort);
}

TcpServer::~TcpServer()
{
    delete ui;
}

void TcpServer::loadConfig()
{
    QFile file(":/server.config");
    if (file.open(QIODevice::ReadOnly)) //只读打开
    {
        QByteArray baData=file.readAll();//全部读取，返回一个 字节 bytearray 数组
        QString strData=baData.toStdString().c_str();// 字节 转为 字符串Qstring
        qDebug() << strData; //打印127.0.0.1\r\n8888
        file.close();

        //对数据进行处理
        strData.replace("\r\n"," ");
        //qDebug() << strData; //打印127.0.0.1 8888
        QStringList strList=strData.split(" ");//空格分割
        // for(int i=0;i<strList.size();i++)
        // {
        //     qDebug()<<strList[i]; //"127.0.0.1" "8888"
        // }
        m_strIP=strList.at(0);//IP
        m_usPort=strList.at(1).toUShort();//转成无符号整型
        qDebug()<<"ip:"<<m_strIP<<"port:"<<m_usPort;//ip: "127.0.0.1" port: 8888
    }
    else
    {
        QMessageBox::critical(this,"open config","open config failed");//若打开失败
    }
}
