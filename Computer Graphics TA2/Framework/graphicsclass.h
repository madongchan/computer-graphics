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
#include "colorshaderclass.h"

#include <vector>
#include <memory> 

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
	void Shutdown();
	bool Frame(float);
	void SetBrightness(float brightness);
	void SetBackColor(char backColor);
	void SetFillMode(D3D11_FILL_MODE fillmode);
	void SetToggleCullMode();

private:
	bool Render(float);

private:
	D3DClass* m_D3D;
	CameraClass* m_Camera;
	
	// Use a vector of unique_ptr to manage models
	vector<unique_ptr<ModelClass>> m_Models;

	ColorShaderClass* m_ColorShader;

	float brightness;

	char backcolor;
};

#endif