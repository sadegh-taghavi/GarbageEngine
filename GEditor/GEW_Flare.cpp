#include "GEW_Flare.h"
#include "GEW_Path.h"
#include <GE_RenderUtility.h>
#include <GE_Occluder.h>
#include <GE_Texture.h>
#include <GE_Light.h>
#include <GE_Lens.h>
#include <GE_View.h>
#include "GEW_Scene.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QDialog>
#include <GE_Texture.h>
#include "GEW_Utility.h"

QList< GEW_Flare* > GEW_Flare::m_list;
QIcon GEW_Flare::m_iconFlare;

GEW_Flare * GEW_Flare::find( QString &i_name )
{
	foreach( GEW_Flare *flare, m_list )
	if( flare->text( 0 ).toLower() == i_name.toLower() )
		return flare;
	return NULL;
}

GEW_Flare *GEW_Flare::CreateFromFile( GEW_Item *i_parent, QFile &i_file, const QString &i_resourcePath )
{
	GEW_Flare::SaveData saveData;
	i_file.read( ( char* ) &saveData, sizeof( GEW_Flare::SaveData ) );

	GEW_Flare *gewFlare = new GEW_Flare( saveData.Name, i_parent );
	GE_LensFlare *flare = gewFlare->m_flare;

	gewFlare->m_enable = saveData.Enable;
	gewFlare->m_boundColor = saveData.BoundColor;

	if( saveData.Map[ 0 ] )
		flare->setTexture( GEW_Utility::absolutePath( saveData.Map, i_resourcePath ).c_str() );
	flare->setLightPosition( saveData.LightPosition );
	flare->setVisiblePixelOffset( saveData.VisiblePixelOffset );
	flare->setVisibleAngleOffset( saveData.VisibleAngleOffset );
	flare->setOccluderPositionOffset( saveData.OccluderPositionOffset );
	flare->setOccluderRotation( saveData.OccluderRotation );
	flare->setOccluderSize( saveData.OccluderSize );
	flare->setNumberOfFlare( saveData.FlareCount );

	for( int i = 0; i < ( int ) saveData.FlareCount; ++i )
	{
		GEW_Flare::FlareSaveData flareData;
		i_file.read( ( char * ) &flareData, sizeof( GEW_Flare::FlareSaveData ) );
		gewFlare->m_flareNames.push_back( flareData.Name );
		flare->getParameter()->data()[ i ] = flareData.Parameter;
	}
	return gewFlare;
}

GEW_Flare::SaveData::SaveData()
{
	memset( this, 0, sizeof( GEW_Flare::SaveData ) );
}

GEW_Flare::GEW_Flare( const QString &i_name, GEW_Item *i_parent )
: GEW_Item( i_parent, GEW_Item::Flare )
{
	m_occluderObject = new GE_RenderUtilityObject();
	m_occluderObject->createBox( GE_Vec3( 0.5f, 0.5f, 0.5f ), GE_Vec3( 0.0f, 0.0f, 0.0f ) );

	vector< GE_Vec3 > *vb = m_occluderObject->verticesDataBeginEdit();
	vb->resize( 9 );
	vb->data()[ 8 ] = GE_Vec3( 0.0f, 0.0f, 1.0f );
	m_occluderObject->verticesDataEndEdit();

	vector< int > *ib = m_occluderObject->indicesDataBeginEdit();
	ib->resize( 22 );
	ib->data()[ 16 ] = 8;
	ib->data()[ 17 ] = 7;

	ib->data()[ 18 ] = 3;
	ib->data()[ 19 ] = 8;

	ib->data()[ 20 ] = 8;
	ib->data()[ 21 ] = 6;
	m_occluderObject->indicesDataEndEdit();

	m_occluderObject->setDepthDisable( false );
	m_occluderObject->setVisible( false );
	
	m_boundColor = GE_Vec3( ( float )( rand() % 128 + 128 ) / 255.0f, ( float )( rand() % 128 + 128 ) / 255.0f, ( float )( rand() % 128 + 128 ) / 255.0f );

	m_flare = new GE_LensFlare();

	setText( 0, i_name );
	setIcon( 0, m_iconFlare );

	setCheckState( GEW_Item::Enable, Qt::Checked );
	setToolTip( GEW_Item::Enable, "Enable" );
	setExpanded( true );
	setFlags(
		Qt::ItemFlag::ItemIsEnabled |
		Qt::ItemFlag::ItemIsUserCheckable |
		Qt::ItemFlag::ItemIsSelectable
		);

	m_list.push_back( this );
}

GEW_Flare::~GEW_Flare()
{
	m_list.removeOne( this );

	//delete all childes
	delete m_flare;
	delete m_occluderObject;
}

void GEW_Flare::update()
{
	if ( m_enable )
		m_occluderObject->setColor( m_boundColor );
	else
		m_occluderObject->setColor( GE_Vec3( 0.2f, 0.2f, 0.2f ) );
	m_occluderObject->setRotation( *m_flare->getOccluderRotation() );
	m_occluderObject->setScale( *m_flare->getOccluderSize() );
	m_occluderObject->setPosition( *m_flare->getLightPosition() + *m_flare->getOccluderPositionOffset() );
	m_occluderObject->update();
	m_flare->update( ( GE_View * )GEW_Scene::getSingleton().m_camera );
}

void GEW_Flare::select()
{
	m_selected = true;
	m_occluderObject->setVisible( true );
}

void GEW_Flare::deSelect()
{
	m_selected = false;
	m_occluderObject->setVisible( false );
}

void GEW_Flare::checkStateChanged( GEW_Item::CheckIndex i_column )
{
	bool checked = checkState( i_column ) == Qt::Checked;
	if( i_column == GEW_Item::Enable )
	{
		m_enable = checked;
	}
}

void GEW_Flare::parentCheckStateChanged( GEW_Item::CheckIndex i_column, Qt::CheckState i_checkState )
{
	if( i_column == GEW_Item::Enable )
	{
		setCheckState( i_column, i_checkState );
		checkStateChanged( i_column );
	}
}

void GEW_Flare::saveToFile( QFile &i_file, const QString &i_resourcePath )
{
	GEW_Item::SaveHeader header;
	header.MyType = ( GEW_Item::Type )type();
	header.ParentId = ( ( GEW_Item* ) parent() )->getId();
	header.MyId = getId();
	i_file.write( ( const char * ) &header, sizeof( GEW_Item::SaveHeader ) );

	SaveData saveData;
	strcpy_s( saveData.Name, text( 0 ).toStdString().c_str() );
	saveData.Enable = m_enable;
	saveData.BoundColor = m_boundColor;
	saveData.LightPosition = *m_flare->getLightPosition();
	saveData.VisiblePixelOffset = m_flare->getVisiblePixelOffset();
	saveData.VisibleAngleOffset = m_flare->getVisibleAngleOffset();
	saveData.OccluderPositionOffset = *m_flare->getOccluderPositionOffset();
	saveData.OccluderRotation = *m_flare->getOccluderRotation();
	saveData.OccluderSize = *m_flare->getOccluderSize();
	if( m_flare->getTexture() )
		strcpy_s( saveData.Map, GEW_Utility::copyFile( m_flare->getTexture()->getTextureName(), i_resourcePath ).c_str() );
	saveData.FlareCount = ( uint32_t ) m_flare->getParameter()->size();
	i_file.write( ( const char* ) &saveData, sizeof( GEW_Flare::SaveData ) );

	for( int i = 0; i < ( int ) saveData.FlareCount; ++i )
	{
		FlareSaveData flareData;
		strcpy_s( flareData.Name, m_flareNames[ i ].toStdString().c_str() );
		flareData.Parameter = m_flare->getParameter()->data()[ i ];
		i_file.write( ( const char* ) &flareData, sizeof( GEW_Flare::FlareSaveData ) );
	}
}

void GEW_Flare::setPosition( const GE_Vec3 &i_position )
{
	m_flare->setLightPosition( i_position );
}

void GEW_Flare::setPosition2( const GE_Vec3 &i_position )
{
	m_flare->setLightPosition( i_position );
}

void GEW_Flare::setRotation( const GE_Vec3 &i_rotation )
{
	GE_Quat rotation = *m_flare->getOccluderRotation();
	rotation.rotationYPR( &i_rotation );
	m_flare->setOccluderRotation( rotation );
}

void GEW_Flare::setRotation( GE_Mat4x4 &i_rot, GE_Vec3 & i_pivot )
{
	GE_Mat4x4 pvotMat;
	pvotMat.transform( i_pivot );
	GE_Mat4x4 mat;
	mat.srp( *m_flare->getLightPosition() - i_pivot, *m_flare->getOccluderRotation(), *m_flare->getOccluderSize() );
	mat *= i_rot;
	mat *= pvotMat;

	GE_Vec3 pos;
	GE_Quat rot;
	GE_Vec3 scale;
	mat.decompose( &pos, &rot, &scale );
	m_flare->setLightPosition( pos );
	m_flare->setOccluderRotation( rot );
}

void GEW_Flare::setScale( const GE_Vec3 &i_scale )
{
	m_flare->setOccluderSize( i_scale );
}

void GEW_Flare::setTransform( GE_Mat4x4 *i_matrix )
{
	GE_Mat4x4 mat;
	mat.srp( *m_flare->getLightPosition(), *m_flare->getOccluderRotation(), *m_flare->getOccluderSize() );
	mat *= ( *i_matrix );
	GE_Vec3 pos;
	GE_Quat rot;
	GE_Vec3 Scale;
	mat.decompose( &pos, &rot, &Scale );
	m_flare->setLightPosition( pos );
	m_flare->setOccluderRotation( rot );
	m_flare->setOccluderSize( Scale );
}

GE_Vec3 GEW_Flare::getPosition()
{
	return *m_flare->getLightPosition();
}

GE_Vec3 GEW_Flare::getRotation()
{
	GE_Vec3 rot;
	m_flare->getOccluderRotation()->toYPR( &rot );
	return rot;
}

GE_Vec3 GEW_Flare::getScale()
{
	return *m_flare->getOccluderSize();
}

void GEW_Flare::getRenderUtilityObjects( std::list<GE_RenderUtilityObject*> &i_outList )
{
	if ( m_occluderObject->isVisible() )
		i_outList.push_back( m_occluderObject );
}
