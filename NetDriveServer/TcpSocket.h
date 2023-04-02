#ifndef _TCPSOCKET_H
#define _TCPSOCKET_H

#include <QTcpSocket>
#include "Protocol.h"
#include "DatabaseOperation.h"
#include <QDir>
#include <QFile>

class TcpSocket :
	public QTcpSocket
{
	Q_OBJECT

public:
	TcpSocket();
	QString getName() const;

signals:
	void offline(TcpSocket* sock);

private slots:
	void receiveMessage();
	void clientOffline();

private:
	QString strName;

	QFile m_file;
	qint64 m_totalsize;
	qint64 m_receivedsize;
	bool m_uploadstatus;
};

#endif