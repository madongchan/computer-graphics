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
	m_shapeType = ShapeType::None; // �ʱⰪ ����
	m_rotation = XMFLOAT3(0.0f, 0.0f, 0.0f); // ȸ�� ���� �ʱ�ȭ
}


ModelClass::ModelClass(const ModelClass& other)
{
}


ModelClass::~ModelClass()
{
}


bool ModelClass::Initialize(ID3D11Device* device, ShapeType shapetype)
{
	bool result;

	// Store the shape type for this instance
	m_shapeType = shapetype;

	// Initialize the vertex and index buffers based on the stored shape type
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	return true;
}

void ModelClass::Shutdown()
{
	// Shutdown the vertex and index buffers.
	ShutdownBuffers();
	return;
}


void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);

	return;
}


int ModelClass::GetIndexCount()
{
	return m_indexCount;
}

// �� ������ ȣ��Ǿ� ���� ȸ�� ������ ������Ʈ�մϴ�.
void ModelClass::UpdateRotation(float deltaTime)
{
	// 1�ʿ� 30�� ȸ�� -> deltaTime �� ���� ȸ���� ���� (����)
	float rotationAmount = XMConvertToRadians(30.0f * deltaTime);

	// ShapeType�� ���� �ش� ���� ȸ�� ������ �����մϴ�.
	switch (m_shapeType)
	{
	case ShapeType::Fan: // X�� ȸ��
		m_rotation.x += rotationAmount;
		// �ʿ��ϴٸ� ������ 0 ~ 2PI ������ ���� (��: fmodf ���)
		// m_rotation.x = fmodf(m_rotation.x, XM_2PI);
		break;
	case ShapeType::Pantagon: // Y�� ȸ��
		m_rotation.y += rotationAmount;
		// m_rotation.y = fmodf(m_rotation.y, XM_2PI);
		break;
	case ShapeType::House: // Z�� ȸ��
		m_rotation.z += rotationAmount;
		// m_rotation.z = fmodf(m_rotation.z, XM_2PI);
		break;
	default:
		// �ٸ� Ÿ���� ȸ�� �� ��
		break;
	}
}

// ���� ȸ�� ������ ������� World Matrix�� ����Ͽ� ��ȯ�մϴ�.
XMMATRIX ModelClass::GetWorldMatrix(const XMMATRIX& baseWorldMatrix)
{
	// 1. ���� ���� �߽� ��ǥ ��� (InitializeBuffers���� ����� ���� �������� ��)
	//    (����: House�� ���, ��Ȯ�� �߽� ��ǥ�� ����ϰų� InitializeBuffers�� basePos�� ����� �� �ֽ��ϴ�.)
	XMFLOAT3 localCenter = XMFLOAT3(0.0f, 0.0f, 0.0f);
	float houseBaseHeight = 1.0f * 1.5f; // InitializeBuffers�� ������ �� ���
	float roofHeight = 1.0f * 1.5f;      // InitializeBuffers�� ������ �� ���

	switch (m_shapeType) {
	case ShapeType::Fan:
		localCenter = XMFLOAT3(-6.0f, 0.0f, 0.0f); // Fan ���� �� �߽� ��ǥ
		break;
	case ShapeType::Pantagon:
		localCenter = XMFLOAT3(0.0f, -2.0f, 0.0f); // Pantagon(Star) ���� �� �߽� ��ǥ
		break;
	case ShapeType::House:
		// House�� �߽��� �뷫������ ��� (��: �ظ� �߽ɰ� ���� ����� ����)
		// ���⼭�� ������ InitializeBuffers�� basePos�� ����մϴ�.
		// �� ��Ȯ�� �߽� ����� �ʿ��� �� �ֽ��ϴ�.
		localCenter = XMFLOAT3(6.0f, -1.0f + (houseBaseHeight + roofHeight) / 2.0f, 0.0f); // ����: ������ �߰���
		// �Ǵ� �ܼ��� basePos:
		// localCenter = XMFLOAT3(6.0f, -1.0f, 0.0f);
		break;
	default:
		// �߽��� 0,0,0 �̰ų� ���ǵ��� ����
		break;
	}

	// 2. �̵� ��� ���
	// �� �߽� -> ���� �̵� ���
	XMMATRIX translationToOrigin = XMMatrixTranslation(-localCenter.x, -localCenter.y, -localCenter.z);
	// ���� -> �� �߽� �̵� ���
	XMMATRIX translationBack = XMMatrixTranslation(localCenter.x, localCenter.y, localCenter.z);

	// 3. ȸ�� ��� ��� (������ ����)
	XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(m_rotation.x, m_rotation.y, m_rotation.z);

	// 4. ���� World Matrix ���: (�������� �̵�) * (ȸ��) * (���� ��ġ�� �̵�)
	// ��� �� ������ ����: ��ȯ�� �����ʿ��� �������� ����˴ϴ�.
	// baseWorldMatrix�� ���� ��������̹Ƿ� �������� ���ص� ū �ǹ̴� ������,
	// Ȥ�� �ٸ� ���� ��ȯ�� �ִٸ� �����մϴ�.
	XMMATRIX finalWorldMatrix = translationToOrigin * rotationMatrix * translationBack * baseWorldMatrix;

	return finalWorldMatrix;
}

// InitializeBuffers: Creates geometry based on m_shapeType
bool ModelClass::InitializeBuffers(ID3D11Device* device) {
	vector<VertexType> vertices;
	vector<unsigned long> indices;
	HRESULT result;

	// Define vertices and indices based on m_shapeType
	switch (m_shapeType) {
	case ShapeType::Fan:
	{
		vertices.resize(6); // Center + 5 outer points
		float radius = 3.0f; // Increased size
		XMFLOAT3 centerPos = XMFLOAT3(-6.0f, 0.0f, 0.0f); // Positioned to the left

		vertices[0].position = centerPos;
		vertices[0].color = XMFLOAT4(1.0f, 0.5f, 0.0f, 1.0f); // Orange center

		for (int i = 0; i < 5; ++i) {
			// Angle from 0 to PI (180 degrees) for the 5 points
			float angle = XM_PI * i / 4.0f;
			float x = centerPos.x + radius * cosf(angle);
			float y = centerPos.y + radius * sinf(angle);
			vertices[i + 1].position = XMFLOAT3(x, y, 0.0f);
			vertices[i + 1].color = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f); // Yellow outer points
		}

		// Create triangles fanning out from the center
		for (int i = 0; i < 4; ++i) { // Indices for 4 triangles
			indices.push_back(0);          // Center
			indices.push_back(i + 2);      // Next outer point
			indices.push_back(i + 1);      // Current outer point
		}
		break;
	}
	case ShapeType::Pantagon: // Represents the star shape
	{
		vertices.resize(10); // 5 outer points, 5 inner points
		float outerRadius = 3.0f; // Increased size
		float innerRadius = 1.5f; // Increased size
		XMFLOAT3 centerPos = XMFLOAT3(0.0f, -2.0f, 0.0f); // Positioned in the center, slightly down

		for (int i = 0; i < 5; ++i) {
			float outerAngle = XM_PI / 2.0f - 2.0f * XM_PI * i / 5.0f; // Angle for outer point
			float innerAngle = outerAngle - XM_PI / 5.0f;            // Angle for inner point

			// Outer vertex
			vertices[2 * i].position = XMFLOAT3(centerPos.x + outerRadius * cosf(outerAngle), centerPos.y + outerRadius * sinf(outerAngle), 0.0f);
			vertices[2 * i].color = XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f); // Magenta

			// Inner vertex
			vertices[2 * i + 1].position = XMFLOAT3(centerPos.x + innerRadius * cosf(innerAngle), centerPos.y + innerRadius * sinf(innerAngle), 0.0f);
			vertices[2 * i + 1].color = XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f); // Cyan
		}

		// Indices to form the star shape (5 triangles pointing outwards)
		indices = {
			1, 9, 0,   // 0 �� 9 �� 1 ������ ����
			3, 1, 2,   // 2 �� 1 �� 3 ������ ����
			5, 3, 4,   // 4 �� 3 �� 5 ������ ����
			7, 5, 6,   // 6 �� 5 �� 7 ������ ����
			9, 7, 8    // 8 �� 7 �� 9 ������ ����
		};
		break;
	}
	case ShapeType::House:
	{
		XMFLOAT3 basePos = XMFLOAT3(6.0f, -1.0f, 0.0f); // Positioned to the right
		float houseWidth = 2.0f * 1.5f; // Increased size
		float houseBaseHeight = 1.0f * 1.5f; // Increased size
		float roofHeight = 1.0f * 1.5f; // Increased size

		// ���� ���� ����
		vertices = {
			// Base rectangle (4 vertices, Gray color)
			{{basePos.x - houseWidth / 2.0f, basePos.y                    , 0.0f}, {0.5f, 0.5f, 0.5f, 1.0f}}, // 0: Bottom left
			{{basePos.x - houseWidth / 2.0f, basePos.y + houseBaseHeight  , 0.0f}, {0.5f, 0.5f, 0.5f, 1.0f}}, // 1: Top left
			{{basePos.x + houseWidth / 2.0f, basePos.y + houseBaseHeight  , 0.0f}, {0.5f, 0.5f, 0.5f, 1.0f}}, // 2: Top right
			{{basePos.x + houseWidth / 2.0f, basePos.y                    , 0.0f}, {0.5f, 0.5f, 0.5f, 1.0f}}, // 3: Bottom right

			// Roof peak (Red color)
			{{basePos.x                    , basePos.y + houseBaseHeight + roofHeight, 0.0f}, {0.8f, 0.2f, 0.2f, 1.0f}}, // 4: Roof peak

			// Roof center (New vertex for splitting the roof into two triangles)
			{{basePos.x                    , basePos.y + houseBaseHeight            , 0.01f}, {1.0f, 1.0f, 1.0f, 1.0f}} // New vertex: Roof center (5)
		};

		indices = {
			// Base rectangle (two triangles)
			0, 1, 2,
			0, 2, 3,

			// Roof triangle (split into two triangles)
			// Left roof triangle
			1, 4, 5,
			// Right roof triangle
			5, 4, 2,
		};
		break;
	}
	case ShapeType::None:
	default:
		// Handle error or empty geometry: Initialize with null buffers
		m_vertexCount = 0;
		m_indexCount = 0;
		m_vertexBuffer = nullptr;
		m_indexBuffer = nullptr;
		// Optionally log an error message here
		return true; // Return true indicating handled, though no geometry loaded
	}

	// If no vertices/indices were generated for a valid type, something is wrong
	if (vertices.empty() || indices.empty()) {
		m_vertexCount = 0;
		m_indexCount = 0;
		m_vertexBuffer = nullptr;
		m_indexBuffer = nullptr;
		return false; // Indicate failure
	}

	m_vertexCount = static_cast<int>(vertices.size());
	m_indexCount = static_cast<int>(indices.size());

	// --- Create Vertex Buffer ---
	D3D11_BUFFER_DESC vertexBufferDesc = {};
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexData = {};
	vertexData.pSysMem = vertices.data();

	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result)) {
		// Clean up potential partial resource allocation
		ShutdownBuffers(); // Ensure buffers are released
		return false;
	}

	// --- Create Index Buffer ---
	D3D11_BUFFER_DESC indexBufferDesc = {};
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA indexData = {};
	indexData.pSysMem = indices.data();

	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result)) {
		// Clean up potential partial resource allocation
		ShutdownBuffers(); // Ensure buffers are released
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

// This sets the vertex buffer and index buffer as active on the input assembler in the GPU. 
// Once the GPU has an active vertex buffer, it can then use the shader to render that buffer. 
// This function also defines how those buffers should be drawn such as triangles, lines, fans, 
// and etc using the IASetPrimitiveTopology DirectX function.
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