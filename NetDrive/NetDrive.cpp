#include "NetDrive.h"
#include <QByteArray>
#include <QDebug>
#include <QMessageBox>
#include <QHostAddress>
#include "Chatting.h"

NetDrive::NetDrive(QWidget* parent)
	: QWidget(parent), ui(new Ui::NetDriveClass)
{
	ui->setupUi(this);
	loadConfig();

	resize(500, 250);

	connect(&tcpSocket, SIGNAL(connected()), this, SLOT(showConnection()));
	connect(&tcpSocket, SIGNAL(readyRead()), this, SLOT(receiveMessage()));
	//connect(ui->btnSend, SIGNAL(clicked()), this, SLOT(onBtnSendClicked()));
	connect(ui->btnLogin, SIGNAL(clicked()), this, SLOT(onBtnLoginClicked()));
	connect(ui->btnSignup, SIGNAL(clicked()), this, SLOT(onBtnSignupClicked()));
	connect(ui->btnLogout, SIGNAL(clicked()), this, SLOT(onBtnLogoutClicked()));
	tcpSocket.connectToHost(QHostAddress(strIP), usPort);//连接服务器
}

NetDrive::~NetDrive()
{
	delete ui;
}

void NetDrive::loadConfig()
{
	QFile file(":/NetDrive/Client.config");
	if (file.open(QIODevice::ReadOnly))
	{
		QByteArray basicData = file.readAll();
		QString strData = basicData.toStdString().c_str();
		file.close();

		strData.replace("\r\n", " ");

		QStringList strList = strData.split(" ");
		strIP = strList.at(0);
		usPort = strList.at(1).toUShort();
	}
	else
	{
		QMessageBox::critical(this, "Open Config", "Failed to open config");
	}
}

void NetDrive::showConnection()
{
	QMessageBox::information(this, "Status", "Successfully Connected");
}

//void NetDrive::onBtnSendClicked()
//{
//	QString strMsg = ui->lineEdit->text();
//	if (!strMsg.isEmpty())
//	{
//		PDU* pdu = makePDU(strMsg.size() + 1);
//		pdu->MsgType = 8998;
//		memcpy(pdu->Msg, strMsg.toStdString().c_str(), strMsg.size());
//		qDebug() << (char*)(pdu->Msg);
//		tcpSocket.write((char*)pdu, pdu->PDULen);
//		free(pdu);
//		pdu = nullptr;
//	}
//	else
//	{
//		QMessageBox::warning(this, "Message", "Can't be empty!");
//	}
//}

void NetDrive::onBtnLoginClicked()
{
	QString strName = ui->lineUname->text();
	QString strPassword = ui->linePsd->text();
	if (!strPassword.isEmpty() && !strName.isEmpty())
	{
		strNameLogin = strName;
		PDU* pdu = makePDU(0);
		pdu->MsgType = MSG_TYPE::LOGIN_REQUEST;
		strncpy(pdu->Data, strName.toStdString().c_str(), 64);
		strncpy(pdu->Data + 64, strPassword.toStdString().c_str(), 64);
		tcpSocket.write((char*)pdu, pdu->PDULen);
		free(pdu);
		pdu = nullptr;
	}
	else
	{
		QMessageBox::critical(this, "Login", "Name or Password is Empty!");
	}
}

void NetDrive::onBtnSignupClicked()
{
	QString strName = ui->lineUname->text();
	QString strPassword = ui->linePsd->text();
	if (!strPassword.isEmpty() && !strName.isEmpty())
	{
		PDU* pdu = makePDU(0);
		pdu->MsgType = MSG_TYPE::REGISTER_REQUEST;
		strncpy(pdu->Data, strName.toStdString().c_str(), 64);
		strncpy(pdu->Data + 64, strPassword.toStdString().c_str(), 64);
		tcpSocket.write((char*)pdu, pdu->PDULen);
		free(pdu);
		pdu = nullptr;
	}
	else
	{
		QMessageBox::critical(this, "Sign up", "Name or Password is required!");
	}
}

void NetDrive::onBtnLogoutClicked()
{
}

void NetDrive::receiveMessage()
{
	qDebug() << tcpSocket.bytesAvailable();
	uint PDULen = 0;
	tcpSocket.read((char*)&PDULen, sizeof(uint));
	uint MsgLen = PDULen - sizeof(PDU);
	PDU* pdu = makePDU(MsgLen);
	tcpSocket.read((char*)pdu + sizeof(uint), PDULen - sizeof(uint));
	switch (pdu->MsgType)
	{
	case REGISTER_RESPOND:
	{
		if (0 == strcmp(pdu->Data, REGISTER_OK))
		{
			QMessageBox::information(this, "Information", REGISTER_OK);
		}
		else
		{
			QMessageBox::warning(this, "Register", REGISTER_FAILURE);
		}
		break;
	}
	case LOGIN_RESPOND:
	{
		if (0 == strcmp(pdu->Data, LOGIN_OK))
		{
			currentPath = QString("D:/UserFiles/%1").arg(strNameLogin);
			QMessageBox::information(this, "Information", LOGIN_OK);
			OperationWidget::getInstance().show();
			hide();
		}
		else
		{
			QMessageBox::warning(this, "Login", LOGIN_FAILURE);
		}
		break;
	}
	case ONLINE_RESPOND:
	{
		OperationWidget::getInstance().getFriendList()->showOnlineUsers(pdu);

		break;
	}
	case SEARCH_RESPOND:
	{
		if (strcmp(SEARCH_USER_INEXIST, pdu->Data) == 0)
		{
			QMessageBox::information(this, "Search", QString("%1:User doesn't exist!").arg(OperationWidget::getInstance().getFriendList()->strSearchResult));
		}
		else if (strcmp(SEARCH_USER_EXIST_ONLINE, pdu->Data) == 0)
		{
			QMessageBox::information(this, "Search", QString("%1:User exists and online!").arg(OperationWidget::getInstance().getFriendList()->strSearchResult));
		}
		else if (strcmp(SEARCH_USER_EXIST_OFFLINE, pdu->Data) == 0)
		{
			QMessageBox::information(this, "Search", QString("%1:User exists but offline!").arg(OperationWidget::getInstance().getFriendList()->strSearchResult));
		}
		break;
	}
	case ADD_REQUEST:
	{
		char friendName[64] = { '\0' };

		strncpy(friendName, pdu->Data + 64, 64);
		int ret = QMessageBox::information(this, "Friend Adding", QString("%1 wants to add you as friend.").arg(friendName), QMessageBox::Yes, QMessageBox::No);

		PDU* responsePDU = makePDU(0);
		memcpy(responsePDU->Data, pdu->Data, 128);
		if (ret == QMessageBox::Yes)
		{
			responsePDU->MsgType = ADD_AGREEMENT;
		}
		else
		{
			responsePDU->MsgType = ADD_REFUSE;
		}
		tcpSocket.write(reinterpret_cast<char*>(responsePDU), responsePDU->PDULen);
		free(responsePDU);
		responsePDU = nullptr;
		break;
	}
	case ADD_RESPOND:
	{
		QMessageBox::information(this, "Friend Adding", pdu->Data);
	}
	case ADD_AGREEMENT:
	{
		char friendName[64] = { '\0' };
		memcpy(friendName, pdu->Data, 64);
		QMessageBox::information(this, "Success", QString("%1 is your friend now").arg(friendName));
		break;
	}
	case ADD_REFUSE:
	{
		char friendName[64] = { '\0' };
		memcpy(friendName, pdu->Data, 64);
		QMessageBox::information(this, "Failure", QString("%1 refused to be your friend ").arg(friendName));
		break;
	}

	case FRIENDLIST_REFRESH_RESPOND:
	{
		OperationWidget::getInstance().getFriendList()->updateFriendList(pdu);
		break;
	}

	case FRIEND_DELETE_REQUEST:
	{
		char name[64] = { '\0' };
		memcpy(name, pdu->Data, 64);
		QMessageBox::information(this, "Friend Deleting", QString("%1 deleted the friendship with you!").arg(name));
		break;
	}
	case FRIEND_DELETE_RESPOND:
	{
		QMessageBox::information(this, "Friend Deleting", "Success");
		break;
	}
	case CHAT_REQUEST:
	{
		if (Chatting::getInstance().isHidden())
		{
			Chatting::getInstance().show();
		}
		char Sender[64] = { '\0' };
		memcpy(Sender, pdu->Data, 64);
		QString strSender = Sender;
		Chatting::getInstance().setChatUser(strSender);
		Chatting::getInstance().updateMessage(pdu);
		break;
	}
	case GROUP_CHAT_REQUEST:
	{
		OperationWidget::getInstance().getFriendList()->updateGroupMessage(pdu);
		break;
	}
	case CREATE_DIRECTORY_RESPOND:
	{
		QMessageBox::information(this, "Directory Creating", pdu->Data);
		break;
	}
	case REFRESH_RESPOND:
	{
		OperationWidget::getInstance().getBook()->updateFileList(pdu);
		QString enteredDir = OperationWidget::getInstance().getBook()->enteredDir();
		if (!enteredDir.isEmpty())
		{
			currentPath = currentPath + "/" + enteredDir;
			qDebug() << "Entering:" << currentPath;
		}
		break;
	}
	case DELETE_DIRECTORY_RESPOND:
	{
		QMessageBox::information(this, "Directory Removing", pdu->Data);
		break;
	}
	case RENAME_RESPOND:
	{
		QMessageBox::information(this, "Renaming", pdu->Data);
		break;
	}
	case ENTER_DIRECTORY_RESPOND:
	{
		OperationWidget::getInstance().getBook()->clearEnteredDir();
		QMessageBox::information(this, "Entering Directory", pdu->Data);
		break;
	}
	case UPLOAD_RESPOND:
	{
		QMessageBox::information(this, "Uploading", pdu->Data);
		break;
	}
	default:
		break;
	}
	free(pdu);
	pdu = nullptr;
}

NetDrive& NetDrive::getInstance()
{
	static NetDrive instance;
	return instance;
}

QTcpSocket& NetDrive::getTcpSocket()
{
	return tcpSocket;
}

QString NetDrive::getLoginName() const
{
	return strNameLogin;
}

QString NetDrive::getCurrentPath() const
{
	return currentPath;
}

void NetDrive::setCurrentPath(QString newPath)
{
	currentPath = newPath;
}