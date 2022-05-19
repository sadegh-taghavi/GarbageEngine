#include "GEW_Light.h"
#include <GE_RenderUtility.h>
#include <GE_Occluder.h>
#include <GE_Texture.h>
#include <GE_Light.h>
#include <GE_Lens.h>
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QDialog>
#include "GEW_Path.h"
#include "GEW_Scene.h"
#include "GEW_Camera.h"
#include "GEW_Postprocess.h"
#include "GEW_PostprocessProperty.h"

// GEW_Light::Probe ---------------------------------------------------------------------------------

GEW_Light::Probe::Probe()
{
	Cage = new GE_RenderUtilityObject();
	Target = new GE_RenderUtilityObject();
	Billboard = new GE_RenderUtilityObject();
	Spot = new GE_RenderUtilityObject();
	Line = new GE_RenderUtilityObject();
	Cross = new GE_RenderUtilityObject();

	std::vector< GE_Vec3 > *vt = Cage->verticesDataBeginEdit();
	vt->resize( 108 );
	float step = GE_ConvertToRadians( 360.0f / 36.0f );
	for ( uint32_t i = 0; i < 36; ++i )
	{
		vt->data()[ i ] = GE_Vec3( cosf( ( float )i * step ), sinf( ( float )i * step ), 0.0f );
		vt->data()[ i + 36 ] = GE_Vec3( 0.0f, cosf( ( float )i * step ), sinf( ( float )i * step ) );
		vt->data()[ i + 72 ] = GE_Vec3( cosf( ( float )i * step ), 0.0f, sinf( ( float )i * step ) );
	}
	Cage->verticesDataEndEdit();

	std::vector<int> *ib = Cage->indicesDataBeginEdit();
	ib->resize( 216 );
	int j = 0;
	for ( int i = 0; i < 36; ++i )
	{
		ib->data()[ j ] = i;
		ib->data()[ j + 1 ] = i + 1;

		ib->data()[ j + 72 ] = i + 36;
		ib->data()[ j + 72 + 1 ] = i + 36 + 1;

		ib->data()[ j + 144 ] = i + 72;
		ib->data()[ j + 144 + 1 ] = i + 72 + 1;

		j += 2;
	}
	ib->data()[ 70 ] = 35;
	ib->data()[ 71 ] = 0;
	ib->data()[ 142 ] = 71;
	ib->data()[ 143 ] = 36;
	ib->data()[ 214 ] = 107;
	ib->data()[ 215 ] = 72;
	Cage->indicesDataEndEdit();
	Cage->setRenderTopology( D3D11_PRIMITIVE_TOPOLOGY_LINELIST );
	Cage->setDepthDisable( false );

	vt = Billboard->verticesDataBeginEdit();
	vt->resize( 4 );
	vt->data()[ 0 ] = GE_Vec3( -1.0f, 1.0f, 0.0f );
	vt->data()[ 1 ] = GE_Vec3( -1.0f, -1.0f, 0.0f );
	vt->data()[ 2 ] = GE_Vec3( 1.0f, 1.0f, 0.0f );
	vt->data()[ 3 ] = GE_Vec3( 1.0f, -1.0f, 0.0f );
	Billboard->verticesDataEndEdit();

	vector< GE_Vec2 > *vtc = Billboard->texcoordDataBeginEdit();
	vtc->resize( 4 );
	vtc->data()[ 0 ] = GE_Vec2( 0.0f, 0.0f );
	vtc->data()[ 1 ] = GE_Vec2( 0.0f, 1.0f );
	vtc->data()[ 2 ] = GE_Vec2( 1.0f, 0.0f );
	vtc->data()[ 3 ] = GE_Vec2( 1.0f, 1.0f );
	Billboard->texcoordDataEndEdit();
	Billboard->setRenderTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
	Billboard->setAlphaTest( 0.5f );
	Billboard->setBillboard( true );

	TextureDir = GE_TextureManager::getSingleton().createTexture( ( string( GEW_TEXTURE_PATH ) + "Dir.dds" ).c_str() );
	TextureSpot = GE_TextureManager::getSingleton().createTexture( ( string( GEW_TEXTURE_PATH ) + "Spot.dds" ).c_str() );
	TexturePoint = GE_TextureManager::getSingleton().createTexture( ( string( GEW_TEXTURE_PATH ) + "Point.dds" ).c_str() );

	Billboard->setSRV( TextureSpot->getTextureSRV() );

	Line->verticesDataBeginEdit()->resize( 2 );
	Line->verticesDataEndEdit();
	Line->setRenderTopology( D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP );
	Line->setDepthDisable( false );

	vt = Target->verticesDataBeginEdit();
	vt->resize( 8 );
	vt->data()[ 0 ] = GE_Vec3( -0.5f, -0.5f, -0.5f );
	vt->data()[ 1 ] = GE_Vec3( 0.5f, -0.5f, -0.5f );
	vt->data()[ 2 ] = GE_Vec3( -0.5f, -0.5f, 0.5f );
	vt->data()[ 3 ] = GE_Vec3( 0.5f, -0.5f, 0.5f );
	vt->data()[ 4 ] = GE_Vec3( -0.5f, 0.5f, -0.5f );
	vt->data()[ 5 ] = GE_Vec3( 0.5f, 0.5f, -0.5f );
	vt->data()[ 6 ] = GE_Vec3( -0.5f, 0.5f, 0.5f );
	vt->data()[ 7 ] = GE_Vec3( 0.5f, 0.5f, 0.5f );
	Target->verticesDataEndEdit();

	ib = Target->indicesDataBeginEdit();
	ib->resize( 16 );

	ib->data()[ 0 ] = 0;
	ib->data()[ 1 ] = 1;
	ib->data()[ 2 ] = 3;
	ib->data()[ 3 ] = 2;
	ib->data()[ 4 ] = 0;

	ib->data()[ 5 ] = 4;
	ib->data()[ 6 ] = 5;
	ib->data()[ 7 ] = 7;
	ib->data()[ 8 ] = 6;
	ib->data()[ 9 ] = 4;

	ib->data()[ 10 ] = 5;
	ib->data()[ 11 ] = 1;
	ib->data()[ 12 ] = 3;
	ib->data()[ 13 ] = 7;
	ib->data()[ 14 ] = 6;
	ib->data()[ 15 ] = 2;

	Target->indicesDataEndEdit();
	Target->setRenderTopology( D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP );

	Spot->setRenderTopology( D3D11_PRIMITIVE_TOPOLOGY_LINELIST );
	Spot->verticesDataBeginEdit()->resize( 37 );
	Spot->verticesDataEndEdit();
	ib = Spot->indicesDataBeginEdit();
	ib->resize( 84 );
	j = 0;
	for ( int i = 0; i < 36; ++i )
	{
		ib->data()[ j ] = i;
		ib->data()[ j + 1 ] = i + 1;
		j += 2;
	}
	ib->data()[ 70 ] = 35;
	ib->data()[ 71 ] = 0;

	ib->data()[ 72 ] = 36;
	ib->data()[ 73 ] = 0;

	ib->data()[ 74 ] = 36;
	ib->data()[ 75 ] = 6;

	ib->data()[ 76 ] = 36;
	ib->data()[ 77 ] = 12;

	ib->data()[ 78 ] = 36;
	ib->data()[ 79 ] = 18;

	ib->data()[ 80 ] = 36;
	ib->data()[ 81 ] = 24;

	ib->data()[ 82 ] = 36;
	ib->data()[ 83 ] = 30;

	Spot->indicesDataEndEdit();
	Spot->setDepthDisable( false );

	vt = Cross->verticesDataBeginEdit();
	vt->resize( 6 );
	vt->data()[ 0 ] = GE_Vec3( -0.5f, 0.0f, 0.0f );
	vt->data()[ 1 ] = GE_Vec3( 0.5f, 0.0f, 0.0f );
	vt->data()[ 2 ] = GE_Vec3( 0.0f, -0.5f, 0.0f );
	vt->data()[ 3 ] = GE_Vec3( 0.0f, 0.5f, 0.0f );
	vt->data()[ 4 ] = GE_Vec3( 0.0f, 0.0f, 0.5f );
	vt->data()[ 5 ] = GE_Vec3( 0.0f, 0.0f, -0.5f );
	Cross->verticesDataEndEdit();
	Cross->setDepthDisable( true );
	Cross->setRenderTopology( D3D11_PRIMITIVE_TOPOLOGY_LINELIST );

	Visible = true;
	LastRange = 0.0f;
	LastSpotSpread = 0.0f;
	LastDir = GE_Vec3( 0.0f, 0.0f, 0.0f );

	BoundColor = GE_Vec3( ( float )( rand( ) % 128 + 128 ) / 255.0f, ( float )( rand( ) % 128 + 128 ) / 255.0f, ( float )( rand( ) % 128 + 128 ) / 255.0f );
}

GEW_Light::Probe::~Probe()
{
	delete Cage ;
	delete Target ;
	delete Billboard ;
	delete Spot ;
	delete Line ;
	delete Cross ;

	GE_TextureManager::getSingleton().remove( &TextureDir );
	GE_TextureManager::getSingleton().remove( &TextureSpot );
	GE_TextureManager::getSingleton().remove( &TexturePoint );
}

void GEW_Light::Probe::update( GE_Light *i_light )
{
	if ( !i_light || !Visible )
		return;

	std::vector< GE_Vec3 > *vt;
	std::vector< int > ib;

	Cage->setVisible( false );
	Target->setVisible( false );
	Line->setVisible( false );
	Spot->setVisible( false );
	Cross->setVisible( false );
	Billboard->setVisible( true );
	
	if ( i_light->iSEnable() )
	{
		Cage->setColor( BoundColor );
		Target->setColor( BoundColor );
		Line->setColor( BoundColor );
		Spot->setColor( BoundColor );
		Cross->setColor( BoundColor );
		Billboard->setColor( GE_Vec3( 1.0f, 1.0f, 1.0f ) );
	}
	else
	{
		Cage->setColor( GE_Vec3( 0.0f, 0.0f, 0.0f ) );
		Target->setColor( GE_Vec3( 0.0f, 0.0f, 0.0f ) );
		Line->setColor( GE_Vec3( 0.0f, 0.0f, 0.0f ) );
		Spot->setColor( GE_Vec3( 0.0f, 0.0f, 0.0f ) );
		Cross->setColor( GE_Vec3( 0.0f, 0.0f, 0.0f ) );
		Billboard->setColor( GE_Vec3( 0.2f, 0.2f, 0.2f ) );
	}

	GE_Light::Parameters *lp = i_light->getParameters();
	if ( lp->LightType == GE_Light::Point )
	{
		Cage->setVisible( true );
		Cage->setScale( GE_Vec3( lp->Range, lp->Range, lp->Range ) );
		Cage->setPosition( lp->Position );
		Billboard->setPosition( lp->Position );
		Billboard->setSRV( TexturePoint->getTextureSRV() );
	}
	else if ( lp->LightType == GE_Light::Spot )
	{
		GE_Vec3 pos = lp->Direction * lp->Range;

		if ( LastDir != lp->Direction || LastSpotSpread != lp->SpotSpread || LastRange != lp->Range )
		{
			LastDir = lp->Direction;
			LastSpotSpread = lp->SpotSpread;
			LastRange = lp->Range;

			float step = GE_ConvertToRadians( 360.0f / 36.0f );
			vt = Spot->verticesDataBeginEdit();
			vt->data()[ 36 ] = GE_Vec3( 0.0f, 0.0f, 0.0f );
			for ( uint32_t i = 0; i < 36; ++i )
			{
				vt->data()[ i ].x = cosf( ( float )i * step ) * lp->Range * tanf( lp->SpotSpread  * 0.5f );
				vt->data()[ i ].y = sinf( ( float )i * step ) * lp->Range * tanf( lp->SpotSpread * 0.5f );
				vt->data()[ i ].z = lp->Range;
			}
			Spot->verticesDataEndEdit();

			GE_Vec3 dir;
			dir.normalizeBy( &( pos - GE_Vec3( 0.0f, 0.0f, 1.0f ) ) );
			float angle = ( float )acosf( dir.dot( &GE_Vec3( 0.0f, 0.0f, 1.0f ) ) );
			GE_Vec3 ax( 0.0f, 0.0f, 1.0f );
			ax.cross( &dir );
			GE_Quat qt;
			if ( !ax.x && !ax.y && !ax.z )
				ax.x = 1.0f;
			qt.rotationAxis( &ax, angle );
			qt.normalize();
			Spot->setRotation( qt );

		}

		Spot->setPosition( lp->Position );

		Spot->setVisible( true );
		Billboard->setSRV( TextureSpot->getTextureSRV() );
		Billboard->setPosition( lp->Position );
		Line->setVisible( true );
		Cross->setVisible( true );
		Cross->setPosition( pos + lp->Position );

		vt = Line->verticesDataBeginEdit();
		vt->data()[ 0 ] = lp->Position;
		vt->data()[ 1 ] = pos + lp->Position;
		Line->verticesDataEndEdit();

		if ( lp->TargetMode )
		{
			Target->setPosition( lp->Target );
			Target->setVisible( true );

		}
	}
	else if ( lp->LightType == GE_Light::Directional )
	{
		Billboard->setSRV( TextureDir->getTextureSRV() );
		Line->setVisible( true );

		if ( lp->TargetMode )
		{
			vt = Line->verticesDataBeginEdit();
			vt->data()[ 0 ] = lp->Direction * GE_ConvertToUnit( 500.0f ) + lp->Position;
			vt->data()[ 1 ] = lp->Direction * -GE_ConvertToUnit( 500.0f ) + lp->Position;
			Line->verticesDataEndEdit();

			Target->setPosition( lp->Target );
			Target->setVisible( true );
			Billboard->setPosition( lp->Position );
		}
		else
		{
			vt = Line->verticesDataBeginEdit();
			vt->data()[ 0 ] = lp->Direction * GE_ConvertToUnit( 500.0f );
			vt->data()[ 1 ] = lp->Direction * -GE_ConvertToUnit( 500.0f );
			Line->verticesDataEndEdit();

			Billboard->setPosition( vt->data()[ 1 ] );
		}
	}

	GE_Vec3 camPos = GEW_Scene::getSingleton().m_camera->m_position;

	float dis = ( camPos - Billboard->getPosition() ).length() * 0.03f;
	Billboard->setScale( GE_Vec3( dis, dis, dis ) );
	dis = ( camPos - Target->getPosition() ).length() * 0.03f;
	Target->setScale( GE_Vec3( dis, dis, dis ) );
	dis = ( camPos - Cross->getPosition() ).length() * 0.03f;
	Cross->setScale( GE_Vec3( dis, dis, dis ) );

	Cage->update();
	Billboard->update();
	Target->update();
	Line->update();
	Spot->update();
	Cross->update();
}

//GEW_Light ----------------------------------------------------------------------------

QIcon				GEW_Light::m_iconDirectional;
QIcon				GEW_Light::m_iconPoint;
QIcon				GEW_Light::m_iconSpot;
QList< GEW_Light* > GEW_Light::m_list;

GEW_Light *GEW_Light::find( QString &i_name )
{
	foreach( GEW_Light *light, m_list )
	if ( light->text( 0 ).toLower() == i_name.toLower() )
		return light;
	return NULL;
}

GEW_Light *GEW_Light::find( uint32_t i_id )
{
	foreach( GEW_Light *light, m_list )
	if ( light->getId() == i_id )
		return light;
	return NULL;
}

GEW_Light *GEW_Light::CreateFromFile( GEW_Item *i_parent, QFile &i_file, const QString &i_resourcePath )
{
	GEW_Light::SaveData lightData;
	i_file.read( ( char* ) &lightData, sizeof( GEW_Light::SaveData ) );

	GEW_Light *gewLight = new GEW_Light( lightData.Name, i_parent );
	gewLight->setType( lightData.Parameters.LightType );
	gewLight->m_probe.BoundColor = lightData.ProbeBoundColor;
	gewLight->m_offDistance = lightData.OffDistance;
	gewLight->m_shadowDistance = lightData.ShadowDistance;
	gewLight->m_light->setParameters( lightData.Parameters );
	gewLight->m_light->createShadowMap();

	gewLight->setItemCheckState( GEW_Item::Enable, lightData.Parameters.Enable );
	gewLight->setItemCheckState( GEW_Item::Dynamic, lightData.Parameters.Dynamic );

	return gewLight;
}

GEW_Light::GEW_Light( const QString &i_name, GEW_Item *i_parent )
: GEW_Item( i_parent, GEW_Item::Light )
{
	GE_Light::Parameters pr;
	pr.Dynamic = false;
	m_light = new GE_Light( pr );

	m_offDistance = GE_ConvertToUnit( 500.0f );
	m_shadowDistance = GE_ConvertToUnit( 100.0f );
	setProbeVisible( false );

	setText( 0, i_name );
	setIcon( 0, m_iconDirectional );

	setCheckState( GEW_Item::Enable, Qt::Checked );
	setCheckState( GEW_Item::Dynamic, Qt::Unchecked );
	setToolTip( GEW_Item::Enable, "Enable" );
	setToolTip( GEW_Item::Dynamic, "Dynamic" );
	setExpanded( true );
	setFlags(
		Qt::ItemFlag::ItemIsEnabled |
		Qt::ItemFlag::ItemIsUserCheckable |
		Qt::ItemFlag::ItemIsSelectable
		);

	m_list.push_back( this );
}

GEW_Light::~GEW_Light()
{
	m_list.removeOne( this );
	if ( GEW_Postprocess::getSingleton()->m_skyLight == this )
	{
		GEW_Postprocess::getSingleton( )->m_skyLight = NULL;
		GEW_PostprocessProperty::getSingleton()->updateUi();
	}
	//delete all childes
	delete m_light ;
}

void GEW_Light::setProbeXrayMode( bool i_xRayMode )
{
	m_probe.Billboard->setDepthDisable( i_xRayMode );
	m_probe.Target->setDepthDisable( i_xRayMode );
	m_probe.Cross->setDepthDisable( i_xRayMode );
}

bool GEW_Light::isProbeXRayMode()
{
	return true;
	//return m_probe.Billboard.isDepthDisable();
}

void GEW_Light::setProbeVisible( bool i_visible )
{
	m_probe.Visible = i_visible;
	if ( i_visible )
		return;
	m_probe.Cage->setVisible( false );
	m_probe.Target->setVisible( false );
	m_probe.Billboard->setVisible( false );
	m_probe.Line->setVisible( false );
	m_probe.Spot->setVisible( false );
	m_probe.Cross->setVisible( false );
}

bool GEW_Light::isProbeVisible()
{
	return GEW_Light::m_probe.Visible;
}

void GEW_Light::setType( GE_Light::Type i_type )
{
	m_light->setType( ( GE_Light::Type )i_type );
	if( i_type == GE_Light::Directional )
		setIcon( 0, GEW_Light::m_iconDirectional );
	else if( i_type == GE_Light::Spot )
		setIcon( 0, GEW_Light::m_iconSpot );
	else
		setIcon( 0, GEW_Light::m_iconPoint );
}

int GEW_Light::getType()
{
	return ( int ) m_light->getType();
}

void GEW_Light::update()
{
	m_light->update();
	m_probe.update( m_light );
}

void GEW_Light::select()
{
	m_selected = true;
	setProbeVisible( true );
}

void GEW_Light::deSelect()
{
	m_selected = false;
	setProbeVisible( false );
}

void GEW_Light::checkStateChanged( GEW_Item::CheckIndex i_column )
{
	GE_Light::Parameters *lp = m_light->getParameters();

	bool checked = checkState( i_column ) == Qt::Checked;
	if ( i_column == GEW_Item::Enable )
		lp->Enable = checked;
	else if( i_column == GEW_Item::Dynamic )
		lp->Dynamic = checked;
}

void GEW_Light::parentCheckStateChanged( GEW_Item::CheckIndex i_column, Qt::CheckState i_checkState )
{
	if(
		i_column == GEW_Item::Enable ||
		i_column == GEW_Item::Dynamic
		)
	{
		setCheckState( i_column, i_checkState );
		checkStateChanged( i_column );
	}
}

void GEW_Light::saveToFile( QFile &i_file, const QString &i_resourcePath )
{
	GEW_Item::SaveHeader header;
	header.MyType = ( GEW_Item::Type )type();
	header.ParentId = ( ( GEW_Item* ) parent() )->getId();
	header.MyId = getId();
	i_file.write( ( const char * ) &header, sizeof( GEW_Item::SaveHeader ) );

	SaveData lightData;
	strcpy_s( lightData.Name, text( 0 ).toStdString().c_str() );
	lightData.OffDistance = m_offDistance;
	lightData.ShadowDistance = m_shadowDistance;
	lightData.ProbeBoundColor = m_probe.BoundColor;
	lightData.Parameters = *m_light->getParameters();
	i_file.write( ( const char * ) &lightData, sizeof( SaveData ) );
}

void GEW_Light::setPosition( const GE_Vec3 &i_position )
{
	m_light->setPosition( i_position );
}

void GEW_Light::setPosition2( const GE_Vec3 &i_position )
{
	GE_Vec3 offset = *m_light->getPosition() - i_position;
	m_light->setTarget( *m_light->getTarget() - offset );
	m_light->setPosition( i_position );
}

void GEW_Light::setRotation( const GE_Quat &i_rotation )
{

}

void GEW_Light::setRotation( GE_Mat4x4 &i_rot, GE_Vec3 & i_pivot )
{
	GE_Mat4x4 pvotMat;
	pvotMat.transform( i_pivot );
	GE_Mat4x4 mat;
	mat.transform( *m_light->getPosition() - i_pivot );
	mat *= i_rot;
	mat *= pvotMat;

	GE_Vec3 pos;
	GE_Quat rot;
	GE_Vec3 scale;
	mat.decompose( &pos, &rot, &scale );
	m_light->setPosition( pos );

	mat.transform( *m_light->getTarget() - i_pivot );
	mat *= i_rot;
	mat *= pvotMat;
	mat.decompose( &pos, &rot, &scale );
	m_light->setTarget( pos );
}

void GEW_Light::setScale( const GE_Vec3 &i_scale )
{

}

void GEW_Light::setTransform( GE_Mat4x4 *i_matrix )
{
	GE_Vec3 pos = getPosition();
	m_light->getTarget()->transform( i_matrix );
	pos.transform( i_matrix );
	setPosition( pos );
}

GE_Vec3 GEW_Light::getPosition()
{
	return *m_light->getPosition();
}

GE_Vec3 GEW_Light::getRotation()
{
	return GE_Vec3( 0, 0, 0 );
}

GE_Vec3 GEW_Light::getScale()
{
	return GE_Vec3( 0, 0, 0 );
}

void GEW_Light::getRenderUtilityObjects( std::list<GE_RenderUtilityObject*> &i_outList )
{
	if( m_probe.Cage->isVisible() )
		i_outList.push_back( m_probe.Cage );

	if( m_probe.Target->isVisible() )
		i_outList.push_back( m_probe.Target );

	if( m_probe.Billboard->isVisible() )
		i_outList.push_back( m_probe.Billboard );

	if( m_probe.Spot->isVisible() )
		i_outList.push_back( m_probe.Spot );

	if( m_probe.Line->isVisible() )
		i_outList.push_back( m_probe.Line );

	if( m_probe.Cross->isVisible() )
		i_outList.push_back( m_probe.Cross );
}