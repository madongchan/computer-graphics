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
	m_shapeType = ShapeType::None; // 초기값 설정
	m_rotation = XMFLOAT3(0.0f, 0.0f, 0.0f); // 회전 각도 초기화
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

// 매 프레임 호출되어 모델의 회전 각도를 업데이트합니다.
void ModelClass::UpdateRotation(float deltaTime)
{
	// 1초에 30도 회전 -> deltaTime 초 동안 회전할 각도 (라디안)
	float rotationAmount = XMConvertToRadians(30.0f * deltaTime);

	// ShapeType에 따라 해당 축의 회전 각도를 누적합니다.
	switch (m_shapeType)
	{
	case ShapeType::Fan: // X축 회전
		m_rotation.x += rotationAmount;
		// 필요하다면 각도를 0 ~ 2PI 범위로 유지 (예: fmodf 사용)
		// m_rotation.x = fmodf(m_rotation.x, XM_2PI);
		break;
	case ShapeType::Pantagon: // Y축 회전
		m_rotation.y += rotationAmount;
		// m_rotation.y = fmodf(m_rotation.y, XM_2PI);
		break;
	case ShapeType::House: // Z축 회전
		m_rotation.z += rotationAmount;
		// m_rotation.z = fmodf(m_rotation.z, XM_2PI);
		break;
	default:
		// 다른 타입은 회전 안 함
		break;
	}
}

// 현재 회전 각도를 기반으로 World Matrix를 계산하여 반환합니다.
XMMATRIX ModelClass::GetWorldMatrix(const XMMATRIX& baseWorldMatrix)
{
	// 1. 모델의 로컬 중심 좌표 얻기 (InitializeBuffers에서 사용한 값을 기준으로 함)
	//    (주의: House의 경우, 정확한 중심 좌표를 계산하거나 InitializeBuffers의 basePos를 사용할 수 있습니다.)
	XMFLOAT3 localCenter = XMFLOAT3(0.0f, 0.0f, 0.0f);
	float houseBaseHeight = 1.0f * 1.5f; // InitializeBuffers와 동일한 값 사용
	float roofHeight = 1.0f * 1.5f;      // InitializeBuffers와 동일한 값 사용

	switch (m_shapeType) {
	case ShapeType::Fan:
		localCenter = XMFLOAT3(-6.0f, 0.0f, 0.0f); // Fan 생성 시 중심 좌표
		break;
	case ShapeType::Pantagon:
		localCenter = XMFLOAT3(0.0f, -2.0f, 0.0f); // Pantagon(Star) 생성 시 중심 좌표
		break;
	case ShapeType::House:
		// House의 중심을 대략적으로 계산 (예: 밑면 중심과 지붕 꼭대기 사이)
		// 여기서는 간단히 InitializeBuffers의 basePos를 사용합니다.
		// 더 정확한 중심 계산이 필요할 수 있습니다.
		localCenter = XMFLOAT3(6.0f, -1.0f + (houseBaseHeight + roofHeight) / 2.0f, 0.0f); // 예시: 높이의 중간점
		// 또는 단순히 basePos:
		// localCenter = XMFLOAT3(6.0f, -1.0f, 0.0f);
		break;
	default:
		// 중심이 0,0,0 이거나 정의되지 않음
		break;
	}

	// 2. 이동 행렬 계산
	// 모델 중심 -> 원점 이동 행렬
	XMMATRIX translationToOrigin = XMMatrixTranslation(-localCenter.x, -localCenter.y, -localCenter.z);
	// 원점 -> 모델 중심 이동 행렬
	XMMATRIX translationBack = XMMatrixTranslation(localCenter.x, localCenter.y, localCenter.z);

	// 3. 회전 행렬 계산 (기존과 동일)
	XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(m_rotation.x, m_rotation.y, m_rotation.z);

	// 4. 최종 World Matrix 계산: (원점으로 이동) * (회전) * (원래 위치로 이동)
	// 행렬 곱 순서에 주의: 변환은 오른쪽에서 왼쪽으로 적용됩니다.
	// baseWorldMatrix는 보통 단위행렬이므로 마지막에 곱해도 큰 의미는 없지만,
	// 혹시 다른 전역 변환이 있다면 유지합니다.
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
			1, 9, 0,   // 0 → 9 → 1 순서로 변경
			3, 1, 2,   // 2 → 1 → 3 순서로 변경
			5, 3, 4,   // 4 → 3 → 5 순서로 변경
			7, 5, 6,   // 6 → 5 → 7 순서로 변경
			9, 7, 8    // 8 → 7 → 9 순서로 변경
		};
		break;
	}
	case ShapeType::House:
	{
		XMFLOAT3 basePos = XMFLOAT3(6.0f, -1.0f, 0.0f); // Positioned to the right
		float houseWidth = 2.0f * 1.5f; // Increased size
		float houseBaseHeight = 1.0f * 1.5f; // Increased size
		float roofHeight = 1.0f * 1.5f; // Increased size

		// 기존 정점 정의
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