#include "DatabaseOperation.h"
#include <QMessageBox>
#include <QDebug>

DatabaseOperation::DatabaseOperation(QObject* parent)
	: QObject(parent)
{
	DataBase = QSqlDatabase::addDatabase("QSQLITE");
}

DatabaseOperation::~DatabaseOperation()
{
	DataBase.close();
}

DatabaseOperation& DatabaseOperation::getInstance()
{
	static DatabaseOperation instance;
	return instance;
}

void DatabaseOperation::initDatabase()
{
	DataBase.setHostName("localhost");
	DataBase.setDatabaseName("F:/C++/Qt/NetDrive/QtProjs/NetDriveServer/UsersInfo.db");
	if (DataBase.open())
	{
		QSqlQuery query;
		query.exec("SELECT * FROM UserInfo");
		while (query.next())
		{
			QString Data = QString("%1,%2,%3").arg(query.value(0).toString()).arg(query.value(1).toString()).arg(query.value(2).toString());
			qDebug() << Data;
		}
	}
	else
	{
		QMessageBox::critical(NULL, "Error", "Database Not Found!");
	}
}

void DatabaseOperation::openDatabase()
{
	DataBase.setHostName("localhost");
	DataBase.setDatabaseName("F:/C++/Qt/NetDrive/QtProjs/NetDriveServer/UsersInfo.db");
	DataBase.open();
}

bool DatabaseOperation::handleRegister(const char* name, const char* pwd)
{
	if (NULL == name || NULL == pwd)
	{
		qDebug() << "Name | Password is null";
		return false;
	}
	openDatabase();
	QString sql = QString("INSERT INTO UserInfo(Name,Password) VALUES(\'%1\',\'%2\')").arg(name).arg(pwd);
	QSqlQuery query;
	return query.exec(sql);
}

bool DatabaseOperation::handleLogin(const char* name, const char* pwd)
{
	if (NULL == name || NULL == pwd)
	{
		qDebug() << "Name | Password is null";
		return false;
	}
	openDatabase();
	QString sql = QString("SELECT * FROM UserInfo WHERE Name=\'%1\' AND Password=\'%2\' AND LineStatus=0").arg(name).arg(pwd);
	qDebug() << sql;
	QSqlQuery query;
	query.exec(sql);
	if (query.next())
	{
		sql = QString("UPDATE UserInfo SET LineStatus=1 WHERE Name=\'%1\' AND Password=\'%2\'").arg(name).arg(pwd);
		qDebug() << sql;
		QSqlQuery query;
		query.exec(sql);

		return true;
	}
	else
	{
		return false;
	}
}

void DatabaseOperation::handleOffline(const char* name)
{
	if (NULL == name)
	{
		qDebug() << "Name is null";
		return;
	}

	openDatabase();
	QString sql = QString("UPDATE UserInfo SET LineStatus=0 WHERE Name = \'%1\'").arg(name);
	qDebug() << sql;
	QSqlQuery query;
	query.exec(sql);
}

QStringList DatabaseOperation::handleAllOnlineUsers()
{
	openDatabase();
	QString sql = QString("SELECT Name FROM UserInfo WHERE LineStatus=1");
	//qDebug() << sql;
	QSqlQuery query;
	query.exec(sql);
	QStringList result;
	result.clear();

	while (query.next())
	{
		result.append(query.value(0).toString());
	}
	return result;
}

int DatabaseOperation::handleSearch(const char* name)
{
	if (nullptr == name)
	{
		return -1;
	}

	openDatabase();
	QString sql = QString("SELECT LineStatus FROM UserInfo WHERE Name=\'%1\' ").arg(name);
	QSqlQuery query;
	query.exec(sql);
	if (query.next())
	{
		int flag = query.value(0).toInt();
		if (flag == 1)
		{
			return 1;
		}
		else if (!flag)
		{
			return 0;
		}
	}
	else
	{
		return -1;
	}
}

int DatabaseOperation::handleAddFriends(const char* friendName, const char* myName)
{
	if (NULL == friendName || NULL == myName)
	{
		return -1;
	}

	openDatabase();
	QString sql = QString("SELECT * FROM Friends WHERE (ID=(SELECT ID FROM UserInfo WHERE Name=\'%1\') AND FriendID=(SELECT ID FROM UserInfo WHERE Name = \'%2\'))"
		"OR (ID=(SELECT ID FROM UserInfo WHERE Name=\'%3\') AND FriendID=(SELECT ID FROM UserInfo WHERE Name = \'%4\'))").arg(friendName).arg(myName).arg(myName).arg(friendName);
	qDebug() << sql;
	QSqlQuery query;
	query.exec(sql);
	if (query.next())
	{
		return 0;//已经是好友
	}
	else
	{
		sql = QString("SELECT LineStatus FROM UserInfo WHERE Name=\'%1\' ").arg(friendName);
		QSqlQuery query;
		query.exec(sql);
		if (query.next())
		{
			int flag = query.value(0).toInt();
			if (flag == 1)
			{
				return 1; //非好友，在线
			}
			else if (!flag)
			{
				return 2;//离线
			}
		}
		else
		{
			return 3;//查无此人
		}
	}
}

QStringList DatabaseOperation::handleRefresh(const char* friendName)
{
	QStringList friendNameList;
	friendNameList.clear();
	if (friendName == NULL)
	{
		return friendNameList;
	}
	QString sql = QString("SELECT Name FROM UserInfo WHERE LineStatus=1 AND ID IN (SELECT ID FROM Friends WHERE FriendID=(SELECT ID FROM UserInfo WHERE Name=\'%1\'))").arg(friendName);
	QSqlQuery query;
	query.exec(sql);
	while (query.next())
	{
		friendNameList.append(query.value(0).toString());
		qDebug() << query.value(0).toString();
	}

	sql = QString("SELECT Name FROM UserInfo WHERE LineStatus=1 AND ID IN (SELECT FriendID FROM Friends WHERE ID=(SELECT ID FROM UserInfo WHERE Name=\'%1\'))").arg(friendName);
	query.exec(sql);
	while (query.next())
	{
		friendNameList.append(query.value(0).toString());
		qDebug() << query.value(0).toString();
	}
	return friendNameList;
}

bool DatabaseOperation::handleDeleteFriend(const char* myName, const char* friendName)
{
	if (myName == nullptr || friendName == nullptr)
	{
		return false;
	}
	QString sql = QString("DELETE FROM Friends WHERE ID=(SELECT ID FROM UserInfo WHERE Name=\'%1\') AND FriendID=(SELECT ID FROM UserInfo WHERE Name=\'%2\')").arg(myName).arg(friendName);
	QSqlQuery query;
	query.exec(sql);

	sql = QString("DELETE FROM Friends WHERE ID=(SELECT ID FROM UserInfo WHERE Name=\'%1\') AND FriendID=(SELECT ID FROM UserInfo WHERE Name=\'%2\')").arg(friendName).arg(myName);
	query.exec(sql);

	return true;
}

void DatabaseOperation::handleAgreeAddFriend(const char* friendName, const char* Name)
{
	if (friendName == NULL || Name == NULL)
	{
		return;
	}
	QString sql = QString("INSERT INTO Friends(ID,FriendID) VALUES((SELECT ID FROM UserInfo WHERE Name=\'%1\'),(SELECT ID FROM UserInfo WHERE Name=\'%2\'))").arg(friendName).arg(Name);
	QSqlQuery query;
	query.exec(sql);
}