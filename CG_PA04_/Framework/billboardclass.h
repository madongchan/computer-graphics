// BillboardClass.h
#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include <string>

using namespace DirectX;
using Microsoft::WRL::ComPtr;

class BillboardClass {
public:
    BillboardClass();
    ~BillboardClass();

    bool Initialize(ID3D11Device* device, const std::wstring& texturePath);
    void Render(ID3D11DeviceContext* context, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMVECTOR cameraPos);
    void Shutdown();

private:
    struct VertexType {
        XMFLOAT3 position;
        XMFLOAT2 texCoord;
    };

    bool InitializeBuffers(ID3D11Device* device);
    bool LoadTexture(ID3D11Device* device, const std::wstring& texturePath);

private:
    ComPtr<ID3D11Buffer> m_vertexBuffer;
    ComPtr<ID3D11ShaderResourceView> m_texture;
};