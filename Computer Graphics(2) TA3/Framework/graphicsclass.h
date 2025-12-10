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
#include "textclass.h"
#include "textureshaderclass.h"
#include "fpsclass.h"
#include "cpuclass.h"
#include "bitmapclass.h"

#include "skyboxclass.h"
#include "skyboxshaderclass.h"
#include "bumpmapshaderclass.h"
#include "texturearrayclass.h"

#include <vector> // 필수

// 1. 오브젝트 구조체 정의
struct GameObject
{
	ModelClass* model;  // 모델 원본 포인터
	XMFLOAT3 position;  // 위치
	XMFLOAT3 rotation;  // 회전
	XMFLOAT3 scale;     // 크기
};
/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

// Scene state enum
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
	bool Frame(InputClass*, double deltaTime, int fps, float cpuUsage);

private:
	// HandleInput 헬퍼 함수 추가
	void HandleInput(InputClass* Input, double deltaTime);
	bool Render(wchar_t* debugText);

	// 충돌 검사 헬퍼 함수 선언
	bool CheckAABBCollision(XMFLOAT3 min1, XMFLOAT3 max1, XMFLOAT3 min2, XMFLOAT3 max2);
	void RenderDebugAABB(XMFLOAT3 min, XMFLOAT3 max);
	void GetWorldAABB(const GameObject& obj, XMFLOAT3& outMin, XMFLOAT3& outMax);

private:
	SceneState m_SceneState;
	D3DClass* m_D3D;
	CameraClass* m_Camera;

	// 2. 배치된 물체 리스트 (여기에 수십 개의 물체가 담깁니다)
	GameObject m_Player; // 플레이어 객체 (모델, 위치, 회전 정보 포함)
	std::vector<GameObject> m_GameObjects;
	std::vector<GameObject> m_GroundTiles;

	// 3. 역할별 모델 리소스 변수 (여기에 FBX를 로딩합니다)
	ModelClass* m_ModelWall;       // 벽 (cube.fbx 추천)
	ModelClass* m_ModelGround;     // 바닥 (Floor.fbx)

	ModelClass* m_ModelCrate;      // 보급상자 (중앙 목표)
	ModelClass* m_ModelBarricade;  // 방어벽/바리케이드
	ModelClass* m_ModelObstacle;   // 작은 장애물 (타이어, 드럼통 등)
	ModelClass* m_ModelCar;        // 차량 (엄폐물)
	ModelClass* m_ModelTent;       // 텐트 (스폰 지점)
	ModelClass* m_ModelTree;       // 나무 (외곽 장식)

	TextureShaderClass* m_DebugShader;
	
	LightShaderClass* m_LightShader;
	LightClass* m_Light;

	TextureShaderClass* m_TextureShader;
	ModelClass* m_DebugModel;

	BitmapClass* m_TitleScreen;
	BitmapClass* m_TutorialScreen;

	// 텍스트 렌더링용 클래스 추가 ★
	TextClass* m_Text;

	SkyboxClass* m_Skybox;
	SkyboxShaderClass* m_SkyboxShader;

	BumpMapShaderClass* m_BumpMapShader;
	TextureArrayClass* m_GroundTextures; // (지면용: 색상 + 노멀맵)

	float m_rotation;
	// Lighting toggles
	bool m_isAmbientOn;
	bool m_isDiffuseOn;
	bool m_isSpecularOn;
	// Normal map toggle
	bool m_isNormalMapOn;

	// 포인트 라이트 강도 변수 추가 ★ ---
	float m_pointLightIntensity; // 8, 9번 키로 조절할 값

	// --- 카메라/마우스 속도 상수 추가 ★ ---
	const float m_cameraMoveSpeed = 10.0f;
	const float m_mouseSensitivity = 0.005f;

	bool m_isCameraControlMode = true;

	int m_FPS;
	float m_CPUUsage;
	long m_PolygonCount;
	float m_ScreenWidth;
	float m_ScreenHeight;

	bool m_showDebug; // 디버그 렌더링 토글용 변수
};

#endif