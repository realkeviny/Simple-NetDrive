#pragma once

#include <QtWidgets/QWidget>
#include <QFile>
#include <QTcpSocket>
#include "ui_NetDrive.h"
#include "Protocol.h"
#include "OperationWidget.h"

class NetDrive : public QWidget
{
	Q_OBJECT

public:
	explicit NetDrive(QWidget* parent = nullptr);
	~NetDrive();

	void loadConfig();

	static NetDrive& getInstance();
	QTcpSocket& getTcpSocket();
	QString getLoginName() const;
	QString getCurrentPath() const;
	void setCurrentPath(QString newPath);

public slots:
	void showConnection();
	void receiveMessage();

private slots:
	//void onBtnSendClicked();
	void onBtnLoginClicked();
	void onBtnSignupClicked();
	void onBtnLogoutClicked();

private:
	Ui::NetDriveClass* ui;
	QString strIP;
	quint16 usPort;

	QTcpSocket tcpSocket;//socket���ӷ��������������ݽ���
	QString strNameLogin;

	QString currentPath;
	QFile m_file;
};
