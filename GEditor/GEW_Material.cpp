#include "GEW_Material.h"
#include <GE_Material.h>
#include <GE_Texture.h>
#include "GEW_Utility.h"
#include <QFile>

GEW_Material GEW_Material::m_singleton;
int GEW_Material::m_groupNewId = 0;
QIcon GEW_Material::m_groupIcon;
QIcon GEW_Material::m_materialIcon;

GEW_Material::Group::~Group()
{
	foreach( GE_Material *material, List )
	{
		//TODO : un set this material
		delete material;
	}
}

int GEW_Material::Group::getMaterialId( const QString &i_name )
{
	int id = 0;
	foreach( GE_Material *material, List )
	{
		if( QString( material->Name.c_str() ).toLower() == i_name )
			return id;
		id++;
	}
	return -1;
}

int GEW_Material::Group::getMaterialId( GE_Material *i_material )
{
	int id = 0;
	foreach( GE_Material *material, List )
	{
		if( material == i_material )
			return id;
		id++;
	}
	return -1;
}

void GEW_Material::Group::removeMaterial( int i_materialId )
{
	GE_Material *material = List[ i_materialId ];
	List.removeAt( i_materialId );
	
	//TODO : un set this material
	delete material;
}

GE_Material * GEW_Material::Group::getMaterial( int i_materialId )
{
	if( i_materialId < 0 || i_materialId >= List.size() )
		return NULL;
	return List[ i_materialId ];
}

void GEW_Material::Group::saveToFile( QFile &i_file, QString i_resourcePath )
{
	char str[ 512 ];
	strcpy_s( str, Name.toStdString().c_str() );
	i_file.write( str, 64 );

	uint32_t materialCount = List.size( );
	i_file.write( ( const char* ) &materialCount, sizeof( uint32_t ) );

	foreach( GE_Material *material, List )
	{
		SaveData data;
		strcpy_s( data.Name, material->Name.c_str() );
		data.SpecularPower = material->SpecularPower;
		data.Roughness = material->Roughness;
		data.Reflectmask = material->Reflectmask;
		data.Bumpiness = material->Bumpiness;
		data.AlphaTest = material->AlphaTest;
		data.Alpha = material->Alpha;
		data.Refract = material->Refract;
		data.Refractness = material->Refractness;
		data.Tint = material->Tint;
		data.TintSpread = material->TintSpread;
		data.EmissiveIntensity = material->getEmissiveIntensity();
		data.AmbientIntensity = material->getAmbientIntensity();
		data.SpecularIntensity = material->getSpecularIntensity();
		data.DiffuseIntensity = material->getDiffuseIntensity();
		data.GlowIntensity = material->getGlowIntensity();
		data.BaseEmissive = material->getBaseEmissive();
		data.BaseAmbient = material->getBaseAmbient();
		data.BaseSpecular = material->getBaseSpecular();
		data.BaseDiffuse = material->getBaseDiffuse();
		data.BaseGlow = material->getBaseGlow();
			
		if( material->DiffuseAlphaMap[ 0 ] )
			strcpy_s( data.DiffuseAlphaMap0,
				GEW_Utility::copyFile( material->DiffuseAlphaMap[ 0 ]->getTextureName(), i_resourcePath ).c_str() );
		if( material->DiffuseAlphaMap[ 1 ] )
			strcpy_s( data.DiffuseAlphaMap1,
				GEW_Utility::copyFile( material->DiffuseAlphaMap[ 1 ]->getTextureName(), i_resourcePath ).c_str() );

		if( material->GlowSpecularReflectMaskMap[ 0 ] )
			strcpy_s( data.GlowSpecularReflectMaskMap0,
				GEW_Utility::copyFile( material->GlowSpecularReflectMaskMap[ 0 ]->getTextureName(), i_resourcePath ).c_str() );
		if( material->GlowSpecularReflectMaskMap[ 1 ] )
			strcpy_s( data.GlowSpecularReflectMaskMap1,
				GEW_Utility::copyFile( material->GlowSpecularReflectMaskMap[ 1 ]->getTextureName(), i_resourcePath ).c_str() );

		if( material->NormalHeightMap[ 0 ] )
			strcpy_s( data.NormalHeightMap0,
				GEW_Utility::copyFile( material->NormalHeightMap[ 0 ]->getTextureName(), i_resourcePath ).c_str() );
		if( material->NormalHeightMap[ 1 ] )
			strcpy_s( data.NormalHeightMap1,
				GEW_Utility::copyFile( material->NormalHeightMap[ 1 ]->getTextureName(), i_resourcePath ).c_str() );

		if( material->ReflectMap )
			strcpy_s( data.ReflectMap,
				GEW_Utility::copyFile( material->ReflectMap->getTextureName(), i_resourcePath ).c_str() );

		if (material->TintMap)
			strcpy_s(data.TintMap,
			GEW_Utility::copyFile(material->TintMap->getTextureName(), i_resourcePath).c_str());

		i_file.write( ( const char* ) &data, sizeof( SaveData ) );
	}
}

GEW_Material::SaveData::SaveData()
{
	memset( this, 0, sizeof( SaveData ) );
}

GEW_Material::GEW_Material()
{
	m_materialP = NULL;
	m_materialPP = NULL;
	m_tick = 0;
	m_et = 0;
}

GEW_Material::~GEW_Material()
{
	foreach( GEW_Material::Group *group, m_list )
		delete group;
	m_materialP = NULL;
	m_materialPP = NULL;
}

GEW_Material & GEW_Material::getSingleton()
{
	return m_singleton;
}

GEW_Material::Group *GEW_Material::addGroup( const QString i_name )
{
	GEW_Material::Group *group = new GEW_Material::Group();
	group->Name = i_name;
	m_list.push_back( group );
	return group;
}

GE_Material *GEW_Material::addMaterial( int i_groupId, const QString &i_name )
{
	if( i_groupId < 0 || i_groupId >= m_list.size() )
		return NULL;
	GEW_Material::Group *group = m_list[ i_groupId ];
	GE_Material *material = new GE_Material();
	material->Name = i_name.toStdString();
	group->List.push_back( material );
	return material;
}

GEW_Material::Group *GEW_Material::getGroup( int i_groupId )
{
	if( i_groupId < 0 || i_groupId >= m_list.size() )
		return NULL;
	return m_list[ i_groupId ];
}

int GEW_Material::getGroupId( const QString &i_name )
{
	int id = 0;
	foreach( GEW_Material::Group *group, m_list )
	{
		if( group->Name.toLower() == i_name )
			return id;
		id++;
	}
	return -1;
}

void GEW_Material::getMaterialId( int &i_outGroupId, int &i_outMaterialId, GE_Material *i_material )
{
	int materialId;
	int groupId = 0;
	foreach( GEW_Material::Group *group, m_list )
	{
		materialId = group->getMaterialId( i_material );
		if( materialId != -1 )
		{
			if( groupId == 0 && materialId == 1 )
				materialId = 0;
			i_outGroupId = groupId;
			i_outMaterialId = materialId;
			return;
		}
		groupId++;
	}
	i_outGroupId = -1;
	i_outMaterialId = -1;
}

void GEW_Material::getMaterialId( int &i_outGroupId, int &i_outMaterialId, const QString &i_materialName )
{
	int materialId;
	int groupId = 0;
	foreach( GEW_Material::Group *group, m_list )
	{
		materialId = group->getMaterialId( i_materialName );
		if( materialId != -1 )
		{
			if( groupId == 0 && materialId == 1 )
				materialId = 0;
			i_outGroupId = groupId;
			i_outMaterialId = materialId;
			return;
		}
		groupId++;
	}
	i_outGroupId = -1;
	i_outMaterialId = -1;
}

GE_Material * GEW_Material::getMaterial( int i_groupId, int i_materialId )
{
	if( i_groupId == 0 && i_materialId == 1 )
		return m_list[ 0 ]->List[ 0 ];
	return m_list[ i_groupId ]->List[ i_materialId ];
}

int GEW_Material::nbGroup()
{
	return m_list.size();
}

void GEW_Material::init()
{
	GE_Material *nullMaterial = new GE_Material();
	nullMaterial->Name = "NULL";
	nullMaterial->DiffuseAlphaMap[ 0 ] = GE_TextureManager::getSingleton().createTexture( "../../GData/Texture/d.dds" );
	nullMaterial->DiffuseAlphaMap[ 1 ] = GE_TextureManager::getSingleton().createTexture( "../../GData/Texture/d1.dds" );
	nullMaterial->NormalHeightMap[ 0 ] = GE_TextureManager::getSingleton().createTexture( "../../GData/Texture/n.dds" );
	nullMaterial->NormalHeightMap[ 1 ] = GE_TextureManager::getSingleton().createTexture( "../../GData/Texture/n.dds" );
	nullMaterial->GlowSpecularReflectMaskMap[ 0 ] = NULL;
	nullMaterial->GlowSpecularReflectMaskMap[ 1 ] = NULL;
	nullMaterial->ReflectMap = NULL;
	nullMaterial->TintMap = NULL;
	GE_Material *selectMaterial = new GE_Material();
	selectMaterial->Name = "SELECT";
	selectMaterial->DiffuseAlphaMap[ 0 ] = NULL;
	selectMaterial->DiffuseAlphaMap[ 1 ] = NULL;
	selectMaterial->NormalHeightMap[ 0 ] = NULL;
	selectMaterial->NormalHeightMap[ 1 ] = NULL;
	selectMaterial->GlowSpecularReflectMaskMap[ 0 ] = NULL;
	selectMaterial->GlowSpecularReflectMaskMap[ 1 ] = NULL;
	selectMaterial->ReflectMap = NULL;
	selectMaterial->Emissive.x = 0;
	selectMaterial->Alpha = 1.0f;

	GEW_Material::Group *group = new GEW_Material::Group();
	group->Name = QString( "NULL" );
	group->List.push_back( nullMaterial );
	group->List.push_back( selectMaterial );
	
	m_list.push_back( group );

	m_materialIcon = QIcon(":/Resources/MetroStyle/poo.png");
	m_groupIcon = QIcon(":/Resources/MetroStyle/folder.png");
}


void GEW_Material::update( float i_et )
{
	m_et += i_et;
	if( m_et >= 1.0f / 20.0f )
	{
		GE_Material *materialSelect = m_list[ 0 ]->List[ 1 ];
		if( m_materialPP )
		{
			m_tick++;
			if( m_tick == 6 )
			{
				m_tick = 0;
				*m_materialPP = m_materialP;
				m_materialPP = NULL;
				materialSelect->DiffuseAlphaMap[ 0 ] = NULL;
				materialSelect->DiffuseAlphaMap[ 1 ] = NULL;
				materialSelect->GlowSpecularReflectMaskMap[ 0 ] = NULL;
				materialSelect->GlowSpecularReflectMaskMap[ 1 ] = NULL;
				materialSelect->NormalHeightMap[ 0 ] = NULL;
				materialSelect->NormalHeightMap[ 1 ] = NULL;
				materialSelect->ReflectMap = NULL;
			}
		}
		if( materialSelect->Emissive.x > 0 )
			materialSelect->Emissive.x = 0.0f;
		else
			materialSelect->Emissive.x = 1.0f;
		m_et = 0;
	}
}

void GEW_Material::removeGroup( int i_groupId )
{
	if( i_groupId < 1 || i_groupId >= m_list.size() )
		return;
	GEW_Material::Group *group = m_list[ i_groupId ];
	m_list.removeAt( i_groupId );
	delete group;
}

void GEW_Material::selectMaterial( GE_Material **i_ppMaterial )
{
	if( m_materialPP == i_ppMaterial )
		return;

	if( m_materialPP )
		*m_materialPP = m_materialP;

	m_materialPP = i_ppMaterial;
	if( m_materialPP && *m_materialPP )
	{
		m_et = 0;
		m_tick = 0;
		
		GE_Material *materialSelect = m_list[ 0 ]->List[ 1 ];

		m_materialP = *m_materialPP;
		*m_materialPP = materialSelect;

		materialSelect->SpecularPower = m_materialP->SpecularPower;
		materialSelect->Roughness = m_materialP->Roughness;
		materialSelect->Bumpiness = m_materialP->Bumpiness;
		materialSelect->AlphaTest = m_materialP->AlphaTest;
		materialSelect->Alpha = m_materialP->Alpha;
		materialSelect->Refract = m_materialP->Refract;
		materialSelect->Refractness = m_materialP->Refractness;
		materialSelect->Tint = m_materialP->Tint;
		materialSelect->TintSpread = m_materialP->TintSpread;

		materialSelect->DiffuseAlphaMap[ 0 ] = m_materialP->DiffuseAlphaMap[ 0 ];
		materialSelect->DiffuseAlphaMap[ 1 ] = m_materialP->DiffuseAlphaMap[ 1 ];

		materialSelect->GlowSpecularReflectMaskMap[ 0 ] = m_materialP->GlowSpecularReflectMaskMap[ 0 ];
		materialSelect->GlowSpecularReflectMaskMap[ 1 ] = m_materialP->GlowSpecularReflectMaskMap[ 1 ];

		materialSelect->NormalHeightMap[ 0 ] = m_materialP->NormalHeightMap[ 0 ];
		materialSelect->NormalHeightMap[ 1 ] = m_materialP->NormalHeightMap[ 1 ];

		materialSelect->ReflectMap = m_materialP->ReflectMap;
		materialSelect->TintMap = m_materialP->TintMap;

		materialSelect->setDiffuseIntensity( m_materialP->getDiffuseIntensity() );
		materialSelect->setBaseDiffuse( m_materialP->getBaseDiffuse() );

		materialSelect->setGlowIntensity( m_materialP->getGlowIntensity() );
		materialSelect->setBaseGlow( m_materialP->getBaseGlow() );

		//materialSelect->setEmissiveIntensity( m_pMaterial->getEmissiveIntensity() );
		//materialSelect->setBaseEmissive( m_pMaterial->getBaseEmissive() );

		materialSelect->setAmbientIntensity( m_materialP->getAmbientIntensity() );
		materialSelect->setBaseAmbient( m_materialP->getBaseAmbient() );

		materialSelect->setSpecularIntensity( m_materialP->getSpecularIntensity() );
		materialSelect->setBaseSpecular( m_materialP->getBaseSpecular() );
	}
}

bool GEW_Material::isNoMaterial( GE_Material *i_material )
{
	return ( i_material == m_list[ 0 ]->List[ 0 ] );
}

bool GEW_Material::isSelectMaterial( GE_Material *i_material )
{
	return ( i_material == m_list[ 0 ]->List[ 1 ] );
}

GE_Material* GEW_Material::getNullMaterial( )
{
	return m_list[ 0 ]->List[ 0 ];
}

void GEW_Material::saveToFile( QFile &i_file, const QString &i_resourcePath, int i_groupId /*= -1 */ )
{
	uint32_t groupCount;

	if( i_groupId >= 0 )
	{
		groupCount = 1;
		i_file.write( ( const char* ) &groupCount, sizeof( uint32_t ) );
		m_list[ i_groupId ]->saveToFile( i_file, i_resourcePath );
	} else
	{
		groupCount = m_list.size() - 1;
		i_file.write( ( const char* ) &groupCount, sizeof( uint32_t ) );
		for( int i = 1; i < m_list.size(); ++i )
			m_list[ i ]->saveToFile( i_file, i_resourcePath );
	}
}

void GEW_Material::loadFromFile( QFile &i_file, const QString &i_resourcePath )
{
	char str[ 512 ];
	uint32_t groupCount;
	i_file.read( ( char* ) &groupCount, sizeof( uint32_t ) );
	
	for( uint32_t i = 0; i < groupCount; ++i )
	{
		i_file.read( str, 64 );//read group name
		QString groupName( str );
		int nameIndex = 1;
		while( true )
		{
			if( getGroupId( groupName.toLower() ) == -1 )
				break;
			groupName = QString( "%1_%2" ).arg( str).arg( nameIndex++ );
		}
		Group *group = addGroup( groupName );
			
		uint32_t materialCount;
		i_file.read( ( char* ) &materialCount, sizeof( uint32_t ) );
		for( uint32_t j = 0; j < materialCount; ++j )
		{
			SaveData data;
			i_file.read( ( char* ) &data, sizeof( SaveData ) );
			GE_Material *material = new GE_Material;
			group->List.push_back( material );

			material->Name = data.Name;
			material->SpecularPower = data.SpecularPower;
			material->Roughness = data.Roughness;
			material->Reflectmask = data.Reflectmask;
			material->Bumpiness = data.Bumpiness;
			material->AlphaTest = data.AlphaTest;
			material->Alpha = data.Alpha;
			material->Refract = data.Refract;
			material->Refractness = data.Refractness;
			material->Tint = data.Tint;
			material->TintSpread = data.TintSpread;
			material->setEmissiveIntensity( data.EmissiveIntensity );
			material->setAmbientIntensity( data.AmbientIntensity );
			material->setSpecularIntensity( data.SpecularIntensity );
			material->setDiffuseIntensity( data.DiffuseIntensity );
			material->setGlowIntensity( data.GlowIntensity );
			material->setBaseEmissive( data.BaseEmissive );
			material->setBaseAmbient( data.BaseAmbient );
			material->setBaseSpecular( data.BaseSpecular );
			material->setBaseDiffuse( data.BaseDiffuse );
			material->setBaseGlow( data.BaseGlow );

			if( data.DiffuseAlphaMap0[ 0 ] )
				material->DiffuseAlphaMap[ 0 ] = GE_TextureManager::getSingleton().createTexture(
				GEW_Utility::absolutePath( data.DiffuseAlphaMap0, i_resourcePath ).c_str() );
			if( data.DiffuseAlphaMap1[ 0 ] )
				material->DiffuseAlphaMap[ 1 ] = GE_TextureManager::getSingleton().createTexture(
				GEW_Utility::absolutePath( data.DiffuseAlphaMap1, i_resourcePath ).c_str() );

			if( data.GlowSpecularReflectMaskMap0[ 0 ] )
				material->GlowSpecularReflectMaskMap[ 0 ] = GE_TextureManager::getSingleton().createTexture(
				GEW_Utility::absolutePath( data.GlowSpecularReflectMaskMap0, i_resourcePath ).c_str() );
			if( data.GlowSpecularReflectMaskMap1[ 0 ] )
				material->GlowSpecularReflectMaskMap[ 1 ] = GE_TextureManager::getSingleton().createTexture(
				GEW_Utility::absolutePath( data.GlowSpecularReflectMaskMap1, i_resourcePath ).c_str() );

			if( data.NormalHeightMap0[ 0 ] )
				material->NormalHeightMap[ 0 ] = GE_TextureManager::getSingleton().createTexture(
				GEW_Utility::absolutePath( data.NormalHeightMap0, i_resourcePath ).c_str() );
			if( data.NormalHeightMap1[ 0 ] )
				material->NormalHeightMap[ 1 ] = GE_TextureManager::getSingleton().createTexture(
				GEW_Utility::absolutePath( data.NormalHeightMap1, i_resourcePath ).c_str() );

			if( data.ReflectMap[ 0 ] )
				material->ReflectMap = GE_TextureManager::getSingleton().createTexture(
				GEW_Utility::absolutePath( data.ReflectMap, i_resourcePath ).c_str() );

			if (data.TintMap[0])
				material->TintMap = GE_TextureManager::getSingleton().createTexture(
				GEW_Utility::absolutePath(data.TintMap, i_resourcePath).c_str());
		}
	}
}

void GEW_Material::clearAll()
{
	int i = m_list.size( );
	for ( ; i > 1 ; --i )
	{
		GEW_Material::Group *group = m_list[ 1 ];
		m_list.removeAt( 1 );
		delete group;
	}
}