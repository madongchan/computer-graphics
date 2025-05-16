////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"


GraphicsClass::GraphicsClass()
{
	m_D3D = 0;
	m_Camera = 0;
	//m_Model = 0;
	m_Models.clear(); // 벡터 초기화
	m_ColorShader = 0;
	brightness = 1.0f; // Default brightness value
	backcolor = ' ';
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
	m_Camera->SetPosition(0.0f, 0.0f, -30.0f);
	
	// Create the color shader object.
	m_ColorShader = new ColorShaderClass;
	if (!m_ColorShader)
	{
		return false;
	}

	// Initialize the color shader object.
	result = m_ColorShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the color shader object.", L"Error", MB_OK);
		return false;
	}

	// --- Create and Initialize Models ---

	// 1. Fan Model
	auto fanModel = std::make_unique<ModelClass>();
	result = fanModel->Initialize(m_D3D->GetDevice(), ShapeType::Fan);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the Fan model object.", L"Error", MB_OK);
		return false; // Initialization failed
	}
	m_Models.push_back(std::move(fanModel)); // Add to vector

	// 2. Pentagon (Star) Model
	auto pentagonModel = std::make_unique<ModelClass>();
	result = pentagonModel->Initialize(m_D3D->GetDevice(), ShapeType::Pantagon);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the Pentagon model object.", L"Error", MB_OK);
		return false; // Initialization failed
	}
	m_Models.push_back(std::move(pentagonModel)); // Add to vector

	// 3. House Model
	auto houseModel = std::make_unique<ModelClass>();
	result = houseModel->Initialize(m_D3D->GetDevice(), ShapeType::House);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the House model object.", L"Error", MB_OK);
		return false; // Initialization failed
	}
	m_Models.push_back(std::move(houseModel)); // Add to vector


	return true; // All initializations successful
}

void GraphicsClass::Shutdown()
{
	// Release the color shader object.
	if(m_ColorShader)
	{
		m_ColorShader->Shutdown();
		delete m_ColorShader;
		m_ColorShader = 0;
	}

	// m_Models에 있는 unique_ptr들이 자동으로 메모리 해제
	m_Models.clear(); // 벡터 비우기

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


bool GraphicsClass::Frame(float deltaTime)
{
	bool result;

	// --- 각 모델의 회전 상태 업데이트 ---
	for (auto& model : m_Models)
	{
		if (model) // unique_ptr 유효성 검사 (필수는 아님)
		{
			model->UpdateRotation(deltaTime); // 각 모델의 UpdateRotation 함수 호출
		}
	}
	// ---------------------------------

	// Render the graphics scene.
	result = Render(deltaTime); // deltaTime을 Render 함수에 넘길 필요는 없을 수 있습니다. Render 함수 시그니처 확인.
	if (!result)
	{
		return false;
	}

	return true;
}

void GraphicsClass::SetBrightness(float brightness)
{
	this->brightness = brightness;
}

void GraphicsClass::SetBackColor(char backColor)
{
	this->backcolor = backColor;
}

void GraphicsClass::SetFillMode(D3D11_FILL_MODE fillmode)
{
	m_D3D->SetFILLMode(fillmode);
}

void GraphicsClass::SetToggleCullMode()
{
	if (m_D3D->GetCullMode() == D3D11_CULL_BACK)
	{
		m_D3D->SetCullMode(D3D11_CULL_NONE);
	}
	else
	{
		m_D3D->SetCullMode(D3D11_CULL_BACK);
	}
}


bool GraphicsClass::Render(float deltaTime /*deltaTime이 필요 없다면 제거*/)
{
	XMMATRIX baseWorldMatrix, viewMatrix, projectionMatrix; // baseWorldMatrix 추가
	bool result;

	switch (backcolor)
	{
	case 'r':
		// Clear the buffers to begin the scene.
		m_D3D->BeginScene(1.0f, 0.0f, 0.0f, 1.0f, brightness);
		break;
	case 'g':
		// Clear the buffers to begin the scene.
		m_D3D->BeginScene(0.0f, 1.0f, 0.0f, 1.0f, brightness);
		break;
	case 'b':
		// Clear the buffers to begin the scene.
		m_D3D->BeginScene(0.0f, 0.0f, 1.0f, 1.0f, brightness);
		break;
	default:
		// Clear the buffers to begin the scene.
		m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f, brightness);
		break;
	}
	

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the view and projection matrices.
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	// Get the base world matrix (보통 단위 행렬 또는 초기 설정된 행렬).
	m_D3D->GetWorldMatrix(baseWorldMatrix);

	// Iterate through all models in the vector and render them
	for (const auto& model : m_Models) {
		if (model) // Check if the unique_ptr is valid
		{
			// --- 각 모델의 최종 World Matrix 계산 ---
			XMMATRIX modelWorldMatrix = model->GetWorldMatrix(baseWorldMatrix);
			// ------------------------------------

			// Put the model's vertex and index buffers on the pipeline.
			model->Render(m_D3D->GetDeviceContext());

			// Render the model using the color shader with its specific world matrix.
			result = m_ColorShader->Render(m_D3D->GetDeviceContext(), model->GetIndexCount(), modelWorldMatrix, viewMatrix, projectionMatrix, this->brightness);
			if (!result) {
				return false; // 렌더링 실패 시 중단
			}
		}
	}
	// Present the rendered scene to the screen.
	m_D3D->EndScene();

	return true;
}