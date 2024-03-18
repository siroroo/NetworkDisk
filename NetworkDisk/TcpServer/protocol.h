#ifndef PROTOCOL_H
#define PROTOCOL_H
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


typedef unsigned int uint;
#define REGIST_OK "regist ok"
#define REGIST_FAILED "regist failed: Usrname already exists!"

#define LOGIN_OK "login ok"
#define LOGIN_FAILED "login failed"

#define SEARCH_USR_NO "no such people"
#define SEARCH_USR_ONLINE "online"
#define SEARCH_USR_OFFLINE "offline"

#define UNKNOW_ERROR "unknow errror" //未知错误
#define EXISTED_FRIEND "friend exist"
#define ADD_FRIEND_OK "add friend ok"
#define ADD_FRIEND_OFFLINE "user offline"
#define ADD_FRIEND_NO_EXIST "user no exist"
#define DELETE_FRIEND_OK "delete friend ok"


enum ENUM_MSG_TYPE
{
    ENUM_MSG_TYPE_MIN=0,
    ENUM_MSG_TYPE_REGIST_REQUEST,   //注册请求
    ENUM_MSG_TYPE_REGIST_RESPOND,   //注册回复

    ENUM_MSG_TYPE_LOGIN_REQUEST,    //登录请求
    ENUM_MSG_TYPE_LOGIN_RESPOND,    //登录请求

    ENUM_MSG_TYPE_ALL_ONLINE_REQUEST,   //在线用户请求
    ENUM_MSG_TYPE_ALL_ONLINE_RESPOND,   //在线用户回复


    ENUM_MSG_TYPE_SEARCH_USR_REQUEST,//搜索用户请求
    ENUM_MSG_TYPE_SEARCH_USR_RESPOND,//搜索用户回复


    ENUM_MSG_TYPE_ADD_FRIEND_REQUEST,   //加好友请求
    ENUM_MSG_TYPE_ADD_FRIEND_RESPOND,   //加好友回复

    ENUM_MSG_TYPE_ADD_FRIEND_AGGREE,   //同意添加好友
    ENUM_MSG_TYPE_ADD_FRIEND_REFUSE,   //不同意

    ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST,   //刷新好友请求
    ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND,   //刷新好友回复

    ENUM_MSG_TYPE_MAX=0x00ffffff,
};

//协议结构体
struct PDU              //协议数据单元
{
    uint uiPDULen;      //总的协议数据单元大小
    uint uiMsgType;     //消息类型
    char caData[64];    //文件名
    uint uiMsgLen;      //实际消息长度
    int caMsg[];        //实际消息
};

PDU *mkPDU(uint uiMsgLen);


#endif
