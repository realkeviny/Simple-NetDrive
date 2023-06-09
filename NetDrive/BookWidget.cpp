#include "BookWidget.h"
#include "NetDrive.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>
#include "OperationWidget.h"
#include "ShareFile.h"

BookWidget::BookWidget(QWidget* parent)
	: QWidget(parent)
{
	strEnteredDir.clear();

	m_bDownload = false;
	timer = new QTimer;

	m_BookList = new QListWidget;
	m_btnReturn = new QPushButton("Back");
	m_btnCreateFolder = new QPushButton("Create Directory");
	m_btnDeleteFolder = new QPushButton("Remove Directory");
	m_btnRename = new QPushButton("Rename");
	m_btnRefresh = new QPushButton("Refresh");

	QVBoxLayout* directoryButtonSet = new QVBoxLayout;
	directoryButtonSet->addWidget(m_btnReturn);
	directoryButtonSet->addWidget(m_btnCreateFolder);
	directoryButtonSet->addWidget(m_btnDeleteFolder);
	directoryButtonSet->addWidget(m_btnRename);
	directoryButtonSet->addWidget(m_btnRefresh);

	m_btnUploadFile = new QPushButton("Upload");
	m_btnDownloadFile = new QPushButton("Download");
	m_btnDeleteFile = new QPushButton("Delete");
	m_btnShare = new QPushButton("Share");
	m_btnMove = new QPushButton("Move");
	m_btnSelectDir = new QPushButton("Target Path");
	m_btnSelectDir->setEnabled(false);

	QVBoxLayout* fileButtonSet = new QVBoxLayout;
	fileButtonSet->addWidget(m_btnUploadFile);
	fileButtonSet->addWidget(m_btnDownloadFile);
	fileButtonSet->addWidget(m_btnDeleteFile);
	fileButtonSet->addWidget(m_btnShare);
	fileButtonSet->addWidget(m_btnMove);
	fileButtonSet->addWidget(m_btnSelectDir);

	QHBoxLayout* mainLayout = new QHBoxLayout;
	mainLayout->addWidget(m_BookList);
	mainLayout->addLayout(directoryButtonSet);
	mainLayout->addLayout(fileButtonSet);

	setLayout(mainLayout);

	connect(m_btnCreateFolder, SIGNAL(clicked(bool)), this, SLOT(onbtnCreateFolderClicked()));
	connect(m_btnRefresh, SIGNAL(clicked()), this, SLOT(onBtnRefreshClicked()));
	connect(m_btnDeleteFolder, SIGNAL(clicked()), this, SLOT(onBtnDeleteFolderClicked()));
	connect(m_btnRename, SIGNAL(clicked()), this, SLOT(onBtnRenameClicked()));
	connect(m_BookList, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onListDoubleClicked(QModelIndex)));
	connect(m_btnReturn, SIGNAL(clicked()), this, SLOT(onBtnReturnClicked()));
	connect(m_btnUploadFile, SIGNAL(clicked()), this, SLOT(onBtnUploadFileClicked()));
	connect(timer, SIGNAL(timeout()), this, SLOT(uploadFileTime()));
	connect(m_btnDownloadFile, SIGNAL(clicked()), this, SLOT(onBtnDownloadFileClicked()));
	connect(m_btnDeleteFile, SIGNAL(clicked()), this, SLOT(onBtnDeleteFileClicked()));
	connect(m_btnShare, SIGNAL(clicked()), this, SLOT(onBtnShareClicked()));
	connect(m_btnMove, SIGNAL(clicked()), this, SLOT(onBtnMoveClicked()));
	connect(m_btnSelectDir, SIGNAL(clicked()), this, SLOT(onBtnSelectDirClicked()));
}

BookWidget::~BookWidget()
{}

void BookWidget::updateFileList(const PDU* pdu)
{
	if (nullptr == pdu)
	{
		return;
	}

	QListWidgetItem* temp = nullptr;
	int totalRow = m_BookList->count();
	while (m_BookList->count() > 0)
	{
		temp = m_BookList->item(totalRow - 1);
		m_BookList->removeItemWidget(temp);
		delete temp;
		totalRow--;
	}

	FileInfo* pointerFileInfo = nullptr;
	int count = pdu->MsgLen / sizeof(FileInfo);
	for (int index = 0; index < count; index++)
	{
		pointerFileInfo = (FileInfo*)(pdu->Msg) + index;
		qDebug() << pointerFileInfo->fileName << pointerFileInfo->fileType;
		QListWidgetItem* item = new QListWidgetItem;
		if (0 == pointerFileInfo->fileType)
		{
			item->setIcon(QIcon(QPixmap(":/Icons/Folder.png")));
		}
		else if (1 == pointerFileInfo->fileType)
		{
			item->setIcon(QIcon(QPixmap(":/Icons/File.png")));
		}
		item->setText(pointerFileInfo->fileName);
		m_BookList->addItem(item);
	}
}

void BookWidget::clearEnteredDir()
{
	strEnteredDir.clear();
}

QString BookWidget::enteredDir()
{
	return strEnteredDir;
}

void BookWidget::setDownloadStatus(bool status)
{
	m_bDownload = status;
}

bool BookWidget::getDownloadStatus()
{
	return m_bDownload;
}

QString BookWidget::getSaveFilePath()
{
	return m_strSaveFilePath;
}

QString BookWidget::getShareFileName()
{
	return m_strShareFileName;
}

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

void BookWidget::onBtnDeleteFolderClicked()
{
	QString currentPath = NetDrive::getInstance().getCurrentPath();
	QListWidgetItem* item = m_BookList->currentItem();
	if (nullptr == item)
	{
		QMessageBox::warning(this, "Delete Directory", "Please choose a directory to remove!");
	}
	else
	{
		QString strDelName = item->text();
		PDU* pdu = makePDU(currentPath.size() + 1);
		pdu->MsgType = DELETE_DIRECTORY_REQUEST;
		strncpy(pdu->Data, strDelName.toStdString().c_str(), strDelName.size());
		memcpy(pdu->Msg, currentPath.toStdString().c_str(), currentPath.size());
		NetDrive::getInstance().getTcpSocket().write(reinterpret_cast<char*>(pdu), pdu->PDULen);
		free(pdu);
		pdu = nullptr;
	}
}

void BookWidget::onBtnRenameClicked()
{
	QString currentPath = NetDrive::getInstance().getCurrentPath();
	QListWidgetItem* item = m_BookList->currentItem();
	if (nullptr == item)
	{
		QMessageBox::warning(this, "File Renaming", "Please choose a file to rename!");
	}
	else
	{
		QString strOldName = item->text();
		QString strNewName = QInputDialog::getText(this, "Renaming", "Input new file name:");
		if (!strNewName.isEmpty())
		{
			PDU* pdu = makePDU(currentPath.size() + 1);
			pdu->MsgType = RENAME_REQUEST;
			strncpy(pdu->Data, strOldName.toStdString().c_str(), strOldName.size());
			strncpy(pdu->Data + 64, strNewName.toStdString().c_str(), strNewName.size());
			memcpy(pdu->Msg, currentPath.toStdString().c_str(), currentPath.size());
			NetDrive::getInstance().getTcpSocket().write(reinterpret_cast<char*>(pdu), pdu->PDULen);
			free(pdu);
			pdu = nullptr;
		}
		else
		{
			QMessageBox::warning(this, "File Renaming", "Characters required!");
		}
	}
}

void BookWidget::onListDoubleClicked(const QModelIndex& index)
{
	QString strDirName = index.data().toString();
	strEnteredDir = strDirName;
	QString currentPath = NetDrive::getInstance().getCurrentPath();
	PDU* pdu = makePDU(currentPath.size() + 1);
	pdu->MsgType = ENTER_DIRECTORY_REQUEST;
	strncpy(pdu->Data, strDirName.toStdString().c_str(), strDirName.size());
	memcpy(pdu->Msg, currentPath.toStdString().c_str(), currentPath.size());

	NetDrive::getInstance().getTcpSocket().write(reinterpret_cast<char*>(pdu), pdu->PDULen);
	free(pdu);
	pdu = nullptr;
}

void BookWidget::onBtnReturnClicked()
{
	QString currentPath = NetDrive::getInstance().getCurrentPath();
	QString rootPath = "D:/UserFiles/" + NetDrive::getInstance().getLoginName();
	if (currentPath == rootPath)
	{
		QMessageBox::warning(this, "Returning", "Failed:Already at the top level!");
	}
	else
	{
		int index = currentPath.lastIndexOf('/');
		currentPath.remove(index, currentPath.size() - index);
		qDebug() << "Effect:" << currentPath;
		NetDrive::getInstance().setCurrentPath(currentPath);

		clearEnteredDir();

		onBtnRefreshClicked();
	}
}

void BookWidget::onBtnUploadFileClicked()
{
	strUploadFilePath = QFileDialog::getOpenFileName();
	qDebug() << strUploadFilePath;
	if (!strUploadFilePath.isEmpty())
	{
		//aa/bb/cc -> cc
		int index = strUploadFilePath.lastIndexOf('/');
		QString strFileName = strUploadFilePath.right(strUploadFilePath.size() - index - 1);
		qDebug() << strFileName;

		QFile file(strUploadFilePath);
		qint64 fileSize = file.size();//�ļ���С

		QString currentPath = NetDrive::getInstance().getCurrentPath();
		PDU* pdu = makePDU(currentPath.size() + 1);
		pdu->MsgType = UPLOAD_REQUEST;
		memcpy(pdu->Msg, currentPath.toStdString().c_str(), currentPath.size());
		sprintf(pdu->Data, "%s %lld", strFileName.toStdString().c_str(), fileSize);

		NetDrive::getInstance().getTcpSocket().write(reinterpret_cast<char*>(pdu), pdu->PDULen);
		free(pdu);
		pdu = nullptr;

		timer->start(1500);
	}
	else
	{
		QMessageBox::warning(this, "Uploading", "File Required");
	}
}

void BookWidget::uploadFileTime()
{
	timer->stop();
	QFile file(strUploadFilePath);
	if (!file.open(QIODevice::ReadOnly))
	{
		QMessageBox::warning(this, "Uploading", "Unable to open file");
		return;
	}

	char* pBuffer = new char[4096];
	qint64 ret = 0;
	while (true)
	{
		ret = file.read(pBuffer, 4096);
		if (ret > 0 && ret <= 4096)
		{
			NetDrive::getInstance().getTcpSocket().write(pBuffer, ret);
		}
		else if (0 == ret)
		{
			break;
		}
		else
		{
			QMessageBox::warning(this, "Uploading", "Failed to upload:Unable to read!");
			break;
		}
	}
	file.close();
	delete[] pBuffer;
	pBuffer = nullptr;
}

void BookWidget::onBtnDeleteFileClicked()
{
	QString currentPath = NetDrive::getInstance().getCurrentPath();
	QListWidgetItem* item = m_BookList->currentItem();
	if (nullptr == item)
	{
		QMessageBox::warning(this, "Delete File", "Please choose a file to remove!");
	}
	else
	{
		QString strDelName = item->text();
		PDU* pdu = makePDU(currentPath.size() + 1);
		pdu->MsgType = DELETE_FILE_REQUEST;
		strncpy(pdu->Data, strDelName.toStdString().c_str(), strDelName.size());
		memcpy(pdu->Msg, currentPath.toStdString().c_str(), currentPath.size());
		NetDrive::getInstance().getTcpSocket().write(reinterpret_cast<char*>(pdu), pdu->PDULen);
		free(pdu);
		pdu = nullptr;
	}
}

void BookWidget::onBtnDownloadFileClicked()
{
	QListWidgetItem* pItem = m_BookList->currentItem();
	if (nullptr == pItem)
	{
		QMessageBox::warning(this, "Download", "Please choose a file to download!");
	}
	else
	{
		QString strSaveFilePath = QFileDialog::getSaveFileName();
		if (strSaveFilePath.isEmpty())
		{
			QMessageBox::warning(this, "Download", "Please choose save path!");
			m_strSaveFilePath.clear();
		}
		else
		{
			m_strSaveFilePath = strSaveFilePath;
		}
		QString strCurPath = NetDrive::getInstance().getCurrentPath();
		PDU* pdu = makePDU(strCurPath.size() + 1);
		pdu->MsgType = DOWNLOAD_REQUEST;
		QString strFileName = pItem->text();
		strcpy(pdu->Data, strFileName.toStdString().c_str());
		memcpy(pdu->Msg, strCurPath.toStdString().c_str(), strCurPath.size());
		NetDrive::getInstance().getTcpSocket().write(reinterpret_cast<char*>(pdu), pdu->PDULen);
		free(pdu);
		pdu = nullptr;
	}
}

void BookWidget::onBtnShareClicked()
{
	QListWidgetItem* pItem = m_BookList->currentItem();
	if (nullptr == pItem)
	{
		QMessageBox::warning(this, "Share", "Please choose a file to share!");
		return;
	}
	else
	{
		m_strShareFileName = pItem->text();
	}

	FriendList* pFList = OperationWidget::getInstance().getFriendList();
	QListWidget* friendList = pFList->getFriendList();
	ShareFile::getInstance().updateFriend(friendList);
	if (ShareFile::getInstance().isHidden())
	{
		ShareFile::getInstance().show();
	}
}

void BookWidget::onBtnMoveClicked()
{
	QListWidgetItem* currentItem = m_BookList->currentItem();
	if (currentItem != nullptr)
	{
		m_strMoveFileName = currentItem->text();
		QString strCurPath = NetDrive::getInstance().getCurrentPath();
		m_strMoveFilePath = strCurPath + '/' + m_strMoveFileName;

		m_btnSelectDir->setEnabled(true);
	}
	else
	{
		QMessageBox::warning(this, "Move", "Please choose a file to move!");
	}
}

void BookWidget::onBtnSelectDirClicked()
{
	QListWidgetItem* currentItem = m_BookList->currentItem();
	if (currentItem != nullptr)
	{
		QString strDestDir = currentItem->text();
		QString strCurPath = NetDrive::getInstance().getCurrentPath();
		m_strDestDir = strCurPath + '/' + strDestDir;

		int srcLength = m_strMoveFilePath.size();
		int dstLength = m_strDestDir.size();
		PDU* pdu = makePDU(srcLength + dstLength + 2);
		pdu->MsgType = MOVE_FILE_REQUEST;
		sprintf(pdu->Data, "%d %d %s", srcLength, dstLength, m_strMoveFileName.toStdString().c_str());

		memcpy(pdu->Msg, m_strMoveFilePath.toStdString().c_str(), srcLength);
		memcpy(reinterpret_cast<char*>(pdu->Msg) + (srcLength + 1), m_strDestDir.toStdString().c_str(), dstLength);

		NetDrive::getInstance().getTcpSocket().write(reinterpret_cast<char*>(pdu), pdu->PDULen);
		free(pdu);
		pdu = nullptr;
	}
	else
	{
		QMessageBox::warning(this, "Move", "Please choose a file to move!");
	}
	m_btnSelectDir->setEnabled(false);
}