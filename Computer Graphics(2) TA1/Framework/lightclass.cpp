////////////////////////////////////////////////////////////////////////////////
// Filename: lightclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "lightclass.h"


LightClass::LightClass()
{
	// ★ (수정 1) 생성자에서 포인트 라이트 배열 초기화 ★
	for (int i = 0; i < NUM_POINT_LIGHTS; ++i)
	{
		m_pointLightColor[i] = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f); // 기본값: 검은색
		m_pointLightPosition[i] = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f); // 기본값: 원점
	}
}


LightClass::LightClass(const LightClass& other)
{
}


LightClass::~LightClass()
{
}
// --- ★ (수정 2) 포인트 라이트 Set/Get 함수 구현 추가 ★ ---

void LightClass::SetPointLightColor(int index, float red, float green, float blue, float alpha)
{
	if (index >= 0 && index < NUM_POINT_LIGHTS)
	{
		m_pointLightColor[index] = XMFLOAT4(red, green, blue, alpha);
	}
	return;
}

void LightClass::SetPointLightPosition(int index, float x, float y, float z)
{
	if (index >= 0 && index < NUM_POINT_LIGHTS)
	{
		// W 컴포넌트는 1.0f로 설정 (HLSL float4와 맞추기 위함)
		m_pointLightPosition[index] = XMFLOAT4(x, y, z, 1.0f);
	}
	return;
}

XMFLOAT4 LightClass::GetPointLightColor(int index)
{
	if (index >= 0 && index < NUM_POINT_LIGHTS)
	{
		return m_pointLightColor[index];
	}
	// 인덱스 범위를 벗어나면 검은색 반환 (안전 처리)
	return XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
}

XMFLOAT4 LightClass::GetPointLightPosition(int index)
{
	if (index >= 0 && index < NUM_POINT_LIGHTS)
	{
		return m_pointLightPosition[index];
	}
	// 인덱스 범위를 벗어나면 원점 반환 (안전 처리)
	return XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
}

void LightClass::SetAmbientColor(float red, float green, float blue, float alpha)
{
	m_ambientColor = XMFLOAT4(red, green, blue, alpha);
	return;
}

void LightClass::SetDiffuseColor(float red, float green, float blue, float alpha)
{
	m_diffuseColor = XMFLOAT4(red, green, blue, alpha);
	return;
}

void LightClass::SetDirection(float x, float y, float z)
{
	m_direction = XMFLOAT3(x, y, z);
	return;
}

void LightClass::SetSpecularColor(float red, float green, float blue, float alpha)
{
	m_specularColor = XMFLOAT4(red, green, blue, alpha);
	return;
}

void LightClass::SetSpecularPower(float power)
{
	m_specularPower = power;
	return;
}

XMFLOAT4 LightClass::GetAmbientColor()
{
	return m_ambientColor;
}

XMFLOAT4 LightClass::GetDiffuseColor()
{
	return m_diffuseColor;
}

XMFLOAT3 LightClass::GetDirection()
{
	return m_direction;
}

XMFLOAT4 LightClass::GetSpecularColor()
{
	return m_specularColor;
}

float LightClass::GetSpecularPower()
{
	return m_specularPower;
}