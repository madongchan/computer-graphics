#include "groundplaneclass.h"

GroundPlaneClass::GroundPlaneClass()
{
    m_vertexBuffer = 0;
    m_indexBuffer = 0;
    m_Texture = 0;
}

GroundPlaneClass::GroundPlaneClass(const GroundPlaneClass& other)
{
}

GroundPlaneClass::~GroundPlaneClass()
{
}

bool GroundPlaneClass::Initialize(ID3D11Device* device, float width, float depth, int tilesX, int tilesZ, const WCHAR* textureFilename)
{
    bool result;

    // 버퍼 초기화
    result = InitializeBuffers(device, width, depth, tilesX, tilesZ);
    if (!result)
    {
        return false;
    }

    // 텍스처 로드
    result = LoadTexture(device, textureFilename);
    if (!result)
    {
        return false;
    }

    return true;
}

void GroundPlaneClass::Shutdown()
{
    // 텍스처 해제
    ReleaseTexture();

    // 버퍼 해제
    ShutdownBuffers();

    return;
}

void GroundPlaneClass::Render(ID3D11DeviceContext* deviceContext)
{
    // 버퍼 렌더링
    RenderBuffers(deviceContext);

    return;
}

int GroundPlaneClass::GetIndexCount()
{
    return m_indexCount;
}

ID3D11ShaderResourceView* GroundPlaneClass::GetTexture()
{
    return m_Texture->GetTexture();
}

bool GroundPlaneClass::InitializeBuffers(ID3D11Device* device, float width, float depth, int tilesX, int tilesZ)
{
    VertexType* vertices;
    unsigned long* indices;
    D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;
    HRESULT result;

    // 4개의 정점으로 구성된 간단한 사각형 생성
    m_vertexCount = 4;
    m_indexCount = 6; // 2개의 삼각형 = 6개의 인덱스

    // 정점 배열 생성
    vertices = new VertexType[m_vertexCount];
    if (!vertices)
    {
        return false;
    }

    // 인덱스 배열 생성
    indices = new unsigned long[m_indexCount];
    if (!indices)
    {
        return false;
    }

    // 정점 배열을 데이터로 채우기
    // 넓은 바닥을 위해 바닥 사이즈 확장 (width x depth)
    float halfWidth = width / 2.0f;
    float halfDepth = depth / 2.0f;

    // 정점 위치 설정
    // XZ 평면에 바닥을 생성 (Y=0)
    vertices[0].position = XMFLOAT3(-halfWidth, 0.0f, -halfDepth); // 좌하단
    vertices[1].position = XMFLOAT3(-halfWidth, 0.0f, halfDepth);  // 좌상단
    vertices[2].position = XMFLOAT3(halfWidth, 0.0f, halfDepth);   // 우상단
    vertices[3].position = XMFLOAT3(halfWidth, 0.0f, -halfDepth);  // 우하단

    // 텍스처 좌표 설정 (타일링을 위해 tilesX, tilesZ 만큼 반복)
    vertices[0].texture = XMFLOAT2(0.0f, tilesZ);       // 좌하단
    vertices[1].texture = XMFLOAT2(0.0f, 0.0f);         // 좌상단
    vertices[2].texture = XMFLOAT2(tilesX, 0.0f);       // 우상단
    vertices[3].texture = XMFLOAT2(tilesX, tilesZ);     // 우하단

    // 법선 벡터 설정 (모두 위쪽 방향)
    vertices[0].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
    vertices[1].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
    vertices[2].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
    vertices[3].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);

    // 인덱스 배열 설정 (두 삼각형으로 사각형 형성)
    indices[0] = 0;  // 첫 번째 삼각형
    indices[1] = 1;
    indices[2] = 2;
    indices[3] = 0;  // 두 번째 삼각형
    indices[4] = 2;
    indices[5] = 3;

    // 정점 버퍼 설명 설정
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    // 정점 데이터 지정
    vertexData.pSysMem = vertices;
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    // 정점 버퍼 생성
    result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
    if (FAILED(result))
    {
        return false;
    }

    // 인덱스 버퍼 설명 설정
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = 0;

    // 인덱스 데이터 지정
    indexData.pSysMem = indices;
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    // 인덱스 버퍼 생성
    result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
    if (FAILED(result))
    {
        return false;
    }

    // 배열 해제
    delete[] vertices;
    vertices = 0;
    delete[] indices;
    indices = 0;

    return true;
}

void GroundPlaneClass::ShutdownBuffers()
{
    // 인덱스 버퍼 해제
    if (m_indexBuffer)
    {
        m_indexBuffer->Release();
        m_indexBuffer = 0;
    }

    // 정점 버퍼 해제
    if (m_vertexBuffer)
    {
        m_vertexBuffer->Release();
        m_vertexBuffer = 0;
    }

    return;
}

void GroundPlaneClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
    unsigned int stride;
    unsigned int offset;

    // 정점 버퍼 스트라이드와 오프셋 설정
    stride = sizeof(VertexType);
    offset = 0;

    // 정점 버퍼를 입력 어셈블러에 활성화
    deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

    // 인덱스 버퍼를 입력 어셈블러에 활성화
    deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // 렌더링할 프리미티브 유형 설정: 삼각형
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    return;
}

bool GroundPlaneClass::LoadTexture(ID3D11Device* device, const WCHAR* filename)
{
    bool result;

    // TextureClass 객체 생성
    m_Texture = new TextureClass;
    if (!m_Texture)
    {
        return false;
    }

    // TextureClass 객체 초기화
    result = m_Texture->Initialize(device, filename);
    if (!result)
    {
        return false;
    }

    return true;
}

void GroundPlaneClass::ReleaseTexture()
{
    // TextureClass 객체 해제
    if (m_Texture)
    {
        m_Texture->Shutdown();
        delete m_Texture;
        m_Texture = 0;
    }

    return;
}
