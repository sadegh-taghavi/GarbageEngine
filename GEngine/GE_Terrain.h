#pragma once
#include "GE_Math.h"
#include "GE_Prerequisites.h"
#include "GE_RenderUtility.h"
#include <vector>
#define GE_MAX_TERRAIN_PAINT_LAYER 4
#define GE_MAX_TERRAIN_PROCEDURAL_LAYER 8

class GE_Terrain
{
	struct Quad;
	uint32_t	m_numOut;
	uint32_t	m_numVisit;
	float		m_lodMultiplier;
	float		m_width;
	uint32_t	m_quadDensity;
public:
	struct PaintParameters
	{
		GE_Vec2		UVTile;
		GE_Vec2		UVPosition;
		float		Opacity;
		float		__padding0;
		float		__padding1;
		float		__padding2;
		bool		Enable;
		PaintParameters();
		~PaintParameters();
	};
	struct ProceduralParameters
	{
		GE_Vec2		UVTile;
		GE_Vec2		UVPosition;
		GE_Vec2		MaskPosition;
		GE_Vec2		MaskUVTile;
		GE_Vec2		MaskUVPosition;
		float		MaskField;
		float		MaskFalloff;
		float		MaskHardness;
		float		Opacity;
		float		Height;
		float		HeightField;
		float		HeightFalloff;
		float		HeightHardness;
		float		Angle;
		float		AngleField;
		float		AngleFalloff;
		float		AngleHardness;
		float		__Padding0;
		float		__Padding1;
		bool		Enable;
		ProceduralParameters();
		~ProceduralParameters();
	};
	struct PerMask
	{
		GE_Vec2		UVTile;
		GE_Vec2		UVPosition;
		float		Opacity;
		float		__padding0;
		float		__padding1;
		float		__padding2;
	}m_cPerMask[ GE_MAX_TERRAIN_PAINT_LAYER ];
	struct PerProcedural
	{
		GE_Vec2		UVTile;
		GE_Vec2		UVPosition;
		GE_Vec2		MaskPosition;
		GE_Vec2		MaskUVTile;
		GE_Vec2		MaskUVPosition;
		float		MaskField;
		float		MaskFalloff;
		float		MaskHardness;
		float		Opacity;
		float		Height;
		float		HeightField;
		float		HeightFalloff;
		float		HeightHardness;
		float		Angle;
		float		AngleField;
		float		AngleFalloff;
		float		AngleHardness;
		float		__Padding0;
		float		__Padding1;
	}m_cPerProcedural[ GE_MAX_TERRAIN_PROCEDURAL_LAYER ];
	struct MaterialParameters
	{
		float		SpecularPower;
		GE_Vec3		Emissive;
		GE_Vec3		Ambient;
		GE_Vec3		Specular;
		GE_Vec3		Diffuse;
		GE_Vec3		Glow;
		float		Roughness;
		float		Reflectmask;
		float		Bumpiness;
	private:
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
	public:
		PaintParameters Painting[ GE_MAX_TERRAIN_PAINT_LAYER ];
		ProceduralParameters Procedural[ GE_MAX_TERRAIN_PROCEDURAL_LAYER ];
		MaterialParameters();
		~MaterialParameters();

		float		&getDiffuseIntensity();
		void		setDiffuseIntensity( float i_intensity );
		GE_Vec3		&getBaseDiffuse();
		void		setBaseDiffuse( const GE_Vec3 &i_color );

		float		&getGlowIntensity();
		void		setGlowIntensity( float i_intensity );
		GE_Vec3		&getBaseGlow();
		void		setBaseGlow( const GE_Vec3 &i_color );

		float		&getEmissiveIntensity();
		void		setEmissiveIntensity( float i_intensity );
		GE_Vec3		&getBaseEmissive();
		void		setBaseEmissive( const GE_Vec3 &i_color );

		float		&getAmbientIntensity();
		void		setAmbientIntensity( float i_intensity );
		GE_Vec3		&getBaseAmbient();
		void		setBaseAmbient( const GE_Vec3 &i_color );

		float		&getSpecularIntensity();
		void		setSpecularIntensity( float i_intensity );
		GE_Vec3		&getBaseSpecular();
		void		setBaseSpecular( const GE_Vec3 &i_color );

	}m_materialParameters;

	struct PaintMaps
	{
		GE_Texture *DiffuseMap;
		GE_Texture *NormalMap;
		GE_Texture *GSRMap;
		PaintMaps();
		~PaintMaps();
	};
	struct ProceduralMaps
	{
		GE_Texture *DiffuseMap;
		GE_Texture *NormalMap;
		GE_Texture *GSRMap;
		GE_Texture *MaskMap;
		ProceduralMaps();
		~ProceduralMaps();
	};
	struct Maps
	{
		PaintMaps		Painting[ GE_MAX_TERRAIN_PAINT_LAYER ];
		ProceduralMaps	Procedural[ GE_MAX_TERRAIN_PROCEDURAL_LAYER ];
		GE_Texture *ReflectMap;
		GE_Texture *NormalNoiseMap;
		Maps();
		~Maps();
	}m_maps;

	struct VertexBuffer
	{
		GE_Vec3		Position;
	};
	struct IndexBuffer
	{
		uint32_t Triangle[ 3 ];
	};
	struct PerFrame
	{
		GE_Mat4x4	ViewProjection;
		GE_Mat4x4	TexcoordViewProjection;
		GE_Vec4		ViewPosition;
		GE_Vec4		EnvironmentPosition;
		GE_Vec2		NormalNoiseUVTile;
		GE_Vec2		NormalNoiseEnable;
		float		EnvironmentRadiusInv;
		float		Bumpiness;
		float		Roughness;
		float		Reflectmask;
		float		MaterialID;
		float		LayerMultiply;
		float		LayerOffset;
		float		Glowness;
		float		HeightBias;
		float		MaxHeight;
		float		NormalAspect;
		float		FieldSize;
	}m_cPerFrame;

	struct Level
	{
		enum
		{
			CENTER = 0,
			DOWN,
			DOWN_LEFT,
			DOWN_LEFT_RIGHT,
			MAX,
		};
		vector< VertexBuffer > Vertices;
		vector< IndexBuffer > Indices[ MAX ];
		ID3D11Buffer *VB;
		ID3D11Buffer *IB[ MAX ];
		void create( float i_sizeWidth, uint32_t i_quadDensityWidth );
		Level();
		~Level();
	};

	std::vector<Level> m_levels;
	float getLodMultiplier();
	void setLodMultiplier( float i_lodMultiplier );
	float getMaxHeight();
	void setMaxHeight( float i_maxHeight );
	float getHeightBias();
	void setHeightBias( float i_heightBias );
	float getWidth();
	uint32_t getQuadDensity();
	float getNormalAspect();
	void setNormalAspect( float i_normalAspect );
	ID3D11ShaderResourceView * getHeightMap();
	void setHeightMap( ID3D11ShaderResourceView * i_heightMap );
	ID3D11ShaderResourceView * getMaskMap();
	void setMaskMap( ID3D11ShaderResourceView * i_maskMap );
	uint32_t getNumVisit(){ return m_numVisit; }
	uint32_t getNumOut(){ return m_numOut; }
	void create( float i_sizeWidth, uint32_t i_chunkQuadDensityWidth, uint32_t i_numberOfChunksWidth );
	void render( const class GE_View *i_view );
	void init();
	void getVerticesData( vector<GE_Vec3> *i_vertices, vector<uint32_t> *i_indices, GE_Vec2 &i_heightMinMax, uint32_t i_quadDensity );
	inline static GE_Terrain &getSingleton(){ return m_singleton; }

	struct Buffer
	{
		class _GE_BufferResource *BufferResource[ Level::MAX ];
		uint32_t				  NumberOfRenderInstances[ Level::MAX ];
		vector<GE_Vec4>			  InstancesData[ Level::MAX ];
		vector<Quad *>			  BaseNodes;
		void createBuffer( uint32_t i_size );
		Buffer();
		~Buffer();
	} *m_instances;


private:
	ID3D11ShaderResourceView *m_heightMap;
	ID3D11ShaderResourceView *m_maskMap;

	struct Quad
	{
		bool	IsChunk;
		GE_Vec3	Position;
		float	HalfSize;
		int32_t	ChunkID;
		int32_t	X;
		int32_t	Y;
		Quad	*Parrent;
		Quad	*UpLeft;
		Quad	*UpRight;
		Quad	*DownLeft;
		Quad	*DownRight;
		bool	calculate( float i_halfSize, float i_minHalfSize, const Quad *i_base );
		Quad();
		~Quad();
	} *m_tree;
	GE_Terrain();
	~GE_Terrain();
	void check( Quad *i_quad, const GE_View *i_view );
	static GE_Terrain		m_singleton;
	ID3D11InputLayout		*m_vertexDeclaration;
	ID3D11Buffer			*m_perFrame;
	ID3D11Buffer			*m_perMask;
	ID3D11Buffer			*m_perProcedural;
	ID3D11VertexShader		*m_vS_StreamOut;
	ID3D11GeometryShader	*m_gS_StreamOut;
	ID3D11VertexShader		*m_vS;
	ID3D11PixelShader		*m_pS[ GE_MAX_TERRAIN_PAINT_LAYER + 1 ][ GE_MAX_TERRAIN_PROCEDURAL_LAYER + 1 ];

};

#include "GE_Terrain.inl"
