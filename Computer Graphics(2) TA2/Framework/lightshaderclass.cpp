////////////////////////////////////////////////////////////////////////////////
// Filename: lightshaderclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "lightshaderclass.h"
#include <cstring>

LightShaderClass::LightShaderClass()
{
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_sampleState = 0;
	m_matrixBuffer = 0;
	m_lightBuffer = 0;
	m_lightBuffer = 0;
	m_lightToggleBuffer = 0;
	m_pointLightBuffer = 0;
}


LightShaderClass::LightShaderClass(const LightShaderClass& other)
{
}


LightShaderClass::~LightShaderClass()
{
}

// The new HLSL shader files are used as input to initialize the light shader.
bool LightShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;


	// Initialize the vertex and pixel shaders.
	result = InitializeShader(device, hwnd, L"./data/phongLightShader.hlsl");
	if(!result)
	{
		return false;
	}

	return true;
}


void LightShaderClass::Shutdown()
{
	// Shutdown the vertex and pixel shaders as well as the related objects.
	ShutdownShader();

	return;
}

bool LightShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, const WCHAR* fileName)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[5];
	unsigned int numElements;
    D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC lightBufferDesc;
	D3D11_BUFFER_DESC cameraBufferDesc;
	D3D11_BUFFER_DESC lightToggleBufferDesc;

	// Initialize the pointers this function will use to null.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

    // Compile the vertex shader code.
	result = D3DCompileFromFile(fileName, NULL, NULL, "LightVertexShader", "vs_5_0", 
		D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
	if(FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if(errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, fileName);
		}
		// If there was nothing in the error message then it simply could not find the shader file itself.
		else
		{
			MessageBox(hwnd, fileName, L"Missing Shader File", MB_OK);
		}

		return false;
	}

    // Compile the pixel shader code.
	result = D3DCompileFromFile(fileName, NULL, NULL, "LightPixelShader", "ps_5_0", 
		D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
	if(FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if(errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, fileName);
		}
		// If there was nothing in the error message then it simply could not find the file itself.
		else
		{
			MessageBox(hwnd,fileName, L"Missing Shader File", MB_OK);
		}

		return false;
	}

    // Create the vertex shader from the buffer.
    result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if(FAILED(result))
	{
		return false;
	}

    // Create the pixel shader from the buffer.
    result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if(FAILED(result))
	{
		return false;
	}

	// Create the vertex input layout description.
	// This setup needs to match the VertexType stucture in the ModelClass and in the shader.
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

	// Get a count of the elements in the layout.
    numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), 
		                               &m_layout);
	if(FAILED(result))
	{
		return false;
	}

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Setup the description of the light dynamic constant buffer that is in the pixel shader.
	// Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&lightBufferDesc, NULL, &m_lightBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Setup the description of the camera dynamic constant buffer that is in the vertex shader.
	cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufferDesc.ByteWidth = sizeof(CameraBufferType);
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufferDesc.MiscFlags = 0;
	cameraBufferDesc.StructureByteStride = 0;

	// Create the camera constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&cameraBufferDesc, NULL, &m_cameraBuffer);
	if (FAILED(result))
	{
		return false;
	}

	lightToggleBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightToggleBufferDesc.ByteWidth = sizeof(LightToggleBufferType);
	lightToggleBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightToggleBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightToggleBufferDesc.MiscFlags = 0;
	lightToggleBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&lightToggleBufferDesc, NULL, &m_lightToggleBuffer);
	if (FAILED(result))
	{
		return false;
	}
	// ★ 2. (추가) 포인트 라이트 버퍼 생성
	D3D11_BUFFER_DESC pointLightBufferDesc;
	pointLightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	pointLightBufferDesc.ByteWidth = sizeof(PointLightBufferType); // 새 구조체 크기 사용
	pointLightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	pointLightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pointLightBufferDesc.MiscFlags = 0;
	pointLightBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&pointLightBufferDesc, NULL, &m_pointLightBuffer);
	if (FAILED(result))
	{
		return false;
	}


	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if (FAILED(result))
	{
		return false;
	}
	return true;
}


void LightShaderClass::ShutdownShader()
{
	// Release the light constant buffer.
	if(m_lightBuffer)
	{
		m_lightBuffer->Release();
		m_lightBuffer = 0;
	}

	// Release the matrix constant buffer.
	if(m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}
	// Release the point light constant buffer.
	if (m_pointLightBuffer)
	{
		m_pointLightBuffer->Release();
		m_pointLightBuffer = 0;
	}
	// Release the light toggle constant buffer.
	if (m_lightToggleBuffer)
	{
		m_lightToggleBuffer->Release();
		m_lightToggleBuffer = 0;
	}
	// Release the sampler state.
	if(m_sampleState)
	{
		m_sampleState->Release();
		m_sampleState = 0;
	}

	// Release the layout.
	if(m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	// Release the pixel shader.
	if(m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	// Release the vertex shader.
	if(m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}

	// Release the camera constant buffer.
	if (m_cameraBuffer)
	{
		m_cameraBuffer->Release();
		m_cameraBuffer = 0;
	}

	return;
}


void LightShaderClass::OutputShaderErrorMessage(ID3DBlob* errorMessage, HWND hwnd, const WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long long bufferSize, i;
	ofstream fout;

	// 1. errorMessage blob에서 8비트 ASCII 오류 문자열 포인터를 가져옵니다.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// 2. 문자열의 길이를 가져옵니다.
	bufferSize = errorMessage->GetBufferSize();

	// 3. shader-error.txt 파일을 엽니다.
	fout.open("shader-error.txt");

	// 4. ASCII 문자열을 파일에 그대로 씁니다.
	for (i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// 5. 파일을 닫습니다.
	fout.close();

	// --- (★가장 중요★) ---
	// 6. ASCII 오류 메시지를 WCHAR* (유니코드)로 변환하여 메시지 박스로 띄웁니다.
	// (이 코드를 위해 <string>과 <locale> 헤더가 필요할 수 있습니다)
	const size_t cSize = strlen(compileErrors) + 1;
	wchar_t* wc = new wchar_t[cSize];
	size_t convertedChars;
	mbstowcs_s(&convertedChars, wc, cSize, compileErrors, _TRUNCATE);

	// 7. 사용자에게 변환된 오류 메시지를 보여줍니다.
	MessageBoxW(hwnd, wc, L"Shader Compile Error", MB_OK); // WCHAR* 사용

	delete[] wc; // 메모리 해제

	// 8. errorMessage blob을 해제합니다.
	errorMessage->Release();
	errorMessage = 0;

	return;
}

bool LightShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount,
	XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
	ID3D11ShaderResourceView* texture,
	XMFLOAT3 lightDirection, XMFLOAT4 ambientColor, XMFLOAT4 diffuseColor,
	XMFLOAT3 cameraPosition, XMFLOAT4 specularColor, float specularPower,
	bool isAmbientOn, bool isDiffuseOn, bool isSpecularOn,
	XMFLOAT4 pointLightPositions[], XMFLOAT4 pointLightColors[], float pointLightIntensity)
{
	bool result;


	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture,
		lightDirection, ambientColor, diffuseColor, cameraPosition, specularColor, specularPower,
		isAmbientOn, isDiffuseOn, isSpecularOn,
		pointLightPositions, pointLightColors, pointLightIntensity);
	if (!result)
	{
		return false;
	}

	// Now render the prepared buffers with the shader.
	RenderShader(deviceContext, indexCount);

	return true;
}


// The SetShaderParameters function now takes in lightDirection and diffuseColor as inputs.
bool LightShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, 
	XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, 
	ID3D11ShaderResourceView* texture, 
	XMFLOAT3 lightDirection, XMFLOAT4 ambientColor, XMFLOAT4 diffuseColor, XMFLOAT3 cameraPosition, XMFLOAT4 specularColor, float specularPower,
	bool isAmbientOn, bool isDiffuseOn, bool isSpecularOn,
	XMFLOAT4 pointLightPositions[], XMFLOAT4 pointLightColors[], float pointLightIntensity)
{
	HRESULT result;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int bufferNumber;
	MatrixBufferType* dataPtr;
	LightBufferType* dataPtr2;
	CameraBufferType* dataPtr3;
	LightToggleBufferType* dataPtr_Toggle;
	PointLightBufferType* dataPtr_PointLight;

	// Transpose the matrices to prepare them for the shader.
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// Unlock the constant buffer.
    deviceContext->Unmap(m_matrixBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	// Now set the constant buffer in the vertex shader with the updated values.
    deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);


	// Lock the light constant buffer so it can be written to.
	result = deviceContext->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr2 = (LightBufferType*)mappedResource.pData;

	// Copy the lighting variables into the constant buffer.
	dataPtr2->ambientColor = ambientColor;
	dataPtr2->diffuseColor = diffuseColor;
	dataPtr2->lightDirection = lightDirection;
	dataPtr2->specularColor = specularColor;
	dataPtr2->specularPower = specularPower;

	// Unlock the constant buffer.
	deviceContext->Unmap(m_lightBuffer, 0);

	// Set the position of the light constant buffer in the pixel shader.
	bufferNumber = 0;

	// Finally set the light constant buffer in the pixel shader with the updated values.
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_lightBuffer);

	// Lock the camera constant buffer so it can be written to.
	result = deviceContext->Map(m_cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr3 = (CameraBufferType*)mappedResource.pData;

	// Copy the camera position into the constant buffer.
	dataPtr3->cameraPosition = cameraPosition;
	dataPtr3->padding = 0.0f;

	// Unlock the camera constant buffer.
	deviceContext->Unmap(m_cameraBuffer, 0);

	// Set the position of the camera constant buffer in the vertex shader.
	bufferNumber = 1;

	// Now set the camera constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_cameraBuffer);

	// ------------------ Lighting Toggle Buffer ------------------
	result = deviceContext->Map(m_lightToggleBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// 데이터 포인터를 가져옵니다.
	dataPtr_Toggle = (LightToggleBufferType*)mappedResource.pData;

	// C++의 bool 값을 HLSL의 float (1.0f/0.0f)로 변환하여 복사합니다.
	dataPtr_Toggle->isAmbientOn = isAmbientOn ? 1.0f : 0.0f;
	dataPtr_Toggle->isDiffuseOn = isDiffuseOn ? 1.0f : 0.0f;
	dataPtr_Toggle->isSpecularOn = isSpecularOn ? 1.0f : 0.0f;
	dataPtr_Toggle->padding = 0.0f;

	// 버퍼를 풉니다.
	deviceContext->Unmap(m_lightToggleBuffer, 0);

	// 픽셀 셰이더의 상수 버퍼 슬롯(register) 1번에 설정합니다.
	deviceContext->PSSetConstantBuffers(1, 1, &m_lightToggleBuffer);

	// --- ★ (추가) Point Light Buffer 데이터 전송 ★ ---
	// ----------------------------------------------------
	// 포인트 라이트 버퍼 Map
	result = deviceContext->Map(m_pointLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) { return false; }

	// 데이터 포인터 설정
	dataPtr_PointLight = (PointLightBufferType*)mappedResource.pData;

	// C++ 배열 데이터를 HLSL 구조체 배열로 복사 (memcpy 사용)
	// 위치 배열 복사
	memcpy(dataPtr_PointLight->pointLightPosition, pointLightPositions, sizeof(XMFLOAT4) * NUM_POINT_LIGHTS);
	// 색상 배열 복사
	memcpy(dataPtr_PointLight->pointLightColor, pointLightColors, sizeof(XMFLOAT4) * NUM_POINT_LIGHTS);
	// 강도 값 복사
	dataPtr_PointLight->pointLightIntensity = pointLightIntensity;
	// 패딩 값 설정 (HLSL과 일치)
	dataPtr_PointLight->padding = XMFLOAT3(0.0f, 0.0f, 0.0f); // 또는 0.0f 하나만 해도 될 수 있음

	// 포인트 라이트 버퍼 Unmap
	deviceContext->Unmap(m_pointLightBuffer, 0);

	deviceContext->PSSetConstantBuffers(2, 1, &m_pointLightBuffer);

	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &texture);

	return true;
}


void LightShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(m_layout);

    // Set the vertex and pixel shaders that will be used to render this triangle.
    deviceContext->VSSetShader(m_vertexShader, NULL, 0);
    deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// Set the sampler state in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	// Render the triangle.
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}