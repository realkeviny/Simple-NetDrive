#include "OnlineUsers.h"
#include <QDebug>
#include "NetDrive.h"

OnlineUsers::OnlineUsers(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	connect(ui.btnAddFriend, SIGNAL(clicked()), this, SLOT(onbtnAddFriendClicked()));
}

OnlineUsers::~OnlineUsers()
{}

void OnlineUsers::showUsers(PDU* pdu)
{
	if (NULL == pdu)
	{
		return;
	}
	uint Size = pdu->MsgLen / 64;
	char temp[64];
	for (uint i = 0; i < Size; ++i)
	{
		memcpy(temp, (char*)(pdu->Msg) + i * 64, 64);
		ui.friendList->addItem(temp);
	}
}

void OnlineUsers::onbtnAddFriendClicked()
{
	QListWidgetItem* LItem = ui.friendList->currentItem();
	QString neededUserName = LItem->text();
	QString receivedUserName = NetDrive::getInstance().getLoginName();
	PDU* pdu = makePDU(0);
	pdu->MsgType = ADD_REQUEST;
	memcpy(pdu->Data, neededUserName.toStdString().c_str(), neededUserName.size());
	memcpy(pdu->Data + 64, receivedUserName.toStdString().c_str(), receivedUserName.size());
	NetDrive::getInstance().getTcpSocket().write((char*)pdu, pdu->PDULen);
	free(pdu);
	pdu = nullptr;
}