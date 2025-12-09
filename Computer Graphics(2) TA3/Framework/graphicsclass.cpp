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
	m_GroundTiles = std::vector<GameObject>();
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
	m_SceneState = SceneState::MainScene;
	m_showDebug = true; // 기본값: 켜짐
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
	m_ModelGround->Initialize(m_D3D->GetDevice(), "./data/Ground.fbx", L"./data/Ground.dds");
	// -----------------------------------------------------------
	// [수정] 바닥 타일 10x10 배치 (Tiling)
	// -----------------------------------------------------------

	// 1. 요청하신 스케일 (텍스처가 예쁘게 나오는 크기)
	float groundScale = 0.02f;

	// 2. ★중요★ 타일 간격 (Gap)
	// 모델의 원본 크기를 모르기 때문에 이 값을 조절해서 틈을 맞춰야 합니다.
	// 팁: 실행 후 바닥 사이에 틈이 보이면 이 값을 줄이고, 겹치면 늘리세요.
	float tileSpacing = 8.0f; // <-- 화면 보시면서 이 숫자를 조절하세요!

	// 3. 10x10 배치 (-5 ~ +4 범위로 하면 중앙이 (0,0) 근처가 됩니다)
	for (int x = -5; x < 5; x++)
	{
		for (int z = -5; z < 5; z++)
		{
			GameObject tile;
			tile.model = m_ModelGround; // 모델 리소스 공유

			// 위치 선정: (인덱스 * 간격)
			// x는 -5, -4 ... 4 까지 변함
			tile.position = XMFLOAT3(x * tileSpacing, 0.0f, z * tileSpacing);

			tile.rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
			tile.scale = XMFLOAT3(groundScale, groundScale, groundScale);

			// 리스트에 추가
			m_GroundTiles.push_back(tile);
		}
	}
	//  지면 텍스처 배열 초기화
	m_GroundTextures = new TextureArrayClass;
	if (!m_GroundTextures) { return false; }
	result = m_GroundTextures->Initialize(m_D3D->GetDevice(),
		L"./data/Ground.dds",  // (지면 색상 텍스처)
		L"./data/Ground_normal.dds"); // (지면 노멀맵 텍스처)
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
	float modelYOffset = 0.35f; // 모델별 Y축 오프셋 (필요시 조절)

	// -----------------------------------------------------------
	// [A] 벽 (Walls) - "울타리 스타일" (여러 개 이어 붙이기)
	// -----------------------------------------------------------

	// 각 모델의 스케일
	float brickScale = 0.02f;

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

	// [1] 디버그 모델 초기화 (파일 로딩 X, 코드 생성 O)
	m_DebugModel = new ModelClass;
	// InitializeGeneratedCube 호출! (경로 인자 필요 없음)
	result = m_DebugModel->InitializeGeneratedCube(m_D3D->GetDevice());

	// [2] 디버그 셰이더 초기화 (debug.hlsl 로드)
	m_DebugShader = new TextureShaderClass;
	// 기존 TextureShaderClass를 쓰지만 파일만 debug.hlsl로 교체
	result = m_DebugShader->InitializeShader(m_D3D->GetDevice(), hwnd, L"data/debug.hlsl");
	// 주의: TextureShaderClass::Initialize()는 파일명이 고정되어 있을 수 있으니, 
	// InitializeShader를 public으로 열거나, TextureShaderClass를 복사해서 쓰세요.
	// 만약 수정이 어렵다면 data/textureShader.hlsl 내용을 아까 만든 빨간색 코드로 덮어쓰세요! (가장 쉬움)

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
	// -------------------------------------------------------------
	// 1. 기본 설정 및 변수 준비
	// -------------------------------------------------------------
	m_FPS = fps;
	m_CPUUsage = cpuUsage;

	// 프레임 시작 시점의 위치 (이전 프레임의 최종 위치)
	XMFLOAT3 currentPos = m_Camera->GetPosition();

	// 키보드 입력 처리 (카메라 내부 변수 갱신)
	if (m_isCameraControlMode)
	{
		HandleInput(Input, deltaTime);
	}

	// ★ 핵심 1: 카메라 클래스에서 "이동하려는 양(Vector)"만 받아옵니다.
	// (이때 m_Camera 내부의 실제 위치는 변하지 않아야 합니다. 아까 수정한 GetFrameTranslation 사용)
	XMFLOAT3 moveVector = m_Camera->GetFrameTranslation();

	// -------------------------------------------------------------
	// 2. 점프 및 중력 (Y축) 물리 계산
	// -------------------------------------------------------------
	// 점프/중력용 변수 (멤버 변수로 선언하면 더 좋지만, 여기선 static으로 유지)
	static float verticalVelocity = 0.0f;
	const float GRAVITY = -20.0f;       // 중력 가속도
	const float JUMP_FORCE = 10.0f;     // 점프 힘
	const float GROUND_LEVEL = 2.0f;    // 바닥 높이 (하드코딩)

	// (1) 점프 입력 확인 (바닥에 있을 때만 가능)
	// 약간의 오차(epsilon)를 두어 바닥 체크
	bool isGrounded = (currentPos.y <= GROUND_LEVEL + 0.01f);

	if (isGrounded && Input->IsKeyPressed(DIK_SPACE))
	{
		verticalVelocity = JUMP_FORCE; // 위로 솟구침
		isGrounded = false;
	}

	// (2) 중력 적용 (공중에 있을 때만)
	if (!isGrounded)
	{
		verticalVelocity += GRAVITY * (float)deltaTime;
	}

	// (3) Y축 이동량 계산
	float deltaY = verticalVelocity * (float)deltaTime;
	float nextY = currentPos.y + deltaY;

	// (4) 바닥 충돌 처리 (땅 뚫기 방지)
	if (nextY < GROUND_LEVEL)
	{
		nextY = GROUND_LEVEL;
		verticalVelocity = 0.0f; // 땅에 닿았으니 속도 초기화
		isGrounded = true;
	}

	// -------------------------------------------------------------
	// 3. 충돌 검사 및 이동 (X, Z축 분리 - 슬라이딩 구현)
	// -------------------------------------------------------------

	// 플레이어 충돌 박스 크기
	float pRadius = 0.5f;
	float pHeight = 2.0f;
	float footY = nextY - 1.8f; // 눈높이(1.8)를 뺀 발바닥 위치

	// 최종적으로 이동할 목표 좌표
	XMFLOAT3 targetPos = { currentPos.x, nextY, currentPos.z };

	// 디버그 텍스트용 플래그
	bool isCollidedX = false;
	bool isCollidedZ = false;
	int collidedIndex = -1;

	// [Step A] X축 이동 시도
	float testX = currentPos.x + moveVector.x;

	// X축 이동 가상 박스 생성
	XMFLOAT3 minX = { testX - pRadius, footY, currentPos.z - pRadius };
	XMFLOAT3 maxX = { testX + pRadius, footY + pHeight, currentPos.z + pRadius };

	for (int i = 0; i < m_GameObjects.size(); ++i)
	{
		if (!m_GameObjects[i].model || m_GameObjects[i].model == m_ModelGround) continue;

		XMFLOAT3 worldMin, worldMax;
		GetWorldAABB(m_GameObjects[i], worldMin, worldMax); // 회전 반영 AABB

		// Y축 무시하고 검사 (CheckAABBCollision 함수는 Y검사 주석 처리된 상태여야 함)
		if (CheckAABBCollision(minX, maxX, worldMin, worldMax))
		{
			isCollidedX = true;
			collidedIndex = i;
			break;
		}
	}

	// X축 충돌 없으면 이동 적용, 있으면 현상 유지(이동 안 함)
	if (!isCollidedX) targetPos.x = testX;


	// [Step B] Z축 이동 시도 (X축은 이미 결정됨)
	float testZ = currentPos.z + moveVector.z;

	// Z축 이동 가상 박스 생성 (X는 targetPos.x 사용 - 슬라이딩 핵심)
	XMFLOAT3 minZ = { targetPos.x - pRadius, footY, testZ - pRadius };
	XMFLOAT3 maxZ = { targetPos.x + pRadius, footY + pHeight, testZ + pRadius };

	for (int i = 0; i < m_GameObjects.size(); ++i)
	{
		if (!m_GameObjects[i].model || m_GameObjects[i].model == m_ModelGround) continue;

		XMFLOAT3 worldMin, worldMax;
		GetWorldAABB(m_GameObjects[i], worldMin, worldMax);

		if (CheckAABBCollision(minZ, maxZ, worldMin, worldMax))
		{
			isCollidedZ = true;
			collidedIndex = i;
			break;
		}
	}

	// Z축 충돌 없으면 이동 적용
	if (!isCollidedZ) targetPos.z = testZ;


	// -------------------------------------------------------------
	// 4. 최종 위치 적용 (Render 전, 유일한 위치 갱신 시점)
	// -------------------------------------------------------------
	m_Camera->SetPosition(targetPos.x, targetPos.y, targetPos.z);


	// -------------------------------------------------------------
	// 5. 디버그 정보 출력 메시지 생성
	// -------------------------------------------------------------
	static wchar_t debugMsg[256];
	if (isCollidedX || isCollidedZ)
	{
		swprintf_s(debugMsg, 256, L"COLLISION! Idx:%d / Blocked: %s%s",
			collidedIndex,
			isCollidedX ? L"[X] " : L"",
			isCollidedZ ? L"[Z]" : L"");
	}
	else
	{
		swprintf_s(debugMsg, 256, L"Status: Free / VelY: %.1f", verticalVelocity);
	}

	// 렌더링
	if (!Render(debugMsg)) return false;

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
		if (Input->IsKeyToggle(DIK_NUMPAD1))
		{
			m_showDebug = !m_showDebug;
		}
	}
}

bool GraphicsClass::Render(wchar_t* debugText)
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

		// -------------------------------------------------------------
		// [수정] 바닥(Ground) 10x10 타일 렌더링
		// -------------------------------------------------------------
		// 기존의 단일 바닥 렌더링 코드는 주석 처리하거나 삭제하세요.

		for (const auto& tile : m_GroundTiles)
		{
			// 월드 행렬 초기화
			worldMatrix = XMMatrixIdentity();

			// 1. 크기 (0.02f)
			worldMatrix *= XMMatrixScaling(tile.scale.x, tile.scale.y, tile.scale.z);

			// 2. 회전
			worldMatrix *= XMMatrixRotationRollPitchYaw(tile.rotation.x, tile.rotation.y, tile.rotation.z);

			// 3. 이동 (위치)
			worldMatrix *= XMMatrixTranslation(tile.position.x, tile.position.y, tile.position.z);

			// 모델 버퍼 준비
			tile.model->Render(m_D3D->GetDeviceContext());

			// 셰이더 렌더링 (BumpMapShader 사용)
			result = m_BumpMapShader->Render(m_D3D->GetDeviceContext(), tile.model->GetIndexCount(),
				worldMatrix, viewMatrix, projectionMatrix,
				m_GroundTextures->GetTextureArray(), // 바닥 텍스처 배열
				m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(),
				m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower(),
				m_isAmbientOn, m_isDiffuseOn, m_isSpecularOn, m_isNormalMapOn,
				pointLightPositions, pointLightColors, m_pointLightIntensity
			);

			// 폴리곤 카운트 집계
			if (tile.model)
			{
				totalPolygons += tile.model->GetIndexCount() / 3;
			}
		}

	// =============================================================
	// [충돌 박스 디버깅: BeginScene과 EndScene 사이에서 그려야 보입니다!]
	// =============================================================

		if (m_showDebug) // ★ 1번 키로 제어됨
		{
			// 1. 플레이어 충돌 박스 그리기
			XMFLOAT3 camPos = m_Camera->GetPosition();
			float playerR = 0.5f;
			float playerH = 2.0f;
			XMFLOAT3 pMin = { camPos.x - playerR, camPos.y - 2.0f, camPos.z - playerR }; // 높이 보정 주의
			XMFLOAT3 pMax = { camPos.x + playerR, camPos.y,        camPos.z + playerR };
			RenderDebugAABB(pMin, pMax);

			// 2. 장애물 충돌 박스 그리기
			for (const GameObject& obj : m_GameObjects)
			{
				if (obj.model == nullptr) continue;
				if (obj.model == m_ModelGround) continue;

				XMFLOAT3 worldMin, worldMax;
				GetWorldAABB(obj, worldMin, worldMax);
				RenderDebugAABB(worldMin, worldMax);
			}
		}
		// =============================================================

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
		// ★★★ 디버그 메시지 화면 출력 ★★★
		// debugText가 들어왔다면(충돌 정보 등) 출력
		if (debugText)
		{
			// 빨간색이나 노란색으로 잘 보이게 출력 (DrawTextLine 함수에 색상 인자가 있다면 활용)
			m_Text->DrawTextLine(debugText, 10.0f, 100.0f);

			// 플레이어 좌표도 출력하면 도움됨
			wchar_t posText[64];
			XMFLOAT3 p = m_Camera->GetPosition();
			swprintf_s(posText, 64, L"Pos: %.2f, %.2f, %.2f", p.x, p.y, p.z);
			m_Text->DrawTextLine(posText, 10.0f, 120.0f);
		}
		// 텍스트 렌더링
		m_D3D->TurnZBufferOff();
		m_D3D->EnableAlphaBlending();

		m_Text->DrawTextLine(fpsText, 10.0f, 0.0f);
		m_Text->DrawTextLine(cpuText, 10.0f, 15.0f);
		m_Text->DrawTextLine(polyText, 10.0f, 30.0f);
		m_Text->DrawTextLine(ObjectText, 10.0f, 45.0f);
		m_Text->DrawTextLine(resolText, 10.0f, 60.0f);
		// End the text drawing.
		m_D3D->DisableAlphaBlending();
		m_D3D->TurnZBufferOn();
		break;
	}

	

	m_D3D->EndScene(); // <-- 이 함수 바로 위에 있어야 합니다.
	return true;
}

bool GraphicsClass::CheckAABBCollision(XMFLOAT3 min1, XMFLOAT3 max1, XMFLOAT3 min2, XMFLOAT3 max2)
{
	// X축 검사 (좌우)
	if (max1.x < min2.x || min1.x > max2.x) return false;

	// Z축 검사 (앞뒤)
	if (max1.z < min2.z || min1.z > max2.z) return false;
	// Y축 검사 (상하)
	if (max1.y < min2.y || min1.y > max2.y) return false;

	// X, Y, Z가 모두 겹칠 때만 충돌
	return true;
}

void GraphicsClass::RenderDebugAABB(XMFLOAT3 min, XMFLOAT3 max)
{
	// 디버그 모델이 없으면 리턴
	if (!m_DebugModel) return;

	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	// 1. 카메라 및 투영 행렬 가져오기
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	// 2. 타겟 크기(Size) 계산
	// (우리가 만든 큐브는 기본 크기가 1.0이므로, 스케일값이 곧 크기가 됩니다)
	float sizeX = max.x - min.x;
	float sizeY = max.y - min.y;
	float sizeZ = max.z - min.z;

	// 너무 작아서 안 보이는 경우 방지 (최소 크기 보장)
	if (sizeX < 0.001f) sizeX = 0.001f;
	if (sizeY < 0.001f) sizeY = 0.001f;
	if (sizeZ < 0.001f) sizeZ = 0.001f;

	// 3. 타겟 중심(Center) 계산
	float centerX = (min.x + max.x) * 0.5f;
	float centerY = (min.y + max.y) * 0.5f;
	float centerZ = (min.z + max.z) * 0.5f;

	// 4. 월드 행렬 생성 (스케일 -> 이동)
	// 복잡한 오프셋 계산 없이 깔끔하게 스케일과 이동만 적용하면 됩니다.
	worldMatrix = XMMatrixScaling(sizeX, sizeY, sizeZ) * XMMatrixTranslation(centerX, centerY, centerZ);

	// 5. 렌더링 설정 (와이어프레임 + Z버퍼 끄기)
	m_D3D->TurnOnWireframe();
	// m_D3D->TurnZBufferOff(); // 벽 뒤에 있는 박스도 보고 싶으면 주석 해제 (D3DClass에 함수 필요)

	// 모델 버퍼 등록
	m_DebugModel->Render(m_D3D->GetDeviceContext());

	// 6. 셰이더 렌더링 (빨간색 출력)
	// m_DebugShader(또는 m_TextureShader)를 사용하되 텍스처 인자에 nullptr을 넣습니다.
	// (debug.hlsl을 사용한다면 텍스처가 없어도 빨간색을 리턴합니다)
	if (m_DebugShader)
	{
		m_DebugShader->Render(m_D3D->GetDeviceContext(), m_DebugModel->GetIndexCount(),
			worldMatrix, viewMatrix, projectionMatrix,
			nullptr); // <--- 텍스처 없이 렌더링
	}
	else
	{
		// 만약 m_DebugShader 변수를 따로 안 만들고 m_TextureShader를 재사용한다면 이렇게 쓰세요.
		m_TextureShader->Render(m_D3D->GetDeviceContext(), m_DebugModel->GetIndexCount(),
			worldMatrix, viewMatrix, projectionMatrix,
			nullptr);
	}

	// 7. 설정 복구
	// m_D3D->TurnZBufferOn(); // Z버퍼 껐다면 복구 필요
	m_D3D->TurnOffWireframe();
}

void GraphicsClass::GetWorldAABB(const GameObject& obj, XMFLOAT3& outMin, XMFLOAT3& outMax)
{
	// 1. 모델의 로컬 AABB (회전 전)
	XMFLOAT3 localMin, localMax;
	obj.model->GetMinMax(localMin, localMax);

	// 2. 로컬 AABB의 8개 코너 점 생성
	XMVECTOR corners[8];
	corners[0] = XMVectorSet(localMin.x, localMin.y, localMin.z, 1.0f);
	corners[1] = XMVectorSet(localMin.x, localMin.y, localMax.z, 1.0f);
	corners[2] = XMVectorSet(localMin.x, localMax.y, localMin.z, 1.0f);
	corners[3] = XMVectorSet(localMin.x, localMax.y, localMax.z, 1.0f);
	corners[4] = XMVectorSet(localMax.x, localMin.y, localMin.z, 1.0f);
	corners[5] = XMVectorSet(localMax.x, localMin.y, localMax.z, 1.0f);
	corners[6] = XMVectorSet(localMax.x, localMax.y, localMin.z, 1.0f);
	corners[7] = XMVectorSet(localMax.x, localMax.y, localMax.z, 1.0f);

	// 3. 월드 행렬 생성 (크기 -> 회전 -> 이동) ★★★ 회전 반영!
	XMMATRIX worldMatrix = XMMatrixIdentity();
	worldMatrix *= XMMatrixScaling(obj.scale.x, obj.scale.y, obj.scale.z);
	worldMatrix *= XMMatrixRotationRollPitchYaw(obj.rotation.x, obj.rotation.y, obj.rotation.z);
	worldMatrix *= XMMatrixTranslation(obj.position.x, obj.position.y, obj.position.z);

	// 4. 8개 점을 월드 좌표로 변환 후 새로운 Min/Max 찾기
	XMVECTOR vMin = XMVectorSet(FLT_MAX, FLT_MAX, FLT_MAX, 0.0f);
	XMVECTOR vMax = XMVectorSet(-FLT_MAX, -FLT_MAX, -FLT_MAX, 0.0f);

	for (int i = 0; i < 8; ++i)
	{
		corners[i] = XMVector3TransformCoord(corners[i], worldMatrix);
		vMin = XMVectorMin(vMin, corners[i]);
		vMax = XMVectorMax(vMax, corners[i]);
	}

	XMStoreFloat3(&outMin, vMin);
	XMStoreFloat3(&outMax, vMax);
}

void GraphicsClass::Shutdown()
{
	// Release the debug shader object.
	if (m_DebugShader)
	{
		m_DebugShader->Shutdown();
		delete m_DebugShader;
		m_DebugShader = 0;
	}
	// Release the debug model object.
	if (m_DebugModel)
	{
		m_DebugModel->Shutdown();
		delete m_DebugModel;
		m_DebugModel = 0;
	}
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