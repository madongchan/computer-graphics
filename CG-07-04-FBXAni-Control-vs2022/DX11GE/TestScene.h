#pragma once
#include "GameNode.h"
#include "DeviceManager.h"
#include "RMModel.h"
#include "ModelLoader.h"
#include "lightclass.h"
#include "lightshaderclass.h"
#include "modelclass.h"
#include "Player.h"

class TestScene : public GameNode
{
public:
	TestScene();
	~TestScene();

private:
	ModelLoader *m_modelLoader;
	SkinModel *m_skinModel;
	ModelClass *m_groundModel;
//	SkinModel* m_groundModel;
	Player *m_player;

	LightShaderClass* m_LightShader;
	LightClass* m_Light;
		
public:
	HRESULT Init(void);
	void Update(void);
	void Render(ID3D11DeviceContext* dc);


};

