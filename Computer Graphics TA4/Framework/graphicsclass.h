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
#include "ModelManager.h"

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

// 개별 오브젝트 변환 정보 구조체
struct ObjectTransform {
    XMMATRIX worldMatrix = XMMatrixIdentity();
    ModelType modelType = ModelType::ROAD;
    bool isAnimated = false;
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
    bool Render();
    bool InitializeModels(ID3D11Device* device, HWND hwnd);
    void InitializeInstances();
    void InitializeIndividualObjects();
    void UpdateAnimations(float deltaTime);
    void RenderInstancedObjects(XMMATRIX& viewMatrix, XMMATRIX& projectionMatrix);
    void RenderIndividualObjects(XMMATRIX& viewMatrix, XMMATRIX& projectionMatrix);

    

private:
    SceneState m_SceneState;
    D3DClass* m_D3D;
    CameraClass* m_Camera;
    
    ModelClass* m_HelthBarBillboardModel;

    TextureShaderClass* m_TextureShader;
    BitmapClass* m_BackGround;
    BitmapClass* m_TitleScreen;
    BitmapClass* m_TutorialScreen;

    TextClass* m_Text;

    // 모델 관리
    ModelManager* m_ModelManager;

    // 인스턴싱 데이터
    std::vector<XMMATRIX> m_RoadInstances;
    std::vector<XMMATRIX> m_TreeInstances;

    // 개별 오브젝트들
    std::vector<ObjectTransform> m_IndividualObjects;

    // 애니메이션
    float m_AnimationTime;
    float m_LastFrameTime;

    int m_FPS;
    float m_CPUUsage;
    long m_PolygonCount;
    float m_ScreenWidth;
    float m_ScreenHeight;
};

#endif
