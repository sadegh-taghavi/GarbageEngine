#include "GE_Particle.h"
#include "GE_BufferResource.h"
#include "GE_Texture.h"
#include "GE_Core.h"
#include "GE_Math.h"
#include "GE_View.h"
#include <boost\foreach.hpp>
#include "GE_Utility.h"

GE_Particle::Data::Data()
{
	Position_Layer = GE_Vec4(0.0f, 0.0f, 0.0f, 0.0f);
	Emissive_Rotation = GE_Vec4(0,0,0,0);
	Diffuse = GE_Vec4(1.0f,1.0f,1.0f,1.0f);
	Size = GE_Vec2(1.0f,1.0f);
}

GE_Particle::GE_Particle(uint32_t i_numberOfParticle)
{
	m_diffuseTexture=NULL;
	m_diffuseSRV=NULL;
	m_softness=0.5f;
	m_position=GE_Vec3(0,0,0);
	m_rotation.identity();
	m_scale=GE_Vec3(1.0f,1.0f,1.0f);
	m_axisMask=GE_Vec3(1.0f,1.0f,1.0f);
	m_visible=m_translateChange=m_particleChange=true;
	m_boundingRadius=0;
	m_userData=0;
	m_shimmerness=0;
	m_instanceChange=false;
	m_numberOfParticle=i_numberOfParticle;
	m_particleData=new Data[m_numberOfParticle];
	m_particleBuffer=GE_Core::getSingleton().createVertexBuffer(m_particleData,sizeof(Data),m_numberOfParticle,false);
	m_blendState=GE_Core::getSingleton().getRenderStates().BlendStateAlpha;
}

void GE_Particle::setDiffuseTexture(const char *i_fileName)
{
	if(m_diffuseTexture)
		GE_TextureManager::getSingleton().remove(&m_diffuseTexture);
	m_diffuseTexture=GE_TextureManager::getSingleton().createTexture(i_fileName);
}
ID3D11ShaderResourceView *GE_Particle::getDiffuseSRV()
{
	if(m_diffuseSRV)
		return m_diffuseSRV;
	if(m_diffuseTexture)
		return m_diffuseTexture->getTextureSRV();
	return NULL;
}
void GE_Particle::createInstance(uint32_t i_numberOfInstance)
{
	SAFE_DELETE_ARRAY( m_instanceBuffer.m_data );
	m_instanceBuffer.release();
	m_instanceBuffer.createBuffer(i_numberOfInstance,sizeof(GE_Mat4x4),NULL,false,DXGI_FORMAT_R32G32B32A32_FLOAT,4);
	m_instanceBuffer.m_data=new GE_Mat4x4[i_numberOfInstance];
}
void GE_Particle::setParticleData(uint32_t i_index,const Data &i_particleData)
{
	float maxS=XMMax(i_particleData.Size.x,i_particleData.Size.y);
	m_boundingRadius=XMMax(XMMax(XMMax(i_particleData.Position_Layer.x + maxS,i_particleData.Position_Layer.y + maxS),i_particleData.Position_Layer.z + maxS),m_boundingRadius);
	m_particleData[i_index]=i_particleData;
	m_particleChange=true;

}
GE_Particle::Data *GE_Particle::getParticleData(uint32_t i_index)
{
	return &m_particleData[i_index];
}
GE_Mat4x4 *GE_Particle::getInstanceData(uint32_t i_index)
{
	m_instanceChange=true;
	return &((GE_Mat4x4 *)m_instanceBuffer.m_data)[i_index];
}
uint32_t &GE_Particle::getInstanceCount()
{
	return m_instanceBuffer.m_lenght;
}
void GE_Particle::setInstanceData(uint32_t i_index,const GE_Mat4x4 &i_matrix)
{
	m_instanceChange=true;
	((GE_Mat4x4 *)m_instanceBuffer.m_data)[i_index]=i_matrix;
}
void GE_Particle::setInstanceData(uint32_t i_index,const GE_Vec3 &i_position,const GE_Quat &i_rotation,const GE_Vec3 &i_scale)
{
	m_instanceChange=true;
	((GE_Mat4x4 *)m_instanceBuffer.m_data)[i_index].srp(i_position, i_rotation, i_scale);
}
void GE_Particle::render()
{
	GE_Core::getSingleton().renderTriangle(m_particleBuffer,sizeof(Data),m_numberOfParticle);
}
void GE_Particle::renderInstance()
{
	uint32_t il=m_instanceBuffer.m_lenght;
	GE_Core::getSingleton().renderTriangle(m_particleBuffer,sizeof(Data),m_numberOfParticle,il);
}
void GE_Particle::update()
{
	if(!m_visible)
		return;

	if(m_instanceChange)
	{
		m_instanceBuffer.fill();
		m_instanceChange=false;
	}
	if(m_translateChange)
	{
		m_world.srp(m_position, m_rotation, m_scale);
		m_translateChange=false;
	}
	if(m_particleChange)
	{

		D3D11_MAPPED_SUBRESOURCE ptr;
		GE_Core::getSingleton().getImmediateContext()->Map(m_particleBuffer,0,D3D11_MAP_WRITE_DISCARD,0,&ptr);
		memcpy(ptr.pData,m_particleData,sizeof(Data) * m_numberOfParticle);
		GE_Core::getSingleton().getImmediateContext()->Unmap(m_particleBuffer,0);
		m_particleChange=false;
	}
}
ID3D11ShaderResourceView *GE_Particle::getInstanceSRV()
{
	return m_instanceBuffer.m_sRV;
}

GE_Particle::~GE_Particle()
{
	if(m_diffuseTexture)
			GE_TextureManager::getSingleton().remove(&m_diffuseTexture);
	SAFE_DELETE_ARRAY(m_instanceBuffer.m_data);	
	SAFE_DELETE_ARRAY(m_particleData);
	SAFE_RELEASE(m_particleBuffer);
}
//----------------------------------------------------------------------------------------------------------------------------------
GE_ParticelRenderer GE_ParticelRenderer::m_singleton;

void GE_ParticelRenderer::init()
{
	if(m_vertexDeclaration)
		return;

	const D3D11_INPUT_ELEMENT_DESC _vdl[] =
    {
		{ "POSITION"	, 0, DXGI_FORMAT_R32G32B32A32_FLOAT	, 0, 0	, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD"	, 0, DXGI_FORMAT_R32G32B32A32_FLOAT	, 0, 16	, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD"	, 1, DXGI_FORMAT_R32G32B32A32_FLOAT	, 0, 32	, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD"	, 2, DXGI_FORMAT_R32G32_FLOAT		, 0, 48	, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
	
	_GE_FileChunk fc;
	fc.load(string(GE_SHADER_PATH) + "GE_P_VS.cso");
	GE_Core::getSingleton().getDevice()->CreateInputLayout(_vdl,4,fc.Data,fc.Len,&m_vertexDeclaration);
	GE_Core::getSingleton().getDevice()->CreateVertexShader(fc.Data,fc.Len,NULL,&m_vS);

	m_gS=GE_Core::getSingleton().createGS(string(GE_SHADER_PATH) + "GE_P_GS.cso");
	m_gS_Instance=GE_Core::getSingleton().createGS(string(GE_SHADER_PATH) + "GE_P_GS_INSTANCE.cso");

	m_pS=GE_Core::getSingleton().createPS(string(GE_SHADER_PATH) + "GE_P_PS.cso");
	m_pS_Shimmer=GE_Core::getSingleton().createPS(string(GE_SHADER_PATH) + "GE_P_PS_SHIMMER.cso");

	m_perFrame=GE_Core::getSingleton().createConstBuffer(sizeof(PerFrame));
	m_perObject=GE_Core::getSingleton().createConstBuffer(sizeof(PerObject));

}
void GE_ParticelRenderer::render(std::list<GE_Particle *> *i_particleList,GE_View *i_view,ID3D11ShaderResourceView *i_depthMap,ID3D11ShaderResourceView *i_backBufferMAP)
{
	ID3D11ShaderResourceView *srv[4];

	if(!i_particleList->size())
		return;
	GE_Core::getSingleton().getImmediateContext()->OMSetDepthStencilState(GE_Core::getSingleton().getRenderStates().DepthStencilStateDisableZWrite,0);

	m_cPerFrame.ViewProjection.transposeBy(&i_view->m_matViewProjection);
	m_cPerFrame.InverseProjection.transposeBy(&i_view->m_matProjectionInv);
	
	srv[0]=i_depthMap;
	srv[1]=i_backBufferMAP;
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources(2,2,srv);

	GE_Core::getSingleton().getImmediateContext()->UpdateSubresource(m_perFrame,0,NULL,&m_cPerFrame,0,0);
	GE_Core::getSingleton().getImmediateContext()->PSSetConstantBuffers(0,1,&m_perFrame);
	GE_Core::getSingleton().getImmediateContext()->GSSetConstantBuffers(0,1,&m_perFrame);

	GE_Core::getSingleton().getImmediateContext()->PSSetSamplers(0,1,&GE_Core::getSingleton().getRenderStates().SamplerLinearWrap);

	GE_Core::getSingleton().getImmediateContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	GE_Core::getSingleton().getImmediateContext()->IASetInputLayout(m_vertexDeclaration);

	BOOST_FOREACH(GE_Particle *par,*i_particleList)
	{
		m_cPerObject.Softness=par->getSoftness();
		m_cPerObject.Shimmerness=par->getShimmerness();
		m_cPerObject.ViewPosition.x = i_view->m_position.x * par->m_axisMask.x;
		m_cPerObject.ViewPosition.y = i_view->m_position.y * par->m_axisMask.y;
		m_cPerObject.ViewPosition.z = i_view->m_position.z * par->m_axisMask.z;

		GE_Core::getSingleton().getImmediateContext()->OMSetBlendState(par->getBlendState(),0,0xffffffff);
	
		m_cPerObject.World.transposeBy(par->getWorld());
		m_cPerObject.WorldViewProjection.transposeBy(&(*((GE_Mat4x4*)par->getWorld()) * i_view->m_matViewProjection));

		GE_Core::getSingleton().getImmediateContext()->UpdateSubresource(m_perObject,0,NULL,&m_cPerObject,0,0);
		GE_Core::getSingleton().getImmediateContext()->PSSetConstantBuffers(1,1,&m_perObject);
		GE_Core::getSingleton().getImmediateContext()->GSSetConstantBuffers(1,1,&m_perObject);

		srv[0]=par->getInstanceSRV();
		GE_Core::getSingleton().getImmediateContext()->GSSetShaderResources(0,1,srv);

		srv[0]=par->getDiffuseSRV();
		GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources(1,1,srv);

		uint32_t IL=par->getInstanceCount();
		GE_Core::getSingleton().getImmediateContext()->VSSetShader(m_vS,NULL,0);
		
		if(!IL && !m_cPerObject.Shimmerness)
		{
			GE_Core::getSingleton().getImmediateContext()->GSSetShader(m_gS,NULL,0);
			GE_Core::getSingleton().getImmediateContext()->PSSetShader(m_pS,NULL,0);
			par->render();
		}else if(!IL && m_cPerObject.Shimmerness)
		{
			GE_Core::getSingleton().getImmediateContext()->GSSetShader(m_gS,NULL,0);
			GE_Core::getSingleton().getImmediateContext()->PSSetShader(m_pS_Shimmer,NULL,0);
			par->render();
		}else if(IL && !m_cPerObject.Shimmerness)
		{
			GE_Core::getSingleton().getImmediateContext()->GSSetShader(m_gS_Instance,NULL,0);
			GE_Core::getSingleton().getImmediateContext()->PSSetShader(m_pS,NULL,0);
			par->renderInstance();
		}else
		{
			GE_Core::getSingleton().getImmediateContext()->GSSetShader(m_gS_Instance,NULL,0);
			GE_Core::getSingleton().getImmediateContext()->PSSetShader(m_pS_Shimmer,NULL,0);
			par->renderInstance();
		}
		
	}
	
	srv[0]=NULL;
	srv[1]=NULL;
	srv[2]=NULL;
	srv[3]=NULL;
	GE_Core::getSingleton().getImmediateContext()->GSSetShader(NULL,NULL,0);
	GE_Core::getSingleton().getImmediateContext()->PSSetShaderResources(0,4,srv);
	GE_Core::getSingleton().getImmediateContext()->OMSetBlendState(GE_Core::getSingleton().getRenderStates().BlendStateDisableAlpha,0,0xffffffff);
	GE_Core::getSingleton().getImmediateContext()->OMSetDepthStencilState(NULL,0);
}
void GE_ParticelRenderer::destroy()
{
	SAFE_RELEASE(m_perFrame);
	SAFE_RELEASE(m_perObject);
	SAFE_RELEASE(m_vertexDeclaration);
	SAFE_RELEASE(m_vS);
	SAFE_RELEASE(m_gS);
	SAFE_RELEASE(m_gS_Instance);
	SAFE_RELEASE(m_pS);
	SAFE_RELEASE(m_pS_Shimmer);

}

GE_ParticelRenderer::GE_ParticelRenderer()
{
	memset(this, 0, sizeof(GE_ParticelRenderer));
}

GE_ParticelRenderer::~GE_ParticelRenderer()
{
	destroy();
}
