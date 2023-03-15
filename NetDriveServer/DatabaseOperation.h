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

	bool handleRegister(const char* name, const char* pwd);//����ע��ľ��
	bool handleLogin(const char* name, const char* pwd);//�����¼�ľ��
	void handleOffline(const char* name);//���ߴ���
	QStringList handleAllOnlineUsers();
	int handleSearch(const char* name);
	int handleAddFriends(const char* friendName, const char* myName);
	QStringList handleRefresh(const char* friendName);
	bool handleDeleteFriend(const char* myName, const char* friendName);
	void handleAgreeAddFriend(const char* friendName, const char* Name);
private slots:

private:
	QSqlDatabase DataBase;//�������ݿ�
};
