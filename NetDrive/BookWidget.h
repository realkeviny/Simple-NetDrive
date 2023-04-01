#pragma once

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "Protocol.h"

class BookWidget : public QWidget
{
	Q_OBJECT

public:
	explicit BookWidget(QWidget* parent = 0);
	~BookWidget();
	void updateFileList(const PDU* pdu);
	void clearEnteredDir();
	QString enteredDir();

private slots:
	void onbtnCreateFolderClicked();
	void onBtnRefreshClicked();
	void onBtnDeleteFolderClicked();
	void onBtnRenameClicked();
	void onListDoubleClicked(const QModelIndex& index);
	void onBtnReturnClicked();
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

	QString strEnteredDir;
};
