#include "skyboxclass.h"
#include <vector> // (InitializeBuffers용)

SkyboxClass::SkyboxClass()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_cubeTexture = 0;
	m_vertexCount = 0;
	m_indexCount = 0;
}

SkyboxClass::SkyboxClass(const SkyboxClass& other) {}
SkyboxClass::~SkyboxClass() {}

bool SkyboxClass::Initialize(ID3D11Device* device, const WCHAR* cubeMapFilename)
{
	bool result;

	// 1. 구(Sphere) 모델의 정점/인덱스 버퍼 생성 (main.cpp의 CreateSphere 로직)
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	// 2. 큐브맵 텍스처 로드 (main.cpp의 큐브맵 로딩 로직)
	result = LoadCubeMapTexture(device, cubeMapFilename);
	if (!result)
	{
		return false;
	}

	return true;
}

void SkyboxClass::Shutdown()
{
	ReleaseTexture();
	ShutdownBuffers();
}

void SkyboxClass::Render(ID3D11DeviceContext* deviceContext)
{
	RenderBuffers(deviceContext);
}

//
bool SkyboxClass::InitializeBuffers(ID3D11Device* device)
{
	int LatLines = 10;
	int LongLines = 10;

	int NumSphereVertices = ((LatLines - 2) * LongLines) + 2;
	int NumSphereFaces = ((LatLines - 3) * (LongLines) * 2) + (LongLines * 2);

	m_vertexCount = NumSphereVertices;
	m_indexCount = NumSphereFaces * 3;

	float sphereYaw = 0.0f;
	float spherePitch = 0.0f;

	// ModelClass::VertexType 사용
	std::vector<VertexType> vertices(NumSphereVertices);
	// VertexType 구조체 전체를 0으로 초기화
	ZeroMemory(&vertices[0], sizeof(VertexType) * NumSphereVertices);

	XMVECTOR currVertPos = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

	// 첫 번째 정점 (북극)
	vertices[0].position = XMFLOAT3(0.0f, 0.0f, 1.0f);

	// 중간 정점들
	for (DWORD i = 0; i < DWORD(LatLines - 2); ++i)
	{
		spherePitch = (float)(i + 1) * (3.14f / (float)(LatLines - 1));
		XMMATRIX Rotationx = XMMatrixRotationX(spherePitch);
		for (DWORD j = 0; j < DWORD(LongLines); ++j)
		{
			sphereYaw = (float)j * (6.28f / (float)(LongLines));
			XMMATRIX Rotationy = XMMatrixRotationZ(sphereYaw);
			currVertPos = XMVector3TransformNormal(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), (Rotationx * Rotationy));
			currVertPos = XMVector3Normalize(currVertPos);

			float posX, posY, posZ;
			posX = XMVectorGetX(currVertPos);
			posY = XMVectorGetY(currVertPos);
			posZ = XMVectorGetZ(currVertPos);

			// VertexType의 position 멤버에만 값 할당
			vertices[i * LongLines + j + 1].position = XMFLOAT3(posX, posY, posZ);
			// (texcoord와 normal은 0.0f로 유지)
		}
	}

	// 마지막 정점 (남극)
	vertices[NumSphereVertices - 1].position = XMFLOAT3(0.0f, 0.0f, -1.0f);

	// 정점 버퍼 생성
	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * NumSphereVertices;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;
	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
	vertexBufferData.pSysMem = &vertices[0];
	HRESULT result = device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &m_vertexBuffer);
	if (FAILED(result)) { return false; }

	// 인덱스 버퍼 생성
	std::vector<DWORD> indices(m_indexCount);
	int k = 0;
	for (DWORD l = 0; l < DWORD(LongLines - 1); ++l)
	{
		indices[k] = 0;
		indices[k + 1] = l + 1;
		indices[k + 2] = l + 2;
		k += 3;
	}
	indices[k] = 0;
	indices[k + 1] = LongLines;
	indices[k + 2] = 1;
	k += 3;
	for (DWORD i = 0; i < DWORD(LatLines - 3); ++i)
	{
		for (DWORD j = 0; j < DWORD(LongLines - 1); ++j)
		{
			indices[k] = i * LongLines + j + 1;
			indices[k + 1] = i * LongLines + j + 2;
			indices[k + 2] = (i + 1) * LongLines + j + 1;
			indices[k + 3] = (i + 1) * LongLines + j + 1;
			indices[k + 4] = i * LongLines + j + 2;
			indices[k + 5] = (i + 1) * LongLines + j + 2;
			k += 6;
		}
		indices[k] = (i * LongLines) + LongLines;
		indices[k + 1] = (i * LongLines) + 1;
		indices[k + 2] = ((i + 1) * LongLines) + LongLines;
		indices[k + 3] = ((i + 1) * LongLines) + LongLines;
		indices[k + 4] = (i * LongLines) + 1;
		indices[k + 5] = ((i + 1) * LongLines) + 1;
		k += 6;
	}
	for (DWORD l = 0; l < (DWORD)(LongLines - 1); ++l)
	{
		indices[k] = NumSphereVertices - 1;
		indices[k + 1] = (NumSphereVertices - 1) - (l + 1);
		indices[k + 2] = (NumSphereVertices - 1) - (l + 2);
		k += 3;
	}
	indices[k] = NumSphereVertices - 1;
	indices[k + 1] = (NumSphereVertices - 1) - LongLines;
	indices[k + 2] = NumSphereVertices - 2;

	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(DWORD) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices[0];
	result = device->CreateBuffer(&indexBufferDesc, &iinitData, &m_indexBuffer);
	if (FAILED(result)) { return false; }

	return true;
}

//
bool SkyboxClass::LoadCubeMapTexture(ID3D11Device* device, const WCHAR* filename)
{
	ID3D11Texture2D* SMTexture = 0;

	// DDSTextureLoader를 사용하여 큐브맵 플래그와 함께 DDS 로드
	HRESULT result = CreateDDSTextureFromFileEx(device, filename, 0, D3D11_USAGE_IMMUTABLE, D3D11_BIND_SHADER_RESOURCE,
		0, D3D11_RESOURCE_MISC_TEXTURECUBE, false, (ID3D11Resource**)&SMTexture, nullptr);

	if (FAILED(result))
	{
		return false;
	}

	// 텍스처에서 SRV(Shader Resource View) 생성
	D3D11_TEXTURE2D_DESC SMTextureDesc;
	SMTexture->GetDesc(&SMTextureDesc);

	D3D11_SHADER_RESOURCE_VIEW_DESC SMViewDesc;
	SMViewDesc.Format = SMTextureDesc.Format;
	SMViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	SMViewDesc.TextureCube.MipLevels = SMTextureDesc.MipLevels;
	SMViewDesc.TextureCube.MostDetailedMip = 0;

	result = device->CreateShaderResourceView(SMTexture, &SMViewDesc, &m_cubeTexture);

	SMTexture->Release();
	SMTexture = 0;

	return SUCCEEDED(result);
}


void SkyboxClass::ShutdownBuffers()
{
	if (m_indexBuffer) { m_indexBuffer->Release(); m_indexBuffer = 0; }
	if (m_vertexBuffer) { m_vertexBuffer->Release(); m_vertexBuffer = 0; }
}

void SkyboxClass::ReleaseTexture()
{
	if (m_cubeTexture) { m_cubeTexture->Release(); m_cubeTexture = 0; }
}

void SkyboxClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}