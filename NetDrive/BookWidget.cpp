#include "BookWidget.h"
#include "NetDrive.h"
#include <QInputDialog>
#include <QMessageBox>

BookWidget::BookWidget(QWidget* parent)
	: QWidget(parent)
{
	m_BookList = new QListWidget;
	m_btnReturn = new QPushButton("Back");
	m_btnCreateFolder = new QPushButton("Create Directory");
	m_btnDeleteFolder = new QPushButton("Remove Directory");
	m_btnRenameFolder = new QPushButton("Rename");
	m_btnRefresh = new QPushButton("Refresh");

	QVBoxLayout* directoryButtonSet = new QVBoxLayout;
	directoryButtonSet->addWidget(m_btnReturn);
	directoryButtonSet->addWidget(m_btnCreateFolder);
	directoryButtonSet->addWidget(m_btnDeleteFolder);
	directoryButtonSet->addWidget(m_btnRenameFolder);
	directoryButtonSet->addWidget(m_btnRefresh);

	m_btnUploadFile = new QPushButton("Upload");
	m_btnDownloadFile = new QPushButton("Download");
	m_btnDeleteFile = new QPushButton("Delete");
	m_btnShare = new QPushButton("Share");

	QVBoxLayout* fileButtonSet = new QVBoxLayout;
	fileButtonSet->addWidget(m_btnUploadFile);
	fileButtonSet->addWidget(m_btnDownloadFile);
	fileButtonSet->addWidget(m_btnDeleteFile);
	fileButtonSet->addWidget(m_btnShare);

	QHBoxLayout* mainLayout = new QHBoxLayout;
	mainLayout->addWidget(m_BookList);
	mainLayout->addLayout(directoryButtonSet);
	mainLayout->addLayout(fileButtonSet);

	setLayout(mainLayout);

	connect(m_btnCreateFolder, SIGNAL(clicked(bool)), this, SLOT(onbtnCreateFolderClicked()));
	connect(m_btnRefresh, SIGNAL(clicked()), this, SLOT(onBtnRefreshClicked()));
}

BookWidget::~BookWidget()
{}

void BookWidget::onbtnCreateFolderClicked()
{
	QString strNewDir = QInputDialog::getText(this, "New Folder", "New Folder Name..");
	if (!strNewDir.isEmpty())
	{
		if (strNewDir.size() > 64)
		{
			QMessageBox::warning(this, "Warning", "Folder name is too long!");
		}
		else
		{
			QString strName = NetDrive::getInstance().getLoginName();
			QString strCurrentPath = NetDrive::getInstance().getCurrentPath();
			PDU* pdu = makePDU(strCurrentPath.size() + 1);
			pdu->MsgType = CREATE_DIRECTORY_REQUEST;
			strncpy(pdu->Data, strName.toStdString().c_str(), strName.size());
			strncpy(pdu->Data + 64, strNewDir.toStdString().c_str(), strNewDir.size());
			memcpy(pdu->Msg, strCurrentPath.toStdString().c_str(), strCurrentPath.size());

			NetDrive::getInstance().getTcpSocket().write(reinterpret_cast<char*>(pdu), pdu->PDULen);
			free(pdu);
			pdu = nullptr;
		}
	}
	else
	{
		QMessageBox::warning(this, "Warning", "Can't left folder name empty!");
	}
}

void BookWidget::onBtnRefreshClicked()
{
	QString currentPath = NetDrive::getInstance().getCurrentPath();
	PDU* pdu = makePDU(currentPath.size() + 1);
	pdu->MsgType = REFRESH_REQUEST;
	strncpy(reinterpret_cast<char*>(pdu->Msg), currentPath.toStdString().c_str(), currentPath.size());

	NetDrive::getInstance().getTcpSocket().write(reinterpret_cast<char*>(pdu), pdu->PDULen);
	free(pdu);
	pdu = nullptr;
}