#pragma once

#include <d2d1.h>
#include <dwrite.h>
#include <wrl.h>   // ComPtr ��

// forward declaration
struct ID3D11Device;
struct IDXGISwapChain;
struct ID3D11DeviceContext;

class TextClass
{
public:
    TextClass();
    ~TextClass();

    // GraphicsClass.Initialize() ���� ���Ŀ� ȣ��
    // d3dDevice, swapChain, deviceContext �� �Ѱ��ָ� ���ο��� D2D/DirectWrite �ʱ�ȭ
    bool Initialize(ID3D11Device* d3dDevice, IDXGISwapChain* swapChain);

    // �� �����Ӹ��� Begin �� DrawTexts �� End
    void BeginDraw();
    void DrawTextLine(const wchar_t* text, float x, float y);
    void EndDraw();

    // ����
    void Shutdown();

private:
    // D2D/DirectWrite ���� COM ������
    Microsoft::WRL::ComPtr<ID2D1Factory>       m_d2dFactory;
    Microsoft::WRL::ComPtr<ID2D1RenderTarget> m_d2dRenderTarget;
    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_brush;

    Microsoft::WRL::ComPtr<IDWriteFactory>     m_dwriteFactory;
    Microsoft::WRL::ComPtr<IDWriteTextFormat>  m_textFormat;
};
