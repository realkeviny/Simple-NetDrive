#pragma once

#include <QWidget>
#include <QListWidget>
#include "FriendList.h"
#include "BookWidget.h"
#include <QStackedWidget>

class OperationWidget : public QWidget
{
	Q_OBJECT

public:
	explicit OperationWidget(QWidget* parent = 0);
	static OperationWidget& getInstance();
	FriendList* getFriendList() const;

private:
	QListWidget* listW;
	FriendList* friendList;
	BookWidget* bookList;

	QStackedWidget* sw;
};
