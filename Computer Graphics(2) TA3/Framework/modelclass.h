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

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "textureclass.h"

////////////////////////////////////////////////////////////////////////////////
// Class name: ModelClass
////////////////////////////////////////////////////////////////////////////////
class ModelClass
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
		// ★ 범프 맵핑을 위한 Tangent, Binormal 추가
		XMFLOAT3 tangent;
		XMFLOAT3 binormal;
	};

public:
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();

	// --- 수정 1: Initialize 함수 선언 변경 ---
	// 1. Assimp용 모델 경로 (char*)
	// 2. TextureClass용 텍스처 경로 (WCHAR*)
	bool Initialize(ID3D11Device* device, const char* modelFilename, const WCHAR* textureFilename);

	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture(); //

	// 모델의 로컬 공간 기준 최소/최대 좌표를 반환하는 함수 추가
	void GetMinMax(XMFLOAT3& min, XMFLOAT3& max) { min = m_minVertex; max = m_maxVertex; }
	bool InitializeGeneratedCube(ID3D11Device* device);
private:
	// Initialize() 내부에서 호출될 함수들입니다.
	bool InitializeBuffers(ID3D11Device*);
	bool LoadModel(const char*); // CString이 아닌 char*를 받도록 수정

	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);


private:
	ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
	int m_vertexCount, m_indexCount; // "unsigned int" -> "int"로 변경 (GetIndexCount와 통일)
	int m_faceCount; // LoadModel에서 사용

	// --- 수정 4: LoadModel에서 사용할 멤버 변수 ---
	VertexType* m_vertices;     // RAM에 임시 저장할 정점 배열
	unsigned long* m_indices;  // RAM에 임시 저장할 인덱스 배열

	TextureClass* m_Texture;

	XMFLOAT3 m_minVertex, m_maxVertex; // 바운딩 박스 좌표 저장
};

#endif