#include "NetDrive.h"
#include <QtWidgets/QApplication>

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);
	//NetDrive w;
	//w.show();

	NetDrive::getInstance().show();
	return a.exec();
}