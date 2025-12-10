//==================================================================
//		## ModelLoader ## (Assimp를 이용해 FBX 모델 로드)
//==================================================================
#pragma once
#include "include/assimp/Importer.hpp"
#include "include/assimp/scene.h"
#include "include/assimp/postprocess.h"
#include "SkinModelClass.h"
#include <d3d11.h>
#include <string>
#include <vector>

using namespace std;

class ModelLoader
{
public:
	ModelLoader(ID3D11Device* device);
	~ModelLoader();

	// 모델 로드 (메시 + 본 구조)
	bool LoadModel(const wchar_t* path, SkinModelClass* skinModel);
	
	// 애니메이션 로드 (기존 모델에 추가)
	bool LoadAnimation(const wchar_t* path, SkinModelClass* skinModel);

private:
	ID3D11Device* m_device;

	// 헬퍼 함수들
	void ProcessNode(const aiNode* node, const aiScene* scene, SkinModelClass* skinModel, NodeInfo* parent = nullptr, int depth = 0);
	void ProcessMesh(const aiMesh* mesh, const aiScene* scene, Vertex& vertices, vector<unsigned long>& indices, SkinModelClass* skinModel);
	void ProcessSkin(const aiMesh* aiMesh, HierarchyMesh* mesh, Vertex& vertices, SkinModelClass* skinModel);
	void ProcessAnimation(const aiScene* scene, SkinModelClass* skinModel);
	NodeInfo* FindNode(const aiString& name, vector<NodeInfo*>& nodeList);
	
	// 유틸리티
	wstring ConvertToWString(const char* str);
	XMMATRIX ConvertMatrix(const aiMatrix4x4& mat);
};
