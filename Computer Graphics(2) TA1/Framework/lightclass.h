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

// --- �� (���� 1) ����Ʈ ����Ʈ ���� ���� �� ---
#define NUM_POINT_LIGHTS 3 // ���� 3.1 �䱸����

////////////////////////////////////////////////////////////////////////////////
// Class name: LightClass
////////////////////////////////////////////////////////////////////////////////
class LightClass
{
public:
	LightClass();
	LightClass(const LightClass&);
	~LightClass();

	// --- ���⼺ ���� Set/Get �Լ��� (����) ---
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

	// --- �� (���� 2) ����Ʈ ����Ʈ Set/Get �Լ� ���� �߰� �� ---
	// (�ε����� �޾� �ش� ����Ʈ ����Ʈ�� ���� ����/��ȯ)
	void SetPointLightColor(int index, float r, float g, float b, float a);
	void SetPointLightPosition(int index, float x, float y, float z);

	XMFLOAT4 GetPointLightColor(int index);
	XMFLOAT4 GetPointLightPosition(int index); // (X,Y,Z�� ��������� float4�� HLSL ���Ŀ� ����)

private:
	// --- ���⼺ ���� ��� ������ (����) ---
	XMFLOAT4 m_ambientColor;
	XMFLOAT4 m_diffuseColor;
	XMFLOAT3 m_direction; // (����: m_lightDirection �ƴ�)
	XMFLOAT4 m_specularColor;
	float m_specularPower;

	// --- �� (���� 3) ����Ʈ ����Ʈ ��� ���� �߰� �� ---
	XMFLOAT4 m_pointLightColor[NUM_POINT_LIGHTS];
	XMFLOAT4 m_pointLightPosition[NUM_POINT_LIGHTS]; // (X,Y,Z�� ���)
};

#endif