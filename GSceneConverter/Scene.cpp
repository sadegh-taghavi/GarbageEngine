#include "Scene.h"
#include "vcacheopt.h"
#include <algorithm>

void GE_SceneManager::unpackScene( const string &i_sceneName )
{
	m_path = i_sceneName;
	m_path = m_path.substr( 0, m_path.find_last_of( '\\' ) + 1 );

	m_sdkManager = FbxManager::Create();
	m_ios = FbxIOSettings::Create( m_sdkManager, IOSROOT );
	m_sdkManager->SetIOSettings( m_ios );
	m_scene = FbxScene::Create( m_sdkManager, "" );
	FbxImporter* lImporter = FbxImporter::Create( m_sdkManager, "" );
	cout << "Active scene: <" << i_sceneName << ">.\n";
	bool lImportStatus = lImporter->Initialize( i_sceneName.data(), -1, m_sdkManager->GetIOSettings() );

	cout << "Loading scene... ";
	if ( !lImportStatus )
	{
		cout << "Error!\n";
		lImporter->Destroy();
		return;
	}
	lImporter->Import( m_scene );
	cout << "done.\n";

	m_animStack = m_scene->GetSrcObject<FbxAnimStack>();
	FbxTimeSpan ts;
	m_scene->GetGlobalSettings().GetTimelineDefaultTimeSpan( ts );
	if ( m_animStack )
		m_animStack->BakeLayers( m_scene->GetAnimationEvaluator(), ts.GetStart(), ts.GetStop(),
		( ts.GetStop() - ts.GetStart() ) / ( uint32_t )FbxGetFrameRate( m_scene->GetGlobalSettings().GetTimeMode() ) );

	FbxNode* lNode = m_scene->GetRootNode();
	if ( lNode )
		dumpNode( lNode );

	lImporter->Destroy();
	m_scene->Destroy();
	m_ios->Destroy();
	m_sdkManager->Destroy();
}

void GE_SceneManager::dumpNode( FbxNode *i_node )
{
	if ( i_node->GetNodeAttribute() )
	{
		string name = i_node->GetName();
		uint32_t firstU = name.find_first_of( '_' );
		string prefix = name.substr( 0, firstU + 1 );
		std::transform( prefix.begin(), prefix.end(), prefix.begin(), ::tolower );
		name = name.substr( firstU + 1, name.size() );

		if ( prefix != BOX && prefix != SPHERE && prefix != CAPSULE && prefix != CONVEX && prefix != TRIANGLE_MESH )
		{
			switch ( i_node->GetNodeAttribute()->GetAttributeType() )
			{
			case FbxNodeAttribute::eMesh:
				dumpMesh( i_node );
				break;
			case FbxNodeAttribute::eCamera:
				dumpCamera( i_node );
				break;
			}
		}
	}
	for ( int32_t i = 0; i < i_node->GetChildCount(); ++i )
		dumpNode( i_node->GetChild( i ) );
}

void GE_SceneManager::dumpMesh( FbxNode *i_node )
{
	FbxMesh* lMesh = i_node->GetMesh();

	m_currentModel = new GS_Model();

	strcpy_s( m_currentModel->m_data.Name, i_node->GetName() );

	cout << "\nActive model: <" << m_currentModel->m_data.Name << ">.\n";
	cout << "Loading model basic data... ";

	FbxAMatrix matAOffset, matAWorld, matAGlobal;
	GetGeometryOffset( i_node, matAOffset );

	matAGlobal = i_node->EvaluateGlobalTransform();
	matAWorld = matAGlobal * matAOffset;

	GE_Mat4x4 matWorld;
	GE_SceneManager::getSingleton().ConvertFBXMatToGEMat( matAWorld, matWorld );
	matWorld.decompose( &m_currentModel->m_data.Position, &m_currentModel->m_data.Rotation, &m_currentModel->m_data.Scale );
	lMesh->GenerateTangentsDataForAllUVSets();
	lMesh->ComputeBBox();
	m_currentModel->m_data.Bound.BoundRadiusMultiply = 1.0f;
	m_currentModel->m_data.Bound.RawBoundBoxMin.x = ( float )lMesh->BBoxMin.Get()[ 0 ];
	m_currentModel->m_data.Bound.RawBoundBoxMin.y = ( float )lMesh->BBoxMin.Get()[ 1 ];
	m_currentModel->m_data.Bound.RawBoundBoxMin.z = ( float )lMesh->BBoxMin.Get()[ 2 ];

	m_currentModel->m_data.Bound.RawBoundBoxMax.x = ( float )lMesh->BBoxMax.Get()[ 0 ];
	m_currentModel->m_data.Bound.RawBoundBoxMax.y = ( float )lMesh->BBoxMax.Get()[ 1 ];
	m_currentModel->m_data.Bound.RawBoundBoxMax.z = ( float )lMesh->BBoxMax.Get()[ 2 ];

	m_currentModel->m_data.Bound.RawBoundCenter = ( m_currentModel->m_data.Bound.RawBoundBoxMax + m_currentModel->m_data.Bound.RawBoundBoxMin ) * 0.5f;
	m_currentModel->m_data.Bound.RawBoundRadius = ( m_currentModel->m_data.Bound.RawBoundBoxMax - m_currentModel->m_data.Bound.RawBoundBoxMin ).length() * 0.5f;

	m_currentModel->m_data.Bound.transform( &matWorld );

	int32_t startFrame = INT_MAX;
	int32_t	endFrame = 0;

	FbxAnimCurve *animCurve = NULL;
	if ( m_animStack )
		animCurve = i_node->LclTranslation.GetCurve( m_animStack->GetMember<FbxAnimLayer>() );
	if ( animCurve )
	{
		for ( int32_t keyIndex = 0; keyIndex < animCurve->KeyGetCount(); ++keyIndex )
		{
			int32_t count = ( int32_t )( animCurve->KeyGetTime( keyIndex ).GetFrameCount( m_scene->GetGlobalSettings().GetTimeMode() ) );
			if ( count > endFrame )
				endFrame = count;
			if ( count < startFrame )
				startFrame = count;
		}
	}
	animCurve = NULL;
	if ( m_animStack )
		animCurve = i_node->LclRotation.GetCurve( m_animStack->GetMember<FbxAnimLayer>() );
	if ( animCurve )
	{
		for ( int32_t keyIndex = 0; keyIndex < animCurve->KeyGetCount(); ++keyIndex )
		{
			int32_t count = ( int32_t )( animCurve->KeyGetTime( keyIndex ).GetFrameCount( m_scene->GetGlobalSettings().GetTimeMode() ) );
			if ( count > endFrame )
				endFrame = count;
			if ( count < startFrame )
				startFrame = count;
		}
	}
	animCurve = NULL;
	if ( m_animStack )
		animCurve = i_node->LclScaling.GetCurve( m_animStack->GetMember<FbxAnimLayer>() );
	if ( animCurve )
	{
		for ( int32_t keyIndex = 0; keyIndex < animCurve->KeyGetCount(); ++keyIndex )
		{
			int32_t count = ( int32_t )( animCurve->KeyGetTime( keyIndex ).GetFrameCount( m_scene->GetGlobalSettings().GetTimeMode() ) );
			if ( count > endFrame )
				endFrame = count;
			if ( count < startFrame )
				startFrame = count;
		}
	}
	if ( startFrame != INT_MAX )
	{
		m_currentModel->m_transformData.NumberOfFrames = endFrame - startFrame;
		m_currentModel->m_transformFrames.resize( m_currentModel->m_transformData.NumberOfFrames );

		uint32_t frame = 0;
		for ( int32_t frameIndex = startFrame; frameIndex < endFrame; ++frameIndex )
		{
			FbxTime time;
			time.SetFrame( frameIndex, m_scene->GetGlobalSettings().GetTimeMode() );

			matAWorld = m_scene->GetAnimationEvaluator()->GetNodeGlobalTransform( i_node, time ) * matAOffset;

			GE_SceneManager::getSingleton().ConvertFBXMatToGEMat( matAWorld, m_currentModel->m_transformFrames[ frame ].Matrix );

			m_currentModel->m_transformFrames[ frame ].Matrix.decompose( &m_currentModel->m_transformFrames[ frame ].Position,
				&m_currentModel->m_transformFrames[ frame ].Rotation, &m_currentModel->m_transformFrames[ frame ].Scale );
			++frame;
		}
	}
	cout << "done.\n";

	cout << "Loading complex bounds data... \n";
	FbxAMatrix matAParWorld = matAWorld;
	matAParWorld = matAParWorld.Inverse();
	for ( int32_t i = 0; i < i_node->GetChildCount(); ++i )
	{
		FbxNode *child = i_node->GetChild( i );
		string name = child->GetName();
		uint32_t firstU = name.find_first_of( '_' );
		string prefix = name.substr( 0, firstU + 1 );
		std::transform( prefix.begin(), prefix.end(), prefix.begin(), ::tolower );
		name = name.substr( firstU + 1, name.size() );

		FbxMesh *cMesh = child->GetMesh();
		if ( !cMesh )
			continue;

		cMesh->ComputeBBox();
		GetGeometryOffset( child, matAOffset );

		if ( prefix == BOX || prefix == SPHERE )
			matAWorld = matAParWorld * child->EvaluateGlobalTransform();
		else if ( prefix == CAPSULE )
		{
			FbxAMatrix matA90, matAGlobal, matALocal, matALocalInv;

			GE_SceneManager::getSingleton().ConvertGEMatToFBXMat( GE_Mat4x4().RotationQuaternion( &GE_Quat().rotationAxis( &GE_Vec3( 0.0f, 1.0f, 0.0f ),
				GE_ConvertToRadians( 90.0f ) ) ), matA90 );
			matALocalInv = matALocal = child->EvaluateLocalTransform();
			matAGlobal = child->EvaluateGlobalTransform() * matALocalInv.Inverse();

			matAGlobal = matAGlobal * ( matALocal * matA90 );

			matAWorld = matAParWorld * matAGlobal;
		}
		else
			matAWorld = matAParWorld * ( child->EvaluateGlobalTransform() * matAOffset );



		GE_SceneManager::getSingleton().ConvertFBXMatToGEMat( matAWorld, matWorld );

		GE_Vec3 preScale;

		if ( prefix == BOX )
		{
			CBBox box;
			strcpy_s( box.Name, name.c_str() );
			matWorld.decompose( &box.Position, &box.Rotation, &preScale );

			box.HalfDimension.x = ( float )( cMesh->BBoxMax.Get()[ 0 ] - cMesh->BBoxMin.Get()[ 0 ] ) * 0.5f * preScale.x;
			box.HalfDimension.y = ( float )( cMesh->BBoxMax.Get()[ 1 ] - cMesh->BBoxMin.Get()[ 1 ] ) * 0.5f * preScale.y;
			box.HalfDimension.z = ( float )( cMesh->BBoxMax.Get()[ 2 ] - cMesh->BBoxMin.Get()[ 2 ] ) * 0.5f * preScale.z;

			m_currentModel->m_cBBoxes.push_back( box );
		}
		else if ( prefix == SPHERE )
		{
			CBSphere sphere;
			strcpy_s( sphere.Name, name.c_str() );
			matWorld.decompose( &sphere.Position, &sphere.Rotation, &preScale );

			sphere.Radius = ( float )max( max( ( cMesh->BBoxMax.Get()[ 0 ] - cMesh->BBoxMin.Get()[ 0 ] ) * preScale.x,
				( cMesh->BBoxMax.Get()[ 1 ] - cMesh->BBoxMin.Get()[ 1 ] ) * preScale.y ),
				( cMesh->BBoxMax.Get()[ 2 ] - cMesh->BBoxMin.Get()[ 2 ] ) * preScale.z ) * 0.5f;

			m_currentModel->m_cBSpheres.push_back( sphere );
		}
		else if ( prefix == CAPSULE )
		{
			CBCapsule capsule;
			strcpy_s( capsule.Name, name.c_str() );
			matWorld.decompose( &capsule.Position, &capsule.Rotation, &preScale );

			capsule.Radius = ( float )( cMesh->BBoxMax.Get()[ 0 ] - cMesh->BBoxMin.Get()[ 0 ] ) * 0.5f * preScale.x;
			capsule.HalfHeight = ( float )( cMesh->BBoxMax.Get()[ 2 ] - cMesh->BBoxMin.Get()[ 2 ] ) * 0.5f * preScale.z;

			m_currentModel->m_cBCapsules.push_back( capsule );
		}
		else if ( prefix == CONVEX )
		{
			ConvexData convex;
			vector< GE_Vec3 > vertices;
			strcpy_s( convex.Convex.Name, name.c_str() );
			matWorld.decompose( &convex.Convex.Position, &convex.Convex.Rotation, &preScale );

			FbxVector4* lControlPoints = cMesh->GetControlPoints();
			for ( int32_t i = 0; i < cMesh->GetPolygonCount(); ++i )
			{
				for ( int32_t j = 0; j < 3; ++j )
				{
					int32_t lControlPointIndex = cMesh->GetPolygonVertex( i, j );
					GE_Vec3 p;
					p.x = ( float )lControlPoints[ lControlPointIndex ][ 0 ] * preScale.x;
					p.y = ( float )lControlPoints[ lControlPointIndex ][ 1 ] * preScale.y;
					p.z = ( float )lControlPoints[ lControlPointIndex ][ 2 ] * preScale.z;
					vertices.push_back( p );
				}
			}

			for ( uint32_t i = 0; i < vertices.size(); ++i )
			{
				int32_t vertFound = -1;
				for ( uint32_t cI = 0; cI < convex.Vertices.size(); ++cI )
				{
					if ( vertices[ i ] == convex.Vertices[ cI ] )
					{
						vertFound = cI;
						convex.Indices.push_back( cI );
						break;
					}
				}

				if ( vertFound < 0 )
				{
					convex.Indices.push_back( convex.Vertices.size() );
					convex.Vertices.push_back( vertices[ i ] );
				}
			}

			convex.Convex.NumberOfIndices = convex.Indices.size();
			convex.Convex.NumberOfVertices = convex.Vertices.size();

			m_currentModel->m_cBConvexesData.push_back( convex );

		}
		else if ( prefix == TRIANGLE_MESH )
		{
			TriangleMeshData triangleMesh;
			vector< GE_Vec3 > vertices;
			strcpy_s( triangleMesh.TriangleMesh.Name, name.c_str() );
			matWorld.decompose( &triangleMesh.TriangleMesh.Position, &triangleMesh.TriangleMesh.Rotation, &preScale );

			FbxVector4* lControlPoints = cMesh->GetControlPoints();
			for ( int32_t i = 0; i < cMesh->GetPolygonCount(); ++i )
			{
				for ( int32_t j = 0; j < 3; ++j )
				{
					int32_t lControlPointIndex = cMesh->GetPolygonVertex( i, j );
					GE_Vec3 p;
					p.x = ( float )lControlPoints[ lControlPointIndex ][ 0 ] * preScale.x;
					p.y = ( float )lControlPoints[ lControlPointIndex ][ 1 ] * preScale.y;
					p.z = ( float )lControlPoints[ lControlPointIndex ][ 2 ] * preScale.z;
					vertices.push_back( p );
				}
			}

			for ( uint32_t i = 0; i < vertices.size(); ++i )
			{
				int32_t vertFound = -1;
				for ( uint32_t cI = 0; cI < triangleMesh.Vertices.size(); ++cI )
				{
					if ( vertices[ i ] == triangleMesh.Vertices[ cI ] )
					{
						vertFound = cI;
						triangleMesh.Indices.push_back( cI );
						break;
					}
				}

				if ( vertFound < 0 )
				{
					triangleMesh.Indices.push_back( triangleMesh.Vertices.size() );
					triangleMesh.Vertices.push_back( vertices[ i ] );
				}
			}

			triangleMesh.TriangleMesh.NumberOfIndices = triangleMesh.Indices.size();
			triangleMesh.TriangleMesh.NumberOfVertices = triangleMesh.Vertices.size();

			m_currentModel->m_cBTriangleMeshesData.push_back( triangleMesh );
		}
	}
	m_currentModel->m_complexBound.NumberOfBoxes = m_currentModel->m_cBBoxes.size();
	cout << "Boxes {";
	for ( uint32_t i = 0; i < m_currentModel->m_complexBound.NumberOfBoxes; ++i )
	{
		cout << " " << m_currentModel->m_cBBoxes[ i ].Name;
		if ( i != m_currentModel->m_complexBound.NumberOfBoxes - 1 )
			cout << " ,";
	}
	cout << " }\n";

	m_currentModel->m_complexBound.NumberOfSpheres = m_currentModel->m_cBSpheres.size();
	cout << "Spheres {";
	for ( uint32_t i = 0; i < m_currentModel->m_complexBound.NumberOfSpheres; ++i )
	{
		cout << " " << m_currentModel->m_cBSpheres[ i ].Name;
		if ( i != m_currentModel->m_complexBound.NumberOfSpheres - 1 )
			cout << " ,";
	}
	cout << " }\n";

	m_currentModel->m_complexBound.NumberOfCapsules = m_currentModel->m_cBCapsules.size();
	cout << "Capsules {";
	for ( uint32_t i = 0; i < m_currentModel->m_complexBound.NumberOfCapsules; ++i )
	{
		cout << " " << m_currentModel->m_cBCapsules[ i ].Name;
		if ( i != m_currentModel->m_complexBound.NumberOfCapsules - 1 )
			cout << " ,";
	}
	cout << " }\n";

	m_currentModel->m_complexBound.NumberOfConvexes = m_currentModel->m_cBConvexesData.size();
	cout << "Convexes {";
	for ( uint32_t i = 0; i < m_currentModel->m_complexBound.NumberOfConvexes; ++i )
	{
		cout << " " << m_currentModel->m_cBConvexesData[ i ].Convex.Name;
		if ( i != m_currentModel->m_complexBound.NumberOfConvexes - 1 )
			cout << " ,";
	}
	cout << " }\n";

	m_currentModel->m_complexBound.NumberOfTriangleMeshes = m_currentModel->m_cBTriangleMeshesData.size();
	cout << "Triangle meshes {";
	for ( uint32_t i = 0; i < m_currentModel->m_complexBound.NumberOfTriangleMeshes; ++i )
	{
		cout << " " << m_currentModel->m_cBTriangleMeshesData[ i ].TriangleMesh.Name;
		if ( i != m_currentModel->m_complexBound.NumberOfTriangleMeshes - 1 )
			cout << " ,";
	}
	cout << " }\n";

	cout << "done.\n";

	cout << "Loading model polygons data... ";
	dumpPolygon( lMesh );
	cout << "done.\n";

	cout << "Loading model morph data... ";
	dumpShape( lMesh );
	cout << "done.\n";

	cout << "Loading model skeleton data... ";
	dumpLink( lMesh );
	cout << "done.\n";

	cout << "Optimizing model subsets...\n";
	m_currentModel->fillSubsets();
	cout << "done.\n";

	string fileName = m_path + m_currentModel->m_data.Name + ".GModel";
	cout << "Saving model mesh to <" << fileName << ">... ";
	FILE *fp;
	fopen_s( &fp, fileName.c_str(), "wb" );
	fwrite( &m_currentModel->m_data, sizeof( ModelData ), 1, fp );

	fwrite( &m_currentModel->m_complexBound, sizeof( ComplexBound ), 1, fp );

	if ( m_currentModel->m_cBBoxes.size() )
		fwrite( m_currentModel->m_cBBoxes.data(), sizeof( CBBox ), m_currentModel->m_cBBoxes.size(), fp );

	if ( m_currentModel->m_cBSpheres.size() )
		fwrite( m_currentModel->m_cBSpheres.data(), sizeof( CBSphere ), m_currentModel->m_cBSpheres.size(), fp );

	if ( m_currentModel->m_cBCapsules.size() )
		fwrite( m_currentModel->m_cBCapsules.data(), sizeof( CBCapsule ), m_currentModel->m_cBCapsules.size(), fp );

	if ( m_currentModel->m_cBConvexesData.size() )
	{
		for ( uint32_t cbIndex = 0; cbIndex < m_currentModel->m_cBConvexesData.size(); ++cbIndex )
		{
			fwrite( &m_currentModel->m_cBConvexesData[ cbIndex ].Convex, sizeof( CBConvex ), 1, fp );
			fwrite( m_currentModel->m_cBConvexesData[ cbIndex ].Vertices.data(), sizeof( GE_Vec3 ), m_currentModel->m_cBConvexesData[ cbIndex ].Vertices.size(), fp );
			fwrite( m_currentModel->m_cBConvexesData[ cbIndex ].Indices.data(), sizeof( uint32_t ), m_currentModel->m_cBConvexesData[ cbIndex ].Indices.size(), fp );
		}
	}

	if ( m_currentModel->m_cBTriangleMeshesData.size() )
	{
		for ( uint32_t cbIndex = 0; cbIndex < m_currentModel->m_cBTriangleMeshesData.size(); ++cbIndex )
		{
			fwrite( &m_currentModel->m_cBTriangleMeshesData[ cbIndex ].TriangleMesh, sizeof( CBTriangleMesh ), 1, fp );
			fwrite( m_currentModel->m_cBTriangleMeshesData[ cbIndex ].Vertices.data(), sizeof( GE_Vec3 ),
				m_currentModel->m_cBTriangleMeshesData[ cbIndex ].Vertices.size(), fp );
			fwrite( m_currentModel->m_cBTriangleMeshesData[ cbIndex ].Indices.data(), sizeof( uint32_t ),
				m_currentModel->m_cBTriangleMeshesData[ cbIndex ].Indices.size(), fp );
		}
	}

	for ( uint32_t i = 0; i < m_currentModel->m_subsetsData.size(); ++i )
	{
		ModelSubset ms;
		ms.NumberOfIndices = m_currentModel->m_subsetsData[ i ].Indices.size();
		ms.NumberOfVertices = m_currentModel->m_subsetsData[ i ].VertexBufferData.size();
		fwrite( &ms, sizeof( ModelSubset ), 1, fp );
		fwrite( m_currentModel->m_subsetsData[ i ].Indices.data(), sizeof( uint32_t ), m_currentModel->m_subsetsData[ i ].Indices.size(), fp );
		fwrite( m_currentModel->m_subsetsData[ i ].VertexBufferData.data(), sizeof( VertexBuffer ), m_currentModel->m_subsetsData[ i ].VertexBufferData.size(), fp );
		if ( m_currentModel->m_subsetsData[ i ].VerticesDataTexcoord.size() )
			fwrite( m_currentModel->m_subsetsData[ i ].VerticesDataTexcoord.data(), sizeof( GE_Vec4 ), m_currentModel->m_subsetsData[ i ].VerticesDataTexcoord.size(), fp );
		if ( m_currentModel->m_subsetsData[ i ].VerticesDataColor.size() )
			fwrite( m_currentModel->m_subsetsData[ i ].VerticesDataColor.data(), sizeof( VertexDataColor ), m_currentModel->m_subsetsData[ i ].VerticesDataColor.size(), fp );
		if ( m_currentModel->m_subsetsData[ i ].VerticesDataBoneIndex.size() )
		{
			fwrite( m_currentModel->m_subsetsData[ i ].VerticesDataBoneIndex.data(), sizeof( VertexDataBoneIndex ), m_currentModel->m_subsetsData[ i ].VerticesDataBoneIndex.size(), fp );
			fwrite( m_currentModel->m_subsetsData[ i ].VerticesDataBoneWeight.data(), sizeof( VertexDataBoneWeight ), m_currentModel->m_subsetsData[ i ].VerticesDataBoneWeight.size(), fp );
		}
	}
	fclose( fp );
	cout << "done.\n";

	if ( m_currentModel->m_transformData.NumberOfFrames )
	{
		fileName = m_path + m_currentModel->m_data.Name + ".GMTransform";
		cout << "Saving model transform to <" << fileName << ">... ";
		FILE *fp;
		fopen_s( &fp, fileName.c_str(), "wb" );

		fwrite( &m_currentModel->m_transformData, sizeof( TransformData ), 1, fp );
		fwrite( m_currentModel->m_transformFrames.data(), sizeof( Transform ), m_currentModel->m_transformFrames.size(), fp );

		fclose( fp );
		cout << "done.\n";
	}

	if ( m_currentModel->m_morphData.NumberOfMorphTargets )
	{
		fileName = m_path + m_currentModel->m_data.Name + ".GMMorph";
		cout << "Saving model morph to <" << fileName << ">... ";
		FILE *fp;
		fopen_s( &fp, fileName.c_str(), "wb" );

		fwrite( &m_currentModel->m_morphData, sizeof( MorphData ), 1, fp );

		for ( uint32_t targetIndex = 0; targetIndex < m_currentModel->m_morphData.NumberOfMorphTargets; ++targetIndex )
		{
			MorphTarget mt;
			strcpy_s( mt.Name, m_currentModel->m_morphChannels[ targetIndex ]->GetTargetShape( 0 )->GetName() );
			fwrite( &mt, sizeof( MorphTarget ), 1, fp );
		}

		for ( uint32_t i = 0; i < m_currentModel->m_subsetsData.size(); ++i )
		{
			fwrite( m_currentModel->m_subsetsData[ i ].VertexDataMorphTarget.data(), sizeof( MorphTargetVertex ),
				m_currentModel->m_subsetsData[ i ].VertexDataMorphTarget.size(), fp );
		}

		if ( m_currentModel->m_morphFrames.size() )
			fwrite( m_currentModel->m_morphFrames.data(), sizeof( float ), m_currentModel->m_morphFrames.size(), fp );

		fclose( fp );
		cout << "done.\n";
	}

	if ( m_currentModel->m_bones.size() )
	{
		fileName = m_path + m_currentModel->m_data.Name + ".GMBone";
		cout << "Saving model bone to <" << fileName << ">... ";
		FILE *fp;
		fopen_s( &fp, fileName.c_str(), "wb" );

		fwrite( &m_currentModel->m_boneData, sizeof( BoneData ), 1, fp );
		fwrite( m_currentModel->m_bones.data(), sizeof( Bone ), m_currentModel->m_bones.size(), fp );
		if ( m_currentModel->m_boneFrames.size() )
			fwrite( m_currentModel->m_boneFrames.data(), sizeof( GE_Mat4x4 ), m_currentModel->m_boneFrames.size(), fp );

		fclose( fp );
		cout << "done.\n";
	}
	SAFE_DELETE( m_currentModel );
}

void GE_SceneManager::dumpPolygon( FbxMesh *i_mesh )
{
	FbxVector4* lControlPoints = i_mesh->GetControlPoints();
	m_currentModel->m_indexAt.resize( i_mesh->GetControlPointsCount() );
	int32_t vertexId = 0;
	for ( int32_t i = 0; i < i_mesh->GetPolygonCount(); ++i )
	{
		string matID = "";
		for ( int32_t matIndex = 0; matIndex < i_mesh->GetElementMaterialCount(); ++matIndex )
		{
			char idstr[ 5 ];
			_itoa_s( matIndex, idstr, 10 );
			matID += idstr;
			_itoa_s( i_mesh->GetElementMaterial( matIndex )->GetIndexArray().GetAt( i ), idstr, 10 );
			matID += idstr;
		}

		int32_t matIDFound = -1;
		for ( uint32_t findID = 0; findID < m_currentModel->m_materialindexAt.size(); ++findID )
		{
			if ( m_currentModel->m_materialindexAt[ findID ].Name == matID )
				matIDFound = findID;
		}
		if ( matIDFound < 0 )
		{
			materialIndexAt mia;
			mia.Name = matID;
			m_currentModel->m_materialindexAt.push_back( mia );
			matIDFound = m_currentModel->m_materialindexAt.size() - 1;
		}

		for ( int32_t j = 0; j < 3; ++j )
		{
			m_currentModel->m_materialindexAt[ matIDFound ].Vertices.push_back( m_currentModel->m_vPositions.size() );

			int32_t lControlPointIndex = i_mesh->GetPolygonVertex( i, j );
			GE_Vec3 p;
			p.x = ( float )lControlPoints[ lControlPointIndex ][ 0 ];
			p.y = ( float )lControlPoints[ lControlPointIndex ][ 1 ];
			p.z = ( float )lControlPoints[ lControlPointIndex ][ 2 ];
			m_currentModel->m_vPositions.push_back( p );
			m_currentModel->m_posAt.push_back( lControlPointIndex );
			m_currentModel->m_indexAt[ lControlPointIndex ].push_back( vertexId );

			for ( int32_t l = 0; l < i_mesh->GetElementVertexColorCount(); ++l )
			{
				FbxGeometryElementVertexColor* leVtxc = i_mesh->GetElementVertexColor( l );
				switch ( leVtxc->GetMappingMode() )
				{
				case FbxGeometryElement::eByControlPoint:
					switch ( leVtxc->GetReferenceMode() )
					{
					case FbxGeometryElement::eDirect:
					{
														FbxColor c = leVtxc->GetDirectArray().GetAt( lControlPointIndex );
														GE_Vec4 co;
														co.x = ( float )c.mRed;
														co.y = ( float )c.mGreen;
														co.z = ( float )c.mBlue;
														co.w = ( float )c.mAlpha;
														m_currentModel->m_vColors.push_back( co );
					}
						break;
					case FbxGeometryElement::eIndexToDirect:
					{
															   int32_t id = leVtxc->GetIndexArray().GetAt( lControlPointIndex );
															   FbxColor c = leVtxc->GetDirectArray().GetAt( id );
															   GE_Vec4 co;
															   co.x = ( float )c.mRed;
															   co.y = ( float )c.mGreen;
															   co.z = ( float )c.mBlue;
															   co.w = ( float )c.mAlpha;
															   m_currentModel->m_vColors.push_back( co );
					}
						break;
					default:
						break;
					}
					break;

				case FbxGeometryElement::eByPolygonVertex:
				{
															 switch ( leVtxc->GetReferenceMode() )
															 {
															 case FbxGeometryElement::eDirect:
															 {
																								 FbxColor c = leVtxc->GetDirectArray().GetAt( vertexId );
																								 GE_Vec4 co;
																								 co.x = ( float )c.mRed;
																								 co.y = ( float )c.mGreen;
																								 co.z = ( float )c.mBlue;
																								 co.w = ( float )c.mAlpha;
																								 m_currentModel->m_vColors.push_back( co );
															 }
																 break;
															 case FbxGeometryElement::eIndexToDirect:
															 {
																										int32_t id = leVtxc->GetIndexArray().GetAt( vertexId );
																										FbxColor c = leVtxc->GetDirectArray().GetAt( id );
																										GE_Vec4 co;
																										co.x = ( float )c.mRed;
																										co.y = ( float )c.mGreen;
																										co.z = ( float )c.mBlue;
																										co.w = ( float )c.mAlpha;
																										m_currentModel->m_vColors.push_back( co );
															 }
																 break;
															 default:
																 break;
															 }
				}
					break;
				}
			}

			for ( int32_t l = 0; l < i_mesh->GetElementUVCount() && l < 2; ++l )
			{
				FbxGeometryElementUV* leUV = i_mesh->GetElementUV( l );

				switch ( leUV->GetMappingMode() )
				{
				case FbxGeometryElement::eByControlPoint:
					switch ( leUV->GetReferenceMode() )
					{
					case FbxGeometryElement::eDirect:
					{
														FbxVector2 uv = leUV->GetDirectArray().GetAt( lControlPointIndex );
														GE_Vec2 tc;
														tc.x = ( float )uv[ 0 ];
														tc.y = 1.0f - ( float )uv[ 1 ];
														if ( l == 0 )
															m_currentModel->m_vTexcoords.push_back( tc );
														else
															m_currentModel->m_vAdditionalTexcoords.push_back( tc );
					}
						break;
					case FbxGeometryElement::eIndexToDirect:
					{
															   int32_t id = leUV->GetIndexArray().GetAt( lControlPointIndex );
															   FbxVector2 uv = leUV->GetDirectArray().GetAt( id );
															   GE_Vec2 tc;
															   tc.x = ( float )uv[ 0 ];
															   tc.y = 1.0f - ( float )uv[ 1 ];
															   if ( l == 0 )
																   m_currentModel->m_vTexcoords.push_back( tc );
															   else
																   m_currentModel->m_vAdditionalTexcoords.push_back( tc );
					}
						break;
					default:
						break;
					}
					break;

				case FbxGeometryElement::eByPolygonVertex:
				{
															 FbxStringList ls;
															 i_mesh->GetUVSetNames( ls );

															 switch ( leUV->GetReferenceMode() )
															 {
															 case FbxGeometryElement::eDirect:
															 case FbxGeometryElement::eIndexToDirect:
															 {
																										FbxVector2 uv;
																										bool map;

																										i_mesh->GetPolygonVertexUV( i, j, ls[ l ], uv, map );
																										GE_Vec2 tc;
																										tc.x = ( float )uv[ 0 ];
																										tc.y = 1.0f - ( float )uv[ 1 ];
																										if ( l == 0 )
																											m_currentModel->m_vTexcoords.push_back( tc );
																										else
																											m_currentModel->m_vAdditionalTexcoords.push_back( tc );
															 }
																 break;
															 default:
																 break;
															 }
				}
					break;
				}
			}

			for ( int32_t l = 0; l < i_mesh->GetElementNormalCount(); ++l )
			{
				FbxGeometryElementNormal* leVtxn = i_mesh->GetElementNormal( l );
				switch ( leVtxn->GetMappingMode() )
				{
				case FbxGeometryElement::eByControlPoint:
					switch ( leVtxn->GetReferenceMode() )
					{
					case FbxGeometryElement::eDirect:
					{
														FbxVector4 n = leVtxn->GetDirectArray().GetAt( lControlPointIndex );
														GE_Vec3 no;
														no.x = ( float )n[ 0 ];
														no.y = ( float )n[ 1 ];
														no.z = ( float )n[ 2 ];
														m_currentModel->m_vNormals.push_back( no );
														m_currentModel->m_normalAt.push_back( lControlPointIndex );
					}
						break;
					case FbxGeometryElement::eIndexToDirect:
					{
															   int32_t id = leVtxn->GetIndexArray().GetAt( lControlPointIndex );
															   FbxVector4 n = leVtxn->GetDirectArray().GetAt( id );
															   GE_Vec3 no;
															   no.x = ( float )n[ 0 ];
															   no.y = ( float )n[ 1 ];
															   no.z = ( float )n[ 2 ];
															   m_currentModel->m_vNormals.push_back( no );
															   m_currentModel->m_normalAt.push_back( id );
					}
						break;
					default:
						break;
					}
					break;

				case FbxGeometryElement::eByPolygonVertex:
				{
															 switch ( leVtxn->GetReferenceMode() )
															 {
															 case FbxGeometryElement::eDirect:
															 {
																								 FbxVector4 n = leVtxn->GetDirectArray().GetAt( vertexId );
																								 GE_Vec3 no;
																								 no.x = ( float )n[ 0 ];
																								 no.y = ( float )n[ 1 ];
																								 no.z = ( float )n[ 2 ];
																								 m_currentModel->m_vNormals.push_back( no );
																								 m_currentModel->m_normalAt.push_back( vertexId );
															 }
																 break;
															 case FbxGeometryElement::eIndexToDirect:
															 {
																										int32_t id = leVtxn->GetIndexArray().GetAt( vertexId );
																										FbxVector4 n = leVtxn->GetDirectArray().GetAt( id );
																										GE_Vec3 no;
																										no.x = ( float )n[ 0 ];
																										no.y = ( float )n[ 1 ];
																										no.z = ( float )n[ 2 ];
																										m_currentModel->m_vNormals.push_back( no );
																										m_currentModel->m_normalAt.push_back( id );
															 }
																 break;
															 default:
																 break;
															 }
				}
					break;
				}
			}

			for ( int32_t l = 0; l < i_mesh->GetElementTangentCount(); ++l )
			{
				FbxGeometryElementTangent* leVtxt = i_mesh->GetElementTangent( l );
				switch ( leVtxt->GetMappingMode() )
				{
				case FbxGeometryElement::eByControlPoint:
					switch ( leVtxt->GetReferenceMode() )
					{
					case FbxGeometryElement::eDirect:
					{
														FbxVector4 t = leVtxt->GetDirectArray().GetAt( lControlPointIndex );
														GE_Vec4 tn;
														tn.x = ( float )t[ 0 ];
														tn.y = ( float )t[ 1 ];
														tn.z = ( float )t[ 2 ];
														tn.w = ( float )t[ 3 ];
														if ( tn.w == 0.0f )
															tn.w = 1.0f;
														m_currentModel->m_vTangents.push_back( tn );
														m_currentModel->m_tangentAt.push_back( lControlPointIndex );
					}
						break;
					case FbxGeometryElement::eIndexToDirect:
					{
															   int32_t id = leVtxt->GetIndexArray().GetAt( lControlPointIndex );
															   FbxVector4 t = leVtxt->GetDirectArray().GetAt( id );
															   GE_Vec4 tn;
															   tn.x = ( float )t[ 0 ];
															   tn.y = ( float )t[ 1 ];
															   tn.z = ( float )t[ 2 ];
															   tn.w = ( float )t[ 3 ];
															   if ( tn.w == 0.0f )
																   tn.w = 1.0f;
															   m_currentModel->m_vTangents.push_back( tn );
															   m_currentModel->m_tangentAt.push_back( id );
					}
						break;
					default:
						break;
					}
					break;

				case FbxGeometryElement::eByPolygonVertex:
				{
															 switch ( leVtxt->GetReferenceMode() )
															 {
															 case FbxGeometryElement::eDirect:
															 {
																								 FbxVector4 t = leVtxt->GetDirectArray().GetAt( vertexId );
																								 GE_Vec4 tn;
																								 tn.x = ( float )t[ 0 ];
																								 tn.y = ( float )t[ 1 ];
																								 tn.z = ( float )t[ 2 ];
																								 tn.w = ( float )t[ 3 ];
																								 if ( tn.w == 0.0f )
																									 tn.w = 1.0f;
																								 m_currentModel->m_vTangents.push_back( tn );
																								 m_currentModel->m_tangentAt.push_back( vertexId );
															 }
																 break;
															 case FbxGeometryElement::eIndexToDirect:
															 {
																										int32_t id = leVtxt->GetIndexArray().GetAt( vertexId );
																										FbxVector4 t = leVtxt->GetDirectArray().GetAt( id );
																										GE_Vec4 tn;
																										tn.x = ( float )t[ 0 ];
																										tn.y = ( float )t[ 1 ];
																										tn.z = ( float )t[ 2 ];
																										tn.w = ( float )t[ 3 ];
																										if ( tn.w == 0.0f )
																											tn.w = 1.0f;
																										m_currentModel->m_vTangents.push_back( tn );
																										m_currentModel->m_tangentAt.push_back( id );
															 }
																 break;
															 default:
																 break;
															 }
				}
					break;
				}

			}
			++vertexId;
		}
	}
}

void GE_SceneManager::dumpShape( FbxGeometry *i_geometry )
{
	//use first morph modifier

	int32_t startFrame = INT_MAX;
	int32_t endFrame = 0;

	if ( i_geometry->GetDeformerCount( FbxDeformer::eBlendShape ) )
	{
		FbxBlendShape* lBlendShape = ( FbxBlendShape* )i_geometry->GetDeformer( 0, FbxDeformer::eBlendShape );
		m_currentModel->m_vMorphPositions.resize( m_currentModel->m_posAt.size() * lBlendShape->GetBlendShapeChannelCount() );
		m_currentModel->m_vMorphNormals.resize( m_currentModel->m_posAt.size() * lBlendShape->GetBlendShapeChannelCount() );
		m_currentModel->m_vMorphTangents.resize( m_currentModel->m_posAt.size() * lBlendShape->GetBlendShapeChannelCount() );
		for ( int32_t lBlendShapeChannelIndex = 0; lBlendShapeChannelIndex < lBlendShape->GetBlendShapeChannelCount(); ++lBlendShapeChannelIndex )
		{

			FbxBlendShapeChannel* lBlendShapeChannel = lBlendShape->GetBlendShapeChannel( lBlendShapeChannelIndex );

			if ( !lBlendShapeChannel->GetTargetShapeCount() )
				continue;

			m_currentModel->m_morphChannels.push_back( lBlendShapeChannel );

			FbxAnimCurve *animCurve = NULL;
			if ( m_animStack )
				animCurve = lBlendShapeChannel->GetFirstProperty().GetCurve( m_animStack->GetMember<FbxAnimLayer>() );
			if ( animCurve )
			{
				for ( int32_t keyIndex = 0; keyIndex < animCurve->KeyGetCount(); ++keyIndex )
				{
					int32_t count = ( int32_t )( animCurve->KeyGetTime( keyIndex ).GetFrameCount( m_scene->GetGlobalSettings().GetTimeMode() ) );
					if ( count > endFrame )
						endFrame = count;
					if ( count < startFrame )
						startFrame = count;
				}
			}

			FbxShape* lShape = lBlendShapeChannel->GetTargetShape( 0 );

			FbxLayerElementArrayTemplate<FbxVector4> *lNormals = NULL;
			FbxLayerElementArrayTemplate<FbxVector4> *lTangents = NULL;
			FbxVector4* lControlPoints = lShape->GetControlPoints();

			lShape->InitTangents();

			lShape->GetNormals( &lNormals );

			lShape->GetTangents( &lTangents );

			GE_Vec3 val;
			FbxVector4 v4;

			for ( uint32_t i = 0; i < m_currentModel->m_posAt.size(); ++i )
			{

				int32_t II = lBlendShapeChannelIndex * m_currentModel->m_posAt.size() + i;

				v4 = lControlPoints[ m_currentModel->m_posAt[ i ] ];
				val.x = ( float )v4[ 0 ];
				val.y = ( float )v4[ 1 ];
				val.z = ( float )v4[ 2 ];
				m_currentModel->m_vMorphPositions[ II ] = val;

				v4 = lNormals->GetAt( m_currentModel->m_normalAt[ i ] );
				val.x = ( float )v4[ 0 ];
				val.y = ( float )v4[ 1 ];
				val.z = ( float )v4[ 2 ];
				m_currentModel->m_vMorphNormals[ II ] = val;

				if ( lTangents && m_currentModel->m_tangentAt.size() )
				{
					v4 = lTangents->GetAt( m_currentModel->m_tangentAt[ i ] );
					val.x = ( float )v4[ 0 ];
					val.y = ( float )v4[ 1 ];
					val.z = ( float )v4[ 2 ];
					m_currentModel->m_vMorphTangents[ II ] = val;
				}

				m_currentModel->m_vMorphPositions[ II ] -= m_currentModel->m_vPositions[ i ];
				if ( m_currentModel->m_vNormals.size() )
					m_currentModel->m_vMorphNormals[ II ] -= m_currentModel->m_vNormals[ i ];
				if ( m_currentModel->m_vTangents.size() )
				{
					m_currentModel->m_vMorphTangents[ II ].x -= m_currentModel->m_vTangents[ i ].x;
					m_currentModel->m_vMorphTangents[ II ].y -= m_currentModel->m_vTangents[ i ].y;
					m_currentModel->m_vMorphTangents[ II ].z -= m_currentModel->m_vTangents[ i ].z;
				}
			}
		}
	}

	if ( m_currentModel->m_morphChannels.size() && startFrame != INT_MAX )
	{
		m_currentModel->m_morphData.NumberOfFrames = endFrame - startFrame;
		m_currentModel->m_morphData.NumberOfMorphTargets = m_currentModel->m_morphChannels.size();
		m_currentModel->m_morphFrames.resize( m_currentModel->m_morphData.NumberOfFrames * m_currentModel->m_morphChannels.size() );

		for ( uint32_t morphIndex = 0; morphIndex < m_currentModel->m_morphChannels.size(); ++morphIndex )
		{
			uint32_t frame = 0;
			for ( int32_t frameIndex = startFrame; frameIndex < endFrame; ++frameIndex )
			{
				double val;
				FbxTime time;
				time.SetFrame( frameIndex, m_scene->GetGlobalSettings().GetTimeMode() );
				m_scene->GetAnimationEvaluator()->GetPropertyValue( m_currentModel->m_morphChannels[ morphIndex ]->GetFirstProperty(),
					time ).Get( &val, EFbxType::eFbxDouble );
				m_currentModel->m_morphFrames[ frame * m_currentModel->m_morphChannels.size() + morphIndex ] = ( float )val / 100.0f;
				++frame;
			}
		}
	}
}

void GE_SceneManager::dumpLink( FbxGeometry *i_geometry )
{
	//use first skin modifier

	m_currentModel->m_boneStartFrame = INT_MAX;
	m_currentModel->m_boneEndFrame = 0;

	bool doResize = true;
	FbxCluster* lCluster;
	int32_t *icounter = new int32_t[ m_currentModel->m_indexAt.size() ];
	RtlZeroMemory( icounter, m_currentModel->m_indexAt.size() * sizeof( int32_t ) );

	if ( i_geometry->GetDeformerCount( FbxDeformer::eSkin ) )
	{
		for ( int32_t j = 0; j != ( ( FbxSkin * )i_geometry->GetDeformer( 0, FbxDeformer::eSkin ) )->GetClusterCount(); ++j )
		{
			lCluster = ( ( FbxSkin * )i_geometry->GetDeformer( 0, FbxDeformer::eSkin ) )->GetCluster( j );
			FbxNode *link = lCluster->GetLink();
			if ( !link )
				continue;

			if ( doResize )
			{
				m_currentModel->m_vBoneIndices.resize( m_currentModel->m_posAt.size() );
				m_currentModel->m_vBoneWeights.resize( m_currentModel->m_posAt.size() );
				RtlZeroMemory( m_currentModel->m_vBoneWeights.data(), sizeof( VertexDataBoneWeight )* m_currentModel->m_posAt.size() );
				doResize = false;
			}

			int32_t findIndex = -1;
			for ( int32_t boneIndex = 0; boneIndex < ( int32_t )m_currentModel->m_bones.size(); ++boneIndex )
			{
				if ( string( m_currentModel->m_bones[ boneIndex ].Name ) == link->GetName() )
				{
					findIndex = boneIndex;
					break;
				}
			}

			if ( findIndex < 0 )
			{
				Bone bone;

				strcpy_s( bone.Name, link->GetName() );
				FbxAMatrix matrix, matOffset, mattrans;
				GE_SceneManager::getSingleton().GetGeometryOffset( i_geometry->GetNode( 0 ), matOffset );
				lCluster->GetTransformLinkMatrix( matrix );
				lCluster->GetTransformMatrix( mattrans );
				matOffset = mattrans * matOffset;
				matrix = matOffset.Inverse() * matrix;

				GE_SceneManager::getSingleton().ConvertFBXMatToGEMat( matrix, bone.InitializeMatrix );

				bone.InitializeMatrix.inverse();

				FbxAnimCurve *animCurve = NULL;
				if ( m_animStack )
					animCurve = link->LclTranslation.GetCurve( m_animStack->GetMember<FbxAnimLayer>() );
				if ( animCurve )
				{
					for ( int32_t keyIndex = 0; keyIndex < animCurve->KeyGetCount(); ++keyIndex )
					{
						int32_t count = ( int32_t )( animCurve->KeyGetTime( keyIndex ).GetFrameCount( m_scene->GetGlobalSettings().GetTimeMode() ) );
						if ( count > m_currentModel->m_boneEndFrame )
							m_currentModel->m_boneEndFrame = count;
						if ( count < m_currentModel->m_boneStartFrame )
							m_currentModel->m_boneStartFrame = count;
					}
				}

				animCurve = NULL;
				if ( m_animStack )
					animCurve = link->LclRotation.GetCurve( m_animStack->GetMember<FbxAnimLayer>() );
				if ( animCurve )
				{
					for ( int32_t keyIndex = 0; keyIndex < animCurve->KeyGetCount(); ++keyIndex )
					{
						int32_t count = ( int32_t )( animCurve->KeyGetTime( keyIndex ).GetFrameCount( m_scene->GetGlobalSettings().GetTimeMode() ) );
						if ( count > m_currentModel->m_boneEndFrame )
							m_currentModel->m_boneEndFrame = count;
						if ( count < m_currentModel->m_boneStartFrame )
							m_currentModel->m_boneStartFrame = count;
					}
				}

				animCurve = NULL;
				if ( m_animStack )
					animCurve = link->LclScaling.GetCurve( m_animStack->GetMember<FbxAnimLayer>() );
				if ( animCurve )
				{
					for ( int32_t keyIndex = 0; keyIndex < animCurve->KeyGetCount(); ++keyIndex )
					{
						int32_t count = ( int32_t )( animCurve->KeyGetTime( keyIndex ).GetFrameCount( m_scene->GetGlobalSettings().GetTimeMode() ) );
						if ( count > m_currentModel->m_boneEndFrame )
							m_currentModel->m_boneEndFrame = count;
						if ( count < m_currentModel->m_boneStartFrame )
							m_currentModel->m_boneStartFrame = count;
					}
				}

				findIndex = m_currentModel->m_bones.size();
				m_currentModel->m_boneNodes.push_back( link );
				m_currentModel->m_bones.push_back( bone );
			}

			int32_t* lIndices = lCluster->GetControlPointIndices();
			double* lWeights = lCluster->GetControlPointWeights();

			for ( int32_t k = 0; k < lCluster->GetControlPointIndicesCount(); ++k )
			{
				int32_t ik = lIndices[ k ];
				int32_t counter = icounter[ ik ];
				if ( counter > 7 )
					continue;
				++icounter[ ik ];
				for ( uint32_t m = 0; m < m_currentModel->m_indexAt[ ik ].size(); ++m )
				{
					m_currentModel->m_vBoneIndices[ m_currentModel->m_indexAt[ ik ][ m ] ].Indices[ counter ] = findIndex;
					m_currentModel->m_vBoneWeights[ m_currentModel->m_indexAt[ ik ][ m ] ].Weights[ counter ] = ( float )( lWeights[ k ] );
				}
			}
		}
	}
	SAFE_DELETE_ARRAY( icounter );

	/*for( uint32_t boneIndex = 0; boneIndex < m_currentModel->m_bones.size(); ++boneIndex )
	{
	dumpPose( m_currentModel->m_boneNodes[ boneIndex ] );
	dumpPose( m_currentModel->m_boneNodes[ boneIndex ], false );
	}*/

	for ( uint32_t boneIndex = 0; boneIndex < m_currentModel->m_bones.size(); ++boneIndex )
	{
		string name = m_currentModel->m_bones[ boneIndex ].Name;
		uint32_t firstU = name.find_first_of( '_' );
		string prefix = name.substr( 0, firstU + 1 );
		std::transform( prefix.begin(), prefix.end(), prefix.begin(), ::tolower );
		if ( prefix == BOUND )
			m_currentModel->m_boneData.BoundBoneID = boneIndex;
		m_currentModel->m_bones[ boneIndex ].ParentID = -1;
		FbxNode * node = m_currentModel->m_boneNodes[ boneIndex ]->GetParent();
		if ( node )
		{
			string searchName = node->GetName();
			for ( uint32_t i = 0; i < m_currentModel->m_bones.size(); ++i )
			{
				if ( searchName == m_currentModel->m_bones[ i ].Name && boneIndex != i )
				{
					m_currentModel->m_bones[ boneIndex ].ParentID = i;
					break;
				}
			}
		}
	}

	if ( m_currentModel->m_boneStartFrame != INT_MAX )
	{
		m_currentModel->m_boneData.NumberOfBones = m_currentModel->m_bones.size();
		m_currentModel->m_boneData.NumberOfFrames = m_currentModel->m_boneEndFrame - m_currentModel->m_boneStartFrame;

		m_currentModel->m_boneFrames.resize( m_currentModel->m_boneData.NumberOfFrames * m_currentModel->m_boneNodes.size() );

		uint32_t frame = 0;
		for ( int32_t frameIndex = m_currentModel->m_boneStartFrame; frameIndex < m_currentModel->m_boneEndFrame; ++frameIndex )
		{
			FbxTime time;
			time.SetFrame( frameIndex, m_scene->GetGlobalSettings().GetTimeMode() );

			for ( uint32_t boneIndex = 0; boneIndex < m_currentModel->m_boneNodes.size(); ++boneIndex )
			{
				int32_t frameBoneIndex = frame * m_currentModel->m_boneNodes.size() + boneIndex;

				if ( m_currentModel->m_bones[ boneIndex ].ParentID == -1 )
					GE_SceneManager::getSingleton().ConvertFBXMatToGEMat( m_scene->GetAnimationEvaluator()->GetNodeGlobalTransform( m_currentModel->m_boneNodes[ boneIndex ],
					time ), m_currentModel->m_boneFrames[ frameBoneIndex ] );
				else
					GE_SceneManager::getSingleton().ConvertFBXMatToGEMat( m_scene->GetAnimationEvaluator()->GetNodeGlobalTransform( m_currentModel->m_boneNodes[ boneIndex ],
					time ), m_currentModel->m_boneFrames[ frameBoneIndex ] );
			}

			++frame;
		}
	}

}

void GE_SceneManager::dumpCamera( FbxNode *i_node )
{
	FbxCamera *camera = i_node->GetCamera();
	FbxNode *target = i_node->GetTarget();
	m_currentCamera = new GE_Camera();
	strcpy_s( m_currentCamera->m_data.Name, i_node->GetName() );
	cout << "\nActive camera: <" << m_currentCamera->m_data.Name << ">.\n";
	cout << "Loading camera data... ";
	m_currentCamera->m_data.IsOrtho = ( camera->ProjectionType.Get() == FbxCamera::eOrthogonal );
	m_currentCamera->m_data.IsTargetMode = ( i_node->GetTarget() != NULL );
	//m_currentCamera->m_data.DefaultFrameRate = ( uint32_t )FbxGetFrameRate( m_scene->GetGlobalSettings().GetTimeMode() );

	int32_t startFrame = INT_MAX;
	int32_t	endFrame = 0;

	FbxAnimCurve *animCurve = NULL;
	if ( m_animStack )
		animCurve = i_node->LclTranslation.GetCurve( m_animStack->GetMember<FbxAnimLayer>() );
	if ( animCurve )
	{
		for ( int32_t keyIndex = 0; keyIndex < animCurve->KeyGetCount(); ++keyIndex )
		{
			int32_t count = ( int32_t )( animCurve->KeyGetTime( keyIndex ).GetFrameCount( m_scene->GetGlobalSettings().GetTimeMode() ) );
			if ( count > endFrame )
				endFrame = count;
			if ( count < startFrame )
				startFrame = count;
		}
	}
	animCurve = NULL;
	if ( m_animStack )
		animCurve = i_node->LclRotation.GetCurve( m_animStack->GetMember<FbxAnimLayer>() );
	if ( animCurve )
	{
		for ( int32_t keyIndex = 0; keyIndex < animCurve->KeyGetCount(); ++keyIndex )
		{
			int32_t count = ( int32_t )( animCurve->KeyGetTime( keyIndex ).GetFrameCount( m_scene->GetGlobalSettings().GetTimeMode() ) );
			if ( count > endFrame )
				endFrame = count;
			if ( count < startFrame )
				startFrame = count;
		}
	}

	if ( target )
	{
		animCurve = NULL;
		if ( m_animStack )
			animCurve = target->LclTranslation.GetCurve( m_animStack->GetMember<FbxAnimLayer>() );
		if ( animCurve )
		{
			for ( int32_t keyIndex = 0; keyIndex < animCurve->KeyGetCount(); ++keyIndex )
			{
				int32_t count = ( int32_t )( animCurve->KeyGetTime( keyIndex ).GetFrameCount( m_scene->GetGlobalSettings().GetTimeMode() ) );
				if ( count > endFrame )
					endFrame = count;
				if ( count < startFrame )
					startFrame = count;
			}
		}
	}

	animCurve = NULL;
	if ( m_animStack )
		animCurve = camera->FieldOfView.GetCurve( m_animStack->GetMember<FbxAnimLayer>() );
	if ( animCurve )
	{
		for ( int32_t keyIndex = 0; keyIndex < animCurve->KeyGetCount(); ++keyIndex )
		{
			int32_t count = ( int32_t )( animCurve->KeyGetTime( keyIndex ).GetFrameCount( m_scene->GetGlobalSettings().GetTimeMode() ) );
			if ( count > endFrame )
				endFrame = count;
			if ( count < startFrame )
				startFrame = count;
		}
	}

	animCurve = NULL;
	if ( m_animStack )
		animCurve = camera->FocusDistance.GetCurve( m_animStack->GetMember<FbxAnimLayer>() );
	if ( animCurve )
	{
		for ( int32_t keyIndex = 0; keyIndex < animCurve->KeyGetCount(); ++keyIndex )
		{
			int32_t count = ( int32_t )( animCurve->KeyGetTime( keyIndex ).GetFrameCount( m_scene->GetGlobalSettings().GetTimeMode() ) );
			if ( count > endFrame )
				endFrame = count;
			if ( count < startFrame )
				startFrame = count;
		}
	}

	animCurve = NULL;
	if ( m_animStack )
		animCurve = camera->NearPlane.GetCurve( m_animStack->GetMember<FbxAnimLayer>() );
	if ( animCurve )
	{
		for ( int32_t keyIndex = 0; keyIndex < animCurve->KeyGetCount(); ++keyIndex )
		{
			int32_t count = ( int32_t )( animCurve->KeyGetTime( keyIndex ).GetFrameCount( m_scene->GetGlobalSettings().GetTimeMode() ) );
			if ( count > endFrame )
				endFrame = count;
			if ( count < startFrame )
				startFrame = count;
		}
	}

	animCurve = NULL;
	if ( m_animStack )
		animCurve = camera->FarPlane.GetCurve( m_animStack->GetMember<FbxAnimLayer>() );
	if ( animCurve )
	{
		for ( int32_t keyIndex = 0; keyIndex < animCurve->KeyGetCount(); ++keyIndex )
		{
			int32_t count = ( int32_t )( animCurve->KeyGetTime( keyIndex ).GetFrameCount( m_scene->GetGlobalSettings().GetTimeMode() ) );
			if ( count > endFrame )
				endFrame = count;
			if ( count < startFrame )
				startFrame = count;
		}
	}

	//m_currentCamera->m_data.AspectRate = (float)( camera->AspectWidth.Get() / camera->AspectHeight.Get() );
	m_currentCamera->m_data.Fovy = GE_ConvertToRadians( ( float )camera->FieldOfView.Get() );
	m_currentCamera->m_data.ZNear = GE_ConvertToUnit( ( float )camera->NearPlane.Get() );
	m_currentCamera->m_data.ZFar = GE_ConvertToUnit( ( float )camera->FarPlane.Get() );
	m_currentCamera->m_data.FocusDistance = GE_ConvertToUnit( ( float )camera->FocusDistance.Get() );

	GE_Mat4x4 trans;
	GE_Vec3 scale;
	GE_Quat rot;

	GE_SceneManager::getSingleton().ConvertFBXMatToGEMat( m_scene->GetAnimationEvaluator()->GetNodeGlobalTransform( i_node ), trans );
	trans.decompose( &m_currentCamera->m_data.Position, &m_currentCamera->m_data.Rotation, &scale );

	if ( target )
	{
		GE_SceneManager::getSingleton().ConvertFBXMatToGEMat( m_scene->GetAnimationEvaluator()->GetNodeGlobalTransform( target ), trans );
		trans.decompose( &m_currentCamera->m_data.Target, &rot, &scale );
	}

	if ( startFrame != INT_MAX )
	{
		m_currentCamera->m_data.NumberOfFrames = endFrame - startFrame;
		m_currentCamera->m_frames.resize( m_currentCamera->m_data.NumberOfFrames );

		uint32_t frame = 0;
		for ( int32_t frameIndex = startFrame; frameIndex < endFrame; ++frameIndex )
		{
			FbxTime time;
			time.SetFrame( frameIndex, m_scene->GetGlobalSettings().GetTimeMode() );
			Camera *cam = &m_currentCamera->m_frames.data()[ frame ];

			cam->Fovy = GE_ConvertToRadians( ( float )camera->FieldOfView.EvaluateValue( time ) );
			cam->FocusDistance = GE_ConvertToUnit( ( float )camera->FocusDistance.EvaluateValue( time ) );
			cam->ZNear = GE_ConvertToUnit( ( float )camera->NearPlane.EvaluateValue( time ) );
			cam->ZFar = GE_ConvertToUnit( ( float )camera->FarPlane.EvaluateValue( time ) );

			GE_SceneManager::getSingleton().ConvertFBXMatToGEMat( m_scene->GetAnimationEvaluator()->GetNodeGlobalTransform( i_node, time ), trans );
			trans.decompose( &cam->Position, &cam->Rotation, &scale );

			if ( target )
			{
				GE_SceneManager::getSingleton().ConvertFBXMatToGEMat( m_scene->GetAnimationEvaluator()->GetNodeGlobalTransform( target, time ), trans );
				trans.decompose( &cam->Target, &rot, &scale );
			}
			++frame;
		}
	}

	cout << "done.\n";
	string fileName = m_path + m_currentCamera->m_data.Name + ".GCamera";
	cout << "Saving camera to <" << fileName << ">... ";
	FILE *fp;
	fopen_s( &fp, fileName.c_str(), "wb" );

	fwrite( &m_currentCamera->m_data, sizeof( CameraData ), 1, fp );
	if ( m_currentCamera->m_frames.size() )
		fwrite( m_currentCamera->m_frames.data(), sizeof( Camera ), m_currentCamera->m_frames.size(), fp );

	fclose( fp );
	cout << "done.\n";

	SAFE_DELETE( m_currentCamera );
}

GE_SceneManager::GE_SceneManager()
{
	HashWrapper = new md5wrapper();
	m_currentModel = NULL;
	m_currentCamera = NULL;
}

void GE_SceneManager::ConvertFBXMatToGEMat( const FbxAMatrix &i_fbxMat, GE_Mat4x4 &i_geMat )
{
	for ( int32_t r = 0; r < 4; ++r )
	for ( int32_t c = 0; c < 4; ++c )
		i_geMat._m[ r ][ c ] =
		( float )i_fbxMat.GetRow( r )[ c ];
}

void GE_SceneManager::ConvertGEMatToFBXMat( const GE_Mat4x4 &i_geMat, FbxAMatrix &i_fbxMat )
{
	FbxVector4 row;
	for ( int32_t r = 0; r < 4; ++r )
	{
		for ( int32_t c = 0; c < 4; ++c )
			row.mData[ c ] = i_geMat._m[ r ][ c ];
		i_fbxMat.SetRow( r, row );
	}

}

GE_SceneManager GE_SceneManager::m_singleton;


void GS_Model::fillSubsets()
{
	m_data.HasVerticesDataColor = ( m_vColors.size() > 0 );
	m_data.HasVerticesDataTexcoord = ( m_vTexcoords.size() > 0 );
	m_data.HasVerticesDataAdditionTexcoord = ( m_vAdditionalTexcoords.size() > 0 );
	m_data.HasVerticesDataBoneIW = ( m_vBoneIndices.size() > 0 );
	m_data.NumberOfSubsets = m_materialindexAt.size();

	m_subsetsData.resize( m_data.NumberOfSubsets );
	for ( uint32_t i = 0; i < m_data.NumberOfSubsets; ++i )
	{
		if ( m_morphChannels.size() )
			m_subsetsData[ i ].VertexDataMorphTarget.resize( m_materialindexAt[ i ].Vertices.size() * m_morphChannels.size() );
		for ( uint32_t vertIndex = 0; vertIndex < m_materialindexAt[ i ].Vertices.size(); ++vertIndex )
		{
			int32_t vertAt = m_materialindexAt[ i ].Vertices[ vertIndex ];

			VertexCombination combination;

			VertexBuffer vb;
			vb.Position = m_vPositions[ vertAt ];

			if ( m_vNormals.size() )
				vb.Normal = m_vNormals[ vertAt ];
			if ( m_vTangents.size() )
				vb.Tangent = m_vTangents[ vertAt ];
			vb.VertexID = vertIndex;
			m_subsetsData[ i ].VertexBufferData.push_back( vb );

			combination.VB = vb;

			m_subsetsData[ i ].Indices.push_back( vertIndex );

			if ( m_data.HasVerticesDataBoneIW )
			{
				combination.BoneIndex = m_vBoneIndices[ vertAt ];
				combination.BoneWeight = m_vBoneWeights[ vertAt ];
				m_subsetsData[ i ].VerticesDataBoneIndex.push_back( m_vBoneIndices[ vertAt ] );
				m_subsetsData[ i ].VerticesDataBoneWeight.push_back( m_vBoneWeights[ vertAt ] );
			}

			if ( m_data.HasVerticesDataTexcoord )
			{
				GE_Vec4 tc;
				tc.x = m_vTexcoords[ vertAt ].x;
				tc.y = m_vTexcoords[ vertAt ].y;
				if ( m_data.HasVerticesDataAdditionTexcoord )
				{
					tc.z = m_vAdditionalTexcoords[ vertAt ].x;
					tc.w = m_vAdditionalTexcoords[ vertAt ].y;
				}
				else
				{
					tc.z = 0.0f;
					tc.w = 0.0f;
				}
				combination.Texcoord = tc;
				m_subsetsData[ i ].VerticesDataTexcoord.push_back( tc );
			}

			if ( m_data.HasVerticesDataColor )
			{
				m_subsetsData[ i ].VerticesDataColor.push_back( *( VertexDataColor * )&m_vColors[ vertAt ] );
				combination.Color = *( VertexDataColor * )&m_vColors[ vertAt ];
			}

			combination.VB.VertexID = 0;
			m_subsetsData[ i ].Combination.push_back( combination.getHash() );

			for ( uint32_t morphIndex = 0; morphIndex < m_morphChannels.size(); ++morphIndex )
			{
				MorphTargetVertex *vb = &m_subsetsData[ i ].VertexDataMorphTarget[ morphIndex * m_materialindexAt[ i ].Vertices.size() + vertIndex ];
				vb->Position = m_vMorphPositions[ morphIndex * m_vPositions.size() + vertAt ];
				if ( m_vMorphNormals.size() )
					vb->Normal = m_vMorphNormals[ morphIndex * m_vPositions.size() + vertAt ];
				if ( m_vMorphTangents.size() )
					vb->Tangent = m_vMorphTangents[ morphIndex * m_vPositions.size() + vertAt ];
			}
		}

		cout << "Active subset: <" << i << ">.\n";

		if ( !GE_SceneManager::getSingleton().m_ctrlKeyDown )
		{
			VertexCache vertex_cache;
			int cacheMisses = vertex_cache.GetCacheMissCount( ( int * )m_subsetsData[ i ].Indices.data(), m_subsetsData[ i ].Indices.size() / 3 );
			cout << "Vertices, Polygons and Cache misses before: "
				<< m_subsetsData[ i ].VertexBufferData.size()
				<< "\t"
				<< m_subsetsData[ i ].Indices.size() / 3
				<< "\t"
				<< cacheMisses
				<< "\n";

			m_subsetsData[ i ].optimize( this );

			cacheMisses = vertex_cache.GetCacheMissCount( ( int * )m_subsetsData[ i ].Indices.data(), m_subsetsData[ i ].Indices.size() / 3 );
			cout << "Vertices, Polygons and Cache misses step1 : "
				<< m_subsetsData[ i ].VertexBufferData.size()
				<< "\t"
				<< m_subsetsData[ i ].Indices.size() / 3
				<< "\t"
				<< cacheMisses
				<< "\n";

			if ( GE_SceneManager::getSingleton().m_shiftKeyDown )
			{
				vector< uint32_t > indices = m_subsetsData[ i ].Indices;

				VertexCacheOptimizer vco;
				vco.Optimize( ( int * )indices.data(), indices.size() / 3 );

				int cacheMisses1 = vertex_cache.GetCacheMissCount( ( int * )indices.data(), indices.size() / 3 );
				cout << "Vertices, Polygons and Cache misses step2 : "
					<< m_subsetsData[ i ].VertexBufferData.size()
					<< "\t"
					<< indices.size() / 3
					<< "\t"
					<< cacheMisses1
					<< "\n";

				if ( cacheMisses1 < cacheMisses )
					m_subsetsData[ i ].Indices = indices;
			}
		}
		
	}

}

void ModelSubsetData::optimize( GS_Model* i_model )
{
	ModelSubsetData optimized;
	for ( uint32_t i = 0; i < VertexBufferData.size(); ++i )
	{
		int32_t vertFound = -1;
		BigNum combCode = Combination[ i ];
		for ( uint32_t combinationIndex = 0; combinationIndex < optimized.Combination.size(); ++combinationIndex )
		{
			if ( optimized.Combination[ combinationIndex ] == combCode )
			{
				vertFound = combinationIndex;
				break;
			}
		}

		if ( vertFound < 0 )
		{
			optimized.Combination.push_back( combCode );
			vertFound = optimized.Combination.size() - 1;

			optimized.VertexBufferData.push_back( VertexBufferData[ i ] );
			optimized.VertexBufferData[ optimized.VertexBufferData.size() - 1 ].VertexID = optimized.VertexBufferData.size() - 1;
			if ( VerticesDataTexcoord.size() )
				optimized.VerticesDataTexcoord.push_back( VerticesDataTexcoord[ i ] );
			if ( VerticesDataColor.size() )
				optimized.VerticesDataColor.push_back( VerticesDataColor[ i ] );
			if ( VerticesDataBoneIndex.size() )
			{
				optimized.VerticesDataBoneIndex.push_back( VerticesDataBoneIndex[ i ] );
				optimized.VerticesDataBoneWeight.push_back( VerticesDataBoneWeight[ i ] );
			}
		}

		optimized.Indices.push_back( vertFound );
	}

	if ( VertexDataMorphTarget.size() )
	{
		optimized.VertexDataMorphTarget.resize( i_model->m_morphChannels.size() * optimized.VertexBufferData.size() );
		for ( uint32_t morphIndex = 0; morphIndex < i_model->m_morphChannels.size(); ++morphIndex )
		{
			for ( uint32_t index = 0; index < optimized.Indices.size(); ++index )
			{
				optimized.VertexDataMorphTarget[ morphIndex * optimized.VertexBufferData.size() + optimized.Indices[ index ] ] =
					VertexDataMorphTarget[ morphIndex * VertexBufferData.size() + index ];
			}
		}
	}

	*this = optimized;
}


BigNum VertexCombination::getHash()
{
	return BigNum( GE_SceneManager::getSingleton().HashWrapper->getHashBytes( ( unsigned char * )this, sizeof( VertexCombination ) ).c_str() );
}
