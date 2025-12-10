#include "SkinModelClass.h"

SkinModelClass::SkinModelClass()
{
	m_Texture = nullptr;
	_playAniIdx = -1;
}

SkinModelClass::~SkinModelClass()
{
	Release();
}

void SkinModelClass::Release()
{
	for (auto item : _meshList)
	{
		delete item;
	}

	for (auto item : _nodeList)
	{
		delete item;
	}

	_nodeList.clear();
	_meshList.clear();

	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = nullptr;
	}

	BaseModel::Release();
}


void SkinModelClass::Render(ID3D11DeviceContext* dc)
{
	//메쉬 정보가 없으면 리턴
	if (_meshList.empty()) return;

	//렌더링을 위한 버퍼셋팅
	SetRenderBuffers(dc);
		
	//ModelTM Update
	UpdateModelMatrix();

	//Node Update
	UpdateNodeTM();

	// Render all meshes
	for (auto mesh : _meshList)
	{
		// Note: Actual rendering happens in the graphics class with skin shader
		// This just prepares the buffers
	}
}


void SkinModelClass::PlayAni(int idx)
{
	if (idx < 0 || idx >= (int)_aniList.size())
		return;

	if (_playAniIdx != idx) {
		_aniList[idx].Stop();
		_playAniIdx = idx;
	}

	_aniList[idx].Play();
}


void SkinModelClass::UpdateMeshByMaterial()
{
	_meshByMaterial.clear();
	
	// 머티리얼이 없는 경우 기본 머티리얼 생성
	if (_materialList.empty())
	{
		Material defaultMaterial;
		_materialList.push_back(defaultMaterial);
	}
	
	_meshByMaterial.resize(_materialList.size());

	for (auto& mesh : _meshList) {
		// 범위 체크 추가
		if (mesh->matIdx < 0 || mesh->matIdx >= (int)_materialList.size())
		{
			// 범위를 벗어나면 0번 머티리얼로 설정
			mesh->matIdx = 0;
		}
		_meshByMaterial[mesh->matIdx].push_back(mesh);
	}
}


void SkinModelClass::UpdateNodeTM()
{
	if (_aniList.empty())
		return;

	if(_playAniIdx >= 0)
	{
		// Update animation with elapsed time (placeholder - will need time manager)
		_aniList[_playAniIdx].UpdateAnimation(0.016f);  // Assuming ~60fps for now
	}

	for (auto& nodeItem : _nodeList) {
		XMMATRIX tm = nodeItem->localTM;
		
		if (_playAniIdx >= 0)
			_aniList[_playAniIdx].GetAniTM(nodeItem->name, tm);

		nodeItem->worldTM = tm;
		if (nodeItem->parent)
			nodeItem->worldTM = tm * nodeItem->parent->worldTM;
	}
}

bool SkinModelClass::LoadTexture(ID3D11Device* device, const WCHAR* filename)
{
	bool result;
	// Create the texture object.
	m_Texture = new TextureClass;
	if (!m_Texture)
	{
		return false;
	}
	// Initialize the texture object.
	result = m_Texture->Initialize(device, filename);
	if (!result)
	{
		return false;
	}
	return true;
}

ID3D11ShaderResourceView* SkinModelClass::GetTexture()
{
	if (m_Texture)
		return m_Texture->GetTexture();
	
	return nullptr;
}

vector<XMMATRIX> SkinModelClass::GetBoneMatrices()
{
	vector<XMMATRIX> boneMatrices;
	
	// For each mesh, get its bone matrices
	if (!_meshList.empty() && !_meshList[0]->boneList.empty())
	{
		for (auto& boneInfo : _meshList[0]->boneList)
		{
			XMMATRIX boneTM = boneInfo.matOffset * boneInfo.linkNode->worldTM;
			boneMatrices.push_back(boneTM);
		}
	}

	// If no bones, return identity
	if (boneMatrices.empty())
	{
		boneMatrices.push_back(XMMatrixIdentity());
	}

	return boneMatrices;
}
