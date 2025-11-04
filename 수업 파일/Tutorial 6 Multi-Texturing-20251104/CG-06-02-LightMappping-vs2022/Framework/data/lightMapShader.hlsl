// GLOBALS //
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

Texture2D shaderTextures[2];
SamplerState SampleType;

// TYPEDEFS //
struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

// Vertex Shader
PixelInputType LightMapVertexShader(VertexInputType input)
{
    PixelInputType output;

	// 적절한 행렬 계산을 위해 위치 벡터를 4 단위로 변경합니다.
    input.position.w = 1.0f;

	// 월드, 뷰 및 투영 행렬에 대한 정점의 위치를 ​​계산합니다.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
	// 픽셀 쉐이더의 텍스처 좌표를 저장한다.
	output.tex = input.tex;
    
    return output;
}

// Pixel Shader
float4 LightMapPixelShader(PixelInputType input) : SV_TARGET
{
	float4 color;
    float4 lightColor;
    float4 finalColor;

    // 색상 텍스처에서 픽셀 색상을 가져옵니다.
    color = shaderTextures[0].Sample(SampleType, input.tex);

    // 라이트 맵에서 픽셀 색상을 가져옵니다.
    lightColor = shaderTextures[1].Sample(SampleType, input.tex);

    // 두 픽셀을 혼합합니다.
    finalColor = color * lightColor;

	return finalColor;
}