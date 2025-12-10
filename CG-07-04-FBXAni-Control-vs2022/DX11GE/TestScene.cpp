#include "pch.h"
#include "TestScene.h"

TestScene::TestScene()
{
	m_modelLoader = NULL;
	m_skinModel = NULL;
	m_groundModel = NULL;
	m_player = NULL;

	m_LightShader = NULL;
	m_Light = NULL;
}

TestScene::~TestScene()
{
	if (m_modelLoader)	delete m_modelLoader;
	if (m_skinModel)	delete m_skinModel;
	if (m_player)		delete m_player;

	if (m_Light)		delete m_Light;
	if (m_LightShader)
	{
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = 0;
	}

	if (m_groundModel)
	{
//		m_groundModel->Shutdown();
		delete m_groundModel;
		m_groundModel = 0;
	}


}

HRESULT TestScene::Init(void)
{
	ID3D11Device* device = DEVICEMANAGER.GetDevice();

	//light
	{
		m_LightShader = new LightShaderClass;
		m_LightShader->Initialize(device, _hWnd);

		m_Light = new LightClass;
		m_Light->SetAmbientColor(0.3f, 0.3f, 0.3f, 1.0f);
		m_Light->SetDiffuseColor(0.5f, 0.5f, 0.5f, 1.0f);
		m_Light->SetDirection(0.0f, -1.0f, 0.0f);
		m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
		m_Light->SetSpecularPower(32.0f);
	}
	
	m_modelLoader = new ModelLoader;

	//model - not animated
	{
		m_groundModel = new ModelClass;
		m_groundModel->Initialize(device, L"./data/test_ground4.obj", L"./data/ground_d.dds");
//		m_groundModel = new SkinModel;
//		m_groundModel = m_modelLoader->LoadModel(L"./data/test_ground4.obj", aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);
//		m_groundModel->LoadTexture(L"./data/ground_d.dds");
//		m_groundModel->SetNormalize(true);
//		m_groundModel->SetSize(XMFLOAT3(0.001f, 0.001f, 0.001f));
	}
	
	//model - animated
	{
		m_skinModel = new SkinModel;

		m_skinModel = m_modelLoader->LoadModel(L"./data/character.fbx", aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);
		m_modelLoader->LoadAnimation(L"./data/Idle_24.fbx", m_skinModel, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);
		m_modelLoader->LoadAnimation(L"./data/Walking30.fbx", m_skinModel, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);
		m_modelLoader->LoadAnimation(L"./data/RunningBackward.fbx", m_skinModel, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);
		m_modelLoader->LoadAnimation(L"./data/LeftWalking.fbx", m_skinModel, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);
		m_modelLoader->LoadAnimation(L"./data/RightWalking.fbx", m_skinModel, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);
		m_modelLoader->LoadAnimation(L"./data/01Attack.fbx", m_skinModel, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);
//		modelLoader->LoadAnimation(L"./data/anim_joyfuljump.fbx", m_skinModel, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);
//		modelLoader->LoadAnimation(L"./data/Running30.fbx", m_skinModel, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);
	
//		m_skinModel->GetAnimation(5).SetRepeat(true);
		m_skinModel->LoadTexture(L"./data/character_d.dds");
		m_skinModel->SetNormalize(true);
		m_skinModel->SetSize(XMFLOAT3(5.f, 5.f, 5.f));

		m_player = new Player;
	//	XMFLOAT3 rotation(0, 60, 0);
	//	m_skinModel->SetRotate(rotation);
	}
	
	_sceneInfo->viewMatrix = _mainCam.Update(TIMEMANAGER.GetElapedTime());

	return S_OK;
}

void TestScene::Update(void)
{
	m_player->Update(TIMEMANAGER.GetElapedTime());
}

void TestScene::Render(ID3D11DeviceContext* dc)
{
	bool result = true;

	result = m_LightShader->Render(dc, m_skinModel->GetIndexCount(), _sceneInfo->worldMatrix, _sceneInfo->viewMatrix,
		_sceneInfo->projectionMatrix, m_skinModel->GetTexture(), m_Light->GetDirection(),
		m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(),
		_mainCam.GetPositionF(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
	if (!result)
		return;

	RM_SHADER.SetShaderParameters(dc, _sceneInfo->viewMatrix, _sceneInfo->projectionMatrix);
	m_skinModel->Render(dc);
	XMFLOAT3 mov(m_skinModel->GetCenter());
	if (INPUTMANAGER.GetVertical() == 1.f) { //╩С
		if (!m_skinModel->GetAnimation(6).isPlaying()) {
			m_skinModel->PlayAni(2);
			
			mov.x += XMVectorGetX(_mainCam.GetDirection()) * 0.3f;
			mov.y += 0;
			mov.z += XMVectorGetZ(_mainCam.GetDirection()) * 0.3f;
			m_skinModel->SetCenter(mov);
		}
			
	}
	else if (INPUTMANAGER.GetVertical() == -1.f) { //го
		if (!m_skinModel->GetAnimation(6).isPlaying()) {
			m_skinModel->PlayAni(3);

			mov.x -= XMVectorGetX(_mainCam.GetDirection()) * 0.3f;
			mov.y += 0;
			mov.z -= XMVectorGetZ(_mainCam.GetDirection()) * 0.3f;
			m_skinModel->SetCenter(mov);
		}
	}
	else if (INPUTMANAGER.GetHorizontal() == -1.f) { //аб
		if (!m_skinModel->GetAnimation(6).isPlaying()) {
			m_skinModel->PlayAni(4);

			mov.x -= XMVectorGetZ(_mainCam.GetDirection()) * 0.2f;
			mov.y += 0;
			mov.z += XMVectorGetX(_mainCam.GetDirection()) * 0.2f;
			m_skinModel->SetCenter(mov);
		}
	}
	else if (INPUTMANAGER.GetHorizontal() == 1.f) { //©Л
		if (!m_skinModel->GetAnimation(6).isPlaying()) {
			m_skinModel->PlayAni(5);

			mov.x += XMVectorGetZ(_mainCam.GetDirection()) * 0.2f;
			mov.y += 0;
			mov.z -= XMVectorGetX(_mainCam.GetDirection()) * 0.2f;
			m_skinModel->SetCenter(mov);
		}
	}
	else if (INPUTMANAGER.GetShift() == 1.0f) {
		m_skinModel->PlayAni(6);
	}
	else {
		if (m_skinModel->GetAnimationList().size() != 0) {
			if (!m_skinModel->GetAnimation(6).isPlaying())
				m_skinModel->PlayAni(1);
		}
	}

// Ground plane
	m_groundModel->Render(dc);

	result = m_LightShader->Render(dc, m_groundModel->GetIndexCount(),
		_sceneInfo->worldMatrix, 
		_sceneInfo->viewMatrix,
		_sceneInfo->projectionMatrix,
		m_groundModel->GetTexture(),
		m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(),
		_mainCam.GetPositionF(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
		
}
