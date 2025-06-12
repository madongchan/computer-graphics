#include "textclass.h"
#include <d2d1_1.h>
#include <dxgi1_2.h>
#include <d3d11.h>

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

using Microsoft::WRL::ComPtr;

TextClass::TextClass()
{
}

TextClass::~TextClass()
{
    Shutdown();
}

bool TextClass::Initialize(ID3D11Device* d3dDevice, IDXGISwapChain* swapChain)
{
    HRESULT hr;

    // 1) D2D 팩토리 생성
    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, m_d2dFactory.GetAddressOf());
    if (FAILED(hr)) return false;

    // 2) DXGI 백버퍼(렌더 타겟)를 ID2D1RenderTarget 으로 연결하기 위해 먼저 DXGI surface 얻기
    ComPtr<IDXGISurface> dxgiBackBuffer;
    hr = swapChain->GetBuffer(0, __uuidof(IDXGISurface), reinterpret_cast<void**>(dxgiBackBuffer.GetAddressOf()));
    if (FAILED(hr)) return false;

    // 3) D2D RenderTarget 생성 (D2D1_RENDER_TARGET_PROPERTIES + DXGI 서피스)
    D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_DEFAULT,
        D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
        0, 0
    );

    hr = m_d2dFactory->CreateDxgiSurfaceRenderTarget(
        dxgiBackBuffer.Get(),
        &rtProps,
        m_d2dRenderTarget.GetAddressOf()
    );
    if (FAILED(hr)) return false;

    // 4) SolidColorBrush (흰색)
    hr = m_d2dRenderTarget->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::White),
        m_brush.GetAddressOf()
    );
    if (FAILED(hr)) return false;

    // 5) DirectWrite 팩토리
    hr = DWriteCreateFactory(
        DWRITE_FACTORY_TYPE_SHARED,
        __uuidof(IDWriteFactory),
        reinterpret_cast<IUnknown**>(m_dwriteFactory.GetAddressOf())
    );
    if (FAILED(hr)) return false;

    // 6) 텍스트 포맷 (폰트 “Segoe UI”, 16pt, 좌측정렬)
    hr = m_dwriteFactory->CreateTextFormat(
        L"Segoe UI",        // font family
        nullptr,            // font collection (nullptr → 시스템 기본)
        DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        36.0f,              // font size
        L"",                // locale
        m_textFormat.GetAddressOf()
    );
    if (FAILED(hr)) return false;

    // 좌측 정렬, 상단 정렬
    m_textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
    m_textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

    return true;
}

void TextClass::BeginDraw()
{
    // D2D 레이어 투명도 유지 위해 별도 Clear 하지 않음
    m_d2dRenderTarget->BeginDraw();
}

void TextClass::DrawTextLine(const wchar_t* text, float x, float y)
{
    if (!m_d2dRenderTarget) return;

    m_d2dRenderTarget->BeginDraw();

    // 출력 영역
    D2D1_RECT_F layoutRect = D2D1::RectF(x, y, x + 500.f, y + 50.f); // 너무 크면 잘릴 수 있음
    m_d2dRenderTarget->DrawTextW(
        text,
        static_cast<UINT32>(wcslen(text)),
        m_textFormat.Get(),
        layoutRect,
        m_brush.Get()
    );

    HRESULT hr = m_d2dRenderTarget->EndDraw();
    if (FAILED(hr)) {
        OutputDebugStringW(L"[DrawTextLine] EndDraw 실패\n");
    }
}

void TextClass::EndDraw()
{
    m_d2dRenderTarget->EndDraw();
}

void TextClass::Shutdown()
{
    if (m_d2dRenderTarget)     m_d2dRenderTarget.Reset();
    if (m_brush)               m_brush.Reset();
    if (m_d2dFactory)          m_d2dFactory.Reset();
    if (m_textFormat)          m_textFormat.Reset();
    if (m_dwriteFactory)       m_dwriteFactory.Reset();
}
