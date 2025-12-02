////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"
#include <dinput.h> // DIK_ 키 코드를 사용하기 위해 include

GraphicsClass::GraphicsClass()
{
	m_D3D = 0;
	m_Camera = 0;
	m_LightShader = 0;
	m_TitleScreen = 0;
	m_TutorialScreen = 0;
	m_Text = 0;
	m_TextureShader = 0;
	m_Skybox = 0;
	m_SkyboxShader = 0;
	m_BumpMapShader = 0;
	m_GroundTextures = 0;
	m_Light = 0;
	m_rotation = 0.0f;
	m_isAmbientOn = true;
	m_isDiffuseOn = true;
	m_isSpecularOn = true;
	m_isNormalMapOn = true;
	m_pointLightIntensity = 1.0f; // 기본 강도 1.0

	m_GameObjects = std::vector<GameObject>();
	m_ModelWall = 0;
	m_ModelGround = 0;
	m_ModelCrate = 0;
	m_ModelBarricade = 0;
	m_ModelObstacle = 0;
	m_ModelCar = 0;
	m_ModelTent = 0;
	m_ModelTree = 0;

	m_FPS = 0;
	m_CPUUsage = 0.0f;
	m_PolygonCount = 0;
	m_ScreenWidth = 0.0f;
	m_ScreenHeight = 0.0f;
	m_SceneState = SceneState::TITLE;
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
	m_ScreenWidth = (float)screenWidth;
	m_ScreenHeight = (float)screenHeight;

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

	// Create the camera object.
	m_Camera = new CameraClass;
	if(!m_Camera)
	{
		return false;
	}
	// 타이틀
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
	// 튜토리얼
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

	// Set the initial position of the camera.
	m_Camera->SetPosition(-25.0f, 3.0f, -0.0f);
	// 카메라 각도를 오른쪽으로 약간 틀어줌
	m_Camera->SetRotation(0.0f, 90.0f, 0.0f);
	m_Camera->SetGroundLevel(3.0f); // 바닥 모델의 Y위치와 일치시킴
	
	// -----------------------------------------------------------
	// 1. 모델 리소스 로딩 (★ 여기에 사용할 FBX 경로를 넣으세요)
	// -----------------------------------------------------------

	// [벽 & 바닥]
	m_ModelWall = new ModelClass;
	m_ModelWall->Initialize(m_D3D->GetDevice(), "./data/Wall.fbx", L"./data/Texture.dds"); // 벽용 큐브

	m_ModelGround = new ModelClass;
	m_ModelGround->Initialize(m_D3D->GetDevice(), "./data/Floor.fbx", L"./data/Floor_color.dds");
	//  지면 텍스처 배열 초기화
	m_GroundTextures = new TextureArrayClass;
	if (!m_GroundTextures) { return false; }
	result = m_GroundTextures->Initialize(m_D3D->GetDevice(),
		L"./data/Texture.dds",  // (지면 색상 텍스처)
		L"./data/Floor_normal.dds"); // (지면 노멀맵 텍스처)
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the ground texture array object.", L"Error", MB_OK);
		return false;
	}
	// [오브젝트]
	m_ModelCrate = new ModelClass;
	m_ModelCrate->Initialize(m_D3D->GetDevice(), "./data/Crate.fbx", L"./data/Texture.dds"); // 보급상자

	m_ModelBarricade = new ModelClass;
	m_ModelBarricade->Initialize(m_D3D->GetDevice(), "./data/Barricade.fbx", L"./data/Texture.dds"); // 바리케이드

	m_ModelObstacle = new ModelClass;
	m_ModelObstacle->Initialize(m_D3D->GetDevice(), "./data/Tire.fbx", L"./data/Texture.dds"); // 타이어/드럼통

	m_ModelCar = new ModelClass;
	m_ModelCar->Initialize(m_D3D->GetDevice(), "./data/Car.fbx", L"./data/Vehicles_Destroy.dds"); // 차량

	m_ModelTent = new ModelClass;
	m_ModelTent->Initialize(m_D3D->GetDevice(), "./data/Tent.fbx", L"./data/Texture.dds"); // 텐트

	m_ModelTree = new ModelClass;
	m_ModelTree->Initialize(m_D3D->GetDevice(), "./data/Tree.fbx", L"./data/Texture.dds"); // 나무


	// -----------------------------------------------------------
	// 2. 맵 배치 (Blueprint 적용)
	// -----------------------------------------------------------
	// 1. 모델 크기를 조금 키워서 빈 공간을 채웁니다. (너무 작으면 휑해 보임)
	// 화면 보시면서 0.002f ~ 0.004f 사이에서 조절하세요.
	float commonScale = 0.015f;
	float commonScaleY = commonScale + 0.005f; // Y축은 살짝 더 키움
	float modelYOffset = 0.3f; // 모델별 Y축 오프셋 (필요시 조절)

	// -----------------------------------------------------------
	// [A] 벽 (Walls) - "울타리 스타일" (여러 개 이어 붙이기)
	// -----------------------------------------------------------

	// 벽 모델 하나하나의 크기 설정 (늘리지 않고 원래 비율 유지하거나 살짝만 조정)
	// 큐브(cube.fbx)를 벽돌 하나라고 생각하세요.
	float brickScale = 0.02f; // 개별 벽돌 크기 (모델에 따라 조절 필요)

	// 맵 테두리 설정
	float mapLimit = 30.0f;    // 맵 끝 좌표
	float spacing = 3.8f;      // 벽돌 간격 (이 값을 조절해서 틈을 메우세요!)

	// 반복문을 돌며 -12 부터 +12 까지 벽을 쭉 세웁니다.
	for (float i = -mapLimit - 3; i <= mapLimit; i += spacing)
	{
		// 1. 북쪽 벽 (+Z 방향)
		GameObject wNorth;
		wNorth.model = m_ModelWall; // 벽돌/울타리 모델
		wNorth.position = XMFLOAT3(i, 0, mapLimit); // 높이 0.5f (바닥 위)
		wNorth.rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
		wNorth.scale = XMFLOAT3(brickScale, brickScale, brickScale);
		m_GameObjects.push_back(wNorth);

		// 2. 남쪽 벽 (-Z 방향)
		GameObject wSouth;
		wSouth.model = m_ModelWall;
		wSouth.position = XMFLOAT3(i, 0, -mapLimit);
		wSouth.rotation = XMFLOAT3(0.0f, 0.0f, 0.0f); // 필요하면 180도 회전
		wSouth.scale = XMFLOAT3(brickScale, brickScale, brickScale);
		m_GameObjects.push_back(wSouth);

		// 3. 서쪽 벽 (-X 방향)
		GameObject wWest;
		wWest.model = m_ModelWall;
		wWest.position = XMFLOAT3(-mapLimit, 0, i);
		wWest.rotation = XMFLOAT3(0.0f, XM_PI * 0.5f, 0.0f); // 90도 회전
		wWest.scale = XMFLOAT3(brickScale, brickScale, brickScale);
		m_GameObjects.push_back(wWest);

		// 4. 동쪽 벽 (+X 방향)
		GameObject wEast;
		wEast.model = m_ModelWall;
		wEast.position = XMFLOAT3(mapLimit, 0, i);
		wEast.rotation = XMFLOAT3(0.0f, XM_PI * 0.5f, 0.0f); // 90도 회전
		wEast.scale = XMFLOAT3(brickScale, brickScale, brickScale);
		m_GameObjects.push_back(wEast);
	}

	// [B] 중앙 목표 (보급상자) - 맵 정중앙
	{
		GameObject obj; obj.model = m_ModelCrate;
		obj.position = XMFLOAT3(0.0f, modelYOffset, 0.0f);
		obj.rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
		obj.scale = XMFLOAT3(commonScale, commonScaleY, commonScale);
		m_GameObjects.push_back(obj);
	}

	// [C] 차량 (중간 엄폐물) - 중앙 바로 옆으로 밀착!
	// 기존 +/- 10.0f -> +/- 4.0f (아주 가깝게)
	{
		GameObject c1; c1.model = m_ModelCar;
		c1.position = XMFLOAT3(-4.5f, modelYOffset, 4.0f); // 중앙에서 북서쪽으로 조금만 이동
		c1.rotation = XMFLOAT3(0.0f, -0.5f, 0.0f);
		c1.scale = XMFLOAT3(commonScale, commonScaleY, commonScale);
		m_GameObjects.push_back(c1);

		GameObject c2; c2.model = m_ModelCar;
		c2.position = XMFLOAT3(6.5f, modelYOffset, -4.0f); // 중앙에서 남동쪽으로 조금만 이동
		c2.rotation = XMFLOAT3(0.0f, 2.5f, 0.0f);
		c2.scale = XMFLOAT3(commonScale, commonScaleY, commonScale);
		m_GameObjects.push_back(c2);
	}

	// [D] 방어선 (바리케이드) - 교전 거리 확보
	// 기존 +/- 18.0f -> +/- 7.5f (중앙과 가까움)
	float barricadeZ = 7.5f;
	for (int dir : {1, -1})
	{
		// 중앙 바리케이드
		GameObject bar; bar.model = m_ModelBarricade;
		bar.position = XMFLOAT3(0.0f, modelYOffset, barricadeZ * dir);
		bar.rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
		bar.scale = XMFLOAT3(commonScale, commonScaleY, commonScale);
		m_GameObjects.push_back(bar);

		// 양옆 타이어 (바리케이드 바로 옆에 붙임)
		GameObject t1; t1.model = m_ModelObstacle;
		t1.position = XMFLOAT3(-3.0f, modelYOffset, barricadeZ * dir); // 거리 좁힘
		t1.rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
		t1.scale = XMFLOAT3(commonScale, commonScaleY, commonScale);
		m_GameObjects.push_back(t1);

		GameObject t2; t2.model = m_ModelObstacle;
		t2.position = XMFLOAT3(3.0f, modelYOffset, barricadeZ * dir); // 거리 좁힘
		t2.rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
		t2.scale = XMFLOAT3(commonScale, commonScaleY, commonScale);
		m_GameObjects.push_back(t2);
	}

	// [E] 스폰 포인트 (텐트) - 벽 바로 앞
	// 기존 +/- 25.0f -> +/- 10.0f (벽 안쪽)
	float spawnZ = 16.0f;
	{
		GameObject tN; tN.model = m_ModelTent;
		tN.position = XMFLOAT3(0.0f, modelYOffset, spawnZ);
		tN.rotation = XMFLOAT3(0.0f, XM_PI, 0.0f);
		tN.scale = XMFLOAT3(commonScale, commonScaleY, commonScale);
		m_GameObjects.push_back(tN);

		GameObject tS; tS.model = m_ModelTent;
		tS.position = XMFLOAT3(0.0f, modelYOffset, -spawnZ);
		tS.rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
		tS.scale = XMFLOAT3(commonScale, commonScaleY, commonScale);
		m_GameObjects.push_back(tS);
	}

	// [F] 조경 (나무) - 맵 모서리 꽉 채우기
	// 기존 +/- 20.0f -> +/- 11.0f (맵 안쪽 모서리)
	{
		float treeCorner = 11.0f;
		float treeX[2] = { -treeCorner, treeCorner };
		float treeZ[2] = { -treeCorner, treeCorner };

		for (int i = 0; i < 2; ++i) {
			for (int j = 0; j < 2; ++j) {
				GameObject tree; tree.model = m_ModelTree;
				tree.position = XMFLOAT3(treeX[i], modelYOffset, treeZ[j]);
				tree.rotation = XMFLOAT3(0.0f, (float)(i + j), 0.0f);
				tree.scale = XMFLOAT3(commonScale, commonScaleY, commonScale);
				m_GameObjects.push_back(tree);
			}
		}
	}

	// 범프맵 셰이더 초기화
	m_BumpMapShader = new BumpMapShaderClass;
	if (!m_BumpMapShader) { return false; }
	result = m_BumpMapShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the bump map shader object.", L"Error", MB_OK);
		return false;
	}

	

	// Create the light shader object.
	m_LightShader = new LightShaderClass;
	if (!m_LightShader)
	{
		return false;
	}

	// Initialize the light shader object.
	result = m_LightShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the light object.
	m_Light = new LightClass;
	if (!m_Light)
	{
		return false;
	}
	// Initialize the light object.
	m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);     
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);     
	m_Light->SetDirection(0.0f, -0.5f, 1.0f);             
	m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);    
	m_Light->SetSpecularPower(32.0f);    
	
	// Point Light 초기화
	m_pointLightIntensity = 7.0f;

	// 포인트 라이트 빨간색, 왼쪽 위
	m_Light->SetPointLightColor(0, 1.0f, 0.0f, 0.0f, 1.0f); // R, G, B, A
	m_Light->SetPointLightPosition(0, 0.0f, 3.0f, -18.0f);   // X, Y, Z

	// 포인트 라이트 녹색, 중앙 위
	m_Light->SetPointLightColor(1, 0.0f, 1.0f, 0.0f, 1.0f);
	m_Light->SetPointLightPosition(1, 0.0f, 3.0f, 0.0f);

	// 포인트 라이트 파란색, 오른쪽 위
	m_Light->SetPointLightColor(2, 0.0f, 0.0f, 1.0f, 1.0f);
	m_Light->SetPointLightPosition(2, 0.0f, 3.0f, 18.0f);

	// 스카이박스 셰이더 초기화
	m_SkyboxShader = new SkyboxShaderClass;
	if (!m_SkyboxShader)
	{
		return false;
	}
	result = m_SkyboxShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the skybox shader object.", L"Error", MB_OK);
		return false;
	}

	// 스카이박스 모델 초기화
	m_Skybox = new SkyboxClass;
	if (!m_Skybox)
	{
		return false;
	}
	// (NVIDIA 도구로 변환한 큐브맵 .dds 파일 경로를 지정하세요)
	result = m_Skybox->Initialize(m_D3D->GetDevice(), L"./data/skybox.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the skybox model object.", L"Error", MB_OK);
		return false;
	}

	// 텍스처 셰이더 초기화
	m_TextureShader = new TextureShaderClass;
	if (!m_TextureShader)
	{
		return false;
	}
	result = m_TextureShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	return true;
}

bool GraphicsClass::Frame(InputClass* Input, double deltaTime, int fps, float cpuUsage)
{
	bool result;
	m_FPS = fps;
	m_CPUUsage = cpuUsage;
	// 카메라 입력 처리 및 중력 적용
	// 토글 키로 카메라 컨트롤 모드 전환
	if (Input->IsKeyToggle(DIK_F1))
	{
		m_isCameraControlMode = !m_isCameraControlMode;
	}
	if (m_isCameraControlMode)
	{
		HandleInput(Input, deltaTime);
		m_Camera->ApplyGravity(static_cast<float>(deltaTime));
	}
	
	// Render the graphics scene.
	// (기존 코드는 Render 호출이 없었지만, 여기서 해줘야 합니다)
	result = Render();
	if (!result)
	{
		return false;
	}

	return true;
}

// 카메라 입력을 처리하는 헬퍼 함수
void GraphicsClass::HandleInput(InputClass* Input, double deltaTime)
{
	if (Input->IsAnyKeyJustPressed())
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
		long mouseX = 0, mouseY = 0;
		Input->GetMouseDelta(mouseX, mouseY); // 마우스 상대 이동량(Delta) 가져오기

		// 마우스 이동량에 따라 카메라 Yaw(Y축 회전), Pitch(X축 회전) 조절
		if (mouseX != 0)
		{
			m_Camera->AdjustYaw(static_cast<float>(mouseX) * m_mouseSensitivity);
		}
		if (mouseY != 0)
		{
			m_Camera->AdjustPitch(static_cast<float>(mouseY) * m_mouseSensitivity);
		}

		// DeltaTime을 반영한 카메라 이동 속도 계산
		float moveSpeed = m_cameraMoveSpeed * static_cast<float>(deltaTime);

		// WASD 키 입력에 따라 카메라 이동
		if (Input->IsKeyPressed(DIK_W))
		{
			m_Camera->MoveForward(moveSpeed); // 전진
		}
		if (Input->IsKeyPressed(DIK_S))
		{
			m_Camera->MoveForward(-moveSpeed); // 후진
		}
		if (Input->IsKeyPressed(DIK_A))
		{
			m_Camera->MoveRight(-moveSpeed); // 좌측 이동
		}
		if (Input->IsKeyPressed(DIK_D))
		{
			m_Camera->MoveRight(moveSpeed); // 우측 이동
		}

		// Space 키 입력에 따라 점프
		if (Input->IsKeyPressed(DIK_SPACE))
		{
			m_Camera->Jump();
		}

		if (Input->IsKeyToggle(DIK_NUMPAD5)) // 5번 키
		{
			// (간단한 토글을 위해 bool 플래그가 필요하지만, 우선 IsKeyPressed로 대체)
			m_isAmbientOn = !m_isAmbientOn;
		}
		if (Input->IsKeyToggle(DIK_NUMPAD6)) // 6번 키
		{
			m_isDiffuseOn = !m_isDiffuseOn;
		}
		if (Input->IsKeyToggle(DIK_NUMPAD7)) // 7번 키
		{
			m_isSpecularOn = !m_isSpecularOn;
		}

		if (Input->IsKeyPressed(DIK_NUMPAD8)) // '8' - Point Light Intensity 감소
		{
			m_pointLightIntensity -= 0.05f; // 감소량 (조절 가능)
			if (m_pointLightIntensity < 0.0f) // 최소값 0으로 제한
			{
				m_pointLightIntensity = 0.0f;
			}
		}
		if (Input->IsKeyPressed(DIK_NUMPAD9)) // '9' - Point Light Intensity 증가
		{
			m_pointLightIntensity += 0.05f; // 증가량 (조절 가능)
			if (m_pointLightIntensity > 5.0f) // 최대값 5로 제한
			{
				m_pointLightIntensity = 5.0f;
			}
		}
		// 0번 키로 노말맵 토글
		if (Input->IsKeyToggle(DIK_NUMPAD0)) // 0번 키
		{
			m_isNormalMapOn = !m_isNormalMapOn;
		}
	}
}

bool GraphicsClass::Render()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix; // orthoMatrix 변수 선언
	XMMATRIX baseViewMatrix; // UI용 고정 뷰 행렬
	bool result;

	// 1. 버퍼 클리어
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// 2. 뷰 행렬 생성
	m_Camera->Render();

	// 3. 뷰/프로젝션 행렬 가져오기
	m_Camera->GetViewMatrix(viewMatrix);        // 3D 카메라 뷰
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix); // 3D 투영
	m_D3D->GetOrthoMatrix(orthoMatrix);           // [필수] 2D용 직교 투영 행렬 가져오기
	// [Tip] UI는 카메라가 움직여도 화면에 고정되어야 하므로, 
	// 움직이는 viewMatrix 대신 '단위 행렬(Identity Matrix)'을 뷰 행렬로 씁니다.
	baseViewMatrix = XMMatrixIdentity();

	// 씬 상태에 따라 다른 화면을 렌더링
	switch (m_SceneState)
	{
	case SceneState::TITLE:
		m_D3D->TurnZBufferOff();
		result = m_TitleScreen->Render(m_D3D->GetDeviceContext(), 0, 0);
		if (!result) return false;
		result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_TitleScreen->GetIndexCount(),
			worldMatrix, baseViewMatrix, orthoMatrix,
			m_TitleScreen->GetTexture());
		if (!result) return false;
		m_D3D->TurnZBufferOn();
		break;

	case SceneState::Tutorial:
		m_D3D->TurnZBufferOff();
		result = m_TutorialScreen->Render(m_D3D->GetDeviceContext(), 0, 0);
		if (!result) return false;
		result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_TutorialScreen->GetIndexCount(),
			worldMatrix, baseViewMatrix, orthoMatrix,
			m_TutorialScreen->GetTexture());
		if (!result) return false;
		m_D3D->TurnZBufferOn();
		break;

	case SceneState::MainScene:
		// 4. 스카이박스 그리기 (물체보다 먼저)
		// -------------------------------------------------
		// 스카이박스의 월드 행렬을 항상 카메라 위치로 설정
		worldMatrix = XMMatrixTranslationFromVector(m_Camera->GetPositionXM());

		// 렌더 상태 변경: 컬링 끄기 + 깊이 함수 LESS_EQUAL
		m_D3D->TurnOnNoCulling();
		m_D3D->TurnOnDepthLessEqual();

		// 스카이박스 버퍼 설정 및 셰이더 호출
		m_Skybox->Render(m_D3D->GetDeviceContext());
		result = m_SkyboxShader->Render(m_D3D->GetDeviceContext(), m_Skybox->GetIndexCount(),
			worldMatrix, viewMatrix, projectionMatrix,
			m_Skybox->GetTexture());
		if (!result) { return false; }

		// 렌더 상태 복구: 컬링 켜기 + 깊이 함수 기본값
		m_D3D->TurnOnBackCulling();
		m_D3D->TurnOffDefaultDepth();
		// -------------------------------------------------
		// 
		// m_Light 객체에서 3개의 포인트 라이트 위치/색상 배열을 가져옵니다.
		XMFLOAT4 pointLightPositions[NUM_POINT_LIGHTS];
		XMFLOAT4 pointLightColors[NUM_POINT_LIGHTS];
		for (int i = 0; i < NUM_POINT_LIGHTS; ++i)
		{
			pointLightPositions[i] = m_Light->GetPointLightPosition(i);
			pointLightColors[i] = m_Light->GetPointLightColor(i);
		}
		// -------------------------------------------------------------
		// [폴리곤 카운트 집계 로직]
		// -------------------------------------------------------------
		int totalPolygons = 0;
		// -------------------------------------------------------------
		// [메인 씬 오브젝트 집계 로직]
		// -------------------------------------------------------------
		int totalObjects = static_cast<int>(m_GameObjects.size());
		totalObjects += 1; // 바닥 모델 추가
		// 1. 리스트에 있는 모든 오브젝트 일괄 렌더링
		for (const auto& obj : m_GameObjects)
		{
			// 행렬 초기화
			worldMatrix = XMMatrixIdentity();

			// 크기 -> 회전 -> 이동 순서 적용
			worldMatrix *= XMMatrixScaling(obj.scale.x, obj.scale.y, obj.scale.z);
			worldMatrix *= XMMatrixRotationRollPitchYaw(obj.rotation.x, obj.rotation.y, obj.rotation.z);
			worldMatrix *= XMMatrixTranslation(obj.position.x, obj.position.y, obj.position.z);

			// 모델 버퍼 준비
			obj.model->Render(m_D3D->GetDeviceContext());

			// 셰이더 렌더링 (LightShader 사용)
			result = m_LightShader->Render(
				m_D3D->GetDeviceContext(),
				obj.model->GetIndexCount(),
				worldMatrix, viewMatrix, projectionMatrix,
				obj.model->GetTexture(),
				m_Light->GetDirection(),
				m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(),
				m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower(),
				m_isAmbientOn, m_isDiffuseOn, m_isSpecularOn,
				pointLightPositions, pointLightColors, m_pointLightIntensity
			);
			if (!result) return false;
			// 모델의 인덱스 개수를 3으로 나누면 삼각형 개수가 됨
			if (obj.model)
			{
				totalPolygons += obj.model->GetIndexCount() / 3;
			}
		}

		// 2. 바닥(Ground)은 별도 렌더링 (BumpMapShader 등 다른 셰이더를 쓰는 경우)
		if (m_ModelGround)
		{
			worldMatrix = XMMatrixIdentity();
			worldMatrix *= XMMatrixScaling(0.035f, 0.01f, 0.035f); // 바닥 크기 조절
			worldMatrix *= XMMatrixTranslation(0.0f, 0.0f, 0.0f); // 바닥 위치

			m_ModelGround->Render(m_D3D->GetDeviceContext());
			// BumpMapShader 또는 LightShader 사용
			result = m_BumpMapShader->Render(m_D3D->GetDeviceContext(), m_ModelGround->GetIndexCount(),
				worldMatrix, viewMatrix, projectionMatrix,
				m_GroundTextures->GetTextureArray(), // 바닥 텍스처
				m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(),
				m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower(),
				m_isAmbientOn, m_isDiffuseOn, m_isSpecularOn, m_isNormalMapOn,
				pointLightPositions, pointLightColors, m_pointLightIntensity
			);
			// 모델의 인덱스 개수를 3으로 나누면 삼각형 개수가 됨
			if (m_ModelGround)
			{
				totalPolygons += m_ModelGround->GetIndexCount() / 3;
			}
		}
		// 텍스트 출력
		wchar_t fpsText[64], 
			cpuText[64], 
			polyText[64], 
			ObjectText[64],
			resolText[64];
		swprintf_s(fpsText, 64, L"FPS: %d", m_FPS);
		swprintf_s(cpuText, 64, L"CPU: %d%%", static_cast<int>(m_CPUUsage));

		swprintf_s(polyText, 64, L"POLYGON: %d", totalPolygons);
		swprintf_s(ObjectText, 64, L"Model Count: %d", totalObjects);
		swprintf_s(resolText, 64, L"RESOLUTION: %d X %d", static_cast<int>(m_ScreenWidth), static_cast<int>(m_ScreenHeight));

		// 텍스트 렌더링
		m_D3D->TurnZBufferOff();
		m_D3D->EnableAlphaBlending();

		m_Text->DrawTextLine(fpsText, 10.0f, 0.0f);
		m_Text->DrawTextLine(cpuText, 10.0f, 11.0f);
		m_Text->DrawTextLine(polyText, 10.0f, 22.0f);
		m_Text->DrawTextLine(ObjectText, 10.0f, 33.0f);
		m_Text->DrawTextLine(resolText, 10.0f, 44.0f);
		// End the text drawing.
		m_D3D->DisableAlphaBlending();
		m_D3D->TurnZBufferOn();
		break;
	}

	// 5. 씬 종료
	m_D3D->EndScene();

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
	if (m_BumpMapShader)
	{
		m_BumpMapShader->Shutdown();
		delete m_BumpMapShader;
		m_BumpMapShader = 0;
	}
	if (m_GroundTextures)
	{
		m_GroundTextures->Shutdown();
		delete m_GroundTextures;
		m_GroundTextures = 0;
	}
	if (m_Skybox)
	{
		m_Skybox->Shutdown();
		delete m_Skybox;
		m_Skybox = 0;
	}
	if (m_SkyboxShader)
	{
		m_SkyboxShader->Shutdown();
		delete m_SkyboxShader;
		m_SkyboxShader = 0;
	}
	// Release the texture shader object.
	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	// Release the light object.
	if (m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}

	// Release the light shader object.
	if (m_LightShader)
	{
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = 0;
	}

	// Release the camera object.
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	// Release the D3D object.
	if (m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}

	// 모든 3D 모델 리소스 해제
	if (m_ModelWall)
	{
		m_ModelWall->Shutdown();
		delete m_ModelWall;
		m_ModelWall = 0;
	}
	if (m_ModelGround)
	{
		m_ModelGround->Shutdown();
		delete m_ModelGround;
		m_ModelGround = 0;
	}
	if (m_ModelCrate)
	{
		m_ModelCrate->Shutdown();
		delete m_ModelCrate;
		m_ModelCrate = 0;
	}
	if (m_ModelBarricade)
	{
		m_ModelBarricade->Shutdown();
		delete m_ModelBarricade;
		m_ModelBarricade = 0;
	}
	if (m_ModelObstacle)
	{
		m_ModelObstacle->Shutdown();
		delete m_ModelObstacle;
		m_ModelObstacle = 0;
	}
	if (m_ModelCar)
	{
		m_ModelCar->Shutdown();
		delete m_ModelCar;
		m_ModelCar = 0;
	}
	if (m_ModelTent)
	{
		m_ModelTent->Shutdown();
		delete m_ModelTent;
		m_ModelTent = 0;
	}
	if (m_ModelTree)
	{
		m_ModelTree->Shutdown();
		delete m_ModelTree;
		m_ModelTree = 0;
	}

	return;
}