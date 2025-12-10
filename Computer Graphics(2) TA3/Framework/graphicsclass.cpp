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
	m_Camera->SetRotation(0.0f, 90.0f, 0.0f);
	m_Camera->SetGroundLevel(3.0f);

	// -----------------------------------------------------------
	// 1. 모델 리소스 로딩
	// -----------------------------------------------------------

	// [플레이어]
	m_Player.model = new ModelClass;
	result = m_Player.model->Initialize(m_D3D->GetDevice(), "./data/Player.fbx", L"./data/Texture.dds");
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the Player.", L"Error", MB_OK);
		return false;
	}

	m_Player.position = XMFLOAT3(-20.0f, 3.0f, -20.0f);
	m_Player.rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_Player.scale = XMFLOAT3(0.01f, 0.01f, 0.01f); // FBX 단위에 따라 조절

	// 2. TPS 카메라 설정
	// dist: 6.0f (카메라와 캐릭터 거리)
	// height: 2.5f (무시됨, m_targetOffset 사용)
	// offset: 0.0f (플레이어 발바닥부터 머리까지의 높이. 이 점을 바라봅니다)
	// smooth: 10.0f (보간 속도)
	m_Camera->SetFollowParameters(6.0f, 2.5f, 0.0f, 10.0f);

	// [벽 & 바닥]
	m_ModelWall = new ModelClass;
	m_ModelWall->Initialize(m_D3D->GetDevice(), "./data/Wall.fbx", L"./data/Texture.dds");

	m_ModelGround = new ModelClass;
	m_ModelGround->Initialize(m_D3D->GetDevice(), "./data/Ground.fbx", L"./data/Ground.dds");

	// 바닥 타일 배치
	float groundScale = 0.02f;
	float tileSpacing = 8.0f;

	for (int x = -5; x < 5; x++)
	{
		for (int z = -5; z < 5; z++)
		{
			GameObject tile;
			tile.model = m_ModelGround;
			tile.position = XMFLOAT3(x * tileSpacing, 0.0f, z * tileSpacing);
			tile.rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
			tile.scale = XMFLOAT3(groundScale, groundScale, groundScale);
			m_GroundTiles.push_back(tile);
		}
	}

	// 지면 텍스처 배열 초기화
	m_GroundTextures = new TextureArrayClass;
	if (!m_GroundTextures) { return false; }
	result = m_GroundTextures->Initialize(m_D3D->GetDevice(),
		L"./data/Ground.dds",
		L"./data/Ground_normal.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the ground texture array object.", L"Error", MB_OK);
		return false;
	}

	// [오브젝트]
	m_ModelCrate = new ModelClass;
	m_ModelCrate->Initialize(m_D3D->GetDevice(), "./data/Crate.fbx", L"./data/Texture.dds");

	m_ModelBarricade = new ModelClass;
	m_ModelBarricade->Initialize(m_D3D->GetDevice(), "./data/Barricade.fbx", L"./data/Texture.dds");

	m_ModelObstacle = new ModelClass;
	m_ModelObstacle->Initialize(m_D3D->GetDevice(), "./data/Tire.fbx", L"./data/Texture.dds");

	m_ModelCar = new ModelClass;
	m_ModelCar->Initialize(m_D3D->GetDevice(), "./data/Car.fbx", L"./data/Vehicles_Destroy.dds");

	m_ModelTent = new ModelClass;
	m_ModelTent->Initialize(m_D3D->GetDevice(), "./data/Tent.fbx", L"./data/Texture.dds");

	m_ModelTree = new ModelClass;
	m_ModelTree->Initialize(m_D3D->GetDevice(), "./data/Tree.fbx", L"./data/Texture.dds");

	// -----------------------------------------------------------
	// 맵 배치 (Blueprint 적용)
	// -----------------------------------------------------------
	float commonScale = 0.015f;
	float commonScaleY = commonScale + 0.005f;
	float modelYOffset = 0.35f;

	float brickScale = 0.02f;
	float mapLimit = 30.0f;
	float spacing = 3.8f;

	for (float i = -mapLimit - 3; i <= mapLimit; i += spacing)
	{
		// 북쪽
		GameObject wNorth;
		wNorth.model = m_ModelWall;
		wNorth.position = XMFLOAT3(i, 0, mapLimit);
		wNorth.rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
		wNorth.scale = XMFLOAT3(brickScale, brickScale, brickScale);
		m_GameObjects.push_back(wNorth);

		// 남쪽
		GameObject wSouth;
		wSouth.model = m_ModelWall;
		wSouth.position = XMFLOAT3(i, 0, -mapLimit);
		wSouth.rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
		wSouth.scale = XMFLOAT3(brickScale, brickScale, brickScale);
		m_GameObjects.push_back(wSouth);

		// 서쪽
		GameObject wWest;
		wWest.model = m_ModelWall;
		wWest.position = XMFLOAT3(-mapLimit, 0, i);
		wWest.rotation = XMFLOAT3(0.0f, XM_PI * 0.5f, 0.0f);
		wWest.scale = XMFLOAT3(brickScale, brickScale, brickScale);
		m_GameObjects.push_back(wWest);

		// 동쪽
		GameObject wEast;
		wEast.model = m_ModelWall;
		wEast.position = XMFLOAT3(mapLimit, 0, i);
		wEast.rotation = XMFLOAT3(0.0f, XM_PI * 0.5f, 0.0f);
		wEast.scale = XMFLOAT3(brickScale, brickScale, brickScale);
		m_GameObjects.push_back(wEast);
	}

	// [B] 중앙 목표 (보급상자)
	{
		GameObject obj; obj.model = m_ModelCrate;
		obj.position = XMFLOAT3(0.0f, modelYOffset, 0.0f);
		obj.rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
		obj.scale = XMFLOAT3(commonScale, commonScaleY, commonScale);
		m_GameObjects.push_back(obj);
	}

	// [C] 차량
	{
		GameObject c1; c1.model = m_ModelCar;
		c1.position = XMFLOAT3(-4.5f, modelYOffset, 4.0f);
		c1.rotation = XMFLOAT3(0.0f, -0.5f, 0.0f);
		c1.scale = XMFLOAT3(commonScale, commonScaleY, commonScale);
		m_GameObjects.push_back(c1);

		GameObject c2; c2.model = m_ModelCar;
		c2.position = XMFLOAT3(6.5f, modelYOffset, -4.0f);
		c2.rotation = XMFLOAT3(0.0f, 2.5f, 0.0f);
		c2.scale = XMFLOAT3(commonScale, commonScaleY, commonScale);
		m_GameObjects.push_back(c2);
	}

	// [D] 방어선
	float barricadeZ = 7.5f;
	for (int dir : {1, -1})
	{
		GameObject bar; bar.model = m_ModelBarricade;
		bar.position = XMFLOAT3(0.0f, modelYOffset, barricadeZ * dir);
		bar.rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
		bar.scale = XMFLOAT3(commonScale, commonScaleY, commonScale);
		m_GameObjects.push_back(bar);

		GameObject t1; t1.model = m_ModelObstacle;
		t1.position = XMFLOAT3(-3.0f, modelYOffset, barricadeZ * dir);
		t1.rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
		t1.scale = XMFLOAT3(commonScale, commonScaleY, commonScale);
		m_GameObjects.push_back(t1);

		GameObject t2; t2.model = m_ModelObstacle;
		t2.position = XMFLOAT3(3.0f, modelYOffset, barricadeZ * dir);
		t2.rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
		t2.scale = XMFLOAT3(commonScale, commonScaleY, commonScale);
		m_GameObjects.push_back(t2);
	}

	// [E] 스폰 포인트
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

	// [F] 조경 (나무)
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

	// [1] 디버그 모델 초기화
	m_DebugModel = new ModelClass;
	result = m_DebugModel->InitializeGeneratedCube(m_D3D->GetDevice());

	// [2] 디버그 셰이더 초기화
	m_DebugShader = new TextureShaderClass;
	result = m_DebugShader->InitializeShader(m_D3D->GetDevice(), hwnd, L"data/debug.hlsl");

	// 범프맵 셰이더
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
	m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(0.0f, -0.5f, 1.0f);
	m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetSpecularPower(32.0f);

	// Point Light 초기화
	m_pointLightIntensity = 7.0f;
	m_Light->SetPointLightColor(0, 1.0f, 0.0f, 0.0f, 1.0f);
	m_Light->SetPointLightPosition(0, 0.0f, 3.0f, -18.0f);
	m_Light->SetPointLightColor(1, 0.0f, 1.0f, 0.0f, 1.0f);
	m_Light->SetPointLightPosition(1, 0.0f, 3.0f, 0.0f);
	m_Light->SetPointLightColor(2, 0.0f, 0.0f, 1.0f, 1.0f);
	m_Light->SetPointLightPosition(2, 0.0f, 3.0f, 18.0f);

	// 스카이박스
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

	m_Skybox = new SkyboxClass;
	if (!m_Skybox)
	{
		return false;
	}
	result = m_Skybox->Initialize(m_D3D->GetDevice(), L"./data/skybox.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the skybox model object.", L"Error", MB_OK);
		return false;
	}

	// 텍스처 셰이더
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
	m_FPS = fps;
	m_CPUUsage = cpuUsage;

	// 1. 입력 처리 (마우스 회전, 메뉴 토글 등)
	if (m_isCameraControlMode)
	{
		HandleInput(Input, deltaTime);
	}

	// -------------------------------------------------------------
	// [수정됨] 2. 카메라 기준 이동 (Camera-Relative Movement)
	// -------------------------------------------------------------

	// A. [핵심 수정] GetRotation() 대신, 새로 만든 GetYaw()를 사용하여 정확한 라디안 값을 가져옵니다.
	float camYaw = m_Camera->GetYaw();

	// B. 카메라가 바라보는 방향의 행렬 생성
	XMMATRIX cameraRotMatrix = XMMatrixRotationY(camYaw);

	// C. 이동 기준 벡터 생성 (월드 정면/우측 벡터를 카메라 각도만큼 회전)
	XMVECTOR camForwardVector = XMVector3TransformCoord(XMVectorSet(0, 0, 1, 0), cameraRotMatrix);
	XMVECTOR camRightVector = XMVector3TransformCoord(XMVectorSet(1, 0, 0, 0), cameraRotMatrix);

	// Y축 성분 제거 & 정규화 (하늘을 봐도 땅으로 파고들지 않게 함)
	camForwardVector = XMVector3Normalize(XMVectorSetY(camForwardVector, 0.0f));
	camRightVector = XMVector3Normalize(XMVectorSetY(camRightVector, 0.0f));

	float speed = m_cameraMoveSpeed * (float)deltaTime;
	XMVECTOR moveVec = XMVectorSet(0, 0, 0, 0);

	// D. 입력에 따른 이동 (이제 삐딱하지 않고 정확히 카메라 기준 전후좌우로 움직입니다)
	if (Input->IsKeyPressed(DIK_W)) moveVec += camForwardVector;
	if (Input->IsKeyPressed(DIK_S)) moveVec -= camForwardVector;
	if (Input->IsKeyPressed(DIK_D)) moveVec += camRightVector;
	if (Input->IsKeyPressed(DIK_A)) moveVec -= camRightVector;

	// 대각선 이동 속도 보정
	if (XMVectorGetX(XMVector3LengthSq(moveVec)) > 0.0001f)
	{
		moveVec = XMVector3Normalize(moveVec) * speed;
	}

	XMFLOAT3 moveDelta;
	XMStoreFloat3(&moveDelta, moveVec);

	// -------------------------------------------------------------
	// [수정됨] 3. 플레이어 회전 (캐릭터가 이동 방향 바라보기)
	// -------------------------------------------------------------
	if (fabs(moveDelta.x) > 0.0001f || fabs(moveDelta.z) > 0.0001f)
	{
		// 이동 벡터의 각도 계산 (atan2는 (0,0)에서 (x,z)를 바라보는 각도를 줌)
		float targetRotation = atan2f(moveDelta.x, moveDelta.z);

		// [핵심 수정] 반대 방향을 본다면 여기에 XM_PI (180도)를 더해줍니다.
		// 현재 20도 정도 틀어진다고 하셨는데, 모델 자체가 약간 회전되어 있을 수 있습니다.
		// 만약 정반대(180도)가 아니라 미세하게 틀어진다면 XM_PI 대신 값을 조정해야 합니다.
		// 일단 정석적인 180도 반전을 적용합니다. (뒷면이 보인다면 이 값을 지우세요)
		m_Player.rotation.y = targetRotation + XM_PI;
	}

	// -------------------------------------------------------------
	// 4. 점프 및 중력 처리 (기존 유지)
	// -------------------------------------------------------------
	static float verticalVelocity = 0.0f;
	const float GRAVITY = -20.0f;
	const float JUMP_FORCE = 10.0f;
	const float GROUND_LEVEL = 2.0f;

	XMFLOAT3 currentPos = m_Player.position;
	bool isGrounded = (currentPos.y <= GROUND_LEVEL + 0.01f);

	if (isGrounded && Input->IsKeyPressed(DIK_SPACE))
	{
		verticalVelocity = JUMP_FORCE;
		isGrounded = false;
	}

	if (!isGrounded)
	{
		verticalVelocity += GRAVITY * (float)deltaTime;
	}

	float deltaY = verticalVelocity * (float)deltaTime;
	float nextY = currentPos.y + deltaY;

	if (nextY < GROUND_LEVEL)
	{
		nextY = GROUND_LEVEL;
		verticalVelocity = 0.0f;
		isGrounded = true;
	}

	// -------------------------------------------------------------
	// 5. 충돌 검사 (X, Z축 분리 - 슬라이딩 구현) (기존 유지)
	// -------------------------------------------------------------
	float pRadius = 0.5f;
	float pHeight = 2.0f;
	float footY = nextY - 1.8f;

	XMFLOAT3 targetPos = currentPos;
	targetPos.y = nextY;

	bool isCollidedX = false;
	bool isCollidedZ = false;
	int collidedIndex = -1;

	// [X축 이동 검사]
	float testX = currentPos.x + moveDelta.x;
	XMFLOAT3 minX = { testX - pRadius, footY, currentPos.z - pRadius };
	XMFLOAT3 maxX = { testX + pRadius, footY + pHeight, currentPos.z + pRadius };

	for (int i = 0; i < m_GameObjects.size(); ++i)
	{
		if (!m_GameObjects[i].model || m_GameObjects[i].model == m_ModelGround) continue;
		XMFLOAT3 worldMin, worldMax;
		GetWorldAABB(m_GameObjects[i], worldMin, worldMax);
		if (CheckAABBCollision(minX, maxX, worldMin, worldMax))
		{
			isCollidedX = true;
			collidedIndex = i;
			break;
		}
	}
	if (!isCollidedX) targetPos.x = testX;

	// [Z축 이동 검사]
	float testZ = currentPos.z + moveDelta.z;
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
	if (!isCollidedZ) targetPos.z = testZ;

	// 6. 최종 위치 적용
	m_Player.position = targetPos;

	// 7. 카메라 추적 업데이트
	m_Camera->UpdateFollowCamera(m_Player.position);

	// 8. 디버그 메시지 및 렌더링 호출
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
		swprintf_s(debugMsg, 256, L"Move: %.2f, %.2f / Yaw: %.1f",
			moveDelta.x, moveDelta.z, XMConvertToDegrees(m_Player.rotation.y));
	}

	if (!Render(debugMsg)) return false;

	return true;
}

// 입력 처리 헬퍼 함수
void GraphicsClass::HandleInput(InputClass* Input, double deltaTime)
{
	if (Input->IsAnyKeyJustPressed())
	{
		if (m_SceneState == SceneState::TITLE) m_SceneState = SceneState::Tutorial;
		else if (m_SceneState == SceneState::Tutorial) m_SceneState = SceneState::MainScene;
	}

	if (m_SceneState == SceneState::MainScene)
	{
		long mouseX = 0, mouseY = 0;
		Input->GetMouseDelta(mouseX, mouseY);

		// 마우스 회전 -> CameraClass의 Yaw/Pitch 조절
		if (mouseX != 0) m_Camera->AdjustYaw(static_cast<float>(mouseX) * m_mouseSensitivity);
		if (mouseY != 0) m_Camera->AdjustPitch(static_cast<float>(mouseY) * m_mouseSensitivity);

		// 토글 키 처리
		if (Input->IsKeyToggle(DIK_NUMPAD5)) m_isAmbientOn = !m_isAmbientOn;
		if (Input->IsKeyToggle(DIK_NUMPAD6)) m_isDiffuseOn = !m_isDiffuseOn;
		if (Input->IsKeyToggle(DIK_NUMPAD7)) m_isSpecularOn = !m_isSpecularOn;

		if (Input->IsKeyPressed(DIK_NUMPAD8))
		{
			m_pointLightIntensity -= 0.05f;
			if (m_pointLightIntensity < 0.0f) m_pointLightIntensity = 0.0f;
		}
		if (Input->IsKeyPressed(DIK_NUMPAD9))
		{
			m_pointLightIntensity += 0.05f;
			if (m_pointLightIntensity > 5.0f) m_pointLightIntensity = 5.0f;
		}

		if (Input->IsKeyToggle(DIK_NUMPAD0)) m_isNormalMapOn = !m_isNormalMapOn;
		if (Input->IsKeyToggle(DIK_NUMPAD1)) m_showDebug = !m_showDebug;
	}
}

bool GraphicsClass::Render(wchar_t* debugText)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	XMMATRIX baseViewMatrix;
	bool result;

	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라 렌더 (뷰 행렬 생성)
	m_Camera->Render();

	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);
	m_D3D->GetOrthoMatrix(orthoMatrix);
	baseViewMatrix = XMMatrixIdentity();

	switch (m_SceneState)
	{
	case SceneState::TITLE:
		m_D3D->TurnZBufferOff();
		m_TitleScreen->Render(m_D3D->GetDeviceContext(), 0, 0);
		m_TextureShader->Render(m_D3D->GetDeviceContext(), m_TitleScreen->GetIndexCount(), worldMatrix, baseViewMatrix, orthoMatrix, m_TitleScreen->GetTexture());
		m_D3D->TurnZBufferOn();
		break;

	case SceneState::Tutorial:
		m_D3D->TurnZBufferOff();
		m_TutorialScreen->Render(m_D3D->GetDeviceContext(), 0, 0);
		m_TextureShader->Render(m_D3D->GetDeviceContext(), m_TutorialScreen->GetIndexCount(), worldMatrix, baseViewMatrix, orthoMatrix, m_TutorialScreen->GetTexture());
		m_D3D->TurnZBufferOn();
		break;

	case SceneState::MainScene:
		// [A] 스카이박스
		worldMatrix = XMMatrixTranslationFromVector(m_Camera->GetPositionXM());
		m_D3D->TurnOnNoCulling();
		m_D3D->TurnOnDepthLessEqual();
		m_Skybox->Render(m_D3D->GetDeviceContext());
		m_SkyboxShader->Render(m_D3D->GetDeviceContext(), m_Skybox->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Skybox->GetTexture());
		m_D3D->TurnOnBackCulling();
		m_D3D->TurnOffDefaultDepth();

		// [B] 조명 정보
		XMFLOAT4 pointLightPositions[NUM_POINT_LIGHTS];
		XMFLOAT4 pointLightColors[NUM_POINT_LIGHTS];
		for (int i = 0; i < NUM_POINT_LIGHTS; ++i) {
			pointLightPositions[i] = m_Light->GetPointLightPosition(i);
			pointLightColors[i] = m_Light->GetPointLightColor(i);
		}

		// [C] 플레이어 렌더링
		if (m_Player.model)
		{
			worldMatrix = XMMatrixIdentity();
			// 1. 크기(Scale) 적용
			worldMatrix *= XMMatrixScaling(m_Player.scale.x, m_Player.scale.y, m_Player.scale.z);

			// 2. 회전(Rotation) 적용
			worldMatrix *= XMMatrixRotationRollPitchYaw(m_Player.rotation.x, m_Player.rotation.y, m_Player.rotation.z);

			// [핵심 수정] 3. 이동(Translation) 적용 + 높이 보정 (Visual Offset)
			// 플레이어 모델이 2.0f만큼 떠 있으므로, 렌더링할 때만 2.0f만큼 내려줍니다.
			float visualOffsetY = 2.0f; // 모델을 아래로 내릴 수치 (상황에 맞춰 조절하세요)

			worldMatrix *= XMMatrixTranslation(
				m_Player.position.x,
				m_Player.position.y - visualOffsetY, // <--- Y축에서 값을 뺍니다.
				m_Player.position.z
			);

			m_Player.model->Render(m_D3D->GetDeviceContext());
			m_LightShader->Render(
				m_D3D->GetDeviceContext(), m_Player.model->GetIndexCount(),
				worldMatrix, viewMatrix, projectionMatrix, m_Player.model->GetTexture(),
				m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(),
				m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower(),
				m_isAmbientOn, m_isDiffuseOn, m_isSpecularOn,
				pointLightPositions, pointLightColors, m_pointLightIntensity
			);
		}

		// [D] 게임 오브젝트들
		for (const auto& obj : m_GameObjects)
		{
			worldMatrix = XMMatrixIdentity();
			worldMatrix *= XMMatrixScaling(obj.scale.x, obj.scale.y, obj.scale.z);
			worldMatrix *= XMMatrixRotationRollPitchYaw(obj.rotation.x, obj.rotation.y, obj.rotation.z);
			worldMatrix *= XMMatrixTranslation(obj.position.x, obj.position.y, obj.position.z);

			obj.model->Render(m_D3D->GetDeviceContext());
			m_LightShader->Render(
				m_D3D->GetDeviceContext(), obj.model->GetIndexCount(),
				worldMatrix, viewMatrix, projectionMatrix, obj.model->GetTexture(),
				m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(),
				m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower(),
				m_isAmbientOn, m_isDiffuseOn, m_isSpecularOn,
				pointLightPositions, pointLightColors, m_pointLightIntensity
			);
		}

		// [E] 바닥 (Normal Mapping)
		for (const auto& tile : m_GroundTiles)
		{
			worldMatrix = XMMatrixIdentity();
			worldMatrix *= XMMatrixScaling(tile.scale.x, tile.scale.y, tile.scale.z);
			worldMatrix *= XMMatrixRotationRollPitchYaw(tile.rotation.x, tile.rotation.y, tile.rotation.z);
			worldMatrix *= XMMatrixTranslation(tile.position.x, tile.position.y, tile.position.z);

			tile.model->Render(m_D3D->GetDeviceContext());
			m_BumpMapShader->Render(m_D3D->GetDeviceContext(), tile.model->GetIndexCount(),
				worldMatrix, viewMatrix, projectionMatrix, m_GroundTextures->GetTextureArray(),
				m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(),
				m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower(),
				m_isAmbientOn, m_isDiffuseOn, m_isSpecularOn, m_isNormalMapOn,
				pointLightPositions, pointLightColors, m_pointLightIntensity
			);
		}

		// [F] 디버그 박스
		if (m_showDebug)
		{
			float pRadius = 0.5f;
			float pHeight = 2.0f;
			float footY = m_Player.position.y - 1.8f;

			XMFLOAT3 pMin = { m_Player.position.x - pRadius, footY, m_Player.position.z - pRadius };
			XMFLOAT3 pMax = { m_Player.position.x + pRadius, footY + pHeight, m_Player.position.z + pRadius };
			RenderDebugAABB(pMin, pMax);

			for (const auto& obj : m_GameObjects)
			{
				if (!obj.model || obj.model == m_ModelGround) continue;
				XMFLOAT3 wMin, wMax;
				GetWorldAABB(obj, wMin, wMax);
				RenderDebugAABB(wMin, wMax);
			}
		}

		// -------------------------------------------------------------
		// [G] UI 및 텍스트 렌더링 (명세서 요구사항 반영)
		// -------------------------------------------------------------
		m_D3D->TurnZBufferOff();      // 2D 렌더링을 위해 깊이 버퍼 끄기
		m_D3D->EnableAlphaBlending(); // 텍스트 배경 투명 처리를 위해 알파 블렌딩 켜기

		wchar_t infoText[256]; // 넉넉한 버퍼 사이즈

		// 1. 데이터 집계 (실시간 계산)
		int totalObjects = (int)(m_GameObjects.size() + m_GroundTiles.size() + 1); // 장애물 + 바닥 + 플레이어

		int totalPolygons = 0;
		if (m_Player.model) totalPolygons += m_Player.model->GetIndexCount() / 3;
		if (m_Skybox) totalPolygons += m_Skybox->GetIndexCount() / 3;
		for (const auto& obj : m_GameObjects) { if (obj.model) totalPolygons += obj.model->GetIndexCount() / 3; }
		for (const auto& tile : m_GroundTiles) { if (tile.model) totalPolygons += tile.model->GetIndexCount() / 3; }
		m_PolygonCount = totalPolygons; // 멤버 변수 갱신

		// -------------------------------------------------------------
		// 2. [Scene Information] 명세서 요구 항목 출력 (여러 줄로 분리)
		// -------------------------------------------------------------
		float textX = 10.0f;
		float textY = 10.0f;
		float lineGap = 20.0f; // 줄 간격 (폰트 크기에 따라 조절하세요)

		// 첫 번째 줄: FPS 및 CPU 사용량
		swprintf_s(infoText, 256, L"[Performance] FPS: %d | CPU: %d%%", m_FPS, (int)m_CPUUsage);
		m_Text->DrawTextLine(infoText, textX, textY);
		textY += lineGap; // 다음 줄로 이동

		// 두 번째 줄: 폴리곤 및 오브젝트 수
		swprintf_s(infoText, 256, L"[Statistics]  Polygons: %d | Objects: %d", m_PolygonCount, totalObjects);
		m_Text->DrawTextLine(infoText, textX, textY);
		textY += lineGap;

		// 세 번째 줄: 화면 해상도
		swprintf_s(infoText, 256, L"[Resolution]  Screen: %.0f x %.0f", m_ScreenWidth, m_ScreenHeight);
		m_Text->DrawTextLine(infoText, textX, textY);

		// (그 다음 플레이어 정보가 겹치지 않도록 Y축 여유를 둡니다)
		textY += lineGap * 1.5f;

		// 3. [플레이어 정보] (위치 조정됨)
		swprintf_s(infoText, 256, L"[Player] Pos: (%.1f, %.1f, %.1f) | Yaw: %.1f",
			m_Player.position.x, m_Player.position.y, m_Player.position.z,
			XMConvertToDegrees(m_Player.rotation.y));
		m_Text->DrawTextLine(infoText, textX, textY);

		// 4. [상태 메시지] Frame 함수에서 넘어온 디버그 메시지
		if (debugText)
		{
			m_Text->DrawTextLine(L"=== DEBUG STATUS ===", 10.0f, 100.0f);
			m_Text->DrawTextLine(debugText, 10.0f, 115.0f);
		}

		m_D3D->DisableAlphaBlending();
		m_D3D->TurnZBufferOn();
		break;
	}

	m_D3D->EndScene();
	return true;
}

bool GraphicsClass::CheckAABBCollision(XMFLOAT3 min1, XMFLOAT3 max1, XMFLOAT3 min2, XMFLOAT3 max2)
{
	if (max1.x < min2.x || min1.x > max2.x) return false;
	if (max1.z < min2.z || min1.z > max2.z) return false;
	if (max1.y < min2.y || min1.y > max2.y) return false;
	return true;
}

void GraphicsClass::RenderDebugAABB(XMFLOAT3 min, XMFLOAT3 max)
{
	if (!m_DebugModel) return;

	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	float sizeX = max.x - min.x;
	float sizeY = max.y - min.y;
	float sizeZ = max.z - min.z;
	if (sizeX < 0.001f) sizeX = 0.001f;
	if (sizeY < 0.001f) sizeY = 0.001f;
	if (sizeZ < 0.001f) sizeZ = 0.001f;

	float centerX = (min.x + max.x) * 0.5f;
	float centerY = (min.y + max.y) * 0.5f;
	float centerZ = (min.z + max.z) * 0.5f;

	worldMatrix = XMMatrixScaling(sizeX, sizeY, sizeZ) * XMMatrixTranslation(centerX, centerY, centerZ);

	m_D3D->TurnOnWireframe();
	m_DebugModel->Render(m_D3D->GetDeviceContext());

	if (m_DebugShader)
	{
		m_DebugShader->Render(m_D3D->GetDeviceContext(), m_DebugModel->GetIndexCount(),
			worldMatrix, viewMatrix, projectionMatrix, nullptr);
	}
	else
	{
		m_TextureShader->Render(m_D3D->GetDeviceContext(), m_DebugModel->GetIndexCount(),
			worldMatrix, viewMatrix, projectionMatrix, nullptr);
	}

	m_D3D->TurnOffWireframe();
}

void GraphicsClass::GetWorldAABB(const GameObject& obj, XMFLOAT3& outMin, XMFLOAT3& outMax)
{
	XMFLOAT3 localMin, localMax;
	obj.model->GetMinMax(localMin, localMax);

	XMVECTOR corners[8];
	corners[0] = XMVectorSet(localMin.x, localMin.y, localMin.z, 1.0f);
	corners[1] = XMVectorSet(localMin.x, localMin.y, localMax.z, 1.0f);
	corners[2] = XMVectorSet(localMin.x, localMax.y, localMin.z, 1.0f);
	corners[3] = XMVectorSet(localMin.x, localMax.y, localMax.z, 1.0f);
	corners[4] = XMVectorSet(localMax.x, localMin.y, localMin.z, 1.0f);
	corners[5] = XMVectorSet(localMax.x, localMin.y, localMax.z, 1.0f);
	corners[6] = XMVectorSet(localMax.x, localMax.y, localMin.z, 1.0f);
	corners[7] = XMVectorSet(localMax.x, localMax.y, localMax.z, 1.0f);

	XMMATRIX worldMatrix = XMMatrixIdentity();
	worldMatrix *= XMMatrixScaling(obj.scale.x, obj.scale.y, obj.scale.z);
	worldMatrix *= XMMatrixRotationRollPitchYaw(obj.rotation.x, obj.rotation.y, obj.rotation.z);
	worldMatrix *= XMMatrixTranslation(obj.position.x, obj.position.y, obj.position.z);

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
	if (m_DebugShader) { m_DebugShader->Shutdown(); delete m_DebugShader; m_DebugShader = 0; }
	if (m_DebugModel) { m_DebugModel->Shutdown(); delete m_DebugModel; m_DebugModel = 0; }
	if (m_Text) { m_Text->Shutdown(); delete m_Text; m_Text = nullptr; }
	if (m_TitleScreen) { m_TitleScreen->Shutdown(); delete m_TitleScreen; m_TitleScreen = 0; }
	if (m_TutorialScreen) { m_TutorialScreen->Shutdown(); delete m_TutorialScreen; m_TutorialScreen = 0; }
	if (m_BumpMapShader) { m_BumpMapShader->Shutdown(); delete m_BumpMapShader; m_BumpMapShader = 0; }
	if (m_GroundTextures) { m_GroundTextures->Shutdown(); delete m_GroundTextures; m_GroundTextures = 0; }
	if (m_Skybox) { m_Skybox->Shutdown(); delete m_Skybox; m_Skybox = 0; }
	if (m_SkyboxShader) { m_SkyboxShader->Shutdown(); delete m_SkyboxShader; m_SkyboxShader = 0; }
	if (m_TextureShader) { m_TextureShader->Shutdown(); delete m_TextureShader; m_TextureShader = 0; }
	if (m_Light) { delete m_Light; m_Light = 0; }
	if (m_LightShader) { m_LightShader->Shutdown(); delete m_LightShader; m_LightShader = 0; }
	if (m_Camera) { delete m_Camera; m_Camera = 0; }
	if (m_D3D) { m_D3D->Shutdown(); delete m_D3D; m_D3D = 0; }

	if (m_ModelWall) { m_ModelWall->Shutdown(); delete m_ModelWall; m_ModelWall = 0; }
	if (m_ModelGround) { m_ModelGround->Shutdown(); delete m_ModelGround; m_ModelGround = 0; }
	if (m_ModelCrate) { m_ModelCrate->Shutdown(); delete m_ModelCrate; m_ModelCrate = 0; }
	if (m_ModelBarricade) { m_ModelBarricade->Shutdown(); delete m_ModelBarricade; m_ModelBarricade = 0; }
	if (m_ModelObstacle) { m_ModelObstacle->Shutdown(); delete m_ModelObstacle; m_ModelObstacle = 0; }
	if (m_ModelCar) { m_ModelCar->Shutdown(); delete m_ModelCar; m_ModelCar = 0; }
	if (m_ModelTent) { m_ModelTent->Shutdown(); delete m_ModelTent; m_ModelTent = 0; }
	if (m_ModelTree) { m_ModelTree->Shutdown(); delete m_ModelTree; m_ModelTree = 0; }

	return;
}