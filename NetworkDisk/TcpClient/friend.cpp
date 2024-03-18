#include "friend.h"
#include "protocol.h"
#include "tcpclient.h"
#include <QInputDialog>
#include <QDebug>

Friend::Friend(QWidget *parent)
    : QWidget{parent}
{

    m_pInputMsgLE = new QLineEdit;
    m_pShowMsgTE = new QTextEdit;
    m_pFriendListWidget = new QListWidget;

    m_pDelFriendPB = new QPushButton("删除好友");
    m_pFlushFriendPB = new QPushButton("刷新好友");
    m_pShowOnlineUsrPB = new QPushButton("显示在线用户");
    m_pSearchUsrPB = new QPushButton("查找用户");
    m_pMsgSendPB = new QPushButton("发送");
    m_pPrivateChatPB = new QPushButton("私聊");

    m_pOnline=new Online;

    QVBoxLayout *pRightPBVBL = new QVBoxLayout; // 左侧右部分好友操作按钮布局
    pRightPBVBL -> addWidget(m_pPrivateChatPB);
    pRightPBVBL -> addWidget(m_pDelFriendPB);
    pRightPBVBL -> addWidget(m_pFlushFriendPB);
    pRightPBVBL -> addWidget(m_pShowOnlineUsrPB);
    pRightPBVBL -> addWidget(m_pSearchUsrPB);

    QHBoxLayout *pTopHBL= new QHBoxLayout;
    pTopHBL->addWidget(m_pShowMsgTE);
    pTopHBL->addWidget(m_pFriendListWidget);
    pTopHBL->addLayout(pRightPBVBL);


    QHBoxLayout *pMsgHBL= new QHBoxLayout;
    pMsgHBL->addWidget(m_pInputMsgLE);
    pMsgHBL->addWidget(m_pMsgSendPB);


    QVBoxLayout *pMain = new QVBoxLayout;
    pMain->addLayout(pTopHBL);
    pMain->addLayout(pMsgHBL);
    pMain->addWidget(m_pOnline);
    m_pOnline->hide();

    setLayout(pMain);

    connect(m_pShowOnlineUsrPB,SIGNAL(clicked(bool))
            ,this,SLOT(showOnline()));
    connect(m_pSearchUsrPB,SIGNAL(clicked(bool))
            ,this,SLOT(searchUsr()));
    connect(m_pFlushFriendPB,SIGNAL(clicked(bool)),this,SLOT(flushFriend()));//刷新好友

}

void Friend::showAllOnlineUsr(PDU *pdu)
{
    if(NULL==pdu)
    {
        return;
    }
    m_pOnline->showUsr(pdu);
}

void Friend::updateFriendList(PDU *pdu)
{
    if(NULL==pdu)
    {
        return;
    }
    uint uiSize=pdu->uiMsgLen/32;
    char caName[32]={'\0'};
    m_pFriendListWidget->clear();
    m_pFriendListWidget->addItem("我的好友列表如下；");
    for(uint i=0;i<uiSize;i++)
    {
        memcpy(caName,(char*)(pdu->caMsg)+i*32,32);
        //QString str=caName;
        //QString user=str.trimmed();//去空格
        //QStringList strList = user.split(",");//逗号分割
        //if(strList.size()!=2) continue;
        //if(strList.at(1)=="0")
        //{
        m_pFriendListWidget->addItem(caName);
        //}
        //else if(strList.at(1)=="1")
        //{
            //m_pFriendListWidget->addItem(strList.at(0)+"\t 在线");
        //}
    }
}

void Friend::showOnline()
{
    if(m_pOnline->isHidden())
    {
        m_pOnline->show();

        //请求：显示所有在线用户
        PDU *pdu=mkPDU(0);
        pdu->uiMsgType=ENUM_MSG_TYPE_ALL_ONLINE_REQUEST;
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);//发送pdu
        free(pdu);
        pdu=NULL;
    }
    else
    {
        m_pOnline->hide();
    }
}

void Friend::searchUsr()
{
    m_strSearchName=QInputDialog::getText(this,"搜索","用户名");
    if(!m_strSearchName.isEmpty())
    {

        qDebug()<<m_strSearchName;
        PDU *pdu=mkPDU(0);
        pdu->uiMsgType=ENUM_MSG_TYPE_SEARCH_USR_REQUEST;

        memcpy(pdu->caData,m_strSearchName.toStdString().c_str(),m_strSearchName.size());
        //memcpy((char*)(pdu->caData),name.toStdString().c_str(),name.size());//被赋值地址，值，大小

        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);//发送pdu
        free(pdu);
        pdu=NULL;
    }
}

void Friend::flushFriend()
{
    QString strName=TcpClient::getInstance().loginName();
    PDU *pdu=mkPDU(0);
    pdu->uiMsgType=ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST;
    memcpy(pdu->caData,strName.toStdString().c_str(),strName.size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
    free(pdu);
    pdu=NULL;
}
