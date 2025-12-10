#include "PlayerClass.h"

PlayerClass::PlayerClass()
{
	_position = XMFLOAT3(0.f, 1.f, 5.f);  // ✅ 바닥 위!
	_rotation = XMFLOAT3(0.f, 0.f, 0.f);

	_speed = 5.f;
	_mouseSpeed = 10.f;

	skinModel = nullptr;
	_currentAnimState = ANIM_IDLE;
	
	m_gameObjects = nullptr;
	m_groundModel = nullptr;
}

void PlayerClass::SetGameObjects(const std::vector<GameObject>* gameObjects, ModelClass* groundModel)
{
	m_gameObjects = gameObjects;
	m_groundModel = groundModel;
}

void PlayerClass::SetPosition(float x, float y, float z)
{
	//player 객체 위치
	_position.x = x;
	_position.y = y;
	_position.z = z;
	
	//player 메시 위치
	if (skinModel)
		skinModel->SetCenter(_position);
	
	return;
}

void PlayerClass::SetRotation(float x, float y, float z) 
{
	_rotation.x = x;
	_rotation.y = y;
	_rotation.z = z;

	if (skinModel)
		skinModel->SetRotate(_rotation);

	return;
}

XMVECTOR PlayerClass::GetPosition()
{
	return XMLoadFloat3(&_position);
}

XMVECTOR PlayerClass::GetRotation()
{
	return XMLoadFloat3(&_rotation);
}

void PlayerClass::Update(float deltaTime, InputClass* input)
{
	float yaw, pitch, roll;
	XMFLOAT3 up, lookAt;
	XMMATRIX rotationMatrix;

	up = XMFLOAT3(0.f, 1.f, 0.f);
	lookAt = XMFLOAT3(0.f, 0.f, 1.f);

	pitch = _rotation.x * 0.0174532925f;
	yaw = _rotation.y * 0.0174532925f;
	roll = _rotation.z * 0.0174532925f;

	//회전 값으로 카메라 회전 행렬 만듬
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	//카메라 up 벡터, LookAtVector 벡터 회전
	XMVECTOR upVec = XMVector3TransformCoord(XMLoadFloat3(&up), rotationMatrix);
	XMVECTOR lookAtVec = XMVector3TransformCoord(XMLoadFloat3(&lookAt), rotationMatrix);
	_lookAtVec = lookAtVec;

	//LookAtVector에 이동벡터에 대한 값 적용
	lookAtVec = XMLoadFloat3(&_position) + lookAtVec;

	// Handle input and animations
	KeyboardInput(deltaTime, input);
}

void PlayerClass::MouseInput(InputClass* input)
{
	// Mouse input handling can be added here
	// _rotation.x = input->GetPitch();
}

void PlayerClass::KeyboardInput(float duration, InputClass* input)
{
	if (!input || !skinModel)
		return;

	// 현재 위치 저장
	XMFLOAT3 currentPos = _position;

	//위치벡터, 카메라 방향벡터, 업 벡터
	XMVECTOR posVec = XMLoadFloat3(&_position);
	XMVECTOR moveVec = XMVector3Normalize(_lookAtVec);
	XMVECTOR upVec = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	// 이동 벡터 계산
	XMVECTOR movement = XMVectorSet(0, 0, 0, 0);
	bool isMoving = false;
	int newAnimState = ANIM_IDLE;

	// Check for movement input
	if (input->IsWPressed()) { //상 - 앞으로
		movement += moveVec;
		isMoving = true;
		newAnimState = ANIM_WALK;
	} 
	else if (input->IsSPressed()) { //하 - 뒤로
		movement -= moveVec;
		isMoving = true;
		newAnimState = ANIM_WALK;
	}

	if (input->IsAPressed()) { //좌
		movement -= XMVector3Cross(upVec, moveVec);
		isMoving = true;
		newAnimState = ANIM_WALK_LEFT;
	}
	else if (input->IsDPressed()) { //우
		movement += XMVector3Cross(upVec, moveVec);
		isMoving = true;
		newAnimState = ANIM_WALK_RIGHT;
	}

	// 대각선 이동 정규화
	if (XMVectorGetX(XMVector3LengthSq(movement)) > 0.0001f)
	{
		movement = XMVector3Normalize(movement) * duration * _speed;
	}

	XMFLOAT3 moveDelta;
	XMStoreFloat3(&moveDelta, movement);

	// ========== 충돌 검사 (X, Z축 분리 - 슬라이딩) ==========
	float pRadius = 0.5f;
	float pHeight = 2.0f;

	XMFLOAT3 targetPos = currentPos;
	
	// X축 이동 검사
	float testX = currentPos.x + moveDelta.x;
	XMFLOAT3 minX = { testX - pRadius, currentPos.y, currentPos.z - pRadius };
	XMFLOAT3 maxX = { testX + pRadius, currentPos.y + pHeight, currentPos.z + pRadius };

	bool isCollidedX = false;
	if (m_gameObjects)
	{
		for (const auto& obj : *m_gameObjects)
		{
			if (!obj.model || obj.model == m_groundModel) continue;
			
			XMFLOAT3 worldMin, worldMax;
			GetWorldAABB(obj, worldMin, worldMax);
			
			if (CheckAABBCollision(minX, maxX, worldMin, worldMax))
			{
				isCollidedX = true;
				break;
			}
		}
	}
	
	if (!isCollidedX) targetPos.x = testX;

	// Z축 이동 검사
	float testZ = currentPos.z + moveDelta.z;
	XMFLOAT3 minZ = { targetPos.x - pRadius, currentPos.y, testZ - pRadius };
	XMFLOAT3 maxZ = { targetPos.x + pRadius, currentPos.y + pHeight, testZ + pRadius };

	bool isCollidedZ = false;
	if (m_gameObjects)
	{
		for (const auto& obj : *m_gameObjects)
		{
			if (!obj.model || obj.model == m_groundModel) continue;
			
			XMFLOAT3 worldMin, worldMax;
			GetWorldAABB(obj, worldMin, worldMax);
			
			if (CheckAABBCollision(minZ, maxZ, worldMin, worldMax))
			{
				isCollidedZ = true;
				break;
			}
		}
	}
	
	if (!isCollidedZ) targetPos.z = testZ;

	// 최종 위치 업데이트
	_position = targetPos;
	if (skinModel)
		skinModel->SetCenter(_position);

	// Update animation state
	if (newAnimState != _currentAnimState)
	{
		_currentAnimState = newAnimState;
		if (skinModel && skinModel->GetAnimationList().size() > _currentAnimState)
		{
			skinModel->PlayAni(_currentAnimState);
		}
	}
}

bool PlayerClass::CheckAABBCollision(XMFLOAT3 min1, XMFLOAT3 max1, XMFLOAT3 min2, XMFLOAT3 max2)
{
	return (min1.x <= max2.x && max1.x >= min2.x) &&
		   (min1.y <= max2.y && max1.y >= min2.y) &&
		   (min1.z <= max2.z && max1.z >= min2.z);
}

void PlayerClass::GetWorldAABB(const GameObject& obj, XMFLOAT3& outMin, XMFLOAT3& outMax)
{
	// 모델의 로컬 AABB 가져오기
	XMFLOAT3 localMin, localMax;
	obj.model->GetMinMax(localMin, localMax);

	// 월드 변환 행렬 구성
	XMMATRIX worldMatrix = XMMatrixIdentity();
	worldMatrix *= XMMatrixScaling(obj.scale.x, obj.scale.y, obj.scale.z);
	worldMatrix *= XMMatrixRotationRollPitchYaw(obj.rotation.x, obj.rotation.y, obj.rotation.z);
	worldMatrix *= XMMatrixTranslation(obj.position.x, obj.position.y, obj.position.z);

	// 8개 코너 변환
	XMVECTOR corners[8] = {
		XMVectorSet(localMin.x, localMin.y, localMin.z, 1.0f),
		XMVectorSet(localMin.x, localMin.y, localMax.z, 1.0f),
		XMVectorSet(localMin.x, localMax.y, localMin.z, 1.0f),
		XMVectorSet(localMin.x, localMax.y, localMax.z, 1.0f),
		XMVectorSet(localMax.x, localMin.y, localMin.z, 1.0f),
		XMVectorSet(localMax.x, localMin.y, localMax.z, 1.0f),
		XMVectorSet(localMax.x, localMax.y, localMin.z, 1.0f),
		XMVectorSet(localMax.x, localMax.y, localMax.z, 1.0f)
	};

	// 변환된 코너들의 AABB 계산
	XMFLOAT3 transformedMin = XMFLOAT3(FLT_MAX, FLT_MAX, FLT_MAX);
	XMFLOAT3 transformedMax = XMFLOAT3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	for (int i = 0; i < 8; i++)
	{
		XMVECTOR transformed = XMVector3TransformCoord(corners[i], worldMatrix);
		XMFLOAT3 point;
		XMStoreFloat3(&point, transformed);

		transformedMin.x = min(transformedMin.x, point.x);
		transformedMin.y = min(transformedMin.y, point.y);
		transformedMin.z = min(transformedMin.z, point.z);

		transformedMax.x = max(transformedMax.x, point.x);
		transformedMax.y = max(transformedMax.y, point.y);
		transformedMax.z = max(transformedMax.z, point.z);
	}

	outMin = transformedMin;
	outMax = transformedMax;
}
