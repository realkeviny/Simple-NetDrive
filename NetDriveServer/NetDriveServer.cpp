#include "NetDriveServer.h"
#include "TcpServer.h"
#include <QByteArray>
#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <QtNetwork/QHostAddress>

NetDriveServer::NetDriveServer(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	loadConfig();
	TcpServer::getInstance().listen(QHostAddress(strIP), usPort);
}

NetDriveServer::~NetDriveServer()
{}

void NetDriveServer::loadConfig()
{
	QFile file(":/NetDriveServer/Server.config");
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