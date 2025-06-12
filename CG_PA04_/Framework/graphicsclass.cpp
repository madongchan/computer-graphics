////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"
#include <windows.h>

GraphicsClass::GraphicsClass()
	: m_D3D(nullptr),
	m_Camera(nullptr),
	m_Model(nullptr),
	m_Model1(nullptr),
	m_Model2(nullptr),
	m_GroundModel(nullptr),
	m_TextureShader(nullptr),
	m_Timer(nullptr),
	m_FPSClass(nullptr),
	m_CPUClass(nullptr),
	m_Text(nullptr),
	m_Background(nullptr),
	//m_Billboard(nullptr),
	//m_BillboardModel(nullptr),
	m_FPS(0),
	m_CPUUsage(0),
	m_screenWidth(0),
	m_screenHeight(0),
	//m_BillboardInfo{},
	m_SceneState(SceneState::MainScene)
{
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

	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// Create the Direct3D object.
	m_D3D = new D3DClass;
	if(!m_D3D)
	{
		return false;
	}

	// Initialize the Direct3D object.
	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}

	// 배경 이미지 초기화
	m_Background = new BitmapClass;
	if (!m_Background)
		return false;

	result = m_Background->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight,
		L"./data/background.dds", screenWidth, screenHeight);  // 전체 배경
	if (!result)
		return false;

	/*m_BillboardModel = new ModelClass;
	if (!m_BillboardModel) return false;

	result = m_BillboardModel->Initialize(m_D3D->GetDevice(),
		L"./data/quad.obj", L"./data/billboard.dds", 0.0f);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the billboard model.", L"Error", MB_OK);
		return false;
	}*/

	m_TitleScreen = new BitmapClass;
	m_TitleScreen->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight, L"./data/title.dds", screenWidth, screenHeight);

	// billboard의 월드 위치 지정
	//m_BillboardPosition = XMFLOAT3(700.0f, 300.0f, 200.0f);
	//m_BillboardInfo.scale = 1.0f;  // 스케일 기본값

	RegisterModels();
	result = InitializeModels();
	if (!result)
		return false;

	// TimerClass 생성 및 초기화
	m_Timer = new TimerClass;
	if (!m_Timer)
		return false;

	// 여기서 반드시 Initialize() 를 호출해서 m_frequency, m_lastTime을 채워줘야 합니다.
	m_Timer->Initialize();

	// ▶ FPSClass 생성 및 초기화
	m_FPSClass = new FPSClass;
	if (!m_FPSClass)
		return false;
	m_FPSClass->Initialize(m_Timer);

	// ▶ CPUClass 생성 및 초기화
	m_CPUClass = new CPUClass;
	if (!m_CPUClass)
		return false;
	m_CPUClass->Initialize();

	// Create the camera object.
	m_Camera = new CameraClass;
	if(!m_Camera)
	{
		return false;
	}

	// Create the texture shader object.
	m_TextureShader = new TextureShaderClass;
	if(!m_TextureShader)
	{
		return false;
	}

	// Initialize the texture shader object.
	result = m_TextureShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	m_Text = new TextClass;
	if (!m_Text) return false;

	// TextClass::Initialize( ID3D11Device*, IDXGISwapChain*, int screenWidth, int screenHeight )
	result = m_Text->Initialize(
		m_D3D->GetDevice(),      // ID3D11Device*
		m_D3D->GetSwapChain()    // IDXGISwapChain*
	);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize TextClass.", L"Error", MB_OK);
		return false;
	}

	return true;
}

void GraphicsClass::Shutdown()
{
	if (m_Text)
	{
		m_Text->Shutdown();
		delete m_Text;
		m_Text = nullptr;
	}

	if (m_CPUClass)
	{
		delete m_CPUClass;
		m_CPUClass = 0;
	}

	// ▶ FPSClass 해제
	if (m_FPSClass)
	{
		delete m_FPSClass;
		m_FPSClass = 0;
	}

	if (m_Background)
	{
		m_Background->Shutdown();
		delete m_Background;
		m_Background = nullptr;
	}

	if (m_Billboard)
	{
		m_Billboard->Shutdown();
		delete m_Billboard;
		m_Billboard = 0;
	}

	// GraphicsClass::Shutdown()
	if (m_BillboardModel)
	{
		m_BillboardModel->Shutdown();
		delete m_BillboardModel;
		m_BillboardModel = nullptr;
	}


	// Release the texture shader object.
	if(m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	// Release the camera object.
	if(m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	for (auto model : m_models)
	{
		if (model)
		{
			model->Shutdown();
			delete model;
		}
	}
	m_models.clear();

	// Release the D3D object.
	if(m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}

	return;
}


bool GraphicsClass::Frame()
{
	bool result;

	static float rotation = 0.0f;

	// --- (1) FPS / CPU 업데이트 ---
	// 1.1. FPSClass 내부에서 ‘이번 프레임’을 집계하도록 호출
	m_FPSClass->Frame();
	// 1.2. CPUClass 내부에서 ‘현재 CPU 사용률’을 집계하도록 호출
	m_CPUClass->Frame();

	double delta = m_Timer->GetDeltaTime();

	if (!m_FPSClass || !m_CPUClass || !m_Timer) return false;

	// 1.3. 그 결과를 멤버 변수에 저장
	m_FPS = m_FPSClass->GetFPS();
	m_CPUUsage = m_CPUClass->GetCPUPercent();

	// Update the rotation variable each frame.
	rotation += (float)XM_PI * 0.001f;
	if (rotation > 360.0f)
	{
		rotation -= 360.0f;
	}

	// 입력 처리
	static float camYaw = 0.0f;
	static float camPitch = 0.0f;

	float moveLeftRight = 0.0f;
	float moveForwardBack = 0.0f;

	// 키보드 이동
	if (GetAsyncKeyState('W') & 0x8000) moveForwardBack += 0.5f;
	if (GetAsyncKeyState('S') & 0x8000) moveForwardBack -= 0.5f;
	if (GetAsyncKeyState('A') & 0x8000) moveLeftRight -= 0.5f;
	if (GetAsyncKeyState('D') & 0x8000) moveLeftRight += 0.5f;
	if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
		m_Camera->Jump();
	}
	m_Camera->UpdateJump((float)delta); // 매 프레임 점프 상태 업데이트



	// 마우스 회전
	POINT mousePos;
	GetCursorPos(&mousePos);
	ScreenToClient(GetForegroundWindow(), &mousePos);
	static int lastX = 400, lastY = 300;
	int deltaX = mousePos.x - lastX;
	int deltaY = mousePos.y - lastY;

	camYaw += deltaX * 0.002f;
	camPitch += deltaY * 0.002f;

	lastX = mousePos.x;
	lastY = mousePos.y;

	// 카메라 상태 전달
	m_Camera->SetYawPitch(camYaw, camPitch);
	m_Camera->Move(moveLeftRight, moveForwardBack);
	m_Camera->Render();  // view matrix 업데이트

	// 튜토리얼 씬 상태인지 확인을 먼저해야 안전함
	if (m_SceneState == SceneState::Tutorial) {
		for (int keyCode = 0; keyCode < 256; ++keyCode) {
			if (GetAsyncKeyState(keyCode) & 0x8000) {
				// 아무 키나 눌렸음
				m_SceneState = SceneState::MainScene;
				break;
			}
		}
	}
	// 아무 키를 누르면 Tutorial 씬으로 전환
	if (m_SceneState == SceneState::TITLE) {
		for (int keyCode = 0; keyCode < 256; ++keyCode) {
			if (GetAsyncKeyState(keyCode) & 0x8000) {
				// 아무 키나 눌렸음
				m_SceneState = SceneState::Tutorial;
				break;
			}
		}
	}

	// Render the graphics scene.
	result = Render(rotation);
	if(!result)
	{
		return false;
	}

	m_D3D->EndScene();

	return true;
}

bool GraphicsClass::Render(float rotation)
{
	XMMATRIX viewMatrix, projectionMatrix;
	bool result;

	// 1. Begin Scene
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// 2. 카메라 업데이트 및 행렬 구하기
	m_Camera->Render();
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	switch (m_SceneState) {
		case SceneState::TITLE:
		{
			m_D3D->TurnZBufferOff();
			m_D3D->EnableAlphaBlending();

			m_TitleScreen->Render(m_D3D->GetDeviceContext(), 0, 0);
			m_TextureShader->Render(m_D3D->GetDeviceContext(),
				m_TitleScreen->GetIndexCount(),
				XMMatrixIdentity(), XMMatrixIdentity(), XMMatrixIdentity(),
				m_TitleScreen->GetTexture());

			m_D3D->DisableAlphaBlending();
			m_D3D->TurnZBufferOn();
			break;
		}
		case SceneState::MainScene:
		{// 3. 2D: 배경 렌더링
			m_D3D->TurnZBufferOff();
			m_D3D->EnableAlphaBlending();

			m_Background->Render(m_D3D->GetDeviceContext(), 0, 0);
			m_TextureShader->Render(m_D3D->GetDeviceContext(), m_Background->GetIndexCount(),
				XMMatrixIdentity(), XMMatrixIdentity(), XMMatrixIdentity(), m_Background->GetTexture());

			m_D3D->DisableAlphaBlending();
			m_D3D->TurnZBufferOn();

			//XMVECTOR billboardPos = XMLoadFloat3(&m_BillboardPosition);

			// 카메라 방향 계산
			XMVECTOR right = m_Camera->GetRightVector();
			XMVECTOR up = m_Camera->GetUpVector();
			XMVECTOR look = m_Camera->GetLookVector();

			//// 회전 행렬 설정
			//XMMATRIX billboardRot;
			//billboardRot.r[0] = right;
			//billboardRot.r[1] = up;
			//billboardRot.r[2] = look;
			//billboardRot.r[3] = XMVectorSet(0, 0, 0, 1);

			//// 최종 월드 행렬 계산
			//XMMATRIX billboardWorld = billboardRot * XMMatrixTranslationFromVector(billboardPos);

			//// 렌더링
			//m_BillboardModel->Render(m_D3D->GetDeviceContext());
			//result = m_TextureShader->Render(
			//	m_D3D->GetDeviceContext(),
			//	m_BillboardModel->GetIndexCount(),
			//	billboardWorld, viewMatrix, projectionMatrix,
			//	m_BillboardModel->GetTexture());
			//if (!result) return false;

			// 5. 일반 3D 모델 렌더링
			for (size_t i = 0; i < m_models.size(); ++i)
			{
				if (m_modelInfos[i].canBeFound && m_modelInfos[i].found)
					continue;

				float finalRotY = m_modelInfos[i].rotation.y;
				if (m_modelInfos[i].rotateWithScene)
					finalRotY += rotation;

				XMMATRIX rotX = XMMatrixRotationX(m_modelInfos[i].rotation.x);
				XMMATRIX rotY = XMMatrixRotationY(finalRotY);
				XMMATRIX rotZ = XMMatrixRotationZ(m_modelInfos[i].rotation.z);

				XMMATRIX worldMatrix = rotX * rotY * rotZ * XMMatrixTranslation(
					m_modelInfos[i].position.x,
					m_modelInfos[i].position.y,
					m_modelInfos[i].position.z);

				m_models[i]->Render(m_D3D->GetDeviceContext());
				result = m_TextureShader->Render(
					m_D3D->GetDeviceContext(),
					m_models[i]->GetIndexCount(),
					worldMatrix, viewMatrix, projectionMatrix,
					m_models[i]->GetTexture());
				if (!result)
					return false;
			}

			int totalCount = static_cast<int>(m_modelInfos.size());
			int foundCount = 0;

			for (const auto& model : m_modelInfos) {
				if (model.found) {
					foundCount++;
				}
			}
			int remainingCount = totalCount - foundCount;

			int polygonCount = 0;
			for (auto& modelInfo : m_modelInfos) {
				if (modelInfo.modelPtr && !modelInfo.found) { // 찾지 않은(= 화면에 존재하는) 모델만 포함
					polygonCount += modelInfo.modelPtr->GetIndexCount() / 3;
				}
			}


			// 6. 텍스트 출력 (FPS, CPU)
			wchar_t fpsText[64], cpuText[64], foundText[64], objectText[64], polyText[64], resolText[64];
			swprintf_s(fpsText, 64, L"FPS: %d", m_FPS);
			swprintf_s(cpuText, 64, L"CPU: %d%%", m_CPUUsage);
			swprintf_s(foundText, 64, L"Found: %d", foundCount);
			swprintf_s(objectText, 64, L"Object: %d", remainingCount);
			swprintf_s(polyText, 64, L"Polygon: %d", polygonCount);
			swprintf_s(resolText, 64, L"Screen resolution: %d X %d", m_screenWidth, m_screenHeight);

			m_D3D->TurnZBufferOff();
			m_D3D->EnableAlphaBlending();

			m_Text->DrawTextLine(fpsText, 10.0f, 10.0f);
			m_Text->DrawTextLine(cpuText, 10.0f, 40.0f);
			m_Text->DrawTextLine(foundText, 10.0f, 70.0f);
			m_Text->DrawTextLine(objectText, 10.0f, 100.0f);
			m_Text->DrawTextLine(polyText, 10.0f, 130.0f);
			m_Text->DrawTextLine(resolText, 10.0f, 160.0f);

			m_D3D->DisableAlphaBlending();
			m_D3D->TurnZBufferOn();

			break;
			// 7. End Scene
		}
	}
	m_D3D->EndScene();
	return true;

}

int GraphicsClass::GetPolygonCount() const
{
	// 폴리곤(인덱스) 개수를 전부 합산해 반환
	int totalPolys = 0;
	if (m_Model)       totalPolys += m_Model->GetIndexCount() / 3;
	if (m_Model1)      totalPolys += m_Model1->GetIndexCount() / 3;
	if (m_Model2)      totalPolys += m_Model2->GetIndexCount() / 3;
	if (m_GroundModel) totalPolys += m_GroundModel->GetIndexCount() / 3;
	return totalPolys;
}

void GraphicsClass::RegisterModels()
{
	m_modelInfos.push_back({
   L"./data/stone.obj",
   L"./data/T_StylizedRock_1_JFG_BC.dds",
   XMFLOAT3(-3.0f, 0.0f, 10.0f),
   XMFLOAT3(0.0f, 0.0f, 0.0f),
   true,
   false,
   false
	});

	m_modelInfos.push_back({
		L"./data/G06_hotdog.obj",
		L"./data/G06_Hotdog_Albedo.dds",
		XMFLOAT3(-50.0f, -30.0f, 20.0f),
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		true,
   false,
   true
		});

	m_modelInfos.push_back({
		L"./data/Human1.obj",
		L"./data/barbar_albedo.dds",
		XMFLOAT3(10.0f, 45.0f, 30.0f),
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		true,
   false,
   false
		});

	m_modelInfos.push_back({
	L"./data/Bread.obj",
	L"./data/Bread.dds",
	XMFLOAT3(120.0f, 60.0f, 40.0f),
	XMFLOAT3(0.0f, 0.0f, 0.0f),
	true,
   false,
   true
		});

	m_modelInfos.push_back({
	L"./data/Burger.obj",
	L"./data/Burger.dds",
	XMFLOAT3(54.0f, 80.0f, -20.0f),
	XMFLOAT3(0.0f, 0.0f, 0.0f),
	true,
   false,
   true
		});	
	
	m_modelInfos.push_back({
	L"./data/Coke.obj",
	L"./data/Coke.dds",
	XMFLOAT3(-72.0f, 60.0f, -50.0f),
	XMFLOAT3(0.0f, 0.0f, 0.0f),
	true,
   false,
   true
		});

	m_modelInfos.push_back({
	L"./data/FriedRice.obj",
	L"./data/FriedRice.dds",
	XMFLOAT3(-110.0f, -90.0f, -40.0f),
	XMFLOAT3(0.0f, 0.0f, 0.0f),
	true,
   false,
   true
		});

	m_modelInfos.push_back({
	L"./data/Orange.obj",
	L"./data/Orange.dds",
	XMFLOAT3(-30.0f, 70.0f, 60.0f),
	XMFLOAT3(0.0f, 0.0f, 0.0f),
	true,
   false,
   true
		});

	m_modelInfos.push_back({
L"./data/Orange.obj",
L"./data/Orange.dds",
XMFLOAT3(-45.0f, 30.0f, 35.0f),
XMFLOAT3(0.0f, 0.0f, 0.0f),
true,
false,
true
		});

	m_modelInfos.push_back({
L"./data/Orange.obj",
L"./data/Orange.dds",
XMFLOAT3(-50.0f, 20.0f, -60.0f),
XMFLOAT3(0.0f, 0.0f, 0.0f),
true,
false,
true
		});

	m_modelInfos.push_back({
L"./data/Orange.obj",
L"./data/Orange.dds",
XMFLOAT3(-30.0f, 10.0f, 80.0f),
XMFLOAT3(0.0f, 0.0f, 0.0f),
true,
false,
true
		});

	m_modelInfos.push_back({
L"./data/Orange.obj",
L"./data/Orange.dds",
XMFLOAT3(30.0f, 60.0f, 10.0f),
XMFLOAT3(0.0f, 0.0f, 0.0f),
true,
false,
true
		});

	m_modelInfos.push_back({
L"./data/Orange.obj",
L"./data/Orange.dds",
XMFLOAT3(60.0f, 40.0f, -65.0f),
XMFLOAT3(0.0f, 0.0f, 0.0f),
true,
false,
true
		});

	m_modelInfos.push_back({
		L"./data/Taco.obj",
		L"./data/Taco.dds",
		XMFLOAT3(30.0f, 75.0f, 35.0f),
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		true,
   false,
   true
		});


	m_modelInfos.push_back({
		L"./data/Sandwich.obj",
		L"./data/Sandwich.dds",
		XMFLOAT3(30.0f, -120.0f, 10.0f),
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		true,
   false,
   true
		});


	m_modelInfos.push_back({
		L"./data/Rectangle1.obj",
		L"./data/floor-diffuse-texture.dds",
		XMFLOAT3(0.0f, -20.0f, 0.0f),
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		false,
   false,
   false
		});
}
	// 필요시 계속 추가

bool GraphicsClass::InitializeModels()
{
	for (auto& info : m_modelInfos)
	{
		ModelClass* model = new ModelClass;
		if (!model)
			return false;

		bool result = model->Initialize(m_D3D->GetDevice(), info.modelPath.c_str(), info.texturePath.c_str(), 0.0f);
		if (!result)
			return false;
		info.modelPtr = model;
		m_models.push_back(model);
	}
	return true;
}