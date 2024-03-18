#include "mytcpsocket.h"
#include <QDebug>
#include<stdio.h>
#include "mytcpserver.h"

MyTcpSocket::MyTcpSocket(QObject *parent)
    : QTcpSocket{parent}
{
    //各自的socket自己产生的消息，各自处理
    //每一个客户端都有自己的socket
    connect(this,SIGNAL(readyRead()),this,SLOT(recvMsg()));
    connect(this,SIGNAL(disconnected()),this,SLOT(clientOffline()));

    //connect(this, &QIODevice::readyRead, this, &MyTcpSocket::recvMsg);
}

QString MyTcpSocket::getName()
{
    return m_strName;
}

void MyTcpSocket::recvMsg()
{
    qDebug()<<this->bytesAvailable();
    //收数据
    uint uiPDULen =0;
    this->read((char*)&uiPDULen,sizeof(uint));  //收总大小
    uint uiMsgLen=uiPDULen - sizeof(PDU);
    PDU *pdu=mkPDU(uiMsgLen);
    this->read((char*)pdu+sizeof(uint),uiPDULen-sizeof(uint));  //从第二个uint位置收（总长 - 第一个uint大小）的大小  即剩下的数据



    switch(pdu->uiMsgType)
    {
    case ENUM_MSG_TYPE_REGIST_REQUEST:
    {
        char caName[32]={'\0'};     //用户名，或好友的名字
        char caPwd[32]={'\0'};      //密码,或我自己的名字,或目录名
        strncpy(caName,pdu->caData,32);
        strncpy(caPwd,pdu->caData+32,32);
        bool ret=OpeDB::getInstance().handleRegist(caName,caPwd);
        PDU *respdu = mkPDU(0);
        respdu->uiMsgType=ENUM_MSG_TYPE_REGIST_RESPOND;
        if(ret)
        {

            strcpy(respdu->caData,REGIST_OK);
        }
        else
        {
            strcpy(respdu->caData,REGIST_FAILED);
        }
        this->write((char*)respdu,respdu->uiPDULen);
        free(respdu);
        respdu=NULL;
        break;
    }
    case ENUM_MSG_TYPE_LOGIN_REQUEST:
    {
        char caName[32]={'\0'};     //用户名，或好友的名字
        char caPwd[32]={'\0'};      //密码,或我自己的名字,或目录名
        strncpy(caName,pdu->caData,32);
        strncpy(caPwd,pdu->caData+32,32);
        bool ret=OpeDB::getInstance().handleLogin(caName,caPwd);
        PDU *respdu=mkPDU(0);
        respdu->uiMsgType=ENUM_MSG_TYPE_LOGIN_RESPOND;//消息类型：登录回复
        if(ret)
        {

            strcpy(respdu->caData,LOGIN_OK);
            m_strName=caName;//登录成功后，记录用户名
            // //在登录成功后，记录用户根目录
        }
        else
        {
            strcpy(respdu->caData,LOGIN_FAILED);
        }
        //和客户端发送类似
        this->write((char*)respdu,respdu->uiPDULen);
        free(respdu);
        respdu=NULL;
        break;
    }
    case ENUM_MSG_TYPE_ALL_ONLINE_REQUEST:
    {
        QStringList ret=OpeDB::getInstance().handleAllOnline();
        qDebug()<<1;
        qDebug()<<ret;
        uint uiMsgLen=ret.size()*32;//消息长度
        PDU *respdu=mkPDU(uiMsgLen);
        respdu->uiMsgType=ENUM_MSG_TYPE_ALL_ONLINE_RESPOND;
        for(int i=0;i<ret.size();++i)
        {
            //每次偏移32位，char占1位，强转为char*型
            memcpy((char*)(respdu->caMsg)+i*32,ret.at(i).toStdString().c_str(),ret.at(i).size());
            qDebug()<<2;
            qDebug()<<ret;

        }
        this->write((char*)respdu,respdu->uiPDULen);
        free(respdu);
        respdu=NULL;
        break;
    }
    case ENUM_MSG_TYPE_SEARCH_USR_REQUEST:
    {
        qDebug()<<101;
        int ret = OpeDB::getInstance().handleSearchUsr(pdu->caData);
        PDU *respdu=mkPDU(0);
        qDebug()<<102;
            //OpeDB::handleSearchUsr(pdu->caData);
        //QStringList resultList=OpeDB::getInstance().handleSearchUsr(caName);
        //uint uiMsgLen=resultList.size()*32;
        //PDU *respdu=mkPDU(uiMsgLen);
        qDebug()<<103;
        respdu->uiMsgType=ENUM_MSG_TYPE_SEARCH_USR_RESPOND;
        if(-1==ret)
        {
            qDebug()<<1002;
            strcpy(respdu->caData,SEARCH_USR_NO);
        }
        else if (1==ret)
        {
            qDebug()<<1001;
            strcpy(respdu->caData,SEARCH_USR_ONLINE);
        }
        else if (0==ret)
        {
            qDebug()<<1000;
            strcpy(respdu->caData,SEARCH_USR_OFFLINE);
        }
        // if(uiMsgLen==0)
        // {
        //     strcpy((char*)respdu->caMsg,SEARCH_USER_NO);
        // }
        // else
        // {
        //     for(int i=0;i<resultList.size();++i)
        //     {
        //         memcpy((char*)respdu->caMsg+i*32,resultList.at(i).toStdString().c_str(),resultList.at(i).size());
        //     }
        // }
        this->write((char*)respdu,respdu->uiPDULen);
        free(respdu);
        respdu=NULL;
        break;

    }
    case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:
    {
        //判断是不是好友

        //对方在不在线

        //对方同意还是拒绝
        char caPerName[32]={'\0'};     //用户名，或好友的名字
        char caName[32]={'\0'};      //密码,或我自己的名字,或目录名
        strncpy(caPerName,pdu->caData,32);
        strncpy(caName,pdu->caData+32,32);
        int ret=OpeDB::getInstance().handleAddFriend(caPerName,caName);
        PDU  *respdu=NULL;
        qDebug()<<ret;
        if(-1==ret)
        {
            //发生未知错误，没有得到friend和myname
            PDU  *respdu=mkPDU(0);
            respdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
            strcpy(respdu->caData,UNKNOW_ERROR);
            this->write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu=NULL;
        }
        else if (0==ret)
        {
            qDebug()<<000;
            PDU  *respdu=mkPDU(0);
            respdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
            strcpy(respdu->caData,EXISTED_FRIEND);
            this->write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu=NULL;
        }
        else if(1==ret)
        {
            //在线
             qDebug()<<001;
            MyTcpServer::getInstance().resend(caPerName,pdu);


        }
        else if(2==ret)
        {
             qDebug()<<002;
            //此人不在线
            PDU  *respdu=mkPDU(0);
            respdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
            strcpy(respdu->caData,ADD_FRIEND_OFFLINE);
            this->write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu=NULL;
        }
        else if(3==ret) //没这个人
        {
             qDebug()<<003;
            PDU  *respdu=mkPDU(0);
            respdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
            strcpy(respdu->caData,ADD_FRIEND_NO_EXIST);
            this->write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu=NULL;
        }

        break;

    }
    case ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST:
    {
        qDebug()<<001;
        char caName[32]={'\0'};
        strncpy(caName,pdu->caData,32);
        QStringList ret=OpeDB::getInstance().handldFlushFriend(caName);
        uint uiMsgLen = ret.size()*32;
        PDU *respdu=mkPDU(uiMsgLen);
        respdu->uiMsgType=ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND;
        for (int i=0 ; i<ret.size();i++)
        {
            memcpy((char *)(respdu->caMsg)+i*32,ret.at(i).toStdString().c_str(),ret.at(i).size());

        }
        this->write((char*)respdu,respdu->uiPDULen);
        free(respdu);
        respdu=NULL;
        break;
    }
    default:
        break;
    }
    free(pdu);
    pdu=NULL;


    //qDebug()<<caName<<caPwd<<pdu->uiMsgType;
}

void MyTcpSocket::clientOffline()
{
    //设置在线状态
    OpeDB::getInstance().handleOffline(m_strName.toStdString().c_str()); //参数为char*
    emit offline(this);//this 该对象的地址
}
