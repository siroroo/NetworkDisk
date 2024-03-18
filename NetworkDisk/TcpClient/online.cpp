#include "online.h"
#include "ui_online.h"
#include "tcpclient.h"

Online::Online(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Online)
{
    ui->setupUi(this);
}

Online::~Online()
{
    delete ui;
}

void Online::showUsr(PDU *pdu)
{
    if (NULL==pdu)
    {
        return;
    }
    uint uiSize=pdu->uiMsgLen/32;
    char caTmp[32];
    //ui->all_online_list->clear(); // 清空列表
    // qDebug()<<"你好";
    // if(pdu->uiMsgType==ENUM_MSG_ALL_ONLINE_RESPOND)
    // {
    //ui->all_online_list->addItem("所有在线用户如下：");
    //ui->all_online_list->addItem("666");
    for(uint i=0;i<uiSize;++i)
    {
        memcpy(caTmp,(char*)(pdu->caMsg)+i*32,32);
        ui->online_lw->addItem(caTmp);
    }
        // }


}

void Online::on_addFriend_pb_clicked()
{
    //获取左边选择的用户
    QListWidgetItem *pItem=ui->online_lw->currentItem();
    QString strPerUsrName=pItem->text();//未来好友的名字
    // if(0==strcmp(strFriendName.toStdString().c_str(),"所有在线用户如下："))
    // {
    //     QMessageBox::information(this,"添加好友","请选择想要添加的好友！");
    //     return;
    // }
    QString strLoginName=TcpClient::getInstance().loginName();//我的名字
    PDU *pdu=mkPDU(0);

    pdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_REQUEST;
    memcpy(pdu->caData,strPerUsrName.toStdString().c_str(),strPerUsrName.size());//朋友
    memcpy(pdu->caData+32,strLoginName.toStdString().c_str(),strLoginName.size());//我自己
    TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
    free(pdu);
    pdu=NULL;
}

