////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"
#include "inputclass.h"


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

	// Initialize the scene state
	m_FPS = 0;
	m_CPUUsage = 0.0f;
	m_PolygonCount = 0;
	m_ScreenWidth = 0;  // 기본 화면 너비
	m_ScreenHeight = 0;  // 기본 화면 높이
	m_SceneState = SceneState::MainScene;
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

	m_ScreenWidth = screenWidth;  // 화면 너비 설정
	m_ScreenHeight = screenHeight;  // 화면 높이 설정
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
	m_Camera->SetPosition(0.0f, 0.0f, -5.0f);	// for cube
//	m_Camera->SetPosition(0.0f, 0.5f, -3.0f);	// for chair
		
	// Create the model object.
	//m_Model = new ModelClass;
	//if(!m_Model)
	//{
	//	return false;
	//}

	//// Initialize the model object.
	//result = m_Model->Initialize(m_D3D->GetDevice(), L"./data/cube.obj", L"./data/seafloor.dds");
	//if(!result)
	//{
	//	MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
	//	return false;
	//}

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

	// 메인 씬의 배경 화면
	m_BackGround = new BitmapClass;
	if (!m_BackGround) return false;
	result = m_BackGround->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight, L"./data/bluesky.dds", screenWidth, screenHeight);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the background bitmap object.", L"Error", MB_OK);
		return false;
	}

	// 타이틀 씬 화면
	m_TitleScreen = new BitmapClass;
	if (!m_TitleScreen)
	{
		return false;
	}
	result = m_TitleScreen->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight, L"./data/Title.dds", screenWidth, screenHeight);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the bitmap object.", L"Error", MB_OK);
		return false;
	}

	// 튜토리얼 씬 화면
	m_TutorialScreen = new BitmapClass;
	if (!m_TutorialScreen)
	{
		return false;
	}
	result = m_TutorialScreen->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight, L"./data/Tutorial.dds", screenWidth, screenHeight);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the bitmap object.", L"Error", MB_OK);
		return false;
	}

	m_Text = new TextClass;
	if (!m_Text) return false;

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
	// Release the model object.
	if(m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
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

	// Release the texture shader object.
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

	m_FPS = fps;  // FPS 업데이트
	m_CPUUsage = cpuUsage;  // CPU 사용률 업데이트
	static float rotation = 0.0f;

	// 키 상태 업데이트 (매 프레임마다 호출)
	InputClass::UpdateKeyStates();

	if (InputClass::IsAnyKeyPressed())  // 아무 키나 눌리는 순간 감지
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

	// Update the rotation variable each frame.
	rotation += XM_PI * 0.005f;
	if (rotation > 360.0f)
	{
		rotation -= 360.0f;
	}

	// Render the graphics scene.
	result = Render(rotation);
	if(!result)
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

	// 씬 상태에 따라 다른 화면을 렌더링
	switch (m_SceneState)
	{
		// 타이틀 씬
	case SceneState::TITLE:
		m_D3D->TurnZBufferOff();
		result = m_TitleScreen->Render(m_D3D->GetDeviceContext(), 0, 0);
		if (!result) return false;
		result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_TitleScreen->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_TitleScreen->GetTexture());
		if (!result) return false;
		m_D3D->TurnZBufferOn();
		break;
		// 튜토리얼 씬
	case SceneState::Tutorial:
		m_D3D->TurnZBufferOff();
		result = m_TutorialScreen->Render(m_D3D->GetDeviceContext(), 0, 0);
		if (!result) return false;
		result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_TutorialScreen->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_TutorialScreen->GetTexture());
		if (!result) return false;
		m_D3D->TurnZBufferOn();
		break;
		// 게임 씬
	case SceneState::MainScene:
		m_D3D->TurnZBufferOff();
		// Render the background bitmap.
		result = m_BackGround->Render(m_D3D->GetDeviceContext(), 0, 0);
		if (!result) return false;
		// Render the background texture using the texture shader.
		result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_BackGround->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_BackGround->GetTexture());
		if (!result) return false;
		m_D3D->TurnZBufferOn();

		// 텍스트 출력 (FPS, CPU)
		wchar_t fpsText[64], cpuText[64], foundText[64], objectText[64], polyText[64], resolText[64];
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

		// Render the model.
		break;
	}
	

	// Rotate the world matrix by the rotation value so that the triangle will spin.
	worldMatrix = XMMatrixRotationY(rotation);

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	//m_Model->Render(m_D3D->GetDeviceContext());

	// Render the model using the texture shader.
	//result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture());

	
	if(!result)
	{
		return false;
	}

	// Present the rendered scene to the screen.
	m_D3D->EndScene();

	return true;
}