////////////////////////////////////////////////////////////////////////////////
// Filename: texturearrayclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _TEXTUREARRAYCLASS_H_
#define _TEXTUREARRAYCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dcompiler.h> // (DDSTextureLoader.h가 필요로 할 수 있으므로 추가)
#include "DDSTextureLoader.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: TextureArrayClass
////////////////////////////////////////////////////////////////////////////////
class TextureArrayClass
{
public:
	TextureArrayClass();
	TextureArrayClass(const TextureArrayClass&);
	~TextureArrayClass();

	// (수정) 텍스처 2개를 받도록 Initialize 수정
	bool Initialize(ID3D11Device*, const WCHAR*, const WCHAR*);
	void Shutdown();

	// (수정) 텍스처 배열 포인터를 반환
	ID3D11ShaderResourceView** GetTextureArray();

private:
	// (수정) 텍스처 2개 배열
	ID3D11ShaderResourceView* m_textures[2];
};

#endif