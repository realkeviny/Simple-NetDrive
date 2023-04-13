#ifndef _TCPSOCKET_H
#define _TCPSOCKET_H

#include <QTcpSocket>
#include "Protocol.h"
#include "DatabaseOperation.h"
#include <QDir>
#include <QFile>
#include <QTimer>

class TcpSocket :
	public QTcpSocket
{
	Q_OBJECT

public:
	TcpSocket();
	QString getName() const;
	void copyDirectory(QString strSrcDir, QString strDstDir);

signals:
	void offline(TcpSocket* sock);

private slots:
	void receiveMessage();
	void clientOffline();
	void sendFileToClient();

private:
	QString strName;

	QFile m_file;
	qint64 m_totalsize;
	qint64 m_receivedsize;
	bool m_uploadstatus;

	QTimer* m_pTimer;
};

#endif