#pragma once
#include <QList>
#include <QString>
#include <QPair>
#include <QIcon>
#include <QFile>
#include <GE_Vec3.h>

struct GE_Material;

class GEW_Material
{
	GEW_Material();
	~GEW_Material();
	GE_Material **m_materialPP;
	GE_Material *m_materialP;
	float m_et;
	int m_tick;
public:
	struct Group
	{
		QString					Name;
		QList<GE_Material *>	List;
		~Group();
		int getMaterialId( const QString &i_name );
		int getMaterialId( GE_Material *i_material );
		void removeMaterial( int i_materialId );
		GE_Material * getMaterial( int i_materialId );
		void saveToFile( QFile &i_file, QString i_resourcePath );
	};

	struct SaveData
	{
		char		Name[ 64 ];
		float		SpecularPower;
		float		Roughness;
		float		Reflectmask;
		float		Bumpiness;
		float		AlphaTest;
		float		Alpha;
		float		Refract;
		float		Refractness;
		float		Tint;
		float		TintSpread;
		char		DiffuseAlphaMap0[ 512 ];
		char		DiffuseAlphaMap1[ 512 ];
		char		GlowSpecularReflectMaskMap0[ 512 ];
		char		GlowSpecularReflectMaskMap1[ 512 ];
		char		NormalHeightMap0[ 512 ];
		char		NormalHeightMap1[ 512 ];
		char		ReflectMap[ 512 ];
		char		TintMap[512];
		float		EmissiveIntensity;
		float		AmbientIntensity;
		float		SpecularIntensity;
		float		DiffuseIntensity;
		float		GlowIntensity;
		GE_Vec3		BaseEmissive;
		GE_Vec3		BaseAmbient;
		GE_Vec3		BaseSpecular;
		GE_Vec3		BaseDiffuse;
		GE_Vec3		BaseGlow;
		SaveData();
	};

	static GEW_Material &getSingleton();
	void init();
	void update( float i_et );
	void saveToFile( QFile &i_file, const QString &i_resourcePath, int i_groupId = -1 );
	void loadFromFile( QFile &i_file, const QString &i_resourcePath );
	void clearAll();

	Group *addGroup( const QString i_name );
	void removeGroup( int i_groupId );
	GE_Material *addMaterial( int i_groupId, const QString &i_name );
	void removeMaterial( GE_Material* i_material );

	Group *getGroup( int i_groupId );
	int getGroupId( const QString &i_name );
	void getMaterialId( int &i_outGroupId, int &i_outMaterialId, GE_Material *i_material );
	void getMaterialId( int &i_outGroupId, int &i_outMaterialId, const QString &i_materialName );
	GE_Material *getMaterial( int i_groupId, int i_materialId );

	int nbGroup();
	void selectMaterial( GE_Material **i_ppMaterial );
	bool isNoMaterial( GE_Material *i_material );
	bool isSelectMaterial( GE_Material *i_material );
	GE_Material* getNullMaterial();
	static int m_groupNewId;
	static QIcon m_materialIcon;
	static QIcon m_groupIcon;
private:
	static GEW_Material m_singleton;
	QList<Group*> m_list;
};

