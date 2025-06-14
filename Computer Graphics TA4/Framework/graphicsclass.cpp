////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"
#include "inputclass.h"
#include <cmath>

GraphicsClass::GraphicsClass()
{
    m_D3D = 0;
    m_Camera = 0;
    m_HelthBarBillboardModel = 0;

    m_TextureShader = 0;
    m_BackGround = 0;
    m_TitleScreen = 0;
    m_TutorialScreen = 0;

    m_Text = 0;
    m_ModelManager = 0;  // 새로 추가

    // Initialize the scene state
    m_FPS = 0;
    m_CPUUsage = 0.0f;
    m_PolygonCount = 0;
    m_ScreenWidth = 0;
    m_ScreenHeight = 0;
    m_SceneState = SceneState::MainScene;

    m_AnimationTime = 0.0f;
    m_LastFrameTime = 0.0f;

    
}

GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}

GraphicsClass::~GraphicsClass()
{
}

bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
    bool result;

    m_ScreenWidth = screenWidth;
    m_ScreenHeight = screenHeight;

    // Create the Direct3D object.
    m_D3D = new D3DClass;
    if (!m_D3D)
    {
        return false;
    }

    // Initialize the Direct3D object.
    result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
        return false;
    }

    // Create the camera object.
    m_Camera = new CameraClass;
    if (!m_Camera)
    {
        return false;
    }

    // Set the initial position of the camera.
    m_Camera->SetPosition(0.0f, 5.0f, -50.0f);

    // Create the texture shader object.
    m_TextureShader = new TextureShaderClass;
    if (!m_TextureShader)
    {
        return false;
    }

    // Initialize the texture shader object.
    result = m_TextureShader->Initialize(m_D3D->GetDevice(), hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
        return false;
    }

    // 모델들 초기화
    result = InitializeModels(m_D3D->GetDevice(), hwnd);
    if (!result)
    {
        return false;
    }

    // 배경 화면들 초기화 (기존 코드와 동일)
    m_BackGround = new BitmapClass;
    if (!m_BackGround) return false;
    result = m_BackGround->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight, L"./data/BackGround.dds", screenWidth, screenHeight);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the background bitmap object.", L"Error", MB_OK);
        return false;
    }

    m_TitleScreen = new BitmapClass;
    if (!m_TitleScreen)
    {
        return false;
    }
    result = m_TitleScreen->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight, L"./data/Title.dds", screenWidth, screenHeight);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the TitleScreen bitmap object.", L"Error", MB_OK);
        return false;
    }

    m_TutorialScreen = new BitmapClass;
    if (!m_TutorialScreen)
    {
        return false;
    }
    result = m_TutorialScreen->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight, L"./data/Tutorial.dds", screenWidth, screenHeight);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the m_TurorialScreen bitmap object.", L"Error", MB_OK);
        return false;
    }

    // 빌보드 모델 객체를 만듭니다.
    m_HelthBarBillboardModel = new ModelClass;
    if (!m_HelthBarBillboardModel)
    {
        return false;
    }

    // 빌보드 모델 객체를 초기화합니다.
    if (!m_HelthBarBillboardModel->Initialize(m_D3D->GetDevice(), L"./data/HELTHBAR.obj", L"./data/red_image.dds"))
    {
        MessageBox(hwnd, L"Could not initialize the billboard model object.", L"Error", MB_OK);
        return false;
    }

    m_Text = new TextClass;
    if (!m_Text) return false;

    result = m_Text->Initialize(
        m_D3D->GetDevice(),
        m_D3D->GetSwapChain()
    );
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize TextClass.", L"Error", MB_OK);
        return false;
    }

    return true;
}

bool GraphicsClass::InitializeModels(ID3D11Device* device, HWND hwnd)
{
    bool result;

    // ModelManager 초기화
    m_ModelManager = new ModelManager;
    if (!m_ModelManager)
    {
        return false;
    }

    result = m_ModelManager->Initialize(device, hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize ModelManager.", L"Error", MB_OK);
        return false;
    }

    // 인스턴스들 초기화
    InitializeInstances();

    // 개별 오브젝트들 초기화
    InitializeIndividualObjects();

    return true;
}

void GraphicsClass::InitializeInstances()
{
    // Road 인스턴스들 - 격자 패턴으로 25개 배치
    int roadGridSize = 5;
    for (int i = 0; i < roadGridSize * roadGridSize; i++)
    {
        int x = i % roadGridSize;
        int z = i / roadGridSize;

        XMMATRIX roadMatrix = XMMatrixTranslation(
            x * 20.0f - roadGridSize * 5.0f,
            0.0f,
            z * 20.0f - roadGridSize * 5.0f
        );
        m_RoadInstances.push_back(roadMatrix);
    }

    // Tree 인스턴스들 - 랜덤하게 20개 배치
    for (int i = 0; i < 20; i++)
    {
        float x = static_cast<float>(rand() % 200 - 100);
        float z = static_cast<float>(rand() % 200 - 100);
        float scale = 0.8f + static_cast<float>(rand() % 40) / 100.0f;

        XMMATRIX treeMatrix = XMMatrixScaling(scale, scale, scale) *
            XMMatrixTranslation(x, 0.0f, z);
        m_TreeInstances.push_back(treeMatrix);
    }
}

void GraphicsClass::InitializeIndividualObjects()
{
    // 사람 오브젝트 - 원형 경로 애니메이션
    ObjectTransform person;
    person.worldMatrix = XMMatrixTranslation(20.0f, 0.0f, 0.0f);
    person.modelType = ModelType::PERSON;
    person.velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
    person.rotationSpeed = XMFLOAT3(0.0f, 1.0f, 0.0f);
    person.isAnimated = true;
    m_IndividualObjects.push_back(person);

    // 자동차 오브젝트 - 직선 왕복 애니메이션
    ObjectTransform car;
    car.worldMatrix = XMMatrixTranslation(0.0f, 0.0f, -10.0f);
    car.modelType = ModelType::CAR;
    car.velocity = XMFLOAT3(2.0f, 0.0f, 0.0f);
    car.rotationSpeed = XMFLOAT3(0.0f, 0.0f, 0.0f);
    car.isAnimated = true;
    m_IndividualObjects.push_back(car);

    // 건물 오브젝트 - Y축 회전 애니메이션
    ObjectTransform building;
    XMMATRIX scaleBuildMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
    building.worldMatrix = scaleBuildMatrix * XMMatrixTranslation(35.0f, 9.0f, 35.0f);
    building.modelType = ModelType::BUILDING;
    building.isAnimated = true;
    m_IndividualObjects.push_back(building);

    // 정적 오브젝트들 추가

    // 오토바이 오브젝트
    ObjectTransform bike;
    bike.worldMatrix = XMMatrixTranslation(-15.0f, 0.0f, 5.0f);
    bike.modelType = ModelType::BIKE;
    bike.isAnimated = false;
    m_IndividualObjects.push_back(bike);

    // 벽 오브젝트
    ObjectTransform wall;
    wall.worldMatrix = XMMatrixTranslation(30.0f, 0.0f, 0.0f);
    wall.modelType = ModelType::WALL;
    wall.isAnimated = false;
    m_IndividualObjects.push_back(wall);

    // TireStack 오브젝트
    ObjectTransform tireStack;
    tireStack.worldMatrix = XMMatrixTranslation(-30.0f, 0.0f, -10.0f);
    tireStack.modelType = ModelType::TIRESTACK;
    tireStack.isAnimated = false;
    m_IndividualObjects.push_back(tireStack);

    // FENCE 오브젝트
    ObjectTransform fence;
    fence.worldMatrix = XMMatrixTranslation(0.0f, 0.0f, 30.0f);
    fence.modelType = ModelType::FENCE;
    fence.isAnimated = false;
    m_IndividualObjects.push_back(fence);

    // SIGN 오브젝트
    ObjectTransform sign;
    sign.worldMatrix = XMMatrixTranslation(0.0f, 0.0f, 50.0f);
    sign.modelType = ModelType::SIGN;
    sign.isAnimated = false;
    m_IndividualObjects.push_back(sign);
}

void GraphicsClass::UpdateAnimations(float deltaTime)
{
    m_AnimationTime += deltaTime;

    for (size_t i = 0; i < m_IndividualObjects.size(); i++)
    {
        if (!m_IndividualObjects[i].isAnimated) continue;

        switch (i)
        {
        case 0: // 사람 - 원형 경로
        {
            float radius = 20.0f;
            float speed = 1.0f;
            float angle = m_AnimationTime * speed;
            m_IndividualObjects[i].worldMatrix =
                XMMatrixTranslation(cos(angle) * radius, 0.0f, sin(angle) * radius);
        }
        break;

        case 1: // 자동차 - 직선 왕복
        {
            float distance = 30.0f;
            float speed = 2.0f;
            float position = sin(m_AnimationTime * speed) * distance;
            m_IndividualObjects[i].worldMatrix =
                XMMatrixRotationY(XM_PI * 0.5f) *
                XMMatrixTranslation(position, 0.0f, -10.0f);
        }
        break;

        case 9: // SIGN - Y축 회전
        {
            float rotationSpeed = 3.0f;
            m_IndividualObjects[i].worldMatrix =
                XMMatrixRotationY(m_AnimationTime * rotationSpeed) *
                XMMatrixTranslation(0.0f, 20.0f, 25.0f);
        }
        break;
        }
    }
}



void GraphicsClass::RenderInstancedObjects(XMMATRIX& viewMatrix, XMMATRIX& projectionMatrix)
{
    // 스케일링 행렬 생성
    XMMATRIX scaleMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f);

    // Road 인스턴스들 렌더링
    ModelClass* roadModel = m_ModelManager->GetModel(ModelType::ROAD);
    if (roadModel)
    {
        roadModel->Render(m_D3D->GetDeviceContext());

        for (const auto& worldMatrix : m_RoadInstances)
        {
            XMMATRIX scaledMatrix = scaleMatrix * worldMatrix;
            m_TextureShader->Render(m_D3D->GetDeviceContext(),
                roadModel->GetIndexCount(),
                scaledMatrix, viewMatrix, projectionMatrix,
                roadModel->GetTexture());
        }
    }

    // Tree 인스턴스들도 동일하게 처리
    ModelClass* treeModel = m_ModelManager->GetModel(ModelType::TREE);
    if (treeModel)
    {
        treeModel->Render(m_D3D->GetDeviceContext());

        for (const auto& worldMatrix : m_TreeInstances)
        {
            XMMATRIX scaledMatrix = scaleMatrix * worldMatrix;
            m_TextureShader->Render(m_D3D->GetDeviceContext(),
                treeModel->GetIndexCount(),
                scaledMatrix, viewMatrix, projectionMatrix,
                treeModel->GetTexture());
        }
    }
}


void GraphicsClass::RenderIndividualObjects(XMMATRIX& viewMatrix, XMMATRIX& projectionMatrix)
{
    for (const auto& obj : m_IndividualObjects)
    {
        ModelClass* model = m_ModelManager->GetModel(obj.modelType);
        if (model)
        {
            model->Render(m_D3D->GetDeviceContext());

            // 100분의 1로 축소하는 스케일링 행렬 생성
            XMMATRIX scaleMatrix = XMMatrixScaling(0.05f, 0.05f, 0.05f);
            XMMATRIX finalWorldMatrix = scaleMatrix * obj.worldMatrix;

            m_TextureShader->Render(m_D3D->GetDeviceContext(),
                model->GetIndexCount(),
                finalWorldMatrix, viewMatrix, projectionMatrix,
                model->GetTexture());
        }
    }
}


void GraphicsClass::Shutdown()
{
    // ModelManager 해제
    if (m_ModelManager)
    {
        m_ModelManager->Shutdown();
        delete m_ModelManager;
        m_ModelManager = 0;
    }

    // 기존 해제 코드들...
    if (m_Text)
    {
        m_Text->Shutdown();
        delete m_Text;
        m_Text = nullptr;
    }

    if (m_HelthBarBillboardModel)
    {
        m_HelthBarBillboardModel->Shutdown();
        delete m_HelthBarBillboardModel;
        m_HelthBarBillboardModel = 0;
    }

    if (m_Camera)
    {
        delete m_Camera;
        m_Camera = 0;
    }

    if (m_D3D)
    {
        m_D3D->Shutdown();
        delete m_D3D;
        m_D3D = 0;
    }

    if (m_BackGround)
    {
        m_BackGround->Shutdown();
        delete m_BackGround;
        m_BackGround = 0;
    }
    if (m_TitleScreen)
    {
        m_TitleScreen->Shutdown();
        delete m_TitleScreen;
        m_TitleScreen = 0;
    }
    if (m_TutorialScreen)
    {
        m_TutorialScreen->Shutdown();
        delete m_TutorialScreen;
        m_TutorialScreen = 0;
    }

    if (m_TextureShader)
    {
        m_TextureShader->Shutdown();
        delete m_TextureShader;
        m_TextureShader = 0;
    }

    return;
}

bool GraphicsClass::Frame(int fps, float cpuUsage)
{
    bool result = true;

    m_FPS = fps;
    m_CPUUsage = cpuUsage;
    static float rotation = 0.0f;

    // 시간 계산
     // 시간 계산
    static float lastTime = GetTickCount() / 1000.0f;
    float currentTime = GetTickCount() / 1000.0f;
    float deltaTime = currentTime - lastTime;
    lastTime = currentTime;

    // 키 상태 업데이트
    InputClass::UpdateKeyStates();

    if (InputClass::IsAnyKeyPressed())
    {
        if (m_SceneState == SceneState::TITLE)
        {
            m_SceneState = SceneState::Tutorial;
        }
        else if (m_SceneState == SceneState::Tutorial)
        {
            m_SceneState = SceneState::MainScene;
        }
    }

    // 메인 씬에서만 카메라 이동과 애니메이션 처리
    if (m_SceneState == SceneState::MainScene)
    {
        const float moveSpeed = 0.5f;
        const float lookSensitivity = 0.004f;

        // Movement input
        if (InputClass::IsKeyPressed('W') || InputClass::IsKeyPressed(VK_UP)) {
            m_Camera->MoveForward(moveSpeed);
        }
        if (InputClass::IsKeyPressed('S') || InputClass::IsKeyPressed(VK_DOWN)) {
            m_Camera->MoveForward(-moveSpeed);
        }
        if (InputClass::IsKeyPressed('A') || InputClass::IsKeyPressed(VK_LEFT)) {
            m_Camera->MoveRight(-moveSpeed);
        }
        if (InputClass::IsKeyPressed('D') || InputClass::IsKeyPressed(VK_RIGHT)) {
            m_Camera->MoveRight(moveSpeed);
        }

        // 점프 입력 처리 (스페이스바)
        static bool spacePressed = false;
        bool spaceCurrentlyPressed = InputClass::IsKeyPressed(VK_SPACE);

        if (spaceCurrentlyPressed && !spacePressed) {
            m_Camera->Jump();  // 키를 누르는 순간에만 점프
        }
        spacePressed = spaceCurrentlyPressed;

        // 마우스 시점 처리...
        float deltaX = InputClass::GetMouseDeltaX();
        float deltaY = InputClass::GetMouseDeltaY();
        m_Camera->AdjustYaw(deltaX * lookSensitivity);
        m_Camera->AdjustPitch(deltaY * lookSensitivity);

        // 중력 적용 (매 프레임)
        m_Camera->ApplyGravity(deltaTime);

        // 카메라 업데이트
        m_Camera->UpdateCamera();

        // 애니메이션 업데이트
        UpdateAnimations(deltaTime);
    }

    // Update the rotation variable each frame.
    rotation += XM_PI * 0.005f;
    if (rotation > 360.0f)
    {
        rotation -= 360.0f;
    }

    // Render the graphics scene.
    result = Render();
    if (!result)
    {
        return false;
    }
    return true;
}

bool GraphicsClass::Render()
{
    XMMATRIX worldMatrix, viewMatrix, projectionMatrix, translateMatrix, orthoMatrix;
    XMFLOAT3 cameraPosition, modelPosition;
    bool result = true;

    // Clear the buffers to begin the scene.
    m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

    // Generate the view matrix based on the camera's position.
    m_Camera->Render();

    // Get the world, view, and projection matrices from the camera and d3d objects.
    m_Camera->GetViewMatrix(viewMatrix);
    m_D3D->GetWorldMatrix(worldMatrix);
    m_D3D->GetProjectionMatrix(projectionMatrix);
    m_D3D->GetOrthoMatrix(orthoMatrix);

    // 씬 상태에 따라 다른 화면을 렌더링
    switch (m_SceneState)
    {
    case SceneState::TITLE:
        m_D3D->TurnZBufferOff();
        result = m_TitleScreen->Render(m_D3D->GetDeviceContext(), 0, 0);
        if (!result) return false;
        result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_TitleScreen->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_TitleScreen->GetTexture());
        if (!result) return false;
        m_D3D->TurnZBufferOn();
        break;

    case SceneState::Tutorial:
        m_D3D->TurnZBufferOff();
        result = m_TutorialScreen->Render(m_D3D->GetDeviceContext(), 0, 0);
        if (!result) return false;
        result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_TutorialScreen->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_TutorialScreen->GetTexture());
        if (!result) return false;
        m_D3D->TurnZBufferOn();
        break;

    case SceneState::MainScene:
        // 배경 렌더링
        m_D3D->TurnZBufferOff();
        m_BackGround->Render(m_D3D->GetDeviceContext(), 0, 0);
        XMMATRIX identityMatrix = XMMatrixIdentity();
        m_TextureShader->Render(m_D3D->GetDeviceContext(), m_BackGround->GetIndexCount(),
            worldMatrix, identityMatrix, orthoMatrix, m_BackGround->GetTexture());
        m_D3D->TurnZBufferOn();

        // 3D 모델들 렌더링
        RenderInstancedObjects(viewMatrix, projectionMatrix);
        RenderIndividualObjects(viewMatrix, projectionMatrix);

        // 헬스바 빌보드 렌더링
        const auto& playerObject = m_IndividualObjects[0]; // PERSON 모델
        XMVECTOR playerPosition = playerObject.worldMatrix.r[3];

        // 헬스바를 플레이어 머리 위에 위치시키기 위한 오프셋
        XMVECTOR healthBarOffset = XMVectorSet(0.0f, 10.0f, 0.0f, 0.0f); // Y축으로 2.0 유닛 위 (모델 크기에 따라 조절)
        XMVECTOR healthBarWorldPosition = XMVectorAdd(playerPosition, healthBarOffset);

        // 카메라 위치를 얻는다.
        cameraPosition = m_Camera->GetPosition();

        // 빌보드 모델의 위치를 ​​설정합니다.
        XMFLOAT3 modelPosition;
        XMStoreFloat3(&modelPosition, healthBarWorldPosition);
        // 아크 탄젠트 함수를 사용하여 현재 카메라 위치를 향하도록 빌보드 모델에 적용해야하는 회전을 계산합니다.
        double angle = atan2(modelPosition.x - cameraPosition.x, modelPosition.z - cameraPosition.z) * (180.0 / XM_PI);

        // 회전을 라디안으로 변환합니다.
        float rotation = (float)angle * 0.0174532925f;

        // 세계 행렬을 사용하여 원점에서 빌보드 회전을 설정합니다.
        worldMatrix = XMMatrixRotationY(rotation);

        // 빌보드 모델에서 번역 행렬을 설정합니다.
        translateMatrix = XMMatrixTranslation(modelPosition.x, modelPosition.y, modelPosition.z);

        // 마지막으로 회전 및 변환 행렬을 결합하여 빌보드 모델의 최종 행렬을 만듭니다.
        worldMatrix = XMMatrixMultiply(worldMatrix, translateMatrix);

        // 헬스바 모델 렌더링
        m_HelthBarBillboardModel->Render(m_D3D->GetDeviceContext());
        result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_HelthBarBillboardModel->GetIndexCount(),
            worldMatrix, viewMatrix, projectionMatrix,
            m_HelthBarBillboardModel->GetTexture());
        if (!result) return false;

        // 텍스트 출력
        wchar_t fpsText[64], cpuText[64], polyText[64], resolText[64];
        swprintf_s(fpsText, 64, L"FPS: %d", m_FPS);
        swprintf_s(cpuText, 64, L"CPU: %d%%", static_cast<int>(m_CPUUsage));
        swprintf_s(polyText, 64, L"POLYGON: %d", m_PolygonCount);
        swprintf_s(resolText, 64, L"RESOLUTION: %d X %d", static_cast<int>(m_ScreenWidth), static_cast<int>(m_ScreenHeight));

        m_D3D->TurnZBufferOff();
        m_D3D->EnableAlphaBlending();

        m_Text->DrawTextLine(fpsText, 10.0f, 0.0f);
        m_Text->DrawTextLine(cpuText, 10.0f, 11.0f);
        m_Text->DrawTextLine(polyText, 10.0f, 22.0f);
        m_Text->DrawTextLine(resolText, 10.0f, 33.0f);

        m_D3D->DisableAlphaBlending();
        m_D3D->TurnZBufferOn();
        break;
    }

    m_D3D->EndScene();
    return true;
}
