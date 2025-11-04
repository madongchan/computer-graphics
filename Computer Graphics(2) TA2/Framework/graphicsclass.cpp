////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"
#include <dinput.h>

GraphicsClass::GraphicsClass()
{
	m_D3D = 0;
	m_Camera = 0;
	m_Model1 = 0;
	m_Model2 = 0;
	m_Model3 = 0;
	m_GroundModel = 0;
	m_LightShader = 0;
	m_Light = 0;
	m_rotation = 0.0f;
	m_isAmbientOn = true;
	m_isDiffuseOn = true;
	m_isSpecularOn = true;
	m_pointLightIntensity = 1.0f; // 기본 강도 1.0
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

	// Set the initial position of the camera.
	m_Camera->SetPosition(0.0f, 1.0f, -7.0f);	// for cube model
	
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
		"./data/Road.fbx",
		L"./data/PolygonBattleRoyale_Texture_01_A.dds");
	if (!result) { return false; }
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
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

	// --- ★ (수정 2) 포인트 라이트 3개 초기값 설정 ★ ---
	//
	// (위치와 색상은 예시입니다. 동찬님께서 원하시는 값으로 설정하세요.)

	// 포인트 라이트 0 (예: 빨간색, 왼쪽 위)
	m_Light->SetPointLightColor(0, 1.0f, 0.0f, 0.0f, 1.0f); // R, G, B, A
	m_Light->SetPointLightPosition(0, -3.0f, 0.0f, 0.0f);   // X, Y, Z

	// 포인트 라이트 1 (예: 녹색, 중앙 위)
	m_Light->SetPointLightColor(1, 0.0f, 1.0f, 0.0f, 1.0f);
	m_Light->SetPointLightPosition(1, 0.0f, 0.0f, 0.0f);

	// 포인트 라이트 2 (예: 파란색, 오른쪽 위)
	m_Light->SetPointLightColor(2, 0.0f, 0.0f, 1.0f, 1.0f);
	m_Light->SetPointLightPosition(2, 3.0f, 0.0f, 0.0f);
	return true;
}
void GraphicsClass::Shutdown()
{
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


bool GraphicsClass::Frame(InputClass* Input)
{
	bool result;

	// (매 프레임 조금씩 회전값을 누적시킵니다)
	m_rotation += (float)XM_PI * 0.005f; // 속도는 이 값으로 조절
	if (m_rotation > (2.0f * (float)XM_PI))
	{
		m_rotation -= (2.0f * (float)XM_PI);
	}
	if (Input->IsKeyPressed(0x35) || Input->IsKeyPressed(VK_NUMPAD5)) // 5번 키
	{
		m_isAmbientOn = !m_isAmbientOn;
	}
	if (Input->IsKeyPressed(0x36) || Input->IsKeyPressed(VK_NUMPAD6)) // 6번 키
	{
		m_isDiffuseOn = !m_isDiffuseOn;
	}
	if (Input->IsKeyPressed(0x37) || Input->IsKeyPressed(VK_NUMPAD7)) // 7번 키
	{
		m_isSpecularOn = !m_isSpecularOn;
	}
	if (Input->IsKeyDown(0x38) || Input->IsKeyDown(VK_NUMPAD8)) // '8' - Point Light Intensity 증가
	{
		// 키를 계속 누르고 있어도 값이 계속 바뀌도록 IsKeyPressed 대신 IsKeyDown 사용
		m_pointLightIntensity -= 0.05f; // 감소량 (조절 가능)
		if (m_pointLightIntensity < 0.0f) // 최소값 0으로 제한
		{
			m_pointLightIntensity = 0.0f;
		}
	}
	if (Input->IsKeyDown(0x39) || Input->IsKeyDown(VK_NUMPAD9)) // '9' - Point Light Intensity 감소
	{
		m_pointLightIntensity += 0.05f; // 증가량 (조절 가능)
		if (m_pointLightIntensity > 5.0f) // 최대값 5로 제한
		{
			m_pointLightIntensity = 5.0f;
		}
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
	m_D3D->GetProjectionMatrix(projectionMatrix);

	// --- ★ (수정 4) 포인트 라이트 데이터 준비 (배열) ★ ---
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
	worldMatrix *= XMMatrixScaling(0.1f, 0.1f, 0.1f); // 바닥 크기
	worldMatrix *= XMMatrixTranslation(0.0f, -1.0f, 0.0f); // 바닥 위치

	m_GroundModel->Render(m_D3D->GetDeviceContext());
	result = m_LightShader->Render(m_D3D->GetDeviceContext(),
		m_GroundModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_GroundModel->GetTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(),
		m_Light->GetDiffuseColor(), m_Camera->GetPosition(), m_Light->GetSpecularColor(),
		m_Light->GetSpecularPower(),
		m_isAmbientOn, m_isDiffuseOn, m_isSpecularOn,
		pointLightPositions, pointLightColors, m_pointLightIntensity);
	if (!result) { return false; }

	// 5. 씬 종료
	m_D3D->EndScene();

	return true;
}