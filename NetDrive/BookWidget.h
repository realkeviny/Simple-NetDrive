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

private slots:
	void onbtnCreateFolderClicked();
	void onBtnRefreshClicked();
private:
	QListWidget* m_BookList;
	QPushButton* m_btnReturn;
	QPushButton* m_btnCreateFolder;
	QPushButton* m_btnDeleteFolder;
	QPushButton* m_btnRenameFolder;
	QPushButton* m_btnRefresh;
	QPushButton* m_btnUploadFile;
	QPushButton* m_btnDownloadFile;
	QPushButton* m_btnDeleteFile;
	QPushButton* m_btnShare;
};
