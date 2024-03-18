#include "opedb.h"
#include <QMessageBox>
#include <QDebug>
#include <QString>
#include <QSqlQuery>

OpeDB::OpeDB(QObject *parent)
    : QObject{parent}
{
    m_db=QSqlDatabase::addDatabase("QSQLITE");
}

OpeDB &OpeDB::getInstance()
{
    static OpeDB instance;
    return instance;        //因为是静态的，每次返回同一个对象
}
void OpeDB::init()
{
    m_db.setHostName("localhost");
    m_db.setDatabaseName("C:\\Users\\17992\\Documents\\TcpServer\\cloud.db");
    if(m_db.open())
    {
        //打开成功，查询测试一下
        QSqlQuery query;
        query.exec("select * from usrInfo"); //执行sql查询
        while(query.next())//读每条数据
        {
            QString data=QString("%1,%2,%3").arg(query.value(0).toString()).arg(query.value(1).toString()).arg(query.value(2).toString());
            qDebug()<<data;
        }
        qDebug()<<"打开数据库成功";
    }
    else
    {
        QMessageBox::critical(NULL,"打开数据库","打开数据库失败");
    }
}

OpeDB::~OpeDB()
{
    m_db.close();
}

//注册
bool OpeDB::handleRegist(const char *name, const char *pwd)
{
    //考虑形参的有效性
    if(NULL==name||NULL==pwd)
    {
        qDebug()<<"name | pwd is null";
        return false;
    }
    QString data=QString("insert into usrInfo(name,pwd) values(\'%1\',\'%2\')").arg(name).arg(pwd);
    qDebug()<<data;
    QSqlQuery query;
    return query.exec(data);
}

//登录
bool OpeDB::handleLogin(const char *name, const char *pwd)
{
    //考虑形参的有效性
    if(NULL==name||NULL==pwd)
    {
        qDebug()<<"name | pwd is null";
        return false;
    }
    QString data=QString("select * from usrInfo where name=\'%1\' and pwd=\'%2\' and online=0").arg(name).arg(pwd);
    qDebug()<<data;
    QSqlQuery query;
    query.exec(data);
    if(query.next())
    {
        QString data=QString("update usrInfo set online=1 where name=\'%1\' and pwd=\'%2\'").arg(name).arg(pwd);
        qDebug()<<data;
        QSqlQuery query;
        return query.exec(data);
    }
    else
    {
        return false;
    }
}

void OpeDB::handleOffline(const char *name)
{
    //考虑形参的有效性
    if(NULL==name)
    {
        qDebug()<<"name | pwd is null";
        return ;
    }
    QString data=QString("update usrInfo set online=0 where name=\'%1\' ").arg(name);
    qDebug()<<data;
    QSqlQuery query;
    query.exec(data);
}

QStringList OpeDB::handleAllOnline()
{
    QString data=QString("select name from usrInfo where online=1");
    QSqlQuery query;
    query.exec(data);

    QStringList result;
    result.clear();
    while(query.next())
    {
        result.append(query.value(0).toString());
    }
    qDebug()<<result;
    return result;
}

int OpeDB::handleSearchUsr(const char *name)
{
    //QStringList resultList={};
    if(NULL == name)
    {
        return -1;
    }
    QString data=QString("select online from usrInfo where name=\'%1\' ").arg(name);//模糊查询，返回姓名和在线状态
    qDebug()<<data;
    qDebug()<<11;
    QSqlQuery query;
    query.exec(data);
    qDebug()<<12;
    if(query.next())
    {

        int ret=query.value(0).toInt();
        if (1==ret)
        {
            qDebug()<<111;
            return 1;
        }
        else if (ret==0)
        {
            qDebug()<<110;
            return 0;
        }



        // QString name = query.value(0).toString();
        // QString online = query.value(1).toString();

        // QString resultString = name + "," + online;
        // resultList.append(resultString);
    }
    else
    {
        qDebug()<<1111;
        return -1;
    }

    // 输出 QStringList 中的元素
    // for (const QString &str : resultList) {
    //     qDebug() << str;
    // }
    return 0;
}


int OpeDB::getIdByUserName(const char *name)
{
    if(NULL == name)
    {
        return -1;
    }
    QString strQuery = QString("select id from userInfo where name = \'%1\' ").arg(name);
    QSqlQuery query;

    query.exec(strQuery);
    if(query.next())
    {
        return query.value(0).toInt();
    }
    else
    {
        return -1; // 不存在该用户
    }
}

QStringList OpeDB::handldFlushFriend(const char *name)
{
    QStringList strFriendList;
    strFriendList.clear();
    if(name==NULL) return strFriendList;//直接返回空的列表

    QString data=QString("select name from usrInfo where online=1 and id=(select id from friend where friendId=(select id from usrInfo where name=\'%1\'))").arg(name);
    QSqlQuery query;
    query.exec(data);//执行
    while(query.next())
    {
        strFriendList.append(query.value(0).toString());
        qDebug()<<query.value(0).toString();
    }
    return strFriendList;

    data=QString("select name from usrInfo where online=1 and id=(select friendId from friend where id=(select id from usrInfo where name=\'%1\'))").arg(name);
    query.exec(data);//执行
    while(query.next())
    {
        strFriendList.append(query.value(0).toString());
        qDebug()<<query.value(0).toString();
        qDebug()<<1;
    }
    return strFriendList;
}
int OpeDB::handleAddFriend(const char *pername, const char *name)
{
    if(NULL==pername||NULL==name)  return -1;//系统错误
    //QString selectFridendId=QString("select id from userInfo where name=\'%1\'").arg(pername);
    //QString selectMyId=QString("select id from userInfo where name=\'%1\'").arg(name);
    //双向查询
    QString data=QString("select * from friend where (id=(select id from usrInfo where name=\'%1\') and friendId=(select id from usrInfo where name=\'%2\'))"
                           "or(id=(select id from usrInfo where name=\'%3\') and friendId=(select id from usrInfo where name=\'%4\'))").arg(pername).arg(name).arg(name).arg(pername);
    qDebug()<<data;

    QSqlQuery query;
    query.exec(data);
    if(query.next()) return 0;//双方已经是好友
    else
    {
        data=QString("select online from userInfo where name=\'%1\'").arg(pername);
        query.exec(data);
        if(query.next())
        {
            int ret=query.value(0).toInt();
            if(1==ret)
            {
                qDebug()<<0001;
                return 1;//在线
            }
            else if (0==ret)
            {
                 qDebug()<<0002;
                return 2;//不在线
            }
        }
        else
        {
             qDebug()<<0003;
            return 3;//没有这个人
        }
    }
}
