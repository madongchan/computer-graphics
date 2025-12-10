#include "ModelLoader.h"
#include <locale>
#include <codecvt>

ModelLoader::ModelLoader(ID3D11Device* device)
{
	m_device = device;
}

ModelLoader::~ModelLoader()
{
}

bool ModelLoader::LoadModel(const wchar_t* path, SkinModelClass* skinModel)
{
	// wchar_t -> string 변환
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	std::string pathStr = converter.to_bytes(path);

	// Assimp Importer
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(pathStr,
		aiProcess_Triangulate |
		aiProcess_ConvertToLeftHanded |
		aiProcess_JoinIdenticalVertices |
		aiProcess_GenNormals);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		return false;
	}

	// 정점 및 인덱스 데이터
	Vertex vertices;
	vector<unsigned long> indices;

	// 노드 계층 구조 처리
	ProcessNode(scene->mRootNode, scene, skinModel, nullptr, 0);

	// 깊이 순으로 정렬
	sort(skinModel->GetNodeList().begin(), skinModel->GetNodeList().end(),
		[](const NodeInfo* a, const NodeInfo* b) { return a->depth < b->depth; });

	// 모든 메시 처리
	for (unsigned int i = 0; i < scene->mNumMeshes; i++)
	{
		ProcessMesh(scene->mMeshes[i], scene, vertices, indices, skinModel);
	}

	// 스키닝 정보 처리
	for (unsigned int i = 0; i < scene->mNumMeshes && i < skinModel->GetMeshList().size(); i++)
	{
		aiMesh* aiMesh = scene->mMeshes[i];
		if (aiMesh->HasBones())
		{
			HierarchyMesh* mesh = skinModel->GetMeshList()[i];
			ProcessSkin(aiMesh, mesh, vertices, skinModel);
		}
	}

	// 애니메이션 정보 처리
	if (scene->HasAnimations())
	{
		ProcessAnimation(scene, skinModel);
	}

	// 버퍼 생성
	if (!skinModel->CreateModel(m_device, vertices, indices))
	{
		return false;
	}

	// 마테리얼 단위로 메쉬 정보 업데이트
	skinModel->UpdateMeshByMaterial();

	return true;
}

bool ModelLoader::LoadAnimation(const wchar_t* path, SkinModelClass* skinModel)
{
	// wchar_t -> string 변환
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	std::string pathStr = converter.to_bytes(path);

	// Assimp Importer
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(pathStr,
		aiProcess_Triangulate |
		aiProcess_ConvertToLeftHanded);

	if (!scene || !scene->HasAnimations())
	{
		return false;
	}

	// 애니메이션 정보 처리
	ProcessAnimation(scene, skinModel);

	return true;
}

void ModelLoader::ProcessNode(const aiNode* node, const aiScene* scene, SkinModelClass* skinModel, NodeInfo* parent, int depth)
{
	if (!node) return;

	// 노드 변환 행렬
	XMMATRIX nodeTM = ConvertMatrix(node->mTransformation);
	wstring nodeName = ConvertToWString(node->mName.C_Str());

	// NodeInfo 생성
	NodeInfo* nodeInfo = new NodeInfo(parent, nodeName, nodeTM, depth);
	skinModel->GetNodeList().push_back(nodeInfo);

	// 자식 노드 처리
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene, skinModel, nodeInfo, depth + 1);
	}
}

void ModelLoader::ProcessMesh(const aiMesh* mesh, const aiScene* scene, Vertex& vertices, vector<unsigned long>& indices, SkinModelClass* skinModel)
{
	unsigned int startVertex = vertices.position.size();
	unsigned int startIndex = indices.size();

	// 정점 위치
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		vertices.position.push_back(XMFLOAT3(
			mesh->mVertices[i].x,
			mesh->mVertices[i].y,
			mesh->mVertices[i].z));
	}

	// UV 좌표
	if (mesh->mTextureCoords[0])
	{
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			vertices.uv.push_back(XMFLOAT2(
				mesh->mTextureCoords[0][i].x,
				mesh->mTextureCoords[0][i].y));
		}
	}
	else
	{
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			vertices.uv.push_back(XMFLOAT2(0.0f, 0.0f));
		}
	}

	// 본 인덱스와 가중치 초기화
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		vertices.boneidx.push_back(XMUINT4(0, 0, 0, 0));
		vertices.weight.push_back(XMFLOAT4(1.0f, 0.0f, 0.0f, 0.0f));
	}

	// 인덱스
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j] + startVertex);
		}
	}

	// HierarchyMesh 생성
	wstring meshName = ConvertToWString(mesh->mName.C_Str());
	HierarchyMesh* hierarchyMesh = new HierarchyMesh(
		meshName,
		startIndex,
		indices.size() - startIndex,
		mesh->mMaterialIndex,
		startVertex);

	skinModel->GetMeshList().push_back(hierarchyMesh);
}

void ModelLoader::ProcessSkin(const aiMesh* aiMesh, HierarchyMesh* mesh, Vertex& vertices, SkinModelClass* skinModel)
{
	if (!aiMesh->HasBones()) return;

	// 각 본 처리
	for (unsigned int i = 0; i < aiMesh->mNumBones; i++)
	{
		aiBone* bone = aiMesh->mBones[i];
		
		// 본 정보 찾기
		NodeInfo* boneNode = FindNode(bone->mName, skinModel->GetNodeList());
		if (!boneNode) continue;

		// BoneInfo 생성
		BoneInfo boneInfo;
		boneInfo.linkNode = boneNode;
		boneInfo.matOffset = ConvertMatrix(bone->mOffsetMatrix);
		
		mesh->boneList.push_back(boneInfo);

		// 가중치 정보 적용
		for (unsigned int j = 0; j < bone->mNumWeights; j++)
		{
			unsigned int vertexID = bone->mWeights[j].mVertexId + mesh->startVert;
			float weight = bone->mWeights[j].mWeight;

			// 빈 슬롯 찾아서 가중치 저장
			for (int k = 0; k < 4; k++)
			{
				if (((float*)&vertices.weight[vertexID])[k] == 0.0f)
				{
					((UINT*)&vertices.boneidx[vertexID])[k] = i;
					((float*)&vertices.weight[vertexID])[k] = weight;
					break;
				}
			}
		}
	}
}

void ModelLoader::ProcessAnimation(const aiScene* scene, SkinModelClass* skinModel)
{
	if (!scene->HasAnimations()) return;

	for (unsigned int i = 0; i < scene->mNumAnimations; i++)
	{
		aiAnimation* anim = scene->mAnimations[i];
		
		Animation animation;
		animation.SetName(ConvertToWString(anim->mName.C_Str()));
		animation.SetTickPerSecond((float)anim->mTicksPerSecond);
		animation.SetDuration((float)anim->mDuration);
		animation.SetLastFrame((float)anim->mDuration);
		animation.SetRepeat(true);

		// 각 채널(본별 애니메이션) 처리
		for (unsigned int j = 0; j < anim->mNumChannels; j++)
		{
			aiNodeAnim* channel = anim->mChannels[j];
			
			AniNode aniNode;
			aniNode.name = ConvertToWString(channel->mNodeName.C_Str());

			// 키프레임 처리
			unsigned int numKeys = max(max(channel->mNumPositionKeys, channel->mNumRotationKeys), channel->mNumScalingKeys);
			
			for (unsigned int k = 0; k < numKeys; k++)
			{
				KeyFrame keyFrame;
				
				// 시간
				if (k < channel->mNumPositionKeys)
					keyFrame.timePos = (float)channel->mPositionKeys[k].mTime;
				
				// 위치
				if (k < channel->mNumPositionKeys)
				{
					aiVector3D pos = channel->mPositionKeys[k].mValue;
					keyFrame.trans = XMFLOAT3(pos.x, pos.y, pos.z);
				}
				
				// 회전
				if (k < channel->mNumRotationKeys)
				{
					aiQuaternion rot = channel->mRotationKeys[k].mValue;
					keyFrame.rotation = XMFLOAT4(rot.x, rot.y, rot.z, rot.w);
				}
				
				// 스케일
				if (k < channel->mNumScalingKeys)
				{
					aiVector3D scale = channel->mScalingKeys[k].mValue;
					keyFrame.scale = XMFLOAT3(scale.x, scale.y, scale.z);
				}

				aniNode.keyFrame.push_back(keyFrame);
			}

			animation.GetAniNodeList().push_back(aniNode);
		}

		skinModel->GetAnimationList().push_back(animation);
	}
}

NodeInfo* ModelLoader::FindNode(const aiString& name, vector<NodeInfo*>& nodeList)
{
	wstring nodeName = ConvertToWString(name.C_Str());
	
	for (auto node : nodeList)
	{
		if (node->name == nodeName)
			return node;
	}
	
	return nullptr;
}

wstring ModelLoader::ConvertToWString(const char* str)
{
	if (!str) return L"";
	
	int len = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
	if (len == 0) return L"";
	
	wchar_t* wstr = new wchar_t[len];
	MultiByteToWideChar(CP_UTF8, 0, str, -1, wstr, len);
	
	wstring result(wstr);
	delete[] wstr;
	
	return result;
}

XMMATRIX ModelLoader::ConvertMatrix(const aiMatrix4x4& mat)
{
	return XMMATRIX(
		mat.a1, mat.b1, mat.c1, mat.d1,
		mat.a2, mat.b2, mat.c2, mat.d2,
		mat.a3, mat.b3, mat.c3, mat.d3,
		mat.a4, mat.b4, mat.c4, mat.d4
	);
}
