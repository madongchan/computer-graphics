// GameObject.h - GameObject 구조체 정의
#pragma once
#include "modelclass.h"
#include <DirectXMath.h>

using namespace DirectX;

// GameObject 구조체 정의
struct GameObject
{
	ModelClass* model;  // 모델 원본 포인터
	XMFLOAT3 position;  // 위치
	XMFLOAT3 rotation;  // 회전
	XMFLOAT3 scale;     // 크기
};
