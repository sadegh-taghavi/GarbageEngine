#include "Settings.h"
#include <QtWidgets/QApplication>
#include <QStyleFactory>
#include <QFile>
#include "MemoryLeakDetect.h"

int main( int argc, char *argv[ ] )
{
	//argc = 4;
	//char *arg[  ] = { "E:\\1.exe", "E:\\1h_00_00_512.bmp", "E:\\1h_00_01_512.bmp", "E:\\1h_00_02_512.bmp" };
	//argv = arg;
	QApplication a( argc, argv );
	QFile styleFile;
	styleFile.setFileName( "Style.css" );
	if ( styleFile.open( QIODevice::ReadOnly ) )
	{
		QString styleString( styleFile.readAll( ) );
		qApp->setStyleSheet( styleString );
	}
	styleFile.close( );
	
	Settings s;
	if ( !s.setValues( argc, argv ) )
		return 0;
	s.show();
	return a.exec( );
}
