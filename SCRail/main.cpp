#include "SCRail.h"
#include"printinfo.h"
#include <QtWidgets/QApplication>
#include<Windows.h>
int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	SCRail w;

	w.show();
	return a.exec();
}
