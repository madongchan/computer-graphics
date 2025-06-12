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

	// SystemClass �� �ؽ�Ʈ�� ǥ���� ������ ��� ���� �޼���
	int GetPolygonCount() const;       // �� ��ü ������ ��

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
	
	FPSClass* m_FPSClass;      // FPS ����
	CPUClass* m_CPUClass;      // CPU ��뷮 ����

	BillboardInfo m_BillboardInfo;

	int      m_FPS;          // ���� ������ �ӵ� ����
	int      m_CPUUsage;

	SceneState m_SceneState;
};

#endif