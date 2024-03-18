#include "tcpclient.h"
#include "ui_tcpclient.h"
#include <QByteArray> //字节数组
#include <QDebug>
#include <QMessageBox>
#include <QHostAddress>
#include "protocol.h" //协议数据单元


TcpClient::TcpClient(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TcpClient)
{
    ui->setupUi(this);

    //resize(500,250);

    loadConfig();

    connect(&m_tcpSocket,SIGNAL(connected()),this,SLOT(showConnect()));
    connect(&m_tcpSocket,SIGNAL(readyRead()),this,SLOT(recvMsg()));
    //连接服务器
    m_tcpSocket.connectToHost(QHostAddress(m_strIP),m_usPort);
}

TcpClient::~TcpClient()
{
    delete ui;
}

void TcpClient::loadConfig()
{
    QFile file(":/client.config");
    if (file.open(QIODevice::ReadOnly))                 //只读打开
    {
        QByteArray baData=file.readAll();               //全部读取，返回一个 字节 bytearray 数组
        QString strData=baData.toStdString().c_str();   // 字节 转为 字符串Qstring
        //qDebug() << strData;                          //打印127.0.0.1\r\n8888
        file.close();

        //对数据进行处理
        strData.replace("\r\n"," ");
        //qDebug() << strData;                          //打印127.0.0.1 8888
        QStringList strList=strData.split(" ");         //空格分割
        // for(int i=0;i<strList.size();i++)
        // {
        //     qDebug()<<strList[i]; //"127.0.0.1" "8888"
        // }
        m_strIP=strList.at(0);//IP
        m_usPort=strList.at(1).toUShort();              //转成无符号整型
        //qDebug()<<"ip:"<<m_strIP<<"port:"<<m_usPort;  //ip: "127.0.0.1" port: 8888
    }
    else
    {
        QMessageBox::critical(this,"open config","open config failed");//若打开失败
    }

}

TcpClient &TcpClient::getInstance()
{

    static TcpClient instance;
    return instance;
}

QTcpSocket &TcpClient::getTcpSocket()
{
    return m_tcpSocket;
}

QString TcpClient::loginName()
{
    return m_strLoginName;
}

void TcpClient::showConnect()
{
    QMessageBox::information(this,"连接服务器","连接服务器成功");
}

void TcpClient::recvMsg()
{
    qDebug()<<m_tcpSocket.bytesAvailable();
    //收数据
    uint uiPDULen =0;
    m_tcpSocket.read((char*)&uiPDULen,sizeof(uint));  //收总大小
    uint uiMsgLen=uiPDULen - sizeof(PDU);
    PDU *pdu=mkPDU(uiMsgLen);
    m_tcpSocket.read((char*)pdu+sizeof(uint),uiPDULen-sizeof(uint));  //从第二个uint位置收（总长 - 第一个uint大小）的大小  即剩下的数据
    qDebug()<<001;
    switch (pdu->uiMsgType)
    {
    case ENUM_MSG_TYPE_REGIST_RESPOND:      //注册
    {
        if(0==strcmp(pdu->caData,REGIST_OK))
        {
            QMessageBox::information(this,"注册",REGIST_OK);
            qDebug()<<pdu->caData;
        }
        else if (0==strcmp(pdu->caData,REGIST_FAILED))
        {
            QMessageBox::warning(this,"注册",REGIST_FAILED);
            qDebug()<<pdu->caData;
        }
        break;
    }
    case ENUM_MSG_TYPE_LOGIN_RESPOND:       //登录
    {
        if(0==strcmp(pdu->caData,LOGIN_OK))
        {
            QMessageBox::information(this,"登录",LOGIN_OK);
            OpeWidget::getInstance().show();
            this->hide();//隐藏登录注册注销界面
            //qDebug()<<pdu->caData;
        }
        else if(0==strcmp(pdu->caData,LOGIN_FAILED))
        {
            QMessageBox::information(this,"登录",LOGIN_FAILED);
            //qDebug()<<pdu->caData;
        }
        break;
    }
    case ENUM_MSG_TYPE_ALL_ONLINE_RESPOND:
    {
        OpeWidget::getInstance().getFriend()->showAllOnlineUsr(pdu);
        break;
    }
    case ENUM_MSG_TYPE_SEARCH_USR_RESPOND:
    {
        QString name=OpeWidget::getInstance().getFriend()->m_strSearchName;
        qDebug()<<1;
        qDebug()<<pdu->caData;
        if(0==strcmp(SEARCH_USR_NO,pdu->caData))
        {
            qDebug()<<2;
            QMessageBox::information(this,"搜索",QString("%1: not exist").arg(name));
            qDebug()<<OpeWidget::getInstance().getFriend()->m_strSearchName<<1;
        }
        else if (0==strcmp(SEARCH_USR_ONLINE,pdu->caData))
        {
            qDebug()<<1112;
            QMessageBox::information(this,"搜索",QString("%1: online").arg(name));

        }
        else if (0==strcmp(SEARCH_USR_OFFLINE,pdu->caData))
        {
            qDebug()<<3;
            QMessageBox::information(this,"搜索",QString("%1: offline").arg(name));

        }
        // QString name=OpeWidget::getInstance().getFriendList()->m_strSearchName;
        // uint uiSize=(pdu->uiMsgLen)/32;//这是搜索到的人数

        // if(uiSize==0)
        // {
        //     QMessageBox::information(this,"搜索",QString("%1 相关用户不存在！").arg(name));
        // }
        // else
        // {
        //     // OperateWidget::getInstance().getFriendList()->showAllOnlineUser(pdu);//让所有在线用户那个界面去显示，这次会多了用户是否在线
        //     QWidget *qw=new QWidget;
        //     qw->setAttribute(Qt::WA_DeleteOnClose);
        //     QListWidget *w=new QListWidget(nullptr);
        //     char caTmp[32];
        //     w->addItem("搜索结果如下；");
        //     for(uint i=0;i<uiSize;++i)
        //     {
        //         memcpy(caTmp,(char*)(pdu->caMsg)+i*32,32);
        //         QString str=caTmp;
        //         QString user=str.trimmed();//去空格
        //         QStringList strList = user.split(",");//逗号分割
        //         if(strList.size()!=2) continue;
        //         if(strList.at(1)=="0")
        //         {
        //             w->addItem(strList.at(0)+"\t 已下线");
        //         }
        //         else if(strList.at(1)=="1")
        //         {
        //             w->addItem(strList.at(0)+"\t 在线");
        //         }

        //     }
        //     QHBoxLayout *pMain = new QHBoxLayout;
        //     pMain->addWidget(w);
        //     qw->setLayout(pMain);
        //     qw->show();
        // }
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:
    {
        char caName[32]={'\0'};
        strncpy(caName,pdu->caData+32,32);
        int ret=QMessageBox::information(this,"添加好友",QString("%1 want to add you as friend ?").arg(caName),QMessageBox::Yes,QMessageBox::No);
        PDU *respdu = mkPDU(0);

        memcpy(respdu->caData,pdu->caData,32);
        if(ret==QMessageBox::Yes)
        {
            respdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_AGGREE;
        }
        else
        {
            respdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_REFUSE;
        }
        m_tcpSocket.write((char*)respdu,respdu->uiPDULen);
        free(respdu);
        respdu=NULL;
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_RESPOND:
    {
        QMessageBox::information(this,"添加好友",pdu->caData);





        break;
    }
    case ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND:
    {
        OpeWidget::getInstance().getFriend()->updateFriendList(pdu);
        break;
    }
    default:
        break;
    }
    free(pdu);
    pdu=NULL;
}
#if 0
void TcpClient::on_send_pb_clicked()                    //发送按钮槽  在此判断并发送给服务器
{
    QString strMsg=ui->lineEdit->text();
    if(!strMsg.isEmpty())
    {
        PDU *pdu=mkPDU(strMsg.size()+1);
        pdu->uiMsgType=8888;//消息类型，测试用
        //QString data="filenme";
        //memcpy(pdu->caData,data.toStdString().c_str(),data.size());
        memcpy(pdu->caMsg,strMsg.toStdString().c_str(),strMsg.size());//把要发送的数据拷贝到数据单元中
        qDebug()<<(char*)(pdu->caMsg);
        m_tcpSocket.write((char*)pdu,pdu->uiPDULen);
        // qDebug()<<"这是什么情况";
        free(pdu);
        pdu=NULL;
    }
    else
    {
        QMessageBox::warning(this,"信息发送","发送信息不能为空");
    }
}
#endif

void TcpClient::on_login_pb_clicked()       //登录按钮
{
    QString strName=ui->name_le->text();

    QString strPwd=ui->pwd_le->text();
    if(!strName.isEmpty()&&!strPwd.isEmpty())
    {
        m_strLoginName=strName;
        PDU *pdu=mkPDU(0);
        pdu->uiMsgType=ENUM_MSG_TYPE_LOGIN_REQUEST;
        strncpy(pdu->caData,strName.toStdString().c_str(),32);
        strncpy(pdu->caData+32,strPwd.toStdString().c_str(),32);
        qDebug() << pdu -> uiMsgType << " " << pdu -> caData << " " << pdu -> caData + 32;
        m_tcpSocket.write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
    }
    else
    {
        QMessageBox::critical(this,"登录","登录失败：用户名或密码不能为空！");

    }

    // if(!strName.isEmpty()&&!strPwd.isEmpty())
    // {
    //     m_strLoginName=strName;//记录当前登录用户名
    //     PDU *pdu=mkPDU(0);//没有msg
    //     pdu->uiMsgType=ENUM_MSG_LOGIN_REQUEST;
    //     // 都放到caData
    //     strPwd=hashString(strPwd);//加密
    //     // qDebug()<<strPwd;

    //     strncpy(pdu->caData,strName.toStdString().c_str(),32);
    //     strncpy(pdu->caData+32,strPwd.toStdString().c_str(),32);
    //     //发送给服务器
    //     m_tcpSocket.write((char*)pdu,pdu->uiPDULen);
    //     free(pdu);
    //     pdu=NULL;
    // }
    // else
    // {
    //     QMessageBox::critical(this,"登录","登录失败：用户名或密码不能为空！");
    // }
}


void TcpClient::on_regist_pb_clicked()      //注册按钮
{
    QString strName=ui->name_le->text();
    QString strPwd=ui->pwd_le->text();
    if(!strName.isEmpty()&&!strPwd.isEmpty())
    {
        PDU *pdu=mkPDU(0);
        pdu->uiMsgType=ENUM_MSG_TYPE_REGIST_REQUEST;
        strncpy(pdu->caData,strName.toStdString().c_str(),32);
        strncpy(pdu->caData+32,strPwd.toStdString().c_str(),32);
        m_tcpSocket.write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
    }
    else
    {
        QMessageBox::critical(this,"注册","注册失败：用户名密码不能为空！");

    }
    // //对用户名和密码进行一些简单的正则匹配
    // static QRegularExpression regexName("^[^0-9][a-zA-Z0-9_]{2,16}$");
    // //(?=.*[A-Za-z])：正则表达式预查，在这里表示要求字符串中至少包含一个字母（大小写不限）。
    // static QRegularExpression regexPwd("^(?=.*[A-Za-z])(?=.*\\d)[\\w]{4,32}$");


    // if(strName.isEmpty()||strPwd.isEmpty())
    // {
    //     QMessageBox::critical(this,"注册","注册失败：用户名或密码不能为空！");
    // }
    // else if(!regexName.match(strName).hasMatch())
    // {
    //     QMessageBox::critical(this,"注册","注册失败：用户名不能以数字开头，且至少两个字符！");
    // }
    // else if(!regexPwd.match(strPwd).hasMatch())
    // {
    //     QMessageBox::critical(this,"注册","注册失败：密码必须包含至少一个数字和一个字符，且至少4个字符！");
    // }
    // else
    // {
    //     PDU *pdu=mkPDU(0);//没有msg
    //     pdu->uiMsgType=ENUM_MSG_REGIST_REQUEST;
    //     // 都放到caData
    //     strPwd=hashString(strPwd);//加密
    //     // qDebug()<<strPwd;

    //     strncpy(pdu->caData,strName.toStdString().c_str(),32);
    //     strncpy(pdu->caData+32,strPwd.toStdString().c_str(),32);
    //     //发送给服务器
    //     m_tcpSocket.write((char*)pdu,pdu->uiPDULen);
    //     free(pdu);
    //     pdu=NULL;
    // }
}


void TcpClient::on_cancle_pb_clicked()
{

}

