#include "pch.h"
#include "SkinModel.h"

SkinModel::SkinModel()
{\
	_texShader	= RM_SHADER.AddResource(L"./data/textureShader");
	_skinShader = RM_SHADER.AddResource(L"./data/skinShader");
	SetShader(_skinShader);
}

SkinModel::~SkinModel()
{
}

void SkinModel::Release()
{
	for (auto item : _meshList)
		SAFE_DELETE(item);

	for (auto item : _nodeList)
		SAFE_DELETE(item);

	vector<NodeInfo*>().swap(_nodeList);
	vector<HierarchyMesh*>().swap(_meshList);

	BaseModel::Release();
}


void SkinModel::Render(ID3D11DeviceContext * dc)
{
	//�޽� ������ ������ ����
	if (_meshList.empty()) 	return;

	//�������� ���� ���ۼ���
	SetRenderBuffers(dc);
		
	//ModelTM Update
	UpdateModelMatrix();

	//Node Update
	UpdateNodeTM();

	for (UINT i = 0; i < _meshByMaterial.size(); i++) {
		if (_meshByMaterial[i].empty())
			continue;

		//���׸��� ����
		auto texData = _materialList[i].diffuseMap;

		if (texData)
			dc->PSSetShaderResources(0, 1, &texData->data);

		//Mesh Render
		for (auto mesh : _meshByMaterial[i]) {
			
			//�� ���� ����
			if (!mesh->boneList.empty()) {
				SetShader(_skinShader, false);
				vector<XMMATRIX> calcBoneList;
				for (auto boneInfo : mesh->boneList)
					calcBoneList.emplace_back(XMMatrixTranspose(boneInfo.matOffset * boneInfo.linkNode->worldTM));

				RM_SHADER.SetShaderParameters(dc, calcBoneList);
			}
			//�� ������ ���� ���
			else {
				SetShader(_texShader, false);

				if (mesh->linkNode)
					_tmModel = mesh->linkNode->worldTM * _tmModel;
			}
			RM_SHADER.SetShaderParameters(dc, _tmModel);

			//����
			GetShader()->IndexRender(dc, mesh->count, mesh->start);
		}
	}

	//Unbind Texture
	ID3D11ShaderResourceView* nullViews[] = { nullptr };
	dc->PSSetShaderResources(0, 1, nullViews);
}


void SkinModel::PlayAni(int idx)
{
	if (_playAniIdx != idx) {
		_aniList[idx].Stop();
		_playAniIdx = idx;
	}

	_aniList[idx].Play();
}


void SkinModel::UpdateMeshByMaterial()
{

	_meshByMaterial.clear();
	_meshByMaterial.resize(_materialList.size());


	for (auto& mesh : _meshList) {
		_meshByMaterial[mesh->matIdx].push_back(mesh);
	}
	
}


void SkinModel::UpdateNodeTM()
{
	if (_aniList.empty())
		return;


	if(_playAniIdx >= 0)
		_aniList[_playAniIdx].UpdateAnimation(TIMEMANAGER.GetElapedTime());

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

ID3D11ShaderResourceView* SkinModel::GetTexture()
{
	return m_Texture->GetTexture();
}
