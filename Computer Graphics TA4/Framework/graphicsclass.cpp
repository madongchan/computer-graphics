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
    m_Model = 0;

    m_TextureShader = 0;
    m_BackGround = 0;
    m_TitleScreen = 0;
    m_TutorialScreen = 0;

    m_Text = 0;
    m_ModelManager = 0;  // ���� �߰�

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
    m_Camera->SetPosition(0.0f, 10.0f, -50.0f);

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

    // �𵨵� �ʱ�ȭ
    result = InitializeModels(m_D3D->GetDevice(), hwnd);
    if (!result)
    {
        return false;
    }

    // ��� ȭ��� �ʱ�ȭ (���� �ڵ�� ����)
    m_BackGround = new BitmapClass;
    if (!m_BackGround) return false;
    result = m_BackGround->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight, L"./data/bluesky.dds", screenWidth, screenHeight);
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

    // ModelManager �ʱ�ȭ
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

    // �ν��Ͻ��� �ʱ�ȭ
    InitializeInstances();

    // ���� ������Ʈ�� �ʱ�ȭ
    InitializeIndividualObjects();

    return true;
}

void GraphicsClass::InitializeInstances()
{
    // Road �ν��Ͻ��� - ���� �������� 25�� ��ġ
    int roadGridSize = 5;
    for (int i = 0; i < roadGridSize * roadGridSize; i++)
    {
        int x = i % roadGridSize;
        int z = i / roadGridSize;

        XMMATRIX roadMatrix = XMMatrixTranslation(
            x * 10.0f - roadGridSize * 5.0f,
            0.0f,
            z * 10.0f - roadGridSize * 5.0f
        );
        m_RoadInstances.push_back(roadMatrix);
    }

    // Tree �ν��Ͻ��� - �����ϰ� 20�� ��ġ
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
    // ��� ������Ʈ - ���� ��� �ִϸ��̼�
    ObjectTransform person;
    person.worldMatrix = XMMatrixTranslation(20.0f, 0.0f, 0.0f);
    person.modelType = ModelType::PERSON;
    person.velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
    person.rotationSpeed = XMFLOAT3(0.0f, 1.0f, 0.0f);
    person.isAnimated = true;
    m_IndividualObjects.push_back(person);

    // �ڵ��� ������Ʈ - ���� �պ� �ִϸ��̼�
    ObjectTransform car;
    car.worldMatrix = XMMatrixTranslation(0.0f, 0.0f, -10.0f);
    car.modelType = ModelType::CAR;
    car.velocity = XMFLOAT3(2.0f, 0.0f, 0.0f);
    car.rotationSpeed = XMFLOAT3(0.0f, 0.0f, 0.0f);
    car.isAnimated = true;
    m_IndividualObjects.push_back(car);

    // �ǹ� ������Ʈ - Y�� ȸ�� �ִϸ��̼�
    ObjectTransform building;
    building.worldMatrix = XMMatrixTranslation(0.0f, 0.0f, -25.0f);
    building.modelType = ModelType::BUILDING;
    building.velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
    building.rotationSpeed = XMFLOAT3(0.0f, 5.0f, 0.0f);
    building.isAnimated = true;
    m_IndividualObjects.push_back(building);

    // ���� ������Ʈ�� �߰�
    ObjectTransform bike;
    bike.worldMatrix = XMMatrixTranslation(-15.0f, 0.0f, 5.0f);
    bike.modelType = ModelType::BIKE;
    bike.isAnimated = false;
    m_IndividualObjects.push_back(bike);

    ObjectTransform wall;
    wall.worldMatrix = XMMatrixTranslation(30.0f, 0.0f, 0.0f);
    wall.modelType = ModelType::WALL;
    wall.isAnimated = false;
    m_IndividualObjects.push_back(wall);
}

void GraphicsClass::UpdateAnimations(float deltaTime)
{
    m_AnimationTime += deltaTime;

    for (size_t i = 0; i < m_IndividualObjects.size() && i < 3; i++)
    {
        if (!m_IndividualObjects[i].isAnimated) continue;

        switch (i)
        {
        case 0: // ��� - ���� ���
        {
            float radius = 20.0f;
            float speed = 1.0f;
            float angle = m_AnimationTime * speed;
            m_IndividualObjects[i].worldMatrix = XMMatrixTranslation(
                cos(angle) * radius, 0.0f, sin(angle) * radius
            );
        }
        break;

        case 1: // �ڵ��� - ���� �պ�
        {
            float distance = 30.0f;
            float speed = 2.0f;
            float position = sin(m_AnimationTime * speed) * distance;
            m_IndividualObjects[i].worldMatrix =
                XMMatrixRotationY(XM_PI * 0.5f) *
                XMMatrixTranslation(position, 0.0f, -10.0f);
        }
        break;

        case 2: // �ǹ� - Y�� ȸ��
        {
            float rotationSpeed = 5.0f;
            m_IndividualObjects[i].worldMatrix =
                XMMatrixRotationY(m_AnimationTime * rotationSpeed) *
                XMMatrixTranslation(0.0f, 0.0f, 25.0f);
        }
        break;
        }
    }
}



void GraphicsClass::RenderInstancedObjects(XMMATRIX& viewMatrix, XMMATRIX& projectionMatrix)
{
    // �����ϸ� ��� ����
    XMMATRIX scaleMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);

    // Road �ν��Ͻ��� ������
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

    // Tree �ν��Ͻ��鵵 �����ϰ� ó��
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

            // 100���� 1�� ����ϴ� �����ϸ� ��� ����
            XMMATRIX scaleMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
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
    // ModelManager ����
    if (m_ModelManager)
    {
        m_ModelManager->Shutdown();
        delete m_ModelManager;
        m_ModelManager = 0;
    }

    // ���� ���� �ڵ��...
    if (m_Text)
    {
        m_Text->Shutdown();
        delete m_Text;
        m_Text = nullptr;
    }

    if (m_Model)
    {
        m_Model->Shutdown();
        delete m_Model;
        m_Model = 0;
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

    // �ð� ���
    static float lastTime = GetTickCount() / 1000.0f;
    float currentTime = GetTickCount() / 1000.0f;
    float deltaTime = currentTime - lastTime;
    lastTime = currentTime;

    // Ű ���� ������Ʈ
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

    // ���� �������� ī�޶� �̵��� �ִϸ��̼� ó��
    if (m_SceneState == SceneState::MainScene)
    {
        const float moveSpeed = 0.1f;
        const float lookSensitivity = 0.002f;

        // Movement input
        if (InputClass::IsKeyPressed('W')) {
            m_Camera->MoveForward(moveSpeed);
        }
        if (InputClass::IsKeyPressed('S')) {
            m_Camera->MoveForward(-moveSpeed);
        }
        if (InputClass::IsKeyPressed('A')) {
            m_Camera->MoveRight(-moveSpeed);
        }
        if (InputClass::IsKeyPressed('D')) {
            m_Camera->MoveRight(moveSpeed);
        }

        // Mouse look input
        float deltaX = InputClass::GetMouseDeltaX();
        float deltaY = InputClass::GetMouseDeltaY();
        m_Camera->AdjustYaw(deltaX * lookSensitivity);
        m_Camera->AdjustPitch(deltaY * lookSensitivity);

        // Update camera
        m_Camera->UpdateCamera();

        // �ִϸ��̼� ������Ʈ
        UpdateAnimations(deltaTime);
    }

    // Update the rotation variable each frame.
    rotation += XM_PI * 0.005f;
    if (rotation > 360.0f)
    {
        rotation -= 360.0f;
    }

    // Render the graphics scene.
    result = Render(rotation);
    if (!result)
    {
        return false;
    }
    return true;
}

bool GraphicsClass::Render(float rotation)
{
    XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
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

    // �� ���¿� ���� �ٸ� ȭ���� ������
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
        // ��� ������
        m_D3D->TurnZBufferOff();
        m_BackGround->Render(m_D3D->GetDeviceContext(), 0, 0);
        XMMATRIX identityMatrix = XMMatrixIdentity();
        m_TextureShader->Render(m_D3D->GetDeviceContext(), m_BackGround->GetIndexCount(),
            worldMatrix, identityMatrix, orthoMatrix, m_BackGround->GetTexture());
        m_D3D->TurnZBufferOn();

        // 3D �𵨵� ������
        RenderInstancedObjects(viewMatrix, projectionMatrix);
        RenderIndividualObjects(viewMatrix, projectionMatrix);

        // �ؽ�Ʈ ���
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
