#include "GEW_Decal.h"
#include <GE_RenderUtility.h>
#include <GE_Occluder.h>
#include <GE_Texture.h>
#include <GE_Light.h>
#include <GE_Decal.h>
#include <GE_View.h>
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QDialog>
#include "GEW_Scene.h"
#include "GEW_Utility.h"
#include "GE_Decal.h"

QList< GEW_Decal* > GEW_Decal::m_list;
QIcon GEW_Decal::m_iconDecal;
uint32_t GEW_Decal::m_newID = 0;
int32_t GEW_Decal::m_selectedItem = -1;

GEW_Decal *GEW_Decal::find( QString &i_name )
{
	foreach( GEW_Decal *decal, m_list )
		if( decal->text( 0 ).toLower() == i_name.toLower() )
			return decal;
	return NULL;
}

GEW_Decal *GEW_Decal::createFromFile( GEW_Item *i_parent, QFile &i_file, const QString &i_resourcePath )
{
	GEW_Decal::SaveData saveData;
	i_file.read( ( char* ) &saveData, sizeof( GEW_Decal::SaveData ) );

	GEW_Decal *gewDecal = new GEW_Decal( saveData.Name, i_parent );
	GE_Decal *decal = gewDecal->m_decal;

	gewDecal->m_enable = saveData.Enable;
	if( saveData.DiffuseMap[ 0 ] )
		decal->setDiffuseTexture( GEW_Utility::absolutePath( saveData.DiffuseMap, i_resourcePath ).c_str() );
	if( saveData.NormalMap[ 0 ] )
		decal->setNormalTexture( GEW_Utility::absolutePath( saveData.NormalMap, i_resourcePath ).c_str() );
	if( saveData.GlowSpecularMap[ 0 ] )
		decal->setGlowSpecularTexture( GEW_Utility::absolutePath( saveData.GlowSpecularMap, i_resourcePath ).c_str() );
	if( saveData.ReflectMap[ 0 ] )
		decal->setReflectTexture( GEW_Utility::absolutePath( saveData.ReflectMap, i_resourcePath ).c_str() );

	decal->setNumberOfDecal( saveData.DecalCount );
	for( int i = 0; i < ( int ) saveData.DecalCount; ++i )
	{
		GEW_Decal::Property *property = new GEW_Decal::Property();
		GE_Decal::Parameter *parameter = &decal->beginEditParameters()->data()[ i ];
		
		GEW_Decal::DecalSaveData decalData;
		i_file.read( ( char * ) &decalData, sizeof( GEW_Decal::DecalSaveData ) );

		property->Bound->setColor( decalData.BoundColor );
		property->BoundColor = decalData.BoundColor;
		property->Name = QString( decalData.Name );
		property->VisibleDistance = decalData.VisibleDistance;
		property->Enable = decalData.Enable;
		property->Position = decalData.Position;
		property->Rotation = decalData.Rotation;
		property->Scale = decalData.Scale;
		property->Bound->setPosition( decalData.Position );
		property->Bound->setRotation( decalData.Rotation );
		property->Bound->setScale( decalData.Scale );
		gewDecal->m_properties.push_back( property );

		parameter->Visible = decalData.VisibleDistance;
		parameter->MaxAngle = decalData.MaxAngle;
		parameter->Bumpiness = decalData.Bumpiness;
		parameter->Glowness = decalData.Glowness;
		parameter->Roughness = decalData.Roughness;
		parameter->Reflectmask = decalData.Reflectmask;
		parameter->Layer = decalData.Layer;
		parameter->Rotation = decalData.RotationF;
		parameter->UVTile = decalData.UVTile;
		parameter->UVMove = decalData.UVMove;
		parameter->Color = decalData.DiffuseColor;

		GEW_Decal::m_selectedItem = i;
		gewDecal->updateTransform();
	}
	return gewDecal;
}

GEW_Decal::Property::Property()
{
	Bound = new GE_RenderUtilityObject();
	float meter = GE_ConvertToUnit(0.5f);
	Bound->createBox(GE_Vec3(meter, meter, meter), GE_Vec3(0, 0, -meter));
	Position = GE_Vec3( 0.0f, 0.0f, 0.0f );
	Scale = GE_Vec3( 1.0f, 1.0f, 1.0f );
	Rotation.identity();
	vector< int > *ib = Bound->indicesDataBeginEdit();
	ib->resize( 20 );
	ib->data()[ 16 ] = 0;
	ib->data()[ 17 ] = 5;
	ib->data()[ 18 ] = 1;
	ib->data()[ 19 ] = 4;
	Bound->indicesDataEndEdit();

	Bound->setVisible( true );
	Bound->setBillboard( false );
	Bound->setDepthDisable( false );
	Enable = true;
	VisibleDistance = GE_ConvertToUnit( 100.0f );
	BoundColor = GE_Vec3( ( float ) ( rand() % 128 + 128 ) / 255.0f, ( float ) ( rand() % 128 + 128 ) / 255.0f, ( float ) ( rand() % 128 + 128 ) / 255.0f );
	Bound->setColor( BoundColor );
	Bound->setVisible( false );
}

GEW_Decal::Property::~Property()
{
	delete Bound;
}

GEW_Decal::GEW_Decal( const QString &i_name, GEW_Item *i_parent )
: GEW_Item( i_parent, GEW_Item::Decal )
{
	m_decal = new GE_Decal();

	setText( 0, i_name );
	setIcon( 0, m_iconDecal );

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

GEW_Decal::~GEW_Decal()
{
	for( int32_t i = 0; i < m_properties.size(); ++i )
		delete m_properties[ i ];
	m_list.removeOne( this );
	delete m_decal;
}

void GEW_Decal::update()
{
	for( int32_t i = 0; i < m_properties.size(); ++i )
	{
		if( m_properties[ i ]->Bound->isVisible() )
			m_properties[ i ]->Bound->update();
	}
	m_decal->update();
}

void GEW_Decal::select()
{
	m_selected = true;
	if( m_properties.size() )
	{
		GEW_Item *myParent = ( GEW_Item* ) parent();
		if( myParent->isSecected() )
		{
			foreach( GEW_Decal::Property *property, m_properties )
				property->Bound->setVisible( true );
		} else if( m_selectedItem != -1 )
		{
			foreach( GEW_Decal::Property *property, m_properties )
				property->Bound->setVisible( false );
			m_properties[ m_selectedItem ]->Bound->setVisible( true );
		}
	}
}

void GEW_Decal::deSelect()
{
	m_selected = false;
	foreach( GEW_Decal::Property *property, m_properties )
		property->Bound->setVisible( false );
}

void GEW_Decal::checkStateChanged( GEW_Item::CheckIndex i_column )
{
	bool checked = checkState( i_column ) == Qt::Checked;
	if( i_column == GEW_Item::Enable )
	{
		m_enable = checked;
	}
}

void GEW_Decal::parentCheckStateChanged( GEW_Item::CheckIndex i_column, Qt::CheckState i_checkState )
{
	if( i_column == GEW_Item::Enable )
	{
		setCheckState( i_column, i_checkState );
		checkStateChanged( i_column );
	}
}

void GEW_Decal::saveToFile( QFile &i_file, const QString &i_resourcePath )
{
	GEW_Item::SaveHeader header;
	header.MyType = ( GEW_Item::Type )type();
	header.ParentId = ( ( GEW_Item* ) parent() )->getId();
	header.MyId = getId();
	i_file.write( ( const char * ) &header, sizeof( GEW_Item::SaveHeader ) );

	GEW_Decal::SaveData saveData;
	saveData.Enable = m_enable;
	strcpy_s( saveData.Name, text( 0 ).toStdString().c_str() );
	if( m_decal->getDiffuseTexture() )
		strcpy_s( saveData.DiffuseMap,
		GEW_Utility::copyFile( m_decal->getDiffuseTexture()->getTextureName(), i_resourcePath ).c_str() );
	if( m_decal->getNormalTexture() )
		strcpy_s( saveData.NormalMap,
		GEW_Utility::copyFile( m_decal->getNormalTexture()->getTextureName(), i_resourcePath ).c_str() );
	if( m_decal->getGlowSpecularTexture() )
		strcpy_s( saveData.GlowSpecularMap,
		GEW_Utility::copyFile( m_decal->getGlowSpecularTexture()->getTextureName(), i_resourcePath ).c_str() );
	if( m_decal->getReflectTexture() )
		strcpy_s( saveData.ReflectMap,
		GEW_Utility::copyFile( m_decal->getReflectTexture()->getTextureName(), i_resourcePath ).c_str() );
	saveData.DecalCount = m_properties.size();
	i_file.write( ( const char* ) &saveData, sizeof( GEW_Decal::SaveData ) );

	for( int i = 0; i < ( int ) saveData.DecalCount; ++i )
	{
		GEW_Decal::Property *property = m_properties[ i ];
		GE_Decal::Parameter *parameter = &m_decal->beginEditParameters()->data()[ i ];
		GEW_Decal::DecalSaveData decalData;
		decalData.Enable = property->Enable;
		strcpy_s( decalData.Name, property->Name.toStdString().c_str() );
		decalData.BoundColor = property->BoundColor;
		decalData.VisibleDistance = property->VisibleDistance;
		decalData.Position = property->Position;
		decalData.Rotation = property->Rotation;
		decalData.Scale = property->Scale;
		decalData.MaxAngle = parameter->MaxAngle;
		decalData.Bumpiness = parameter->Bumpiness;
		decalData.Glowness = parameter->Glowness;
		decalData.Roughness = parameter->Roughness;
		decalData.Reflectmask = parameter->Reflectmask;
		decalData.Layer = parameter->Layer;
		decalData.RotationF = parameter->Rotation;
		decalData.UVTile = parameter->UVTile;
		decalData.UVMove = parameter->UVMove;
		decalData.DiffuseColor = parameter->Color;
		i_file.write( ( const char* ) &decalData, sizeof( GEW_Decal::DecalSaveData ) );
	}
}

void GEW_Decal::setPosition( const GE_Vec3 &i_position )
{
	if ( m_selectedItem < 0 || !m_properties.size( ) )
		return;
	m_properties[ m_selectedItem ]->Position = i_position;
	m_properties[ m_selectedItem ]->Bound->setPosition( m_properties[ m_selectedItem ]->Position );
	updateTransform();
}

void GEW_Decal::setPosition2( const GE_Vec3 &i_position )
{
	GE_Vec3 offset = ( GE_Vec3 ) i_position - m_groupPos;
	foreach( GEW_Decal::Property *property, m_properties )
	{
		property->Position += offset;
		property->Bound->setPosition( property->Position );
	}
	updateTransform( true );
}

void GEW_Decal::setRotation( const GE_Vec3 &i_rotation )
{
	if ( m_selectedItem < 0 || !m_properties.size( ) )
		return;
	GE_Quat rotation;
	rotation.rotationYPR( &i_rotation );
	m_properties[ m_selectedItem ]->Rotation = rotation;
	m_properties[ m_selectedItem ]->Bound->setRotation( rotation );
	updateTransform();
}

void GEW_Decal::setRotation( GE_Mat4x4 &i_rot, GE_Vec3 & i_pivot )
{
	/*GE_Mat4x4 pvotMat;
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
	m_flare->setOccluderRotation( rot );*/
	updateTransform();
}

void GEW_Decal::setScale( const GE_Vec3 &i_scale )
{
	if ( m_selectedItem < 0 || !m_properties.size( ) )
		return;
	m_properties[ m_selectedItem ]->Scale = i_scale;
	m_properties[ m_selectedItem ]->Bound->setScale( m_properties[ m_selectedItem ]->Scale );
	updateTransform();
}

void GEW_Decal::setTransform( GE_Mat4x4 *i_matrix )
{
	/*GE_Mat4x4 mat;
	mat.srp( *m_flare->getLightPosition(), *m_flare->getOccluderRotation(), *m_flare->getOccluderSize() );
	mat *= ( *i_matrix );
	GE_Vec3 pos;
	GE_Quat rot;
	GE_Vec3 Scale;
	mat.decompose( &pos, &rot, &Scale );
	m_flare->setLightPosition( pos );
	m_flare->setOccluderRotation( rot );
	m_flare->setOccluderSize( Scale );*/
	updateTransform();
}

GE_Vec3 GEW_Decal::getPosition()
{
	if( m_properties.size() )
	{
		GEW_Item *myParent = ( GEW_Item* ) parent();
		if( myParent->isSecected() )
		{
			m_groupPos = GE_Vec3( 0, 0, 0 );
			foreach( GEW_Decal::Property *property, m_properties )
				m_groupPos += property->Position;
			m_groupPos /= ( float ) m_properties.size();
			return m_groupPos;
		} else if( m_selectedItem != -1 )
			return m_properties[ m_selectedItem ]->Position;
	}
	return GE_Vec3( 0, 0, 0 );
}

GE_Vec3 GEW_Decal::getRotation()
{
	if ( m_selectedItem < 0 || !m_properties.size( ) )
		return GE_Vec3( 0, 0, 0 );
	GE_Vec3 rot;
	m_properties[ m_selectedItem ]->Rotation.toYPR( &rot );
	return rot;
}

GE_Vec3 GEW_Decal::getScale()
{
	if ( m_selectedItem < 0 || !m_properties.size( ) )
		return GE_Vec3( 1.0f, 1.0f, 1.0f );
	return m_properties[ m_selectedItem ]->Scale;
}

void GEW_Decal::getRenderUtilityObjects( std::list<GE_RenderUtilityObject*> &i_outList )
{
	for( int32_t i = 0; i < m_properties.size(); ++i )
	{
		if( m_properties[ i ]->Bound->isVisible() )
			i_outList.push_back( m_properties[ i ]->Bound );
	}
}

void GEW_Decal::updateTransform(bool i_updateAll /*= false */)
{
	if( !m_properties.size() )
		return;

	if( i_updateAll )
	{
		int index = 0;
		foreach( GEW_Decal::Property *property, m_properties )
		{
			GE_Mat4x4 matTrans;
			matTrans.RotationQuaternion( &property->Rotation );
			GE_Vec3	Dir = GE_Vec3( 0.0f, 0.0f, -1.0f );
			GE_Vec3	Up = GE_Vec3( 0.0f, 1.0f, 0.0f );
			Dir.transform( &matTrans );
			Up.transform( &matTrans );
			Dir.normalize();
			Dir += property->Position;
			Up.normalize();

			vector<GE_Decal::Parameter > *p = m_decal->beginEditParameters();
			p->data()[ index ].Transform.srp( property->Position, property->Rotation, property->Scale );
			GE_Mat4x4 matV;
			matV.lookAtRH( &property->Position, &Dir, &Up );
			GE_Vec3 size = property->Scale;
			if( size.x <= 0.2f )
				size.x = 0.2f;
			if( size.y <= 0.2f )
				size.y = 0.2f;
			if( size.z <= 0.2f )
				size.z = 0.2f;

			p->data()[ index ].Projection.orthoRH( 
				GE_ConvertToUnit( size.x ),
				GE_ConvertToUnit( size.y ),
				0.1f, 
				GE_ConvertToUnit( size.z ) );
			p->data()[ index ].Projection = matV * p->data()[ index ].Projection;
			m_decal->endEditParameters();
			++index;
		}
	} else if( m_selectedItem >= 0 )
	{
		GE_Mat4x4 matTrans;
		matTrans.RotationQuaternion( &m_properties[ m_selectedItem ]->Rotation );
		GE_Vec3	Dir = GE_Vec3( 0.0f, 0.0f, -1.0f );
		GE_Vec3	Up = GE_Vec3( 0.0f, 1.0f, 0.0f );
		Dir.transform( &matTrans );
		Up.transform( &matTrans );
		Dir.normalize();
		Dir += m_properties[ m_selectedItem ]->Position;
		Up.normalize();

		vector<GE_Decal::Parameter > *p = m_decal->beginEditParameters();
		p->data()[ m_selectedItem ].Transform.srp( m_properties[ m_selectedItem ]->Position, m_properties[ m_selectedItem ]->Rotation, m_properties[ m_selectedItem ]->Scale );
		GE_Mat4x4 matV;
		matV.lookAtRH( &m_properties[ m_selectedItem ]->Position, &Dir, &Up );
		GE_Vec3 size = m_properties[ m_selectedItem ]->Scale;
		if( size.x <= 0.2f )
			size.x = 0.2f;
		if( size.y <= 0.2f )
			size.y = 0.2f;
		if( size.z <= 0.2f )
			size.z = 0.2f;

		p->data()[ m_selectedItem ].Projection.orthoRH( 
			GE_ConvertToUnit( size.x ),
			GE_ConvertToUnit( size.y ),
			0.1f, 
			GE_ConvertToUnit( size.z ) );
		p->data()[ m_selectedItem ].Projection = matV * p->data()[ m_selectedItem ].Projection;
		m_decal->endEditParameters();
	}
}

GEW_Decal::SaveData::SaveData()
{
	memset( this, 0, sizeof( SaveData ) );
}

GEW_Decal::DecalSaveData::DecalSaveData()
{
	memset( this, 0, sizeof( DecalSaveData ) );
}
