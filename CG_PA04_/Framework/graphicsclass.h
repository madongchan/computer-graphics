////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_

#include <d3d11.h>
#include <vector>
#include <directxmath.h>
using namespace DirectX;

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "d3dclass.h"
#include "cameraclass.h"
#include "modelclass.h"
#include "textureshaderclass.h"
#include "textclass.h"
#include "fpsclass.h"
#include "cpuclass.h"
#include "bitmapclass.h"
#include "billboardclass.h"

/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

enum class SceneState {
	TITLE,
	Tutorial,
	MainScene
};

////////////////////////////////////////////////////////////////////////////////
// Class name: GraphicsClass
////////////////////////////////////////////////////////////////////////////////
class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame();

	// SystemClass 가 텍스트에 표시할 정보를 얻기 위한 메서드
	int GetPolygonCount() const;       // 씬 전체 폴리곤 수

	TextureShaderClass* GetTextureShader() { return m_TextureShader; }
	D3DClass* GetD3D() { return m_D3D; }
	
	struct ModelInfo
	{
		std::wstring modelPath;
		std::wstring texturePath;
		XMFLOAT3 position;
		XMFLOAT3 rotation;
		bool rotateWithScene = true;
		bool found = false;
		bool canBeFound = true;

		ModelClass* modelPtr = nullptr;
	};

	struct BillboardInfo
	{
		XMFLOAT3 position;
		float scale;
	};

	std::vector<ModelInfo> m_modelInfos;
	std::vector<ModelClass*> m_models;

private:
	bool Render(float);
	void RegisterModels();
	bool InitializeModels();

private:
	D3DClass* m_D3D;
	CameraClass* m_Camera;
	ModelClass* m_Model;
	ModelClass* m_Model1;
	ModelClass* m_Model2;
	ModelClass* m_GroundModel;
	TextureShaderClass* m_TextureShader;
	BitmapClass* m_TitleScreen;

	ModelClass* m_BillboardModel;
	XMFLOAT3 m_BillboardPosition;

	BitmapClass* m_Background;
	BitmapClass* m_Billboard;

	TextClass* m_Text;
	int m_screenWidth;
	int m_screenHeight;

	

	TimerClass* m_Timer;
	
	FPSClass* m_FPSClass;      // FPS 계산기
	CPUClass* m_CPUClass;      // CPU 사용량 계산기

	BillboardInfo m_BillboardInfo;

	int      m_FPS;          // 현재 프레임 속도 저장
	int      m_CPUUsage;

	SceneState m_SceneState;
};

#endif