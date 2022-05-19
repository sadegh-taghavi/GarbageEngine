#pragma once

#pragma warning ( disable : 4005 ) 
#pragma warning ( disable : 4800 )
#include <Windows.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include <list>
#include <fstream>
#include <atlstr.h>
#include <string>
#include <sstream>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include <D3d11.h>
#include <DXGI.h>


using namespace std;

#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"wininet.lib")
#pragma comment(lib,"DXGI.lib")
#pragma comment(lib,"D3D11.lib")

using namespace DirectX;

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define	SAFE_DELETE(p)  { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if (p) { delete[] (p);   (p)=NULL; } }
#define GE_BEEP Beep(800,200);

#ifdef _DEBUG
#define GE_SHADER_PATH "../../GData/Shader/Debug/"
#define GE_TEXTURE_PATH "../../GData/Texture/"
#else
#define GE_SHADER_PATH "../../GData/Shader/Release/"
#define GE_TEXTURE_PATH "../../GData/Texture/"
#endif

#define MAX_NAME_LENGTH 64
#define BONE_MAX_IW 8

