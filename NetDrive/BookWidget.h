#pragma once

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "Protocol.h"
#include <QTimer>

class BookWidget : public QWidget
{
	Q_OBJECT

public:
	explicit BookWidget(QWidget* parent = nullptr);
	~BookWidget();
	void updateFileList(const PDU* pdu);
	void clearEnteredDir();
	QString enteredDir();
	void setDownloadStatus(bool status);
	bool getDownloadStatus();
	QString getSaveFilePath();
	QString getShareFileName();

	qint64 m_iTotal;//总文件大小
	qint64 m_iReceived;//已接收多少

private slots:
	void onbtnCreateFolderClicked();
	void onBtnRefreshClicked();
	void onBtnDeleteFolderClicked();
	void onBtnRenameClicked();
	void onListDoubleClicked(const QModelIndex& index);
	void onBtnReturnClicked();
	void onBtnUploadFileClicked();
	void uploadFileTime();
	void onBtnDeleteFileClicked();
	void onBtnDownloadFileClicked();
	void onBtnShareClicked();
	void onBtnMoveClicked();
	void onBtnSelectDirClicked();
private:
	QListWidget* m_BookList;
	QPushButton* m_btnReturn;
	QPushButton* m_btnCreateFolder;
	QPushButton* m_btnDeleteFolder;
	QPushButton* m_btnRename;
	QPushButton* m_btnRefresh;
	QPushButton* m_btnUploadFile;
	QPushButton* m_btnDownloadFile;
	QPushButton* m_btnDeleteFile;
	QPushButton* m_btnShare;
	QPushButton* m_btnMove;
	QPushButton* m_btnSelectDir;

	QString strEnteredDir;
	QString strUploadFilePath;

	QTimer* timer;
	QString m_strSaveFilePath;
	bool m_bDownload;

	QString m_strShareFileName;

	QString m_strMoveFileName;
	QString m_strMoveFilePath;
	QString m_strDestDir;
};
