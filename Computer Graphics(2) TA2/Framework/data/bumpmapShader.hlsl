////////////////////////////////////////////////////////////////////////////////
// Filename: bumpmapShader.hlsl
////////////////////////////////////////////////////////////////////////////////

/////////////
// GLOBALS //
/////////////
Texture2D shaderTextures[2] : register(t0); // t0: Color, t1: Normal
SamplerState SampleType : register(s0);


//////////////
// TYPEDEFS //
//////////////
cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

cbuffer LightBuffer : register(b0)
{
    float4 ambientColor;
    float4 diffuseColor;
    float3 lightDirection;
    float padding;
};

// (수정) VS_INPUT 구조체 확장 (Tangent, Binormal 추가)
struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
};

// 노멀맵  토글용 상수 버퍼
cbuffer ToggleBuffer : register(b1)
{
    float isNormalMapOn; // 1.0f = true, 0.0f = false
    float3 nomalpadding;
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


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType BumpMapVertexShader(VertexInputType input)
{
    PixelInputType output;
    matrix worldViewProjection;

	// 픽셀 셰이더에서 TBN 계산을 위해 월드 변환만 적용
    output.position = mul(input.position, worldMatrix);
	
	// 카메라 위치 계산 (월드 좌표계 기준)
    float3 cameraPosition = float3(0.0f, 0.0f, 0.0f); // (참고: 이 셰이더는 카메라 위치를 안 받음)
    output.viewDirection = cameraPosition - output.position.xyz;
    output.viewDirection = normalize(output.viewDirection);

	// WVP 변환
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    output.tex = input.tex;
    
	// TBN 벡터를 월드 좌표계로 변환
    output.normal = mul(input.normal, (float3x3) worldMatrix);
    output.normal = normalize(output.normal);

    output.tangent = mul(input.tangent, (float3x3) worldMatrix);
    output.tangent = normalize(output.tangent);

    output.binormal = mul(input.binormal, (float3x3) worldMatrix);
    output.binormal = normalize(output.binormal);

    return output;
}


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 BumpMapPixelShader(PixelInputType input) : SV_TARGET
{
    float4 textureColor;
    float3 lightDir;
    float lightIntensity;
    float4 color;
    float3 normal; // (최종 법선 벡터)

    // 1. 텍스처 맵 (t0)에서 색상 샘플링 (공통)
    textureColor = shaderTextures[0].Sample(SampleType, input.tex);

    // 0번 키 토글(isNormalMapOn)에 따라 법선 결정
    if (isNormalMapOn > 0.0f)
    {
        // --- A. 노멀맵 사용 (ON) ---
        float4 bumpNormal;
        
        // 2. 노멀 맵 (t1)에서 샘플링
        bumpNormal = shaderTextures[1].Sample(SampleType, input.tex);

        // 3. 노멀 맵 값 (0 ~ 1)을 법선 벡터 (-1 ~ +1)로 변환
        bumpNormal = (bumpNormal * 2.0f) - 1.0f;

        // 4. TBN(Tangent-Binormal-Normal) 행렬 생성
        float3x3 TBN = float3x3(normalize(input.tangent), normalize(input.binormal), normalize(input.normal));
    
        // 5. 노멀 맵에서 읽은 법선(접선 공간)을 월드 공간 법선으로 변환
        normal = mul(bumpNormal.xyz, TBN);
        normal = normalize(normal);
    }
    else
    {
        // --- B. 노멀맵 미사용 (OFF) ---
        // 5. 정점에서 보간된 기본 법선을 그대로 사용
        normal = normalize(input.normal);
    }
    // ---

    // 6. 조명 계산 (공통)
    lightDir = -lightDirection;
    lightIntensity = saturate(dot(normal, lightDir));

    color = saturate(ambientColor + (diffuseColor * lightIntensity));
    color = color * textureColor;

    return color;
}