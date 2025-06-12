// BillboardClass.cpp
#include "BillboardClass.h"
#include "textureclass.h"

BillboardClass::BillboardClass() {}
BillboardClass::~BillboardClass() {}

bool BillboardClass::Initialize(ID3D11Device* device, const std::wstring& texturePath) {
    if (!InitializeBuffers(device))
        return false;
    return LoadTexture(device, texturePath);
}

bool BillboardClass::InitializeBuffers(ID3D11Device* device) {
    VertexType vertices[6];
    float width = 2.0f, height = 2.0f;

    vertices[0] = { XMFLOAT3(-width, height, 0.0f),  XMFLOAT2(0.0f, 0.0f) };
    vertices[1] = { XMFLOAT3(width, -height, 0.0f),  XMFLOAT2(1.0f, 1.0f) };
    vertices[2] = { XMFLOAT3(-width, -height, 0.0f), XMFLOAT2(0.0f, 1.0f) };

    vertices[3] = { XMFLOAT3(-width, height, 0.0f),  XMFLOAT2(0.0f, 0.0f) };
    vertices[4] = { XMFLOAT3(width, height, 0.0f),   XMFLOAT2(1.0f, 0.0f) };
    vertices[5] = { XMFLOAT3(width, -height, 0.0f),  XMFLOAT2(1.0f, 1.0f) };

    D3D11_BUFFER_DESC vbDesc = {};
    vbDesc.Usage = D3D11_USAGE_DEFAULT;
    vbDesc.ByteWidth = sizeof(vertices);
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA vbData = {};
    vbData.pSysMem = vertices;

    return SUCCEEDED(device->CreateBuffer(&vbDesc, &vbData, m_vertexBuffer.GetAddressOf()));
}

bool BillboardClass::LoadTexture(ID3D11Device* device, const std::wstring& texturePath) {
    TextureClass* texture = new TextureClass();
    bool result = texture->Initialize(device, texturePath.c_str());
    if (!result) return false;
    m_texture = texture->GetTexture();
    delete texture;
    return true;
}

void BillboardClass::Render(ID3D11DeviceContext* context, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMVECTOR cameraPos) {
    UINT stride = sizeof(VertexType);
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // View-independent billboard alignment would go in the shader or transformation
    // For simplicity, use identity matrix as world
    XMMATRIX world = XMMatrixIdentity();

    // Set shader parameters and draw (pseudo-code, adapt to your shader)
    // m_TextureShader->Render(context, 6, world, viewMatrix, projectionMatrix, m_texture.Get());
}

void BillboardClass::Shutdown() {
    m_vertexBuffer.Reset();
    m_texture.Reset();
}
