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
	m_Camera->SetPosition(0.0f, 2.0f, -10.0f);	// for cube model
	
	// �� 1
	m_Model1 = new ModelClass;
	if (!m_Model1) { return false; }
	result = m_Model1->Initialize(m_D3D->GetDevice(),
		"./data/SM_Prop_Benchpress_01.fbx",         // char* �� ���
		L"./data/PolygonBattleRoyale_Texture_01_A.dds"); // WCHAR* �ؽ�ó ���
	if (!result) { return false; }

	// �� 2
	m_Model2 = new ModelClass;
	if (!m_Model2) { return false; }
	result = m_Model2->Initialize(m_D3D->GetDevice(),
		"./data/SM_Prop_EmergencyDrop_Crate_01.fbx",
		L"./data/PolygonBattleRoyale_Texture_01_A.dds");
	if (!result) { return false; }

	// �� 3
	m_Model3 = new ModelClass;
	if (!m_Model3) { return false; }
	result = m_Model3->Initialize(m_D3D->GetDevice(),
		"./data/SM_Prop_Propane_01.fbx",
		L"./data/PolygonBattleRoyale_Texture_01_A.dds");
	if (!result) { return false; }

	// �ٴ� ��
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
	m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);     //
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);     //
	m_Light->SetDirection(0.0f, -0.5f, 1.0f);             //
	m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);    //
	m_Light->SetSpecularPower(32.0f);                    //
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

	// (�� ������ ���ݾ� ȸ������ ������ŵ�ϴ�)
	m_rotation += (float)XM_PI * 0.005f; // �ӵ��� �� ������ ����
	if (m_rotation > (2.0f * (float)XM_PI))
	{
		m_rotation -= (2.0f * (float)XM_PI);
	}

	if (Input->IsKeyPressed(DIK_5)) // 5�� Ű
	{
		m_isAmbientOn = !m_isAmbientOn;
	}
	if (Input->IsKeyPressed(DIK_6)) // 6�� Ű
	{
		m_isDiffuseOn = !m_isDiffuseOn;
	}
	if (Input->IsKeyPressed(DIK_7)) // 7�� Ű
	{
		m_isSpecularOn = !m_isSpecularOn;
	}

	// Render the graphics scene.
	// (���� �ڵ�� Render ȣ���� ��������, ���⼭ ����� �մϴ�)
	result = Render(m_rotation);
	if (!result)
	{
		return false;
	}

	return true;
}


bool GraphicsClass::Render(float rotation)
{
	XMMATRIX viewMatrix, projectionMatrix;
	bool result;

	// Clear the buffers to begin the scene.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the view and projection matrices from the camera and d3d objects.
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	// ���̴��� �Ѱ��� �� ���� ������ ���� ����� ����ϴ�.
	XMMATRIX worldMatrix;

	// -----------------------------------------------------------------
	// 4.1 m_Model1 �׸��� (����)
	worldMatrix = XMMatrixIdentity(); // ���� ��� ����

	// --- (����) 10�� �۰� �����ϸ� (0.1f) ---
	worldMatrix *= XMMatrixScaling(0.015f, 0.015f, 0.015f);

	worldMatrix *= XMMatrixRotationY(rotation); // Y�� ����
	worldMatrix *= XMMatrixTranslation(-3.0f, 0.0f, 0.0f); // ��ġ �̵�

	m_Model1->Render(m_D3D->GetDeviceContext());
	result = m_LightShader->Render(m_D3D->GetDeviceContext(),
		m_Model1->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_Model1->GetTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(),
		m_Light->GetDiffuseColor(), m_Camera->GetPosition(), m_Light->GetSpecularColor(),
		m_Light->GetSpecularPower());
	if (!result) { return false; }

	// -----------------------------------------------------------------
	// 4.2 m_Model2 �׸��� (�߾�, �ٸ� �ӵ�)
	worldMatrix = XMMatrixIdentity();

	// --- (����) 10�� �۰� �����ϸ� (0.1f) ---
	worldMatrix *= XMMatrixScaling(0.02f, 0.02f, 0.02f);

	worldMatrix *= XMMatrixRotationY(rotation); // �ٸ� �ӵ�
	worldMatrix *= XMMatrixTranslation(1.0f, 0.0f, 0.0f);

	m_Model2->Render(m_D3D->GetDeviceContext());
	result = m_LightShader->Render(m_D3D->GetDeviceContext(),
		m_Model2->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_Model2->GetTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(),
		m_Light->GetDiffuseColor(), m_Camera->GetPosition(), m_Light->GetSpecularColor(),
		m_Light->GetSpecularPower());
	if (!result) { return false; }

	// -----------------------------------------------------------------
	// 4.3 m_Model3 �׸��� (������)
	worldMatrix = XMMatrixIdentity();

	// --- (����) 10�� �۰� �����ϸ� (0.1f) ---
	worldMatrix *= XMMatrixScaling(0.02f, 0.02f, 0.02f);

	worldMatrix *= XMMatrixRotationY(rotation);
	worldMatrix *= XMMatrixTranslation(5.0f, 0.0f, 0.0f);

	m_Model3->Render(m_D3D->GetDeviceContext());
	result = m_LightShader->Render(m_D3D->GetDeviceContext(),
		m_Model3->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_Model3->GetTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(),
		m_Light->GetDiffuseColor(), m_Camera->GetPosition(), m_Light->GetSpecularColor(),
		m_Light->GetSpecularPower());
	if (!result) { return false; }

	// -----------------------------------------------------------------
	// 4.4 m_GroundModel �׸��� (�ٴ�) - (���� ����)
	worldMatrix = XMMatrixIdentity();
	worldMatrix *= XMMatrixScaling(0.1f, 0.1f, 0.1f); // �ٴ� ũ�� (����)
	worldMatrix *= XMMatrixTranslation(0.0f, -1.0f, 0.0f); // �ٴ� ��ġ

	m_GroundModel->Render(m_D3D->GetDeviceContext());
	result = m_LightShader->Render(m_D3D->GetDeviceContext(),
		m_GroundModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_GroundModel->GetTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(),
		m_Light->GetDiffuseColor(), m_Camera->GetPosition(), m_Light->GetSpecularColor(),
		m_Light->GetSpecularPower());
	if (!result) { return false; }

	// Present the rendered scene to the screen.
	m_D3D->EndScene();

	return true;
}