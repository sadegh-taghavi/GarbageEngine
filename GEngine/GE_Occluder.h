#pragma once
#include "GE_Prerequisites.h"
#include "GE_Math.h"

class GE_RenderTarget;
class GE_View;

struct GE_Occluder
{
	friend class GE_OccluderTester;
	float		Radius;
	GE_Vec3		Size;
	GE_Vec3		Position;
	GE_Quat		Rotation;
	GE_Mat4x4	World;
	uint64_t	NumberOfPixels;
	void update();
	GE_Occluder();
	~GE_Occluder();
private:
	HRESULT		Result;
	ID3D11Query	*Query;
		
};

class GE_OccluderTester
{
	ID3D11Buffer		*m_vB;
	ID3D11Buffer		*m_iB;
	ID3D11VertexShader	*m_vS;
	ID3D11PixelShader	*m_pS;
	ID3D11Buffer		*m_perObject;
	
	
	static GE_OccluderTester m_singleton;
	GE_OccluderTester(void);
	~GE_OccluderTester(void);
public:
	static GE_OccluderTester &getSingleton();
	void init();
	void begin( GE_RenderTarget *i_depthTarget );
	void begin();
	void test( std::list< GE_Occluder *> &i_occluders, GE_View * i_view );
	void end();
};

