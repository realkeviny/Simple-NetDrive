#include "NetDrive.h"
#include <QtWidgets/QApplication>
#include "ShareFile.h"

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);
	//NetDrive w;
	//w.show();

	NetDrive::getInstance().show();
	//ShareFile sf;
	//sf.test();
	//sf.show();

	//BookWidget bw;
	//bw.show();
	return a.exec();
}