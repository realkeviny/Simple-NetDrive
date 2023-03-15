#pragma once

#include <QMainWindow>
#include "ui_OnlineUsers.h"
#include "Protocol.h"

class OnlineUsers : public QMainWindow
{
	Q_OBJECT

public:
	explicit OnlineUsers(QWidget* parent = nullptr);
	~OnlineUsers();

	void showUsers(PDU* pdu);

private slots:
	void onbtnAddFriendClicked();
private:
	Ui::OnlineUsersClass ui;
};
