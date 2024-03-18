#ifndef OPEDB_H
#define OPEDB_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QStringList>


class OpeDB : public QObject
{
    Q_OBJECT
public:
    explicit OpeDB(QObject *parent = nullptr);
    static OpeDB& getInstance();
    void init();
    ~OpeDB();

    bool handleRegist(const char *name,const char *pwd);    //注册操作
    bool handleLogin(const char *name,const char *pwd);     //登录操作
    void handleOffline(const char *name);                   //下线
    QStringList handleAllOnline();                          //查看在线用户
    int handleSearchUsr(const char* name);         //查找用户

    int handleAddFriend(const char* pername,const char* name);     //添加好友
    int getIdByUserName(const char *name);                // 根据用户名获取用户id
    void handleAddRelationship(const char* friendName,const char* myName);// 处理同意好友申请
    QStringList handldFlushFriend(const char* name);        //刷新好友列表
    bool handleDeleteFriend(const char* friendName,const char *myName);
    QStringList handleGroupFriend(const char* name);
    bool handleLogoff(const char* name,const char *pwd);



signals:

public slots:

private:
    QSqlDatabase m_db;    //连接数据库
};

#endif // OPEDB_H
