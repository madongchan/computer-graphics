////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "d3dclass.h"
#include "cameraclass.h"
#include "modelclass.h"

#include "textureshaderclass.h"
#include "bitmapclass.h"

#include "textclass.h"
#include "fpsclass.h"
#include "cpuclass.h"

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
	bool Frame(int, float);

	D3DClass* GetD3D() { return m_D3D; }

private:
	bool Render(float);

private:
	SceneState m_SceneState;
	D3DClass* m_D3D;
	CameraClass* m_Camera;
	ModelClass* m_Model;

	TextureShaderClass* m_TextureShader;
	BitmapClass* m_BackGround;
	BitmapClass* m_TitleScreen;
	BitmapClass* m_TutorialScreen;

	TextClass* m_Text;

	int m_FPS;
	float m_CPUUsage;
	int m_PolygonCount;
	float m_ScreenWidth;
	float m_ScreenHeight;
};

#endif