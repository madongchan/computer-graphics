// GLOBALS //
#define NUM_POINT_LIGHTS 3

cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};
cbuffer CameraBuffer
{
    float3 cameraPosition;
    float padding;
};
cbuffer LightBuffer
{
    float4 ambientColor;
    float4 diffuseColor;
    float3 lightDirection;
    float specularPower;
    float4 specularColor;
};

cbuffer LightToggleBuffer
{
    float isAmbientOn; // 1.0f = On, 0.0f = Off
    float isDiffuseOn;
    float isSpecularOn;
    float togglePadding; // 16바이트 정렬용
};
// Point Lights Buffer
cbuffer PointLightBuffer
{
    float4 pointLightPosition[NUM_POINT_LIGHTS];
    float4 pointLightColor[NUM_POINT_LIGHTS];
    float pointLightIntensity; // 8, 9 키로 조절
    float3 pointLightPadding;
};
Texture2D shaderTexture;
SamplerState SampleType;

// TYPEDEFS //
struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};
struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 viewDirection : TEXCOORD1;
    float4 worldPosition : TEXCOORD2;
};

// Vertex Shader
PixelInputType LightVertexShader(VertexInputType input)
{
    PixelInputType output;
    float4 worldPosition_local;
	
	// Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;
	
	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
	// Store the texture coordinates for the pixel shader.
    output.tex = input.tex;
    
	// Calculate the normal vector against the world matrix only.
    output.normal = mul(input.normal, (float3x3) worldMatrix);
	
    // Normalize the normal vector.
    output.normal = normalize(output.normal);
	
	// Calculate the position of the vertex in the world.
    worldPosition_local = mul(input.position, worldMatrix);
    output.worldPosition = worldPosition_local;

	// Determine the viewing direction based on the position of the camera and the position of the vertex in the world.
    output.viewDirection = cameraPosition.xyz - worldPosition_local.xyz;
	
    // Normalize the viewing direction vector.
    output.viewDirection = normalize(output.viewDirection);
    
    return output;
}

// Pixel Shader
float4 LightPixelShader(PixelInputType input) : SV_TARGET
{
    float4 textureColor;
    float3 lightDir;
    float lightIntensity;
    float4 color;
    float3 reflection;
    float4 specular;
	
	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
    textureColor = shaderTexture.Sample(SampleType, input.tex);
	
	// Initialize output color to black
    color = float4(0.0f, 0.0f, 0.0f, 0.0f);
	
	// Apply ambient light (default to ON if toggle buffer not set)
    if (isAmbientOn > 0.1f)
    {
        color = ambientColor;
    }
	
	// Initialize the specular color.
    specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
	
	// Invert the light direction for calculations.
    lightDir = -lightDirection;
    
	// Calculate the amount of light on this pixel.
    lightIntensity = saturate(dot(input.normal, lightDir));
	
    if (lightIntensity > 0.0f)
    {
		// Apply diffuse light (default to ON if toggle buffer not set)
        if (isDiffuseOn > 0.1f)
        {
			// Determine the final diffuse color based on the diffuse color and the amount of light intensity.
            color += (diffuseColor * lightIntensity);
        }
		
	    // Saturate the color after adding diffuse
        color = saturate(color);
		
		// Apply specular light (default to ON if toggle buffer not set)
        if (isSpecularOn > 0.1f)
        {
			// Calculate the reflection vector based on the light intensity, normal vector, and light direction.
            reflection = normalize(2 * lightIntensity * input.normal - lightDir);
			
			// Determine the amount of specular light based on the reflection vector, viewing direction, and specular power.
            specular = pow(saturate(dot(reflection, input.viewDirection)), specularPower) * specularColor;
        }
    }
	// ========== Point Lights ==========
    // --- ★ 4. 포인트 라이트 계산 추가 ★ ---
    // 디퓨즈가 켜져 있을 때만 포인트 라이트 계산
    if (isDiffuseOn > 0.1f)
    {
        for (int i = 0; i < NUM_POINT_LIGHTS; i++)
        {
            // 픽셀 위치에서 포인트 라이트로 향하는 벡터
            float3 pointLightVector = pointLightPosition[i].xyz - input.worldPosition.xyz;

            // 거리 제곱
            float distSquared = dot(pointLightVector, pointLightVector);

            // 벡터 정규화
            pointLightVector = normalize(pointLightVector);

            // 포인트 라이트 디퓨즈 강도
            float pointLightFactor = saturate(dot(input.normal, pointLightVector));

            // 거리 감쇠
            float attenuation = 1.0f / (distSquared + 1.0f);
            
            // 포인트 라이트 기여도 = 색상 * 강도 * 감쇠 * 전체 강도 조절 (동찬님 구조 사용)
            // 최종 색상(color)에 더합니다.
            color += (pointLightColor[i] * pointLightFactor * attenuation * pointLightIntensity);
        }
        // 포인트 라이트 계산 후 saturate
        color = saturate(color);
    }
    
    // Multiply the texture pixel and the input color to get the textured result.
    color = color * textureColor;
	
	// Add the specular component last to the output color.
    color = saturate(color + specular);
	
    return color;
}