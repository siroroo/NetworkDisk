#include "mytcpserver.h"
#include <QDebug>

MyTcpServer::MyTcpServer(QObject *parent)
    : QTcpServer{parent}
{

}

MyTcpServer &MyTcpServer::getInstance()
{
    static MyTcpServer instance;
    return instance;
}

void MyTcpServer::incomingConnection(qintptr socketDescriptor)
{
    qDebug()<<"new client connected";
    MyTcpSocket *pTcpSocket = new MyTcpSocket(this);
    pTcpSocket->setSocketDescriptor(socketDescriptor);
    m_tcpSocketList.append(pTcpSocket);
    connect(pTcpSocket,SIGNAL(offline(MyTcpSocket*)),this,SLOT(deleteSocket(MyTcpSocket*)));
}

void MyTcpServer::resend(const char *pername, PDU *pdu)
{
    if(pername==NULL || pdu==NULL)   return;
    //遍历链表
    QString strName=pername;
    for(int i=0;i<m_tcpSocketList.size();++i)
    {
        if(strName==m_tcpSocketList.at(i)->getName())
        {
            m_tcpSocketList.at(i)->write((char*)pdu,pdu->uiPDULen);
            break;
        }
    }
}

void MyTcpServer::deleteSocket(MyTcpSocket *mysocket)
{
    //遍历list，然后删除
    QList<MyTcpSocket*>::iterator iter=m_tcpSocketList.begin();
    for(;iter!=m_tcpSocketList.end();++iter)
    {
        if(mysocket==*iter)
        {
            delete *iter;//new的对象删除
            *iter=NULL;
            m_tcpSocketList.erase(iter);//指针删除
            break;
        }
    }
    for(int i=0;i<m_tcpSocketList.size();++i)
    {
        qDebug()<<m_tcpSocketList.at(i)->getName();
    }
}
