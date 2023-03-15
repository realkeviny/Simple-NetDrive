#include "NetDriveServer.h"

#include <QtWidgets/QApplication>

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);

	//DatabaseOperation::getInstance().initDatabase();

	NetDriveServer w;
	w.show();
	return a.exec();
}