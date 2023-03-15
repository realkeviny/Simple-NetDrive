#pragma once

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QStringList>

class DatabaseOperation : public QObject
{
	Q_OBJECT

public:
	explicit DatabaseOperation(QObject* parent = 0);
	~DatabaseOperation();

	static DatabaseOperation& getInstance();
	void initDatabase();
	void openDatabase();

	bool handleRegister(const char* name, const char* pwd);//处理注册的句柄
	bool handleLogin(const char* name, const char* pwd);//处理登录的句柄
	void handleOffline(const char* name);//离线处理
	QStringList handleAllOnlineUsers();
	int handleSearch(const char* name);
	int handleAddFriends(const char* friendName, const char* myName);
	QStringList handleRefresh(const char* friendName);
	bool handleDeleteFriend(const char* myName, const char* friendName);
	void handleAgreeAddFriend(const char* friendName, const char* Name);
private slots:

private:
	QSqlDatabase DataBase;//连接数据库
};
