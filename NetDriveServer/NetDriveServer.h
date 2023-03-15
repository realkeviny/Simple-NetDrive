#pragma once

#include <QtWidgets/QWidget>
#include "ui_NetDriveServer.h"

class NetDriveServer : public QWidget
{
	Q_OBJECT

public:
	NetDriveServer(QWidget* parent = nullptr);
	~NetDriveServer();

	void loadConfig();

private:
	Ui::NetDriveServerClass ui;
	QString strIP;
	quint16 usPort;
};
