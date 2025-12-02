////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"
#include <dinput.h> // DIK_ 키 코드를 사용하기 위해 include

GraphicsClass::GraphicsClass()
{
	m_D3D = 0;
	m_Camera = 0;
	m_Model1 = 0;
	m_Model2 = 0;
	m_Model3 = 0;
	m_GroundModel = 0;
	m_LightShader = 0;
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
	m_Camera->SetPosition(0.0f, 0.0f, -7.0f); // 바닥(-1.0f)보다 높은 곳에서 시작
	m_Camera->SetGroundLevel(0.0f); // 바닥 모델의 Y위치와 일치시킴
	
	// 모델 1
	m_Model1 = new ModelClass;
	if (!m_Model1) { return false; }
	result = m_Model1->Initialize(m_D3D->GetDevice(),
		"./data/SM_Prop_Benchpress_01.fbx",         // char* 모델 경로
		L"./data/PolygonBattleRoyale_Texture_01_A.dds"); // WCHAR* 텍스처 경로
	if (!result) { return false; }

	// 모델 2
	m_Model2 = new ModelClass;
	if (!m_Model2) { return false; }
	result = m_Model2->Initialize(m_D3D->GetDevice(),
		"./data/SM_Prop_EmergencyDrop_Crate_01.fbx",
		L"./data/PolygonBattleRoyale_Texture_01_A.dds");
	if (!result) { return false; }

	// 모델 3
	m_Model3 = new ModelClass;
	if (!m_Model3) { return false; }
	result = m_Model3->Initialize(m_D3D->GetDevice(),
		"./data/SM_Prop_Propane_01.fbx",
		L"./data/PolygonBattleRoyale_Texture_01_A.dds");
	if (!result) { return false; }

	// 바닥 모델
	m_GroundModel = new ModelClass;
	if (!m_GroundModel) { return false; }
	result = m_GroundModel->Initialize(m_D3D->GetDevice(),
		"./data/Floor.fbx",
		L"./data/PolygonBattleRoyale_Texture_01_A.dds");// (이 텍스처는 무시됨)
	if (!result) { return false; }
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
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

	//  지면 텍스처 배열 초기화
	m_GroundTextures = new TextureArrayClass;
	if (!m_GroundTextures) { return false; }
	result = m_GroundTextures->Initialize(m_D3D->GetDevice(),
		L"./data/Floor_color.dds",  // (지면 색상 텍스처)
		L"./data/Floor_normal.dds"); // (지면 노멀맵 텍스처)
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the ground texture array object.", L"Error", MB_OK);
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
	m_pointLightIntensity = 1.0f;

	// 포인트 라이트 빨간색, 왼쪽 위
	m_Light->SetPointLightColor(0, 1.0f, 0.0f, 0.0f, 1.0f); // R, G, B, A
	m_Light->SetPointLightPosition(0, -3.0f, 0.0f, 0.0f);   // X, Y, Z

	// 포인트 라이트 녹색, 중앙 위
	m_Light->SetPointLightColor(1, 0.0f, 1.0f, 0.0f, 1.0f);
	m_Light->SetPointLightPosition(1, 0.0f, 0.0f, 0.0f);

	// 포인트 라이트 파란색, 오른쪽 위
	m_Light->SetPointLightColor(2, 0.0f, 0.0f, 1.0f, 1.0f);
	m_Light->SetPointLightPosition(2, 3.0f, 0.0f, 0.0f);

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

	// Release the model object.
	if(m_Model1)
	{
		m_Model1->Shutdown();
		delete m_Model1;
		m_Model1 = 0;
	}
	if(m_Model2)
	{
		m_Model2->Shutdown();
		delete m_Model2;
		m_Model2 = 0;
	}
	if(m_Model3)
	{
		m_Model3->Shutdown();
		delete m_Model3;
		m_Model3 = 0;
	}
	if(m_GroundModel)
	{
		m_GroundModel->Shutdown();
		delete m_GroundModel;
		m_GroundModel = 0;
	}

	// Release the camera object.
	if(m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	// Release the D3D object.
	if(m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}

	return;
}


bool GraphicsClass::Frame(InputClass* Input, double deltaTime)
{
	bool result;

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

	m_rotation += (float)XM_PI * 0.005f; // 속도는 이 값으로 조절
	if (m_rotation > (2.0f * (float)XM_PI))
	{
		m_rotation -= (2.0f * (float)XM_PI);
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

	// Render the graphics scene.
	// (기존 코드는 Render 호출이 없었지만, 여기서 해줘야 합니다)
	result = Render(m_rotation);
	if (!result)
	{
		return false;
	}

	return true;
}

// 카메라 입력을 처리하는 헬퍼 함수
void GraphicsClass::HandleInput(InputClass* Input, double deltaTime)
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
}

bool GraphicsClass::Render(float rotation)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;

	// 1. 버퍼 클리어
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// 2. 뷰 행렬 생성
	m_Camera->Render();

	// 3. 뷰/프로젝션 행렬 가져오기
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);
	m_D3D->GetOrthoMatrix(projectionMatrix);

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
	// 4. 렌더링 루프 (모델 4개)
	// 4.1 m_Model1 그리기 (왼쪽)
	worldMatrix = XMMatrixIdentity(); // 리셋
	worldMatrix *= XMMatrixScaling(0.01f, 0.01f, 0.01f); // 크기 10%
	worldMatrix *= XMMatrixRotationY(rotation); // Y축 자전
	worldMatrix *= XMMatrixTranslation(-3.0f, 0.0f, 0.0f); // 위치 이동

	m_Model1->Render(m_D3D->GetDeviceContext());
	result = m_LightShader->Render(m_D3D->GetDeviceContext(),
		m_Model1->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_Model1->GetTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(),
		m_Light->GetDiffuseColor(), m_Camera->GetPosition(), m_Light->GetSpecularColor(),
		m_Light->GetSpecularPower(),
		m_isAmbientOn, m_isDiffuseOn, m_isSpecularOn,
		pointLightPositions, pointLightColors, m_pointLightIntensity); // 토글 값 전달
	if (!result) { return false; }

	// 4.2 m_Model2 그리기 (중앙)
	worldMatrix = XMMatrixIdentity();
	worldMatrix *= XMMatrixScaling(0.01f, 0.01f, 0.01f);
	worldMatrix *= XMMatrixRotationY(rotation);
	worldMatrix *= XMMatrixTranslation(0.0f, 0.0f, 0.0f);

	m_Model2->Render(m_D3D->GetDeviceContext());
	result = m_LightShader->Render(m_D3D->GetDeviceContext(),
		m_Model2->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_Model2->GetTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(),
		m_Light->GetDiffuseColor(), m_Camera->GetPosition(), m_Light->GetSpecularColor(),
		m_Light->GetSpecularPower(),
		m_isAmbientOn, m_isDiffuseOn, m_isSpecularOn,
		pointLightPositions, pointLightColors, m_pointLightIntensity);
	if (!result) { return false; }

	// 4.3 m_Model3 그리기 (오른쪽)
	worldMatrix = XMMatrixIdentity();
	worldMatrix *= XMMatrixScaling(0.01f, 0.01f, 0.01f);
	worldMatrix *= XMMatrixRotationY(rotation);
	worldMatrix *= XMMatrixTranslation(3.0f, 0.0f, 0.0f);

	m_Model3->Render(m_D3D->GetDeviceContext());
	result = m_LightShader->Render(m_D3D->GetDeviceContext(),
		m_Model3->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_Model3->GetTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(),
		m_Light->GetDiffuseColor(), m_Camera->GetPosition(), m_Light->GetSpecularColor(),
		m_Light->GetSpecularPower(),
		m_isAmbientOn, m_isDiffuseOn, m_isSpecularOn,
		pointLightPositions, pointLightColors, m_pointLightIntensity);
	if (!result) { return false; }

	// 4.4 m_GroundModel 그리기 (바닥)
	worldMatrix = XMMatrixIdentity();
	worldMatrix *= XMMatrixScaling(0.05f, 0.001f, 0.05f); // 바닥 크기
	worldMatrix *= XMMatrixTranslation(0.0f, -2.0f, 0.0f); // 바닥 위치

	m_GroundModel->Render(m_D3D->GetDeviceContext());
	// (LightShader 대신 BumpMapShader 호출)
	result = m_BumpMapShader->Render(m_D3D->GetDeviceContext(),
		m_GroundModel->GetIndexCount(),
		worldMatrix, viewMatrix, projectionMatrix,
		m_GroundTextures->GetTextureArray(),    // 텍스처 (Color + Normal)
		m_Light->GetDirection(),                // 조명 방향
		m_Light->GetAmbientColor(),             // Ambient Color
		m_Light->GetDiffuseColor(),             // Diffuse Color
		m_Camera->GetPosition(),                // Camera Pos (Specular용)
		m_Light->GetSpecularColor(),            // Specular Color
		m_Light->GetSpecularPower(),            // Specular Power

		m_isAmbientOn,                          // 5번키 상태
		m_isDiffuseOn,                          // 6번키 상태
		m_isSpecularOn,                         // 7번키 상태
		m_isNormalMapOn,                        // 0번키 상태 (BumpMap ON/OFF)

		pointLightPositions,                    // 포인트 라이트 위치 배열
		pointLightColors,                       // 포인트 라이트 색상 배열
		m_pointLightIntensity                   // 포인트 라이트 강도
	);
	if (!result) { return false; }

	// 5. 씬 종료
	m_D3D->EndScene();

	return true;
}