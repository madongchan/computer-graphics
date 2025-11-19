////////////////////////////////////////////////////////////////////////////////
// Filename: bumpmapShader.hlsl
////////////////////////////////////////////////////////////////////////////////

/////////////
// GLOBALS //
/////////////
Texture2D shaderTextures[2] : register(t0); // 0: Color, 1: Normal
SamplerState SampleType : register(s0);

#define NUM_POINT_LIGHTS 3

/////////////////////
// CONSTANT BUFFERS //
/////////////////////
cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

// 조명 파라미터 (기본 조명)
cbuffer LightBuffer : register(b1)
{
    float4 ambientColor;
    float4 diffuseColor;
    float3 lightDirection;
    float specularPower;
    float4 specularColor;
};

// 카메라 위치 (Specular 계산용)
cbuffer CameraBuffer : register(b2)
{
    float3 cameraPosition;
    float padding;
};

// 포인트 라이트 (다중 조명)
cbuffer PointLightBuffer : register(b3)
{
    float4 pointLightPosition[NUM_POINT_LIGHTS];
    float4 pointLightColor[NUM_POINT_LIGHTS];
    float pointLightIntensity;
    float3 pointLightPadding;
};

// ★ 통합 토글 버퍼 (조명 & 노멀맵 제어)
cbuffer ToggleBuffer : register(b4)
{
    float isAmbientOn; // 5번 키
    float isDiffuseOn; // 6번 키
    float isSpecularOn; // 7번 키
    float isNormalMapOn; // 0번 키
};

//////////////
// TYPEDEFS //
//////////////
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
    float4 worldPosition : TEXCOORD2;
};

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType BumpMapVertexShader(VertexInputType input)
{
    PixelInputType output;
    float4 worldPosition;

    input.position.w = 1.0f;

    // 행렬 변환
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    output.tex = input.tex;

    // 법선, 접선, 종선 변환 (월드 공간)
    output.normal = mul(input.normal, (float3x3) worldMatrix);
    output.normal = normalize(output.normal);

    output.tangent = mul(input.tangent, (float3x3) worldMatrix);
    output.tangent = normalize(output.tangent);

    output.binormal = mul(input.binormal, (float3x3) worldMatrix);
    output.binormal = normalize(output.binormal);

    // 픽셀 쉐이더 조명 계산용 월드 위치
    worldPosition = mul(input.position, worldMatrix);
    output.worldPosition = worldPosition;

    // 카메라 방향 계산 (Specular용)
    output.viewDirection = cameraPosition.xyz - worldPosition.xyz;
    output.viewDirection = normalize(output.viewDirection);

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
    float3 normal;
    float4 specular;
    float3 reflection;
    float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);

    // 1. 텍스처 색상 샘플링
    textureColor = shaderTextures[0].Sample(SampleType, input.tex);

    // 2. 노멀 벡터 결정 (노멀맵 사용 여부 분기)
    if (isNormalMapOn > 0.1f)
    {
        // [노멀맵 ON] 텍스처에서 법선을 가져와 변환
        float4 bumpNormal = shaderTextures[1].Sample(SampleType, input.tex);
        bumpNormal = (bumpNormal * 2.0f) - 1.0f;
        float3x3 TBN = float3x3(normalize(input.tangent), normalize(input.binormal), normalize(input.normal));
        normal = mul(bumpNormal.xyz, TBN);
        normal = normalize(normal);
    }
    else
    {
        // [노멀맵 OFF] 정점의 기본 법선 사용
        normal = normalize(input.normal);
    }

    // 3. 앰비언트 조명
    if (isAmbientOn > 0.1f)
    {
        ambient = ambientColor;
    }

    // 4. 디퓨즈(Diffuse) 조명 (Directional)
    lightDir = -lightDirection;
    lightIntensity = saturate(dot(normal, lightDir));

    specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

    if (lightIntensity > 0.0f)
    {
        if (isDiffuseOn > 0.1f)
        {
            diffuse += (diffuseColor * lightIntensity);
        }

        // 5. 스펙큘러(Specular) 조명
        if (isSpecularOn > 0.1f)
        {
            reflection = normalize(2 * lightIntensity * normal - lightDir);
            specular = pow(saturate(dot(reflection, input.viewDirection)), specularPower) * specularColor;
        }
    }

    // 6. 포인트 라이트 계산 (디퓨즈가 켜져있을 때만 추가)
    if (isDiffuseOn > 0.1f)
    {
        for (int i = 0; i < NUM_POINT_LIGHTS; i++)
        {
            float3 pointLightVector = pointLightPosition[i].xyz - input.worldPosition.xyz;
            float distSquared = dot(pointLightVector, pointLightVector);
            pointLightVector = normalize(pointLightVector);

            float pointLightFactor = saturate(dot(normal, pointLightVector));
            float attenuation = 1.0f / (distSquared + 1.0f);

            // 포인트 라이트 더하기 (색상 * 내적값 * 감쇠 * 전체강도)
            diffuse += (pointLightColor[i] * pointLightFactor * attenuation * pointLightIntensity);
        }
    }

    // 7. 최종 색상 합성
    // (Ambient + Diffuse) * Texture + Specular
    color = saturate(ambient + diffuse) * textureColor;
    color = saturate(color + specular);

    return color;
}