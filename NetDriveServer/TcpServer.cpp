#include "TcpServer.h"
#include <QDebug>
#include <QList>

TcpServer::TcpServer()
{
}

TcpServer& TcpServer::getInstance()
{
	static TcpServer instance;
	return instance;
}

void TcpServer::incomingConnection(qintptr socketDescriptor)
{
	qDebug() << "New connection came";
	TcpSocket* pendingTcpSocket = new TcpSocket;
	pendingTcpSocket->setSocketDescriptor(socketDescriptor);
	tcpSocketList.append(pendingTcpSocket);

	connect(pendingTcpSocket, SIGNAL(offline(TcpSocket*)), this, SLOT(deleteSocket(TcpSocket*)));
}

void TcpServer::deleteSocket(TcpSocket* sock)
{
	auto iter = tcpSocketList.begin();
	for (; iter != tcpSocketList.end(); iter++)
	{
		if (sock == *iter)
		{
			//delete* iter;
			//*iter = nullptr;
			tcpSocketList.erase(iter);
			break;
		}
	}

	for (int i = 0; i < tcpSocketList.size(); i++)
	{
		qDebug() << tcpSocketList.at(i)->getName();
	}
}

void TcpServer::forward(const char* friendName, PDU* pdu)
{
	if (friendName == nullptr || pdu == nullptr)
	{
		return;
	}
	QString buf = friendName;
	for (int i = 0; i < tcpSocketList.size(); i++)
	{
		if (buf == tcpSocketList.at(i)->getName())
		{
			tcpSocketList.at(i)->write(reinterpret_cast<char*>(pdu), pdu->PDULen);
			break;
		}
	}
}