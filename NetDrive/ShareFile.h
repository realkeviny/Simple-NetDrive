#pragma once

#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QButtonGroup>
#include <QScrollArea>
#include <QCheckBox>
#include <QListWidget>

class ShareFile : public QWidget
{
	Q_OBJECT

public:
	ShareFile(QWidget* parent = nullptr);

	static ShareFile& getInstance();

	void test();
	void updateFriend(QListWidget* fList);
	~ShareFile();

private slots:

private:
	QPushButton* m_btnSelectAll;
	QPushButton* m_btnCancelSelected;

	QPushButton* m_btnOK;
	QPushButton* m_btnCancel;

	QScrollArea* m_sa;
	QWidget* m_friendWidget;
	QVBoxLayout* m_friendWidgetLayout;
	QButtonGroup* m_btnGroup;
};
