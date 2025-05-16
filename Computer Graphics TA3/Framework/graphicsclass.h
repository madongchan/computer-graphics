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
#include "groundplaneclass.h"
#include "textureshaderclass.h"


/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;


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
	void SetFilterMode(D3D11_FILTER mode);
	void Shutdown();
	bool Frame();

private:
	bool Render(float);

private:
	D3DClass* m_D3D;
	CameraClass* m_Camera;
	ModelClass* m_Model1;
	ModelClass* m_Model2;
	ModelClass* m_Model3;
	GroundPlaneClass* m_Ground; // 새 바닥 클래스 추가
	TextureShaderClass* m_TextureShader;
};

#endif