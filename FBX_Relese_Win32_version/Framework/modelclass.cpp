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

	// 1. (FBX/OBJ) �� ���� �ε�
	// Assimp�� ����� ���Ͽ��� ����/�ε���/UV/���� �����͸� �о� 
	// RAM(m_vertices, m_indices)�� �����մϴ�.
	result = LoadModel(modelFilename); //
	if (!result)
	{
		return false;
	}

	// 2. GPU ���� ����
	// RAM(m_vertices, m_indices)�� ����� �����͸� ������� 
	// GPU�� ����� Vertex Buffer�� Index Buffer�� �����մϴ�.
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	// 3. �ؽ�ó ���� �ε�
	// TextureClass ��ü�� �����ϰ�, DDS �ؽ�ó ������ �ε��մϴ�.
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


void ModelClass::Shutdown()
{
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}

	// GPU ���� ����
	ShutdownBuffers();

	//--- ���� 3: LoadModel�� ����� RAM ���� ---
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
	// ���� ����/�ε��� ���۸� ������ ���������ο� �ø��ϴ�.
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

	// (m_vertices�� m_indices�� LoadModel���� �̹� ä���� �־�� ��)
	if (!m_vertices || !m_indices)
	{
		return false;
	}

	// 1. ���� ���� ���� (m_vertices �迭 ���)
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

	// 2. �ε��� ���� ���� (m_indices �迭 ���)
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
	// aiProcess_Triangulate: ���� �簢��/�������̾ ������ �ﰢ������ �ɰ� (�ʼ�)
	// aiProcess_ConvertToLeftHanded: DirectX�� �޼� ��ǥ�踦 ����ϹǷ� ��ȯ (�ʼ�)
	const aiScene* pScene = importer.ReadFile(filename,
		aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);

	if (!pScene) return false;

	const aiMesh* pMesh = pScene->mMeshes[0];
	if (!pMesh) return false;

	m_vertexCount = pMesh->mNumVertices;
	m_faceCount = static_cast<int>(pMesh->mNumFaces);
	m_indexCount = m_faceCount * 3; // �ﰢ�� 1�� = �ε��� 3��

	if (m_vertexCount == 0 || m_indexCount == 0) return false;

	m_vertices = new VertexType[m_vertexCount];

	for (unsigned int i = 0; i < m_vertexCount; i++)
	{
		// ��ġ
		m_vertices[i].position.x = pMesh->mVertices[i].x;
		m_vertices[i].position.y = pMesh->mVertices[i].y;
		m_vertices[i].position.z = pMesh->mVertices[i].z;

		// UV (�ؽ�ó ��ǥ)
		if (pMesh->HasTextureCoords(0)) {
			m_vertices[i].texture.x = pMesh->mTextureCoords[0][i].x;
			m_vertices[i].texture.y = pMesh->mTextureCoords[0][i].y;
		}
		else {
			m_vertices[i].texture.x = 0;
			m_vertices[i].texture.y = 0;
		}

		// ���� (Normal)
		m_vertices[i].normal.x = pMesh->mNormals[i].x;
		m_vertices[i].normal.y = pMesh->mNormals[i].y;
		m_vertices[i].normal.z = pMesh->mNormals[i].z;
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