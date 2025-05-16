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

    // ���� �ʱ�ȭ
    result = InitializeBuffers(device, width, depth, tilesX, tilesZ);
    if (!result)
    {
        return false;
    }

    // �ؽ�ó �ε�
    result = LoadTexture(device, textureFilename);
    if (!result)
    {
        return false;
    }

    return true;
}

void GroundPlaneClass::Shutdown()
{
    // �ؽ�ó ����
    ReleaseTexture();

    // ���� ����
    ShutdownBuffers();

    return;
}

void GroundPlaneClass::Render(ID3D11DeviceContext* deviceContext)
{
    // ���� ������
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

    // 4���� �������� ������ ������ �簢�� ����
    m_vertexCount = 4;
    m_indexCount = 6; // 2���� �ﰢ�� = 6���� �ε���

    // ���� �迭 ����
    vertices = new VertexType[m_vertexCount];
    if (!vertices)
    {
        return false;
    }

    // �ε��� �迭 ����
    indices = new unsigned long[m_indexCount];
    if (!indices)
    {
        return false;
    }

    // ���� �迭�� �����ͷ� ä���
    // ���� �ٴ��� ���� �ٴ� ������ Ȯ�� (width x depth)
    float halfWidth = width / 2.0f;
    float halfDepth = depth / 2.0f;

    // ���� ��ġ ����
    // XZ ��鿡 �ٴ��� ���� (Y=0)
    vertices[0].position = XMFLOAT3(-halfWidth, 0.0f, -halfDepth); // ���ϴ�
    vertices[1].position = XMFLOAT3(-halfWidth, 0.0f, halfDepth);  // �»��
    vertices[2].position = XMFLOAT3(halfWidth, 0.0f, halfDepth);   // ����
    vertices[3].position = XMFLOAT3(halfWidth, 0.0f, -halfDepth);  // ���ϴ�

    // �ؽ�ó ��ǥ ���� (Ÿ�ϸ��� ���� tilesX, tilesZ ��ŭ �ݺ�)
    vertices[0].texture = XMFLOAT2(0.0f, tilesZ);       // ���ϴ�
    vertices[1].texture = XMFLOAT2(0.0f, 0.0f);         // �»��
    vertices[2].texture = XMFLOAT2(tilesX, 0.0f);       // ����
    vertices[3].texture = XMFLOAT2(tilesX, tilesZ);     // ���ϴ�

    // ���� ���� ���� (��� ���� ����)
    vertices[0].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
    vertices[1].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
    vertices[2].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
    vertices[3].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);

    // �ε��� �迭 ���� (�� �ﰢ������ �簢�� ����)
    indices[0] = 0;  // ù ��° �ﰢ��
    indices[1] = 1;
    indices[2] = 2;
    indices[3] = 0;  // �� ��° �ﰢ��
    indices[4] = 2;
    indices[5] = 3;

    // ���� ���� ���� ����
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    // ���� ������ ����
    vertexData.pSysMem = vertices;
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    // ���� ���� ����
    result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
    if (FAILED(result))
    {
        return false;
    }

    // �ε��� ���� ���� ����
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = 0;

    // �ε��� ������ ����
    indexData.pSysMem = indices;
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    // �ε��� ���� ����
    result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
    if (FAILED(result))
    {
        return false;
    }

    // �迭 ����
    delete[] vertices;
    vertices = 0;
    delete[] indices;
    indices = 0;

    return true;
}

void GroundPlaneClass::ShutdownBuffers()
{
    // �ε��� ���� ����
    if (m_indexBuffer)
    {
        m_indexBuffer->Release();
        m_indexBuffer = 0;
    }

    // ���� ���� ����
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

    // ���� ���� ��Ʈ���̵�� ������ ����
    stride = sizeof(VertexType);
    offset = 0;

    // ���� ���۸� �Է� ������� Ȱ��ȭ
    deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

    // �ε��� ���۸� �Է� ������� Ȱ��ȭ
    deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // �������� ������Ƽ�� ���� ����: �ﰢ��
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    return;
}

bool GroundPlaneClass::LoadTexture(ID3D11Device* device, const WCHAR* filename)
{
    bool result;

    // TextureClass ��ü ����
    m_Texture = new TextureClass;
    if (!m_Texture)
    {
        return false;
    }

    // TextureClass ��ü �ʱ�ȭ
    result = m_Texture->Initialize(device, filename);
    if (!result)
    {
        return false;
    }

    return true;
}

void GroundPlaneClass::ReleaseTexture()
{
    // TextureClass ��ü ����
    if (m_Texture)
    {
        m_Texture->Shutdown();
        delete m_Texture;
        m_Texture = 0;
    }

    return;
}
