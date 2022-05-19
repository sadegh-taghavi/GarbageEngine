#include "geditor.h"
#include "GEW_Group.h"
#include "GEW_Scene.h"
#include "GEW_Material.h"
#include "GEW_Entity.h"
#include <GE_Core.h>
#include "GEW_Light.h"
#include "GEW_Model.h"
#include "GEW_Propeties.h"
#include "GEW_Flare.h"
#include "GEW_Decal.h"
#include <GEW_Prs.h>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QToolBar>
#include <SpinBox.h>
#include <QToolButton>
#include "GEW_Postprocess.h"
#include "GEW_Physics.h"
#include "GEW_TerrainProperty.h"
#include "GEW_DefaultCameraProperty.h"
#include "MemoryLeakDetect.h"

GEditor::GEditor( QWidget *parent )
: QMainWindow( parent )
{
	ui.setupUi( this );
	
	GEW_Group::m_icon = QIcon( ":/Resources/MetroStyle/folder.png" );
	GEW_Entity::m_icon = QIcon( ":/Resources/MetroStyle/add_folder.png" );
	GEW_Light::m_iconDirectional = QIcon( ":/Resources/MetroStyle/sun.png" );
	GEW_Light::m_iconSpot = QIcon( ":/Resources/MetroStyle/pocket_lantern.png" );
	GEW_Light::m_iconPoint = QIcon( ":/Resources/MetroStyle/solutions.png" );
	GEW_Model::m_iconModel = QIcon( ":/Resources/MetroStyle/teapot.png" );
	GEW_Model::m_iconSubset = QIcon( ":/Resources/MetroStyle/pie.png" );
	GEW_Model::m_iconSubsetNoMaterial = QIcon( ":/Resources/MetroStyle/pie1.png" );
	GEW_Prs::m_iconQuestion = QIcon( ":/Resources/MetroStyle/help.png" );
	GEW_Flare::m_iconFlare = QIcon( ":/Resources/MetroStyle/flare.png" );
	GEW_Decal::m_iconDecal = QIcon( ":/Resources/MetroStyle/paper-26.png" );

	ui.Viewport->setFps( 10000.0f );

	GEW_Material::getSingleton().init();
	GEW_Scene::getSingleton().init();
	GEW_Physics::getSingleton().init();
}

GEditor::~GEditor()
{

}

void GEditor::on_Viewport_signalRepaint()
{
	GEW_Material::getSingleton().update( GE_Core::getSingleton().getElapsedTime() );
	GEW_Scene::getSingleton().update( ui.Viewport->m_inputData );
	GEW_Scene::getSingleton().render();
}

void GEditor::on_Viewport_signalResize( int i_width, int i_height )
{
	GEW_Scene::getSingleton().resize( i_width, i_height );
}

void GEditor::on_New_triggered()
{
	if ( GEW_Item::getCount() > 1 ||
		GEW_Material::getSingleton().nbGroup() > 1 )
	{
		if ( QMessageBox::warning(
			this,
			tr( "New..." ),
			tr( "All data will be lost.\nDo you want to continue?" ),
			QMessageBox::Yes,
			QMessageBox::No ) == QMessageBox::No )
			return;
	}

	GEW_ItemList::getSingleton().clearAll();
	GEW_Material::getSingleton().clearAll();
	GEW_Postprocess::getSingleton()->loadDefault();
	GEW_Prs::getSingleton().clearHistory();
	GEW_TerrainProperty::getSingleton().resetAll();
	GEW_DefaultCameraProperty::getSingleton()->on_Reset_clicked();
}

void GEditor::on_Open_triggered()
{
	if( GEW_Item::getCount() > 1 ||
		GEW_Material::getSingleton().nbGroup() > 1 )
	{
		if( QMessageBox::warning(
			this,
			tr( "Open..." ),
			tr( "All data will be lost.\nDo you want to continue?" ),
			QMessageBox::Yes,
			QMessageBox::No ) == QMessageBox::No )
			return;
	}

	GEW_ItemList::getSingleton().clearAll();
	GEW_Material::getSingleton().clearAll();
	GEW_Postprocess::getSingleton()->loadDefault();
	GEW_Prs::getSingleton().clearHistory();
	GEW_TerrainProperty::getSingleton().resetAll();

	QString fileName = QFileDialog::getOpenFileName(
		this,
		QString(),
		QString(),
		tr( "Project file (*.Gep)" ) );
	openFromCmd( fileName );
}


void GEditor::openFromCmd( QString &i_fileName )
{
	if ( i_fileName.isEmpty( ) )
		return;
	QString resourcePath = i_fileName.left( i_fileName.length( ) - 4 ) + "_resources/";
	QFile file( i_fileName );
	if ( file.open( QIODevice::ReadOnly ) )
	{
		m_lastProject = i_fileName;
		setWindowTitle( i_fileName );
		GEW_Material::getSingleton().loadFromFile( file, resourcePath );
		GEW_ItemList::getSingleton().LoadFromFile( file, resourcePath );
		GEW_Postprocess::getSingleton()->loadFromFile( file, resourcePath );
		GEW_Setting::getSingleton().loadFromFile( file, resourcePath );
		GEW_TerrainProperty::getSingleton().loadFromFile( file, resourcePath );
	}
	else
	{
		QMessageBox::critical(
			this,
			tr( "Opening project" ),
			tr( "Unable to open file." ) );
	}
}


void GEditor::on_Save_triggered()
{
	QString fileName;
	if( m_lastProject.isEmpty() )
	{
		fileName = QFileDialog::getSaveFileName(
			this,
			QString(),
			QString(),
			tr( "Project file (*.Gep)" ) );
		if( fileName.isEmpty() )
			return;
	} else
		fileName = m_lastProject;
	
	QString resourcePath = fileName.left( fileName.length() - 4 ) + "_resources/";
	QFile file( fileName );
	QDir dir;
	dir.mkdir( resourcePath );
	if( file.open( QIODevice::WriteOnly ) )
	{
		m_lastProject = fileName;
		setWindowTitle( fileName );

		GEW_Material::getSingleton().saveToFile( file, resourcePath );
		GEW_ItemList::getSingleton().saveToFile( file, resourcePath );
		GEW_Postprocess::getSingleton()->saveToFile( file, resourcePath );
		GEW_Setting::getSingleton().saveToFile( file, resourcePath );
		GEW_TerrainProperty::getSingleton().saveToFile(file, resourcePath);
		GEW_Prs::getSingleton().clearExtraHistory();
	} else
	{
		QMessageBox::critical(
			this,
			tr( "Saving project" ),
			tr( "Unable to create file or directory." ) );
	}
}

void GEditor::on_SaveAs_triggered()
{
	QString fileName = QFileDialog::getSaveFileName(
			this,
			QString(),
			QString(),
			tr( "Project file (*.Gep)" ) );
		if( fileName.isEmpty() )
			return;

	QString resourcePath = fileName.left( fileName.length() - 4 ) + "_resources/";
	QFile file( fileName );
	QDir dir;
	dir.mkdir( resourcePath );
	if( file.open( QIODevice::WriteOnly ) )
	{
		m_lastProject = fileName;
		setWindowTitle( fileName );
		GEW_Material::getSingleton().saveToFile( file, resourcePath );
		GEW_ItemList::getSingleton().saveToFile( file, resourcePath );
		GEW_Postprocess::getSingleton()->saveToFile( file, resourcePath );
		GEW_Setting::getSingleton().saveToFile( file, resourcePath );
		GEW_TerrainProperty::getSingleton().saveToFile(file, resourcePath);
		GEW_Prs::getSingleton().clearExtraHistory();
	} else
	{
		QMessageBox::critical(
			this,
			tr( "Saving project" ),
			tr( "Unable to create file or directory." ) );
	}
}
