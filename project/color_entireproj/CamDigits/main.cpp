#include "camdigits.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	CamDigits w;
	w.show();
	return a.exec();
}
