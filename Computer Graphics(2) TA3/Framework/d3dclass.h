////////////////////////////////////////////////////////////////////////////////
// Filename: d3dclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _D3DCLASS_H_
#define _D3DCLASS_H_


/////////////
// LINKING //
/////////////
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

//////////////
// INCLUDES //
//////////////
#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <directxmath.h>

#include "AlignedAllocationPolicy.h"

using namespace DirectX;

////////////////////////////////////////////////////////////////////////////////
// Class name: D3DClass
////////////////////////////////////////////////////////////////////////////////
class D3DClass : public AlignedAllocationPolicy<16>
{
public:
	D3DClass();
	D3DClass(const D3DClass&);
	~D3DClass();

	bool Initialize(int, int, bool, HWND, bool, float, float);
	void Shutdown();
	
	void BeginScene(float, float, float, float);
	void EndScene();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();
	IDXGISwapChain* GetSwapChain() { return m_swapChain; }
	IDXGISwapChain* GetSwapChain() const { return m_swapChain; }

	// 스카이박스용 렌더 상태 제어 함수
	void TurnOnNoCulling(); // 컬링 끄기 : 앞면/뒷면 모두 렌더링
	void TurnOnBackCulling(); // 컬링 켜기 : 뒷면 렌더링 안함
	void TurnOnDepthLessEqual(); // 깊이 함수 LESS_EQUAL 설정
	void TurnOffDefaultDepth(); // 깊이 함수 기본값 설정
	// ---

	void GetProjectionMatrix(XMMATRIX&);
	void GetWorldMatrix(XMMATRIX&);
	void GetOrthoMatrix(XMMATRIX&);

	void GetVideoCardInfo(char*, int&);

	void TurnZBufferOn();
	void TurnZBufferOff();

	void EnableAlphaBlending();
	void DisableAlphaBlending();

	void TurnOnWireframe();  // 추가: 선만 그리는 모드
	void TurnOffWireframe(); // 추가: 다시 색을 채우는 모드 (기본)

private:
	bool m_vsync_enabled;
	int m_videoCardMemory;
	char m_videoCardDescription[128];
	IDXGISwapChain* m_swapChain;
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_deviceContext;
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11Texture2D* m_depthStencilBuffer; // 깊이버퍼
	ID3D11DepthStencilState* m_depthDisabledStencilState; // 깊이버퍼 비활성화용 렌더 상태 객체
	ID3D11DepthStencilState* m_depthStencilState; // 깊이버퍼 활성화용 렌더 상태 객체
	ID3D11DepthStencilView* m_depthStencilView; // 깊이버퍼 뷰
	ID3D11RasterizerState* m_rasterState; // 래스터라이저 상태 객체
	ID3D11RasterizerState* m_rasterStateWireframe; // 추가: 선만 그리는 렌더 상태 객체

	// 알파 블렌딩용 렌더 상태 객체들
	ID3D11BlendState* m_alphaEnableBlendingState;
	ID3D11BlendState* m_alphaDisableBlendingState;
	// 스카이박스용 렌더 상태 객체들
	ID3D11RasterizerState* m_rasterStateNoCulling;
	ID3D11DepthStencilState* m_depthStateLessEqual;

	XMMATRIX m_projectionMatrix;
	XMMATRIX m_worldMatrix;
	XMMATRIX m_orthoMatrix;
};

#endif