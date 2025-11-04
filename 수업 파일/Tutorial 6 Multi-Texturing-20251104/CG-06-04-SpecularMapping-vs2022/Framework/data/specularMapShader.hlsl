// GLOBALS //
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer CameraBuffer
{
    float3 cameraPosition;
};

cbuffer LightBuffer
{
	float4 diffuseColor;
	float4 specularColor;
	float specularPower;
	float3 lightDirection;
};

Texture2D shaderTextures[3];
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
	float3 viewDirection : TEXCOORD1;
};

// Vertex Shader
PixelInputType SpecMapVertexShader(VertexInputType input)
{
    PixelInputType output;
    float4 worldPosition;

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

	// 세계의 정점 위치를 계산합니다.
    worldPosition = mul(input.position, worldMatrix);

    // 카메라의 위치와 세계의 정점 위치를 기준으로보기 방향을 결정합니다.
    output.viewDirection = cameraPosition.xyz - worldPosition.xyz;
	
    // 뷰 방향 벡터를 표준화합니다.
    output.viewDirection = normalize(output.viewDirection);

    return output;
}

// Pixel Shader
float4 SpecMapPixelShader(PixelInputType input) : SV_TARGET
{
	float4 textureColor;
    float4 bumpMap;
    float3 bumpNormal;
    float3 lightDir;
    float lightIntensity;
    float4 color;
	float4 specularIntensity;
    float3 reflection;
    float4 specular;

    // 이 위치에서 텍스처 픽셀을 샘플링합니다.
    textureColor = shaderTextures[0].Sample(SampleType, input.tex);
	
    // 범프 맵에서 픽셀을 샘플링합니다.
    bumpMap = shaderTextures[1].Sample(SampleType, input.tex);

    // 정상 값의 범위를 (0, +1)에서 (-1, +1)로 확장합니다.
    bumpMap = (bumpMap * 2.0f) - 1.0f;
    
    // 범프 맵의 데이터에서 법선을 계산합니다.
    bumpNormal = input.normal + bumpMap.x * input.tangent + bumpMap.y * input.binormal;
	
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

	if(lightIntensity > 0.0f)
    {
        // specular map texture로부터 픽셀을 샘플링한다.
        specularIntensity = shaderTextures[2].Sample(SampleType, input.tex);

        // 빛의 강도, 법선 벡터 및 빛의 방향에 따라 반사 벡터를 계산합니다.
        reflection = normalize(2 * lightIntensity * bumpNormal - lightDir); 

        // 반사 벡터, 시선 방향 및 반사 출력을 기준으로 반사 조명의 양을 결정합니다.
        specular = pow(saturate(dot(reflection, input.viewDirection)), specularPower);

        // specular map을 사용하여이 픽셀의 반사광의 강도를 결정합니다.
        specular = specular * specularIntensity;
		
        // 출력 색상의 마지막에 반사 컴포넌트를 추가합니다.
        color = saturate(color + specular);
    }	
	
    return color;
}