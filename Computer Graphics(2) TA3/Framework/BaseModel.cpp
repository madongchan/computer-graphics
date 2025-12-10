#include "BaseModel.h"

//변수 초기화
BaseModel::BaseModel()
{
	_vertexCount = 0;
	_indexCount = 0;

	_indexBuffer = NULL;

	_isNormalize = false;
	_name = L"";

	_primitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	_rotate		=	XMFLOAT3(0.f, 0.f, 0.f);										
	_center		=	XMFLOAT3(0.f, 0.f, 0.f);
	_size		=	XMFLOAT3(1.f, 1.f, 1.f);

	_scaleFactor = 1.f;													
}

//버퍼 메모리 해제
void BaseModel::Release()
{
	if (_indexBuffer) {
		_indexBuffer->Release();
		_indexBuffer = NULL;
	}
	
	if (_vertexDataBuffer.positionBuffer) {
		_vertexDataBuffer.positionBuffer->Release();
		_vertexDataBuffer.positionBuffer = NULL;
	}
	if (_vertexDataBuffer.colorBuffer) {
		_vertexDataBuffer.colorBuffer->Release();
		_vertexDataBuffer.colorBuffer = NULL;
	}
	if (_vertexDataBuffer.texcoordBuffer) {
		_vertexDataBuffer.texcoordBuffer->Release();
		_vertexDataBuffer.texcoordBuffer = NULL;
	}
	if (_vertexDataBuffer.normalBuffer) {
		_vertexDataBuffer.normalBuffer->Release();
		_vertexDataBuffer.normalBuffer = NULL;
	}
	if (_vertexDataBuffer.tangentBuffer) {
		_vertexDataBuffer.tangentBuffer->Release();
		_vertexDataBuffer.tangentBuffer = NULL;
	}
	if (_vertexDataBuffer.bitangentBuffer) {
		_vertexDataBuffer.bitangentBuffer->Release();
		_vertexDataBuffer.bitangentBuffer = NULL;
	}
	if (_vertexDataBuffer.boneIdBuffer) {
		_vertexDataBuffer.boneIdBuffer->Release();
		_vertexDataBuffer.boneIdBuffer = NULL;
	}
	if (_vertexDataBuffer.weightBuffer) {
		_vertexDataBuffer.weightBuffer->Release();
		_vertexDataBuffer.weightBuffer = NULL;
	}
}

//모델 행렬 재구성
XMMATRIX BaseModel::UpdateModelMatrix()
{
	//크기
	if (_isNormalize) {
		_tmModel = XMMatrixScaling(_size.x * _scaleFactor, _size.y * _scaleFactor, _size.z * _scaleFactor);
	}
	else {
		_tmModel = XMMatrixScaling(_size.x, _size.y, _size.z);
	}
	
	//회전
	_tmModel = XMMatrixMultiply(_tmModel, XMMatrixRotationX(_rotate.x));
	_tmModel = XMMatrixMultiply(_tmModel, XMMatrixRotationY(_rotate.y));
	_tmModel = XMMatrixMultiply(_tmModel, XMMatrixRotationZ(_rotate.z));

	//이동
	_tmModel = XMMatrixMultiply(_tmModel, XMMatrixTranslation(_center.x, _center.y, _center.z));
	
	return _tmModel;
}

//정점 및 인덱스 버퍼 셋팅
void BaseModel::SetRenderBuffers(ID3D11DeviceContext* dc)
{
	unsigned int offset = 0;

	//정점 버퍼 셋팅
	for (unsigned int i = 0; i < _vertexBuffer.size(); i++) {
		dc->IASetVertexBuffers(i, 1, _vertexBuffer[i], &_vertexStride[i], &offset);
	}

	//인덱스 정보가 있으면 인덱스 버퍼 셋팅
	if (_indexBuffer) {
		dc->IASetIndexBuffer(_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	}

	//모델의 렌더링 타입 설정
	dc->IASetPrimitiveTopology(_primitiveType);
}

bool BaseModel::InitVertextBuffers(ID3D11Device* device, Vertex& vertices, bool dynamicBuffer)
{
	bool result = true;

	_vertexCount = vertices.position.size();

	// Create position buffer
	if (!vertices.position.empty()) {
		result = CreateBuffer(device, &_vertexDataBuffer.positionBuffer, vertices.position, dynamicBuffer);
		if (result) {
			_vertexBuffer.push_back(&_vertexDataBuffer.positionBuffer);
			_vertexStride.push_back(sizeof(XMFLOAT3));
		}
	}

	// Create UV buffer
	if (!vertices.uv.empty()) {
		result = CreateBuffer(device, &_vertexDataBuffer.texcoordBuffer, vertices.uv, dynamicBuffer);
		if (result) {
			_vertexBuffer.push_back(&_vertexDataBuffer.texcoordBuffer);
			_vertexStride.push_back(sizeof(XMFLOAT2));
		}
	}

	// Create bone ID buffer
	if (!vertices.boneidx.empty()) {
		result = CreateBuffer(device, &_vertexDataBuffer.boneIdBuffer, vertices.boneidx, dynamicBuffer);
		if (result) {
			_vertexBuffer.push_back(&_vertexDataBuffer.boneIdBuffer);
			_vertexStride.push_back(sizeof(XMUINT4));
		}
	}

	// Create weight buffer
	if (!vertices.weight.empty()) {
		result = CreateBuffer(device, &_vertexDataBuffer.weightBuffer, vertices.weight, dynamicBuffer);
		if (result) {
			_vertexBuffer.push_back(&_vertexDataBuffer.weightBuffer);
			_vertexStride.push_back(sizeof(XMFLOAT4));
		}
	}

	return result;
}

bool BaseModel::InitIndexBuffers(ID3D11Device* device, vector<unsigned long>& indices)
{
	D3D11_BUFFER_DESC indexBufferDesc;
	D3D11_SUBRESOURCE_DATA indexData;

	_indexCount = indices.size();

	// Index buffer description
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * _indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data
	indexData.pSysMem = &indices[0];
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer
	HRESULT hr = device->CreateBuffer(&indexBufferDesc, &indexData, &_indexBuffer);
	if (FAILED(hr))
		return false;

	return true;
}

bool BaseModel::CreateModel(ID3D11Device* device, Vertex& vertices, vector<unsigned long>& indices, D3D11_PRIMITIVE_TOPOLOGY type)
{
	bool result;

	_primitiveType = type;

	// Initialize vertex buffers
	result = InitVertextBuffers(device, vertices, false);
	if (!result)
		return false;

	// Initialize index buffer
	result = InitIndexBuffers(device, indices);
	if (!result)
		return false;

	return true;
}
