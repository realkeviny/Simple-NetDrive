#pragma once

#include <QWidget>
#include "ui_Chatting.h"
#include "Protocol.h"

QT_BEGIN_NAMESPACE
namespace Ui { class ChattingClass; };
QT_END_NAMESPACE

class Chatting : public QWidget
{
	Q_OBJECT

public:
	Chatting(QWidget* parent = nullptr);
	~Chatting();

	void setChatUser(QString name);

	static Chatting& getInstance();

	void updateMessage(const PDU* pdu);
private slots:
	void onBtnSendClicked();

private:
	Ui::ChattingClass* ui;
	QString chattingUserName;
	QString currentUserName;
};
