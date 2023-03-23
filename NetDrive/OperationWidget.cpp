#include "OperationWidget.h"

OperationWidget::OperationWidget(QWidget* parent)
	: QWidget(parent)
{
	listW = new QListWidget(this);
	listW->addItem("Friends");
	listW->addItem("Books");

	friendList = new FriendList;
	bookList = new BookWidget;

	sw = new QStackedWidget;
	sw->addWidget(friendList);
	sw->addWidget(bookList);

	QHBoxLayout* mainLayout = new QHBoxLayout;
	mainLayout->addWidget(listW);
	mainLayout->addWidget(sw);

	setLayout(mainLayout);

	connect(listW, SIGNAL(currentRowChanged(int)), sw, SLOT(setCurrentIndex(int)));
}

OperationWidget& OperationWidget::getInstance()
{
	static OperationWidget instance;
	return instance;
}

FriendList* OperationWidget::getFriendList() const
{
	return friendList;
}

BookWidget* OperationWidget::getBook() const
{
	return bookList;
}