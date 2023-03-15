#include "FriendList.h"
#include "Protocol.h"
#include "NetDrive.h"
#include <QInputDialog>
#include <QDebug>
#include "Chatting.h"
#include <QMessageBox>

FriendList::FriendList(QWidget* parent)
	: QWidget(parent)
{
	messageTextBox = new QTextEdit;
	friendsList = new QListWidget;
	msgInputBox = new QLineEdit;

	btnDeleteFriend = new QPushButton("Delete Friends");
	btnRefreshFriend = new QPushButton("Refresh");
	btnShowOnlineUsers = new QPushButton("Show Users");
	btnSearchOnlineUsers = new QPushButton("Search..");
	btnSend = new QPushButton("Send");
	btnPrivateTalk = new QPushButton("Chat");

	QVBoxLayout* rightButtons = new QVBoxLayout;
	rightButtons->addWidget(btnDeleteFriend);
	rightButtons->addWidget(btnRefreshFriend);
	rightButtons->addWidget(btnShowOnlineUsers);
	rightButtons->addWidget(btnSearchOnlineUsers);
	rightButtons->addWidget(btnPrivateTalk);

	QHBoxLayout* topButtons = new QHBoxLayout;
	topButtons->addWidget(messageTextBox);
	topButtons->addWidget(friendsList);
	topButtons->addLayout(rightButtons);

	QHBoxLayout* messageBox = new QHBoxLayout;
	messageBox->addWidget(msgInputBox);
	messageBox->addWidget(btnSend);

	onlineu = new OnlineUsers;

	QVBoxLayout* mainLayout = new QVBoxLayout;
	mainLayout->addLayout(topButtons);
	mainLayout->addLayout(messageBox);
	mainLayout->addWidget(onlineu);
	onlineu->hide();

	setLayout(mainLayout);

	connect(btnShowOnlineUsers, SIGNAL(clicked()), this, SLOT(onBtnShowOnlineUsersClicked()));
	connect(btnSearchOnlineUsers, SIGNAL(clicked()), this, SLOT(onBtnSearchClicked()));
	connect(btnRefreshFriend, SIGNAL(clicked(bool)), this, SLOT(onBtnRefreshClicked()));
	connect(btnDeleteFriend, SIGNAL(clicked()), this, SLOT(onBtnDeleteFriendCLicked()));
	connect(btnPrivateTalk, SIGNAL(clicked(bool)), this, SLOT(onBtnPrivateTalkClicked()));
	connect(btnSend, SIGNAL(clicked()), this, SLOT(onBtnSendClicked()));
}

FriendList::~FriendList()
{
}

void FriendList::onBtnShowOnlineUsersClicked()
{
	if (onlineu->isHidden())
	{
		onlineu->show();

		PDU* pdu = makePDU(0);
		pdu->MsgType = ONLINE_REQUEST;
		NetDrive::getInstance().getTcpSocket().write((char*)pdu, pdu->PDULen);
		free(pdu);
		pdu = nullptr;
	}
	else
	{
		onlineu->hide();
	}
}

void FriendList::showOnlineUsers(PDU* pdu)
{
	if (NULL == pdu)
	{
		return;
	}
	onlineu->showUsers(pdu);
}

void FriendList::onBtnSearchClicked()
{
	strSearchResult = QInputDialog::getText(this, "Search User..", "Username");
	if (!strSearchResult.isEmpty())
	{
		qDebug() << strSearchResult;
		PDU* pdu = makePDU(0);
		memcpy(pdu->Data, strSearchResult.toStdString().c_str(), strSearchResult.size());
		pdu->MsgType = SEARCH_REQUEST;
		NetDrive::getInstance().getTcpSocket().write((char*)pdu, pdu->PDULen);
		free(pdu);
		pdu = nullptr;
	}
}

void FriendList::onBtnRefreshClicked()
{
	friendsList->clear();

	QString strName = NetDrive::getInstance().getLoginName();
	PDU* pdu = makePDU(0);
	pdu->MsgType = FRIENDLIST_REFRESH_REQUEST;
	memcpy(pdu->Data, strName.toStdString().c_str(), strName.size());
	NetDrive::getInstance().getTcpSocket().write((char*)pdu, pdu->PDULen);
	free(pdu);
	pdu = nullptr;
}

void FriendList::updateFriendList(PDU* pdu)
{
	if (pdu == nullptr)
	{
		return;
	}
	uint size = pdu->MsgLen / 64;
	char name[64] = { '\0' };
	for (uint i = 0; i < size; i++)
	{
		memcpy(name, (char*)(pdu->Msg) + i * 64, 64);
		friendsList->addItem(name);
	}
}

void FriendList::onBtnDeleteFriendCLicked()
{
	if (friendsList->currentItem() != nullptr)
	{
		QString friendName = friendsList->currentItem()->text();
		PDU* pdu = makePDU(0);
		pdu->MsgType = FRIEND_DELETE_REQUEST;
		QString myName = NetDrive::getInstance().getLoginName();
		memcpy(pdu->Data, myName.toStdString().c_str(), myName.size());
		memcpy(pdu->Data + 64, friendName.toStdString().c_str(), friendName.size());
		NetDrive::getInstance().getTcpSocket().write((char*)pdu, pdu->PDULen);
		free(pdu);
		pdu = nullptr;
	}
}

void FriendList::onBtnPrivateTalkClicked()
{
	if (friendsList->currentItem() != NULL)
	{
		QString friendName = friendsList->currentItem()->text();
		Chatting::getInstance().setChatUser(friendName);
		if (Chatting::getInstance().isHidden())
		{
			Chatting::getInstance().show();
		}
	}
	else
	{
		QMessageBox::warning(this, "Notification", "Choose someone to start a chat!");
	}
}

void FriendList::onBtnSendClicked()
{
	QString strMsg = msgInputBox->text();
	if (!strMsg.isEmpty())
	{
		PDU* pdu = makePDU(strMsg.size() + 1);
		pdu->MsgType = GROUP_CHAT_REQUEST;
		QString strName = NetDrive::getInstance().getLoginName();
		strncpy(pdu->Data, strName.toStdString().c_str(), strName.size());
		strncpy(reinterpret_cast<char*>(pdu->Msg), strMsg.toStdString().c_str(), strMsg.size());
		NetDrive::getInstance().getTcpSocket().write((char*)pdu, pdu->PDULen);
		msgInputBox->clear();
	}
	else
	{
		QMessageBox::warning(this, "Group Chat", "Can't send with nothing!");
	}
}

void FriendList::updateGroupMessage(PDU* pdu)
{
	QString strMsg = QString("%1 says: %2").arg(pdu->Data).arg((char*)pdu->Msg);
	messageTextBox->append(strMsg);
}