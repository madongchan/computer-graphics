#ifndef _BUMPMAPSHADERCLASS_H_
#define _BUMPMAPSHADERCLASS_H_

#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <fstream>

using namespace DirectX;
using namespace std;

class BumpMapShaderClass
{
private:
    struct MatrixBufferType
    {
        XMMATRIX world;
        XMMATRIX view;
        XMMATRIX projection;
    };

    struct LightBufferType
    {
        XMFLOAT4 ambientColor;
        XMFLOAT4 diffuseColor;
        XMFLOAT3 lightDirection;
        float specularPower;
        XMFLOAT4 specularColor;
    };

    struct CameraBufferType
    {
        XMFLOAT3 cameraPosition;
        float padding;
    };

    struct PointLightBufferType
    {
        XMFLOAT4 pointLightPosition[3];
        XMFLOAT4 pointLightColor[3];
        float pointLightIntensity;
        XMFLOAT3 padding;
    };

    // [수정] 4개의 토글을 모두 포함하는 구조체
    struct ToggleBufferType
    {
        float isAmbientOn;
        float isDiffuseOn;
        float isSpecularOn;
        float isNormalMapOn;
    };

public:
    BumpMapShaderClass();
    BumpMapShaderClass(const BumpMapShaderClass&);
    ~BumpMapShaderClass();

    bool Initialize(ID3D11Device*, HWND);
    void Shutdown();

    // [수정] Render 함수 인자 대폭 추가 (LightShader와 동일 수준 + NormalMap 토글)
    bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView**,
        XMFLOAT3, XMFLOAT4, XMFLOAT4, XMFLOAT3, XMFLOAT4, float, // 조명 기본 인자
        bool, bool, bool, bool, // [중요] 4가지 토글 (Ambient, Diffuse, Specular, NormalMap)
        XMFLOAT4[], XMFLOAT4[], float); // 포인트 라이트 인자

private:
    bool InitializeShader(ID3D11Device*, HWND, const WCHAR*);
    void ShutdownShader();
    void OutputShaderErrorMessage(ID3DBlob*, HWND, const WCHAR*);

    bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView**,
        XMFLOAT3, XMFLOAT4, XMFLOAT4, XMFLOAT3, XMFLOAT4, float,
        bool, bool, bool, bool,
        XMFLOAT4[], XMFLOAT4[], float);
    void RenderShader(ID3D11DeviceContext*, int);

private:
    ID3D11VertexShader* m_vertexShader;
    ID3D11PixelShader* m_pixelShader;
    ID3D11InputLayout* m_layout;
    ID3D11SamplerState* m_sampleState;
    ID3D11Buffer* m_matrixBuffer;
    ID3D11Buffer* m_lightBuffer;
    ID3D11Buffer* m_cameraBuffer;
    ID3D11Buffer* m_pointLightBuffer;
    ID3D11Buffer* m_toggleBuffer;
};

#endif