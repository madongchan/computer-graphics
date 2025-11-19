////////////////////////////////////////////////////////////////////////////////
// Filename: lightclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _LIGHTCLASS_H_
#define _LIGHTCLASS_H_

//////////////
// INCLUDES //
//////////////
#include <directxmath.h>

using namespace DirectX;

// --- ★ (수정 1) 포인트 라이트 개수 정의 ★ ---
#define NUM_POINT_LIGHTS 3 // 과제 3.1 요구사항

////////////////////////////////////////////////////////////////////////////////
// Class name: LightClass
////////////////////////////////////////////////////////////////////////////////
class LightClass
{
public:
	LightClass();
	LightClass(const LightClass&);
	~LightClass();

	// --- 방향성 조명 Set/Get 함수들 (기존) ---
	void SetAmbientColor(float, float, float, float);
	void SetDiffuseColor(float, float, float, float);
	void SetDirection(float, float, float);
	void SetSpecularColor(float, float, float, float);
	void SetSpecularPower(float);

	XMFLOAT4 GetAmbientColor();
	XMFLOAT4 GetDiffuseColor();
	XMFLOAT3 GetDirection();
	XMFLOAT4 GetSpecularColor();
	float GetSpecularPower();

	// --- ★ (수정 2) 포인트 라이트 Set/Get 함수 선언 추가 ★ ---
	// (인덱스를 받아 해당 포인트 라이트의 값을 설정/반환)
	void SetPointLightColor(int index, float r, float g, float b, float a);
	void SetPointLightPosition(int index, float x, float y, float z);

	XMFLOAT4 GetPointLightColor(int index);
	XMFLOAT4 GetPointLightPosition(int index); // (X,Y,Z만 사용하지만 float4가 HLSL 정렬에 유리)

private:
	// --- 방향성 조명 멤버 변수들 (기존) ---
	XMFLOAT4 m_ambientColor;
	XMFLOAT4 m_diffuseColor;
	XMFLOAT3 m_direction; // (주의: m_lightDirection 아님)
	XMFLOAT4 m_specularColor;
	float m_specularPower;

	// --- ★ (수정 3) 포인트 라이트 멤버 변수 추가 ★ ---
	XMFLOAT4 m_pointLightColor[NUM_POINT_LIGHTS];
	XMFLOAT4 m_pointLightPosition[NUM_POINT_LIGHTS]; // (X,Y,Z만 사용)
};

#endif