#ifndef _GROUNDPLANECLASS_H_
#define _GROUNDPLANECLASS_H_

#include <d3d11.h>
#include <DirectXMath.h>
#include "textureclass.h"

using namespace DirectX;

class GroundPlaneClass
{
private:
    struct VertexType
    {
        XMFLOAT3 position;
        XMFLOAT2 texture;
        XMFLOAT3 normal;
    };

public:
    GroundPlaneClass();
    GroundPlaneClass(const GroundPlaneClass&);
    ~GroundPlaneClass();

    bool Initialize(ID3D11Device*, float width, float depth, int tilesX, int tilesZ, const WCHAR* textureFilename);
    void Shutdown();
    void Render(ID3D11DeviceContext*);
    int GetIndexCount();
    ID3D11ShaderResourceView* GetTexture();

private:
    bool InitializeBuffers(ID3D11Device*, float width, float depth, int tilesX, int tilesZ);
    void ShutdownBuffers();
    void RenderBuffers(ID3D11DeviceContext*);
    bool LoadTexture(ID3D11Device*, const WCHAR*);
    void ReleaseTexture();

private:
    ID3D11Buffer* m_vertexBuffer;
    ID3D11Buffer* m_indexBuffer;
    int m_vertexCount;
    int m_indexCount;
    TextureClass* m_Texture;
};

#endif
