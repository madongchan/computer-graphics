#pragma once

#include <d3d11.h>
#include <directxmath.h>
#include <vector>
#include "DDSTextureLoader.h" // 큐브맵 로딩에 필요

using namespace DirectX;
using namespace std;

class SkyboxClass
{
private:
	// ModelClass::VertexType과 동일한 구조
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
	};

public:
	SkyboxClass();
	SkyboxClass(const SkyboxClass&);
	~SkyboxClass();

	// 큐브맵 dds 파일명을 받음
	bool Initialize(ID3D11Device* device, const WCHAR* cubeMapFilename);
	void Shutdown();
	void Render(ID3D11DeviceContext* deviceContext);

	int GetIndexCount() const { return m_indexCount; }
	ID3D11ShaderResourceView* GetTexture() { return m_cubeTexture; }

private:
	bool InitializeBuffers(ID3D11Device*);
	bool LoadCubeMapTexture(ID3D11Device* device, const WCHAR* filename);

	void ShutdownBuffers();
	void ReleaseTexture();
	void RenderBuffers(ID3D11DeviceContext* deviceContext);

private:
	ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
	int m_vertexCount, m_indexCount;
	ID3D11ShaderResourceView* m_cubeTexture;
};