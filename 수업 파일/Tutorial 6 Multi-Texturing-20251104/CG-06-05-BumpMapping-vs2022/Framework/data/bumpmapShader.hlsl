// GLOBALS //
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer LightBuffer
{
	float4 diffuseColor;
	float3 lightDirection;
};

Texture2D shaderTextures[2];
SamplerState SampleType;

// TYPEDEFS //
struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
   	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
};

// Vertex Shader
PixelInputType BumpMapVertexShader(VertexInputType input)
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
    
    // 월드 행렬에 대해서만 법선 벡터를 계산 한 다음 최종 값을 정규화합니다.
    output.normal = mul(input.normal, (float3x3)worldMatrix);
    output.normal = normalize(output.normal);

	// 월드 행렬에 대해서만 접선 벡터를 계산 한 다음 최종 값을 정규화합니다.
    output.tangent = mul(input.tangent, (float3x3)worldMatrix);
    output.tangent = normalize(output.tangent);

    // 세계 행렬에 대해서만 비 유효 벡터를 계산 한 다음 최종 값을 정규화합니다.
    output.binormal = mul(input.binormal, (float3x3)worldMatrix);
    output.binormal = normalize(output.binormal);

    return output;
}

// Pixel Shader
float4 BumpMapPixelShader(PixelInputType input) : SV_TARGET
{
	float4 textureColor;
    float4 bumpMap;
    float3 bumpNormal;
    float3 lightDir;
    float lightIntensity;
    float4 color;

    // 이 위치에서 텍스처 픽셀을 샘플링합니다.
    textureColor = shaderTextures[0].Sample(SampleType, input.tex);
	
    // 범프 맵에서 픽셀을 샘플링합니다.
    bumpMap = shaderTextures[1].Sample(SampleType, input.tex);

    // 정상 값의 범위를 (0, +1)에서 (-1, +1)로 확장합니다.
    bumpMap = (bumpMap * 2.0f) - 1.0f;
    
    // 범프 맵의 데이터에서 법선을 계산합니다.
    bumpNormal = (bumpMap.x * input.tangent) + (bumpMap.y * input.binormal) + (bumpMap.z * input.normal);
	
    // 결과 범프 법선을 표준화합니다.
    bumpNormal = normalize(bumpNormal);

    // 계산을 위해 빛 방향을 반전시킵니다.
    lightDir = -lightDirection;

    // 범프 맵 일반 값을 기반으로 픽셀의 빛의 양을 계산합니다.
    lightIntensity = saturate(dot(bumpNormal, lightDir));

    // 확산 색과 광 강도의 양에 따라 최종 확산 색을 결정합니다.
    color = saturate(diffuseColor * lightIntensity);

    // 최종 범프 라이트 색상과 텍스처 색상을 결합합니다.
    color = color * textureColor;
	
    return color;
}
