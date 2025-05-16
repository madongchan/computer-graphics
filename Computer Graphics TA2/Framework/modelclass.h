////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <directxmath.h>
#include <cmath> // sin, cos �Լ� ���
#include <vector>

using namespace DirectX;
using namespace std;


enum class ShapeType
{
	None,
	Pantagon,
	Fan,
	House
};
////////////////////////////////////////////////////////////////////////////////
// Class name: ModelClass
////////////////////////////////////////////////////////////////////////////////
class ModelClass
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
	    XMFLOAT4 color;
	};

public:
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();

	bool Initialize(ID3D11Device*, ShapeType shapetype);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();

	void UpdateRotation(float deltaTime); // ȸ�� ���� ������Ʈ �Լ�
	XMMATRIX GetWorldMatrix(const XMMATRIX& baseWorldMatrix); // ȸ�� ����� World Matrix ��ȯ �Լ�
	ShapeType GetShapeType() { return m_shapeType; } // ShapeType ��ȯ �Լ� (�ʿ��)

private:
	bool InitializeBuffers(ID3D11Device*);

	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	

private:
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;
	ShapeType m_shapeType;
	XMFLOAT3 m_rotation;
};

#endif