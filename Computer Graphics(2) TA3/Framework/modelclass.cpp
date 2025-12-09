////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "modelclass.h"


ModelClass::ModelClass()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_vertexCount = 0;
	m_indexCount = 0;
	m_Texture = 0;
	m_vertices = 0;
	m_indices = 0;
	m_faceCount = 0;
}


ModelClass::ModelClass(const ModelClass& other)
{
}


ModelClass::~ModelClass()
{
}


bool ModelClass::Initialize(ID3D11Device* device, const char* modelFilename, const WCHAR* textureFilename)
{
	bool result;

	// 1. (FBX/OBJ) 모델 파일 로드
	// Assimp를 사용해 파일에서 정점/인덱스/UV/법선 데이터를 읽어 
	// RAM(m_vertices, m_indices)에 저장합니다.
	result = LoadModel(modelFilename); //
	if (!result)
	{
		return false;
	}

	// 2. GPU 버퍼 생성
	// RAM(m_vertices, m_indices)에 저장된 데이터를 기반으로 
	// GPU가 사용할 Vertex Buffer와 Index Buffer를 생성합니다.
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	// 3. 텍스처 파일 로드
	// TextureClass 객체를 생성하고, DDS 텍스처 파일을 로드합니다.
	m_Texture = new TextureClass;
	if (!m_Texture)
	{
		return false;
	}

	result = m_Texture->Initialize(device, textureFilename);
	if (!result)
	{
		return false;
	}

	return true;
}

bool ModelClass::InitializeGeneratedCube(ID3D11Device* device)
{
	// 1. 1x1x1 큐브의 정점 (8개)
	// 위치(Pos), 텍스처(Tex), 법선(Normal) 순서 (LightShader와 포맷 맞춤)
	// 텍스처와 법선은 디버그용이라 0으로 채워도 됩니다.
	int vertexCount = 8;
	int indexCount = 36;

	VertexType* vertices = new VertexType[vertexCount];
	unsigned long* indices = new unsigned long[indexCount];

	// 정점 좌표 설정 (-0.5 ~ +0.5) -> 정확히 크기 1.0인 큐브
	vertices[0].position = XMFLOAT3(-0.5f, -0.5f, -0.5f); // 0: 좌하뒤
	vertices[1].position = XMFLOAT3(-0.5f, 0.5f, -0.5f); // 1: 좌상뒤
	vertices[2].position = XMFLOAT3(0.5f, 0.5f, -0.5f); // 2: 우상뒤
	vertices[3].position = XMFLOAT3(0.5f, -0.5f, -0.5f); // 3: 우하뒤
	vertices[4].position = XMFLOAT3(-0.5f, -0.5f, 0.5f); // 4: 좌하앞
	vertices[5].position = XMFLOAT3(-0.5f, 0.5f, 0.5f); // 5: 좌상앞
	vertices[6].position = XMFLOAT3(0.5f, 0.5f, 0.5f); // 6: 우상앞
	vertices[7].position = XMFLOAT3(0.5f, -0.5f, 0.5f); // 7: 우하앞

	// UV, Normal은 0으로 초기화 (안 쓸 거니까)
	for (int i = 0; i < 8; i++) {
		vertices[i].texture = XMFLOAT2(0, 0);
		vertices[i].normal = XMFLOAT3(0, 1, 0);
	}

	// 인덱스 (삼각형 12개 = 36개 인덱스)
	// 앞면 (4-5-6-7)
	indices[0] = 4; indices[1] = 5; indices[2] = 6;
	indices[3] = 4; indices[4] = 6; indices[5] = 7;
	// 뒷면 (3-2-1-0)
	indices[6] = 3; indices[7] = 2; indices[8] = 1;
	indices[9] = 3; indices[10] = 1; indices[11] = 0;
	// 윗면 (1-5-4-0) -> 죄송합니다 순서 정정: (1-5-6-2)
	indices[12] = 1; indices[13] = 5; indices[14] = 6;
	indices[15] = 1; indices[16] = 6; indices[17] = 2;
	// 아랫면 (4-0-3-7)
	indices[18] = 4; indices[19] = 0; indices[20] = 3;
	indices[21] = 4; indices[22] = 3; indices[23] = 7;
	// 좌측면 (0-1-5-4)
	indices[24] = 0; indices[25] = 1; indices[26] = 5;
	indices[27] = 0; indices[28] = 5; indices[29] = 4;
	// 우측면 (7-6-2-3)
	indices[30] = 7; indices[31] = 6; indices[32] = 2;
	indices[33] = 7; indices[34] = 2; indices[35] = 3;

	// 멤버 변수 설정
	m_vertexCount = vertexCount;
	m_indexCount = indexCount;

	// 2. 버퍼 생성 (기존 InitializeBuffers 코드의 뒷부분과 동일)
	// 코드가 중복되므로 InitializeBuffers의 버퍼 생성 로직을 따로 함수로 빼거나,
	// 여기에 버퍼 생성 코드(D3D11_BUFFER_DESC ...)를 복사해 넣으세요.
	// (보통 InitializeBuffers 안에 있는 'device->CreateBuffer' 부분입니다)

	// [간략 예시: 버퍼 생성 코드는 반드시 있어야 함!]
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result)) return false;

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result)) return false;

	// 배열 메모리 해제
	delete[] vertices;
	delete[] indices;

	return true;
}

void ModelClass::Shutdown()
{
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}

	// GPU 버퍼 해제
	ShutdownBuffers();

	//--- 수정 3: LoadModel이 사용한 RAM 해제 ---
	if (m_vertices)
	{
		delete[] m_vertices;
		m_vertices = 0;
	}
	if (m_indices)
	{
		delete[] m_indices;
		m_indices = 0;
	}

	return;
}


void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	// 모델의 정점/인덱스 버퍼를 렌더링 파이프라인에 올립니다.
	RenderBuffers(deviceContext);

	return;
}


int ModelClass::GetIndexCount()
{
	return m_indexCount;
}

ID3D11ShaderResourceView* ModelClass::GetTexture()
{
	return m_Texture->GetTexture(); //
}

bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	// (m_vertices와 m_indices는 LoadModel에서 이미 채워져 있어야 함)
	if (!m_vertices || !m_indices)
	{
		return false;
	}
	// 초기값을 무한대/최소값으로 설정
	m_minVertex = XMFLOAT3(100000.0f, 100000.0f, 100000.0f);
	m_maxVertex = XMFLOAT3(-100000.0f, -100000.0f, -100000.0f);
	// 모든 정점을 돌면서 최소/최대 좌표 갱신
	for (int i = 0; i < m_vertexCount; i++)
	{
		// X축
		if (m_vertices[i].position.x < m_minVertex.x) m_minVertex.x = m_vertices[i].position.x;
		if (m_vertices[i].position.x > m_maxVertex.x) m_maxVertex.x = m_vertices[i].position.x;

		// Y축
		if (m_vertices[i].position.y < m_minVertex.y) m_minVertex.y = m_vertices[i].position.y;
		if (m_vertices[i].position.y > m_maxVertex.y) m_maxVertex.y = m_vertices[i].position.y;

		// Z축
		if (m_vertices[i].position.z < m_minVertex.z) m_minVertex.z = m_vertices[i].position.z;
		if (m_vertices[i].position.z > m_maxVertex.z) m_maxVertex.z = m_vertices[i].position.z;
	}

	// 1. 정점 버퍼 생성 (m_vertices 배열 사용)
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem = m_vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// 2. 인덱스 버퍼 생성 (m_indices 배열 사용)
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	indexData.pSysMem = m_indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}


void ModelClass::ShutdownBuffers()
{
	// Release the index buffer.
	if(m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if(m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}


void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType); 
	offset = 0;
    
	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

    // Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

bool ModelClass::LoadModel(const char* filename)
{
	Assimp::Importer importer;
	// aiProcess_Triangulate: 모델이 사각형/오각형이어도 강제로 삼각형으로 쪼갬 (필수)
	// aiProcess_ConvertToLeftHanded: DirectX는 왼손 좌표계를 사용하므로 변환 (필수)
	// ★ aiProcess_CalcTangentSpace 플래그 추가
	// (이 플래그가 없으면 pMesh->mTangents, pMesh->mBitangents가 생성되지 않음)
	const aiScene* pScene = importer.ReadFile(filename,
		aiProcess_Triangulate |
		aiProcess_ConvertToLeftHanded |
		aiProcess_CalcTangentSpace); // ★ Tangent 계산 플래그

	if (!pScene) return false;

	// (주의: 이 코드는 첫 번째 메쉬만 로드합니다)
	const aiMesh* pMesh = pScene->mMeshes[0];
	if (!pMesh) return false;

	// (Tangent, Binormal 데이터가 있는지 확인)
	if (!pMesh->HasTangentsAndBitangents())
	{
		// (필요시: MessageBox 등으로 오류 알림)
		return false;
	}

	m_vertexCount = pMesh->mNumVertices;
	m_faceCount = static_cast<int>(pMesh->mNumFaces);
	m_indexCount = m_faceCount * 3; // 삼각형 1개 = 인덱스 3개

	if (m_vertexCount == 0 || m_indexCount == 0) return false;

	m_vertices = new VertexType[m_vertexCount];

	for (unsigned int i = 0; i < m_vertexCount; i++)
	{
		// 위치
		m_vertices[i].position.x = pMesh->mVertices[i].x;
		m_vertices[i].position.y = pMesh->mVertices[i].y;
		m_vertices[i].position.z = pMesh->mVertices[i].z;

		// UV (텍스처 좌표)
		if (pMesh->HasTextureCoords(0)) {
			m_vertices[i].texture.x = pMesh->mTextureCoords[0][i].x;
			m_vertices[i].texture.y = pMesh->mTextureCoords[0][i].y;
		}
		else {
			m_vertices[i].texture.x = 0;
			m_vertices[i].texture.y = 0;
		}

		// 법선 (Normal)
		m_vertices[i].normal.x = pMesh->mNormals[i].x;
		m_vertices[i].normal.y = pMesh->mNormals[i].y;
		m_vertices[i].normal.z = pMesh->mNormals[i].z;

		// ★ (신규) Tangent (접선)
		m_vertices[i].tangent.x = pMesh->mTangents[i].x;
		m_vertices[i].tangent.y = pMesh->mTangents[i].y;
		m_vertices[i].tangent.z = pMesh->mTangents[i].z;

		// ★ (신규) Binormal (이중접선)
		m_vertices[i].binormal.x = pMesh->mBitangents[i].x;
		m_vertices[i].binormal.y = pMesh->mBitangents[i].y;
		m_vertices[i].binormal.z = pMesh->mBitangents[i].z;
	}

	m_indices = new unsigned long[m_indexCount];

	for (int i = 0; i < m_faceCount; i++)
	{
		aiFace face = pMesh->mFaces[i];
		m_indices[i * 3 + 0] = face.mIndices[0];
		m_indices[i * 3 + 1] = face.mIndices[1];
		m_indices[i * 3 + 2] = face.mIndices[2];
	}

	return true;
}