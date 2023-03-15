#pragma once

#include <QWidget>
#include <QTextEdit>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "OnlineUsers.h"

class FriendList : public QWidget
{
	Q_OBJECT

public:
	explicit FriendList(QWidget* parent = 0);
	void showOnlineUsers(PDU* pdu);
	void updateFriendList(PDU* pdu);
	void updateGroupMessage(PDU* pdu);
	~FriendList();

	QString strSearchResult;

private slots:
	void onBtnShowOnlineUsersClicked();
	void onBtnSearchClicked();
	void onBtnRefreshClicked();
	void onBtnDeleteFriendCLicked();
	void onBtnPrivateTalkClicked();
	void onBtnSendClicked();

private:
	QTextEdit* messageTextBox;
	QListWidget* friendsList;
	QLineEdit* msgInputBox;

	QPushButton* btnDeleteFriend;
	QPushButton* btnRefreshFriend;
	QPushButton* btnShowOnlineUsers;
	QPushButton* btnSearchOnlineUsers;
	QPushButton* btnSend;
	QPushButton* btnPrivateTalk;

	OnlineUsers* onlineu;
};
