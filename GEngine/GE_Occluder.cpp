#include "GE_Occluder.h"
#include "GE_View.h"
#include "GE_Core.h"
#include "GE_Utility.h"
#include "boost/foreach.hpp"
#include "GE_Frustum.h"
#include "GE_RenderTarget.h"

void GE_Occluder::update()
{
	Radius = max( max( Size.x, Size.y ), Size.z );
	World.srp( Position, Rotation, Size );
}

GE_Occluder::GE_Occluder()
{
	D3D11_QUERY_DESC qDesc;
	qDesc.MiscFlags = 0;
	qDesc.Query = D3D11_QUERY_OCCLUSION;
	GE_Core::getSingleton().getDevice()->CreateQuery( &qDesc, &Query );
	Result = S_OK;

	Position = GE_Vec3( 0.0f, 0.0f, 0.0f );
	Rotation.identity();
	Size = GE_Vec3( GE_ConvertToUnit( 1.0f ), GE_ConvertToUnit( 1.0f ), GE_ConvertToUnit( 1.0f ) );
	NumberOfPixels = 0;
	update();
}

GE_Occluder::~GE_Occluder()
{
	SAFE_RELEASE( Query );
}

GE_OccluderTester::GE_OccluderTester( void )
{
	m_vB = NULL;
	m_iB = NULL;
	m_vS = NULL;
	m_perObject = NULL;
}


void GE_OccluderTester::init()
{
	if( m_vB )
		return;
	m_vS = GE_Core::getSingleton().createVS( string( GE_SHADER_PATH ) + "GE_O_VS.cso" );
	m_perObject = GE_Core::getSingleton().createConstBuffer( sizeof( GE_Mat4x4 ) );
	GE_BoxVertex bv;
	bv.createVertices( GE_Vec3( 1.0f, 1.0f, 1.0f ) );
	m_vB = GE_Core::getSingleton().createVertexBuffer( bv.Vertices.data(), sizeof( GE_Vec3 ), ( uint32_t ) bv.Vertices.size() );
	m_iB = GE_Core::getSingleton().createIndexBuffer( bv.Indices.data(), sizeof( uint32_t ), ( uint32_t ) bv.Indices.size() );
}


GE_OccluderTester::~GE_OccluderTester( void )
{
	SAFE_RELEASE( m_perObject );
	SAFE_RELEASE( m_vB );
	SAFE_RELEASE( m_iB );
	SAFE_RELEASE( m_vS );
	SAFE_RELEASE( m_pS );
}

void GE_OccluderTester::test( std::list< GE_Occluder *> &i_occluders, GE_View * i_view )
{
	BOOST_FOREACH( GE_Occluder *occ, i_occluders )
	{
		if( occ->Result == S_OK )
		{
			if( i_view->m_frustum.sphereInFrustum( &occ->Position, occ->Radius ) == GE_Frustum::OutSide )
			{
				occ->NumberOfPixels = 0;
				continue;
			}

			GE_Mat4x4 transWVP;
			occ->World.multiplyOut( &transWVP, &i_view->m_matViewProjection );
			transWVP.transpose();
			GE_Core::getSingleton().getImmediateContext()->UpdateSubresource( m_perObject, 0, NULL, &transWVP, 0, 0 );
			GE_Core::getSingleton().getImmediateContext()->VSSetConstantBuffers( 0, 1, &m_perObject );
			uint32_t ic = 36;

			GE_Core::getSingleton().getImmediateContext()->Begin( occ->Query );

			GE_Core::getSingleton().renderTriangleListTest( m_vB, sizeof( GE_Vec3 ), m_iB, ic );

			GE_Core::getSingleton().getImmediateContext()->End( occ->Query );

			occ->Result = S_FALSE;

		} else
		{
			uint64_t pix;
			occ->Result = GE_Core::getSingleton().getImmediateContext()->GetData( occ->Query, &pix, sizeof( uint64_t ), 0 );
			if( occ->Result == S_OK )
				occ->NumberOfPixels = pix;
		}
	}

}

void GE_OccluderTester::begin( GE_RenderTarget *i_depthTarget )
{
	begin();
	i_depthTarget->setDepthToDevice();
}

void GE_OccluderTester::begin()
{
	GE_Core::getSingleton().getImmediateContext()->OMSetBlendState( GE_Core::getSingleton().getRenderStates().BlendStateDisableColorWrite, 0, 0xffffffff );
	GE_Core::getSingleton().getImmediateContext()->OMSetDepthStencilState( GE_Core::getSingleton().getRenderStates().DepthStencilStateDisableZWrite, 0 );
	GE_Core::getSingleton().getImmediateContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	GE_Core::getSingleton().setScreenAlignQuadVertexDeclarationToContext();
	GE_Core::getSingleton().getImmediateContext()->VSSetShader( m_vS, NULL, 0 );
	GE_Core::getSingleton().getImmediateContext()->PSSetShader( NULL, NULL, 0 );
	GE_Core::getSingleton().getImmediateContext()->GSSetShader( NULL, NULL, 0 );
}

void GE_OccluderTester::end()
{
	GE_Core::getSingleton().setClearSwapChain( *GE_Core::getSingleton().getCurrentSwapchain(), false, false, false );
	GE_Core::getSingleton().getImmediateContext()->OMSetDepthStencilState( NULL, 0 );
	GE_Core::getSingleton().getImmediateContext()->OMSetBlendState( GE_Core::getSingleton().getRenderStates().BlendStateDisableAlpha, 0, 0xffffffff );
}

GE_OccluderTester & GE_OccluderTester::getSingleton()
{
	return m_singleton;
}

GE_OccluderTester GE_OccluderTester::m_singleton;
