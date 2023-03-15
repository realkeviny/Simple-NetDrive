#include "Chatting.h"
#include "Protocol.h"
#include "NetDrive.h"
#include <QMessageBox>

Chatting::Chatting(QWidget* parent)
	: QWidget(parent)
	, ui(new Ui::ChattingClass())
{
	ui->setupUi(this);

	connect(ui->btnSend, SIGNAL(clicked()), this, SLOT(onBtnSendClicked()));
}

Chatting::~Chatting()
{
	delete ui;
}

void Chatting::setChatUser(QString name)
{
	chattingUserName = name;
	currentUserName = NetDrive::getInstance().getLoginName();
}

void Chatting::onBtnSendClicked()
{
	QString chatMessage = ui->textBox->text();
	ui->textBox->clear();
	if (!chatMessage.isEmpty())
	{
		PDU* pdu = makePDU(chatMessage.size() + 1);
		pdu->MsgType = CHAT_REQUEST;

		memcpy(pdu->Data, currentUserName.toStdString().c_str(), currentUserName.size());
		memcpy(pdu->Data + 64, chattingUserName.toStdString().c_str(), chattingUserName.size());

		strcpy((char*)pdu->Msg, chatMessage.toStdString().c_str());

		NetDrive::getInstance().getTcpSocket().write((char*)pdu, pdu->PDULen);
		free(pdu);
		pdu = nullptr;
	}
	else
	{
		QMessageBox::warning(this, "Notification", "Can't send with nothing!");
	}
}

void Chatting::updateMessage(const PDU* pdu)
{
	if (pdu == nullptr)
	{
		return;
	}
	char Sender[64] = { '\0' };
	memcpy(Sender, pdu->Data, 64);
	QString strMessage = QString("%1 says: %2").arg(Sender).arg((char*)pdu->Msg);
	ui->messageDisplay->append(strMessage);
}

Chatting& Chatting::getInstance()
{
	static Chatting instance;
	return instance;
}