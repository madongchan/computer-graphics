#include "SkyboxShaderClass.h"
#include <cstring> // (OutputShaderErrorMessage용)

SkyboxShaderClass::SkyboxShaderClass()
{
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_matrixBuffer = 0;
	m_samplerState = 0;
}

SkyboxShaderClass::SkyboxShaderClass(const SkyboxShaderClass& other) {}
SkyboxShaderClass::~SkyboxShaderClass() {}

bool SkyboxShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	// 1단계에서 만든 skybox.hlsl 파일을 로드합니다.
	return InitializeShader(device, hwnd, L"./data/skybox.hlsl");
}

void SkyboxShaderClass::Shutdown()
{
	ShutdownShader();
}

bool SkyboxShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount,
	XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
	ID3D11ShaderResourceView* cubeTexture)
{
	bool result;
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, cubeTexture);
	if (!result)
	{
		return false;
	}
	RenderShader(deviceContext, indexCount);
	return true;
}

bool SkyboxShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, const WCHAR* hlslFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage = 0;
	ID3D10Blob* vertexShaderBuffer = 0;
	ID3D10Blob* pixelShaderBuffer = 0;

	// 스카이박스 정점 셰이더 (SKYMAP_VS) 컴파일
	result = D3DCompileFromFile(hlslFilename, NULL, NULL, "SKYMAP_VS", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		if (errorMessage) { OutputShaderErrorMessage(errorMessage, hwnd, hlslFilename); }
		else { MessageBox(hwnd, hlslFilename, L"Missing Shader File", MB_OK); }
		return false;
	}

	// 스카이박스 픽셀 셰이더 (SKYMAP_PS) 컴파일
	result = D3DCompileFromFile(hlslFilename, NULL, NULL, "SKYMAP_PS", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		if (errorMessage) { OutputShaderErrorMessage(errorMessage, hwnd, hlslFilename); }
		else { MessageBox(hwnd, hlslFilename, L"Missing Shader File", MB_OK); }
		return false;
	}

	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result)) { return false; }

	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result)) { return false; }

	// Input Layout (ModelClass::VertexType과 일치해야 함)
	D3D11_INPUT_ELEMENT_DESC polygonLayout[5];
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	// (신규) Tangent
	polygonLayout[3].SemanticName = "TANGENT";
	polygonLayout[3].SemanticIndex = 0;
	polygonLayout[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[3].InputSlot = 0;
	polygonLayout[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[3].InstanceDataStepRate = 0;

	// (신규) Binormal
	polygonLayout[4].SemanticName = "BINORMAL";
	polygonLayout[4].SemanticIndex = 0;
	polygonLayout[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[4].InputSlot = 0;
	polygonLayout[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[4].InstanceDataStepRate = 0;
	unsigned int numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result)) { return false; }

	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;
	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// WVP 상수 버퍼 생성 (LightShaderClass와 동일한 구조)
	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result)) { return false; }

	// 샘플러 상태 생성
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	result = device->CreateSamplerState(&samplerDesc, &m_samplerState);
	if (FAILED(result)) { return false; }

	return true;
}

void SkyboxShaderClass::ShutdownShader()
{
	if (m_samplerState) { m_samplerState->Release(); m_samplerState = 0; }
	if (m_matrixBuffer) { m_matrixBuffer->Release(); m_matrixBuffer = 0; }
	if (m_layout) { m_layout->Release(); m_layout = 0; }
	if (m_pixelShader) { m_pixelShader->Release(); m_pixelShader = 0; }
	if (m_vertexShader) { m_vertexShader->Release(); m_vertexShader = 0; }
}

// (LightShaderClass.cpp에서 복사)
void SkyboxShaderClass::OutputShaderErrorMessage(ID3DBlob* errorMessage, HWND hwnd, const WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long long bufferSize, i;
	ofstream fout;
	compileErrors = (char*)(errorMessage->GetBufferPointer());
	bufferSize = errorMessage->GetBufferSize();
	fout.open("shader-error-skybox.txt"); // (에러 로그 파일 이름 변경)
	for (i = 0; i < bufferSize; i++) { fout << compileErrors[i]; }
	fout.close();
	const size_t cSize = strlen(compileErrors) + 1;
	wchar_t* wc = new wchar_t[cSize];
	size_t convertedChars;
	mbstowcs_s(&convertedChars, wc, cSize, compileErrors, _TRUNCATE);
	MessageBoxW(hwnd, wc, L"Skybox Shader Compile Error", MB_OK);
	delete[] wc;
	errorMessage->Release();
	errorMessage = 0;
}

bool SkyboxShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext,
	XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
	ID3D11ShaderResourceView* cubeTexture)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;

	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) { return false; }

	dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = XMMatrixTranspose(worldMatrix);
	dataPtr->view = XMMatrixTranspose(viewMatrix);
	dataPtr->projection = XMMatrixTranspose(projectionMatrix);

	deviceContext->Unmap(m_matrixBuffer, 0);

	// VS의 b0 레지스터에 WVP 버퍼 설정
	deviceContext->VSSetConstantBuffers(0, 1, &m_matrixBuffer);

	// PS의 t0 레지스터에 큐브맵 텍스처 설정
	deviceContext->PSSetShaderResources(0, 1, &cubeTexture);

	// PS의 s0 레지스터에 샘플러 설정
	deviceContext->PSSetSamplers(0, 1, &m_samplerState);

	return true;
}

void SkyboxShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	deviceContext->IASetInputLayout(m_layout);
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);
	deviceContext->DrawIndexed(indexCount, 0, 0);
}