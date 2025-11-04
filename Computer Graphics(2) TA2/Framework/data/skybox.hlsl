//--------------------------------------------------------------------------------------
// 전역 변수
//--------------------------------------------------------------------------------------

// LightShaderClass의 MatrixBufferType과 동일한 구조 (b0)
cbuffer MatrixBuffer : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
};

// 큐브맵 텍스처 (t0)와 샘플러 (s0)
TextureCube SkyMap : register(t0);
SamplerState ObjSamplerState : register(s0);

//--------------------------------------------------------------------------------------
// 입출력 구조체
//--------------------------------------------------------------------------------------

// VS 입력 (ModelClass의 VertexType과 일치)
struct VS_INPUT
{
    float4 Pos : POSITION;
    float2 Tex : TEXCOORD;
    float3 Normal : NORMAL;
};

// VS -> PS 출력
struct SKYMAP_VS_OUTPUT
{
    float4 Pos : SV_POSITION;   // 화면 좌표
    float3 texCoord : TEXCOORD; // 3D 텍스처 좌표 (로컬 위치)
};


//--------------------------------------------------------------------------------------
// 스카이박스 정점 셰이더 (SKYMAP_VS)
//
//--------------------------------------------------------------------------------------
SKYMAP_VS_OUTPUT SKYMAP_VS(VS_INPUT input)
{
    SKYMAP_VS_OUTPUT output = (SKYMAP_VS_OUTPUT)0;

    // 1. 3D 텍스처 좌표로 정점의 로컬 위치(inPos)를 그대로 사용합니다.
    output.texCoord = input.Pos.xyz;

    // 2. WVP 변환을 수행합니다. (World는 카메라 위치, View/Proj는 카메라 기준)
    matrix WVP = mul(World, View);
    WVP = mul(WVP, Projection);

    output.Pos = mul(input.Pos, WVP);

    // 3. Z값을 W와 같게(z=1.0) 만들어 항상 가장 뒤에 그려지도록 합니다.
    output.Pos = output.Pos.xyww;

    return output;
}


//--------------------------------------------------------------------------------------
// 스카이박스 픽셀 셰이더 (SKYMAP_PS)
//
//--------------------------------------------------------------------------------------
float4 SKYMAP_PS(SKYMAP_VS_OUTPUT input) : SV_Target
{
    // 3D 텍스처 좌표로 큐브맵을 샘플링합니다.
    // (RGBA 8-bit 텍스처를 사용하므로 톤 매핑이 필요 없습니다)
    return SkyMap.Sample(ObjSamplerState, input.texCoord);
}