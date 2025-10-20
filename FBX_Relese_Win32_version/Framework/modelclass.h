////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_

/////////////
// LINKING //
/////////////
#ifdef _DEBUG
	// 'Debug' ���� ������ ��
#pragma comment(lib, "lib/assimp-vc142-mtd.lib")
#else
	// 'Release' ���� ������ ��
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
	};

public:
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();

	// --- ���� 1: Initialize �Լ� ���� ���� ---
	// 1. Assimp�� �� ��� (char*)
	// 2. TextureClass�� �ؽ�ó ��� (WCHAR*)
	bool Initialize(ID3D11Device* device, const char* modelFilename, const WCHAR* textureFilename);

	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture(); //

private:
	// --- ���� 2: ���� ���� �Լ��� ���� ---
	// Initialize() ���ο��� ȣ��� �Լ����Դϴ�.
	bool InitializeBuffers(ID3D11Device*);
	bool LoadModel(const char*); // CString�� �ƴ� char*�� �޵��� ����

	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	// --- ���� 3: ���ʿ��� �Լ� ���� ���� ---
	// (LoadTexture, ReleaseModel ���� Initialize/Shutdown�� ó��)

private:
	ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
	int m_vertexCount, m_indexCount; // "unsigned int" -> "int"�� ���� (GetIndexCount�� ����)
	int m_faceCount; // LoadModel���� ���

	// --- ���� 4: LoadModel���� ����� ��� ���� ---
	VertexType* m_vertices;     // RAM�� �ӽ� ������ ���� �迭
	unsigned long* m_indices;  // RAM�� �ӽ� ������ �ε��� �迭

	TextureClass* m_Texture; //
};

#endif