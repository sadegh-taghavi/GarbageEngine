#include <windows.h>
#include <QtWidgets/QApplication>
#include <QStyleFactory>
#include <QFile>
#include "geditor.h"

#include "MemoryLeakDetect.h"

int main( int argc, char *argv[] )
{
#if defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif
	QApplication a( argc, argv );
	srand( GetTickCount() );
	//qApp->setStyle( QStyleFactory::create( "Fusion" ) );
	QFile styleFile;
	styleFile.setFileName( "../../GEditorData/Style.css" );
	if( styleFile.open( QIODevice::ReadOnly ) )
	{
		QString styleString( styleFile.readAll() );
		qApp->setStyleSheet( styleString );
	}
	styleFile.close();

	GEditor w;
	w.show();
	w.setWindowState( Qt::WindowState::WindowMaximized );
	if ( argc > 1 )
		w.openFromCmd( QString(argv[ 1 ]) );
	return a.exec();
}
