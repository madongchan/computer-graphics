1. 속성->C/C++ 일반 추가 포함 디렉토리에 include
2. 링커->시스템->하위 시스템에 창으로
3. include 폴더랑 lib 폴더는 프레임워크 폴더에 넣기 bin 폴더는 넣지 말고 dll 파일들만 넣기
4.
////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_

/////////////
// LINKING //
/////////////
#ifdef _DEBUG
	// 'Debug' 모드로 빌드할 때
#pragma comment(lib, "lib/assimp-vc142-mtd.lib")
#else
	// 'Release' 모드로 빌드할 때
#pragma comment(lib, "lib/assimp-vc143-mt.lib")
#endif

//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <directxmath.h>

using namespace DirectX;

#include <fstream>
#include <atlstr.h>

using namespace std;

// ASSIMP library
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"