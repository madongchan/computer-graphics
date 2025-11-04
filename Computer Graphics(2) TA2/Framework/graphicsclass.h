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
#include "lightshaderclass.h" 
#include "lightclass.h"      
#include "inputclass.h"

#include "skyboxclass.h"
#include "skyboxshaderclass.h"
#include "bumpmapshaderclass.h"
#include "texturearrayclass.h"

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
	bool Frame(InputClass*, double deltaTime);

private:
	// HandleInput 헬퍼 함수 추가
	void HandleInput(InputClass* Input, double deltaTime);
	bool Render(float);

private:
	D3DClass* m_D3D;
	CameraClass* m_Camera;
	ModelClass* m_Model1;
	ModelClass* m_Model2;
	ModelClass* m_Model3;
	ModelClass* m_GroundModel;
	
	LightShaderClass* m_LightShader;
	LightClass* m_Light;

	SkyboxClass* m_Skybox;
	SkyboxShaderClass* m_SkyboxShader;

	BumpMapShaderClass* m_BumpMapShader;
	TextureArrayClass* m_GroundTextures; // (지면용: 색상 + 노멀맵)

	float m_rotation;
	// Lighting toggles
	bool m_isAmbientOn;
	bool m_isDiffuseOn;
	bool m_isSpecularOn;

	// 포인트 라이트 강도 변수 추가 ★ ---
	float m_pointLightIntensity; // 8, 9번 키로 조절할 값

	// --- 카메라/마우스 속도 상수 추가 ★ ---
	const float m_cameraMoveSpeed = 10.0f;
	const float m_mouseSensitivity = 0.005f;
};

#endif