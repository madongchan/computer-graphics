////////////////////////////////////////////////////////////////////////////////
// Filename: skinshaderclass.h
// Shader class for rendering skinned meshes with bone transformations
////////////////////////////////////////////////////////////////////////////////
#ifndef _SKINSHADERCLASS_H_
#define _SKINSHADERCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <directxmath.h>
#include <d3dcompiler.h>
#include <fstream>
#include <vector>

using namespace std;
using namespace DirectX;


////////////////////////////////////////////////////////////////////////////////
// Class name: SkinShaderClass
////////////////////////////////////////////////////////////////////////////////
class SkinShaderClass
{
private:
	struct MatrixBufferType
	{
		XMMATRIX view;
		XMMATRIX projection;
	};

	struct WorldBufferType
	{
		XMMATRIX world;
	};

	struct BoneBufferType
	{
		XMMATRIX bones[128];
	};

public:
	SkinShaderClass();
	SkinShaderClass(const SkinShaderClass&);
	~SkinShaderClass();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, 
				ID3D11ShaderResourceView*, vector<XMMATRIX>& boneMatrices);

	bool InitializeShader(ID3D11Device*, HWND, const WCHAR*);
private:
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, const WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, 
							 ID3D11ShaderResourceView*, vector<XMMATRIX>& boneMatrices);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_worldBuffer;
	ID3D11Buffer* m_boneBuffer;
	ID3D11SamplerState* m_sampleState;
};

#endif
