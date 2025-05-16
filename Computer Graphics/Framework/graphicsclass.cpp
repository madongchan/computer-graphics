////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"


GraphicsClass::GraphicsClass()
{
	m_D3D = 0;
	m_Camera = 0;
	m_Model1 = 0;
	m_Model2 = 0;
	m_Model3 = 0;
	Ground_Model = 0;
	m_TextureShader = 0;
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
	m_Camera->SetPosition(0.0f, 5.0f, -15.0f);	// for cube model
	m_Camera->SetRotation(10.0f, 0.0f, 0.0f);	// for chair model
	
	// Create the model object.
	m_Model1 = new ModelClass;
	if(!m_Model1)
	{
		return false;
	}

	// Initialize the model object.
	result = m_Model1->Initialize(m_D3D->GetDevice(), L"./data/chair.obj", L"./data/chair_d.dds");
//	result = m_Model->Initialize(m_D3D->GetDevice(), L"./data/chair.obj", L"./data/chair_d.dds");
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	// Create the model object.
	m_Model2= new ModelClass;
	if (!m_Model2)
	{
		return false;
	}

	// Initialize the model object.
	result = m_Model2->Initialize(m_D3D->GetDevice(), L"./data/chair.obj", L"./data/chair_d.dds");
	//	result = m_Model->Initialize(m_D3D->GetDevice(), L"./data/chair.obj", L"./data/chair_d.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	// Create the model object.
	m_Model3 = new ModelClass;
	if (!m_Model3)
	{
		return false;
	}

	// Initialize the model object.
	result = m_Model3->Initialize(m_D3D->GetDevice(), L"./data/chair.obj", L"./data/chair_d.dds");
	//	result = m_Model->Initialize(m_D3D->GetDevice(), L"./data/chair.obj", L"./data/chair_d.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	Ground_Model = new ModelClass;
	if (!Ground_Model)
	{
		return false;
	}
	// Initialize the model object.
	result = Ground_Model->Initialize(m_D3D->GetDevice(), L"./data/chair.obj", L"./data/chair_d.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the ground model object.", L"Error", MB_OK);
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

	return true;
}


void GraphicsClass::Shutdown()
{
	// Release the texture shader object.
	if(m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	// Release the model object.
	if(m_Model1)
	{
		m_Model1->Shutdown();
		delete m_Model1;
		m_Model1 = 0;
	}
	if (m_Model2)
	{
		m_Model2->Shutdown();
		delete m_Model2;
		m_Model2 = 0;
	}
	if (m_Model3)
	{
		m_Model3->Shutdown();
		delete m_Model3;
		m_Model3 = 0;
	}
	if (Ground_Model)
	{
		Ground_Model->Shutdown();
		delete Ground_Model;
		Ground_Model = 0;
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


bool GraphicsClass::Frame()
{
	bool result;

	static float rotation = 0.0f;


	// Update the rotation variable each frame.
	rotation += (float)XM_PI * 0.01f;
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
	XMMATRIX worldMatrix1, worldMatrix2, worldMatrix3, groundWorldMatrix, viewMatrix, projectionMatrix;
	bool result;

	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	m_Camera->Render();

	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	worldMatrix1 = XMMatrixRotationY(rotation) * XMMatrixTranslation(0.0f, 0.0f, 0.0f);

	worldMatrix2 = XMMatrixRotationY(rotation) * XMMatrixTranslation(-3.0f, 0.0f, 0.0f);

	worldMatrix3 = XMMatrixRotationY(rotation) * XMMatrixTranslation(3.0f, 0.0f, 0.0f);

	groundWorldMatrix = XMMatrixTranslation(0.0f, -1.0f, 0.0f);

	m_Model1->Render(m_D3D->GetDeviceContext());
	result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_Model1->GetIndexCount(),
		worldMatrix1, viewMatrix, projectionMatrix, m_Model1->GetTexture());
	if (!result)
	{
		return false;
	}

	m_Model2->Render(m_D3D->GetDeviceContext());
	result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_Model2->GetIndexCount(),
		worldMatrix2, viewMatrix, projectionMatrix, m_Model2->GetTexture());
	if (!result)
	{
		return false;
	}

	m_Model3->Render(m_D3D->GetDeviceContext());
	result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_Model3->GetIndexCount(),
		worldMatrix3, viewMatrix, projectionMatrix, m_Model3->GetTexture());
	if (!result)
	{
		return false;
	}

	Ground_Model->Render(m_D3D->GetDeviceContext());
	result = m_TextureShader->Render(m_D3D->GetDeviceContext(), Ground_Model->GetIndexCount(),
		groundWorldMatrix, viewMatrix, projectionMatrix, Ground_Model->GetTexture());
	if (!result)
	{
		return false;
	}

	m_D3D->EndScene();

	return true;
}
