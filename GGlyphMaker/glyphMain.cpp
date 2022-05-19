#include "gglyphmaker.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	GGlyphMaker w;
	w.show();
	return a.exec();
}
