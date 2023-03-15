#ifndef _TCPSERVER_H
#define _TCPSERVER_H

#include <QtNetwork/QTcpServer>
#include <QList>
#include "TcpSocket.h"

class TcpServer :
	public QTcpServer
{
	Q_OBJECT
public:
	TcpServer();

	static TcpServer& getInstance();

	void incomingConnection(qintptr socketDescripter) override;

	void forward(const char* friendName, PDU* pdu);//×ª·¢
private slots:
	void deleteSocket(TcpSocket* sock);
private:
	QList<TcpSocket*> tcpSocketList;
};

#endif