#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include <QTcpServer>
#include <QList>
#include "mytcpsocket.h"

class MyTcpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit MyTcpServer(QObject *parent = nullptr);
    static MyTcpServer &getInstance();
    //重写父类的虚函数
    void incomingConnection(qintptr socketDescriptor);
    void resend(const char*pername,PDU *pdu);
    bool forwardMsg(const QString caDesName, PDU *pdu); // 服务器转发给其他客户端消息

public slots:
    void deleteSocket(MyTcpSocket *mysocket);

private:

    QList<MyTcpSocket*> m_tcpSocketList;

};

#endif // MYTCPSERVER_H
