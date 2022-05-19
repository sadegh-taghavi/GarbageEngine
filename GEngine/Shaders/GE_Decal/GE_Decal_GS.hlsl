#include "GE_Decal.hlsli"

[ maxvertexcount( 3 ) ]
void main( triangle GS_Input Input[ 3 ], inout TriangleStream<GS_Output> StreamOut )
{
	GS_Output Output;
	uint index = Input[ 0 ].InstanceID * 12;
	
	float4 fl4 = m_instances[ index ];
	if ( !fl4.x )
		return;
	Output.MoveUV_MaxAngle.z = fl4.y;
	Output.Bumpiness_Glowness_Roughness_Reflectmask.xy = fl4.zw;
	fl4 = m_instances[ index + 1 ];
	Output.Bumpiness_Glowness_Roughness_Reflectmask.zw = fl4.xy;
	Output.TileUV_Layer_Rotation.zw = fl4.zw;
	fl4 = m_instances[ index + 2 ];
	Output.TileUV_Layer_Rotation.xy = fl4.xy;
	Output.MoveUV_MaxAngle.xy = fl4.zw;
	Output.Color = m_instances[ index + 3 ];

	Output.Bumpiness_Glowness_Roughness_Reflectmask.z *= m_numberOfMips.x;

	float4x4 world = float4x4( m_instances[ index + 4 ], m_instances[ index + 5 ], m_instances[ index + 6 ], m_instances[ index + 7 ] );
	Output.ProjectionR0 = m_instances[ index + 8 ];
	Output.ProjectionR1 = m_instances[ index + 9 ];
	Output.ProjectionR2 = m_instances[ index + 10 ];
	Output.ProjectionR3 = m_instances[ index + 11 ];

	Output.Normal = normalize( mul( float3( 0.0f, 0.0f, 1.0f ), ( float3x3 )world ) );
	
	for( uint i = 0; i < 3; i++ )
	{
		Output.Position = mul( float4( Input[ i ].Position, 1.0f ), mul( world, m_viewProjection ) );
		Output.Texcoord = Output.Position.xyw;
		StreamOut.Append( Output );
	}
	StreamOut.RestartStrip( );
}