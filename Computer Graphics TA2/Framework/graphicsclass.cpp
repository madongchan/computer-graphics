////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"


GraphicsClass::GraphicsClass()
{
	m_D3D = 0;
	m_Camera = 0;
	//m_Model = 0;
	m_Models.clear(); // ���� �ʱ�ȭ
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

	// m_Models�� �ִ� unique_ptr���� �ڵ����� �޸� ����
	m_Models.clear(); // ���� ����

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

	// --- �� ���� ȸ�� ���� ������Ʈ ---
	for (auto& model : m_Models)
	{
		if (model) // unique_ptr ��ȿ�� �˻� (�ʼ��� �ƴ�)
		{
			model->UpdateRotation(deltaTime); // �� ���� UpdateRotation �Լ� ȣ��
		}
	}
	// ---------------------------------

	// Render the graphics scene.
	result = Render(deltaTime); // deltaTime�� Render �Լ��� �ѱ� �ʿ�� ���� �� �ֽ��ϴ�. Render �Լ� �ñ״�ó Ȯ��.
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


bool GraphicsClass::Render(float deltaTime /*deltaTime�� �ʿ� ���ٸ� ����*/)
{
	XMMATRIX baseWorldMatrix, viewMatrix, projectionMatrix; // baseWorldMatrix �߰�
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

	// Get the base world matrix (���� ���� ��� �Ǵ� �ʱ� ������ ���).
	m_D3D->GetWorldMatrix(baseWorldMatrix);

	// Iterate through all models in the vector and render them
	for (const auto& model : m_Models) {
		if (model) // Check if the unique_ptr is valid
		{
			// --- �� ���� ���� World Matrix ��� ---
			XMMATRIX modelWorldMatrix = model->GetWorldMatrix(baseWorldMatrix);
			// ------------------------------------

			// Put the model's vertex and index buffers on the pipeline.
			model->Render(m_D3D->GetDeviceContext());

			// Render the model using the color shader with its specific world matrix.
			result = m_ColorShader->Render(m_D3D->GetDeviceContext(), model->GetIndexCount(), modelWorldMatrix, viewMatrix, projectionMatrix, this->brightness);
			if (!result) {
				return false; // ������ ���� �� �ߴ�
			}
		}
	}
	// Present the rendered scene to the screen.
	m_D3D->EndScene();

	return true;
}