//====================================================================================
//		## PlayerClass ## (플레이어 컨트롤러 - 이동 및 애니메이션 제어)
//====================================================================================
#pragma once
#include "SkinModelClass.h"
#include "AlignedAllocationPolicy.h"
#include "inputclass.h"
#include "GameObject.h"
#include <DirectXMath.h>

using namespace DirectX;

class PlayerClass : public AlignedAllocationPolicy<16>
{
public:
	PlayerClass();
	~PlayerClass() {}
	
	void Update(float deltaTime, InputClass* input);
	
	// 충돌 검사를 위한 게임 오브젝트 설정
	void SetGameObjects(const std::vector<GameObject>* gameObjects, ModelClass* groundModel);
	
	SkinModelClass* skinModel;

	XMVECTOR GetLookAt() { return _lookAtVec; }
	XMFLOAT3 GetPositionF() { return _position; }
	XMVECTOR GetPosition();
	XMVECTOR GetRotation();

private:
	//이동, 회전 속도
	float _speed, _mouseSpeed;

	//위치, 회전 값
	XMFLOAT3	_position, _rotation;

	XMVECTOR _lookAtVec;		//direction

	// 충돌 검사용
	const std::vector<GameObject>* m_gameObjects;
	ModelClass* m_groundModel;

	//Set Function
	void SetPosition(float, float, float);
	void SetRotation(float, float, float);

	void MouseInput(InputClass* input);
	void KeyboardInput(float duration, InputClass* input);

	// 충돌 검사 함수들
	bool CheckAABBCollision(XMFLOAT3 min1, XMFLOAT3 max1, XMFLOAT3 min2, XMFLOAT3 max2);
	void GetWorldAABB(const GameObject& obj, XMFLOAT3& outMin, XMFLOAT3& outMax);

	// Animation state management
	int _currentAnimState;
	enum AnimState {
		ANIM_WALK = 0,
		ANIM_WALK_LEFT = 1,
		ANIM_WALK_RIGHT = 2,
		ANIM_IDLE = 3
	};
};
