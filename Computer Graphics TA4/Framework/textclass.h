#pragma once

#include <d2d1.h>
#include <dwrite.h>
#include <wrl.h>   // ComPtr 용

// forward declaration
struct ID3D11Device;
struct IDXGISwapChain;
struct ID3D11DeviceContext;

class TextClass
{
public:
    TextClass();
    ~TextClass();

    // GraphicsClass.Initialize() 종료 직후에 호출
    // d3dDevice, swapChain, deviceContext 만 넘겨주면 내부에서 D2D/DirectWrite 초기화
    bool Initialize(ID3D11Device* d3dDevice, IDXGISwapChain* swapChain);

    // 한 프레임마다 Begin → DrawTexts → End
    void BeginDraw();
    void DrawTextLine(const wchar_t* text, float x, float y);
    void EndDraw();

    // 해제
    void Shutdown();

private:
    // D2D/DirectWrite 관련 COM 포인터
    Microsoft::WRL::ComPtr<ID2D1Factory>       m_d2dFactory;
    Microsoft::WRL::ComPtr<ID2D1RenderTarget> m_d2dRenderTarget;
    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_brush;

    Microsoft::WRL::ComPtr<IDWriteFactory>     m_dwriteFactory;
    Microsoft::WRL::ComPtr<IDWriteTextFormat>  m_textFormat;
};
