#define MAX_MASK_LAYER		 4
#define MAX_PROCEDURAL_LAYER 8

cbuffer m_perFrame : register( b0 )
{
	float4x4	m_viewProjection;
	float4x4	m_texcoordViewProjection;
	float4		m_viewPosition;
	float4		m_environmentPosition;
	float2		m_normalNoiseUVTile;
	float2		m_normalNoiseEnable;
	float		m_environmentRadiusInv;
	float		m_bumpiness;
	float		m_roughness;
	float		m_reflectmask;
	float		m_materialID;
	float		m_layerMultiply;
	float		m_layerOffset;
	float		m_glowness;
	float		m_heightBias;
	float		m_maxHeight;
	float		m_normalAspect;
	float		m_fieldSize;
};
struct MaskParameters
{
	float2		UVTile;
	float2		UVPosition;
	float		Opacity;
	float		__padding0;
	float		__padding1;
	float		__padding2;
};

struct ProceduralParams
{
	float2		UVTile;
	float2		UVPosition;
	float2		MaskPosition;
	float2		MaskUVTile;
	float2		MaskUVPosition;
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
};

cbuffer m_perMask : register( b1 )
{
	MaskParameters m_perMaskParams[ MAX_MASK_LAYER ];
};

cbuffer m_perProcedural : register( b2 )
{
	ProceduralParams m_perProceduralParams[ MAX_PROCEDURAL_LAYER ];
};

Buffer<float4>		m_instances			: register( t0 );
Texture2D<float>	m_heightMap			: register( t1 );
Texture2D			m_bfnMap			: register( t2 );
Texture2D			m_maskMap			: register( t3 );
Texture2D			m_normalNoiseMap	: register( t4 );
TextureCube			m_reflectMap		: register( t5 );

#define	MASK_DIFFUSE_ADDRESS (6)
#define	MASK_NORMAL_ADDRESS  (MASK_DIFFUSE_ADDRESS + MAX_MASK_LAYER)
#define	MASK_GSR_ADDRESS  (MASK_NORMAL_ADDRESS + MAX_MASK_LAYER)

#define	PROCEDURAL_DIFFUSE_ADDRESS (MASK_GSR_ADDRESS + MAX_MASK_LAYER)
#define	PROCEDURAL_NORMAL_ADDRESS  (PROCEDURAL_DIFFUSE_ADDRESS + MAX_PROCEDURAL_LAYER)
#define	PROCEDURAL_GSR_ADDRESS  (PROCEDURAL_NORMAL_ADDRESS + MAX_PROCEDURAL_LAYER)
#define	PROCEDURAL_MASK_ADDRESS  (PROCEDURAL_GSR_ADDRESS + MAX_PROCEDURAL_LAYER)

Texture2D			m_diffuseMapM[ MAX_MASK_LAYER ]		: register( t[ MASK_DIFFUSE_ADDRESS ] );
Texture2D			m_normalMapM[ MAX_MASK_LAYER ]		: register( t[ MASK_NORMAL_ADDRESS ] );
Texture2D			m_gSRMapM[ MAX_MASK_LAYER ]			: register( t[ MASK_GSR_ADDRESS ] );

Texture2D			m_diffuseMapP[ MAX_PROCEDURAL_LAYER ]	: register( t[ PROCEDURAL_DIFFUSE_ADDRESS ] );
Texture2D			m_normalMapP[ MAX_PROCEDURAL_LAYER ]	: register( t[ PROCEDURAL_NORMAL_ADDRESS ] );
Texture2D			m_gSRMapP[ MAX_PROCEDURAL_LAYER ]		: register( t[ PROCEDURAL_GSR_ADDRESS ] );
Texture2D<float>	m_maskMapP[ MAX_PROCEDURAL_LAYER ]	: register( t[ PROCEDURAL_MASK_ADDRESS ] );

SamplerState m_samLinear : register( s0 );
SamplerState m_samPoint  : register( s1 );

struct VS_Input
{
	float3 Position		:	POSITION;
	uint   InstanceID	:	SV_InstanceID;
};

struct VS_StreamOut
{
	float3 Position		:	POSITION;
};

struct PS_Input
{
	float4 Position				:	SV_POSITION;
	float4 Texcoord				:	TEXCOORD0;
	float4 PositionInstanceID	:	TEXCOORD1;
	float4 ViewVec_Height		:	TEXCOORD2;
	//float3 Normal				:	NORMAL0;
};

struct PS_Output
{
	float4 Normal_Specular	: SV_Target0;
	float4 Diffuse_Glow		: SV_Target1;
	float4 MaterialID		: SV_Target2;
};
void packNormal( float3 i_normal, out float3 i_packedNormal )
{
	float3 vNormalUns = abs( i_normal );
		float maxNAbs = max( vNormalUns.z, max( vNormalUns.x, vNormalUns.y ) );
	float2 vTexCoord = vNormalUns.z < maxNAbs ? ( vNormalUns.y < maxNAbs ? vNormalUns.yz : vNormalUns.xz ) : vNormalUns.xy;
		vTexCoord = vTexCoord.x < vTexCoord.y ? vTexCoord.yx : vTexCoord.xy;
	vTexCoord.y /= vTexCoord.x;
	i_packedNormal = i_normal / maxNAbs;
	i_packedNormal *= m_bfnMap.Sample( m_samLinear, vTexCoord ).w;
	i_packedNormal = i_packedNormal * 0.5f + 0.5f;
}
