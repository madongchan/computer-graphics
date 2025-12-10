#include "cameraclass.h"

CameraClass::CameraClass()
{
    m_position = XMFLOAT3(0.0f, 0.0f, 0.0f);
    m_rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);

    // 초기 벡터 설정
    camPosition = XMVectorSet(0.0f, 5.0f, -10.0f, 0.0f);
    camTarget = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    camUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    camYaw = 0.0f;
    camPitch = 0.0f;

    // TPS 기본값
    m_followDist = 6.0f;
    m_targetOffset = 2.0f; // 플레이어 발바닥 기준 +2.0f 높이 (머리)
    m_smoothness = 10.0f;

    // 물리 초기화 (기존 유지)
    m_gravity = -19.8f;
    m_jumpForce = 13.8f;
    m_groundLevel = 0.0f;
    m_isGrounded = true;
    m_canJump = true;
    m_verticalVelocity = 0.0f;

    UpdateViewMatrix();
}

CameraClass::CameraClass(const CameraClass& other) {}
CameraClass::~CameraClass() {}

void CameraClass::SetPosition(float x, float y, float z)
{
    m_position = XMFLOAT3(x, y, z);
    camPosition = XMLoadFloat3(&m_position);
}

void CameraClass::SetRotation(float x, float y, float z)
{
    m_rotation = XMFLOAT3(x, y, z);
    // 라디안 변환
    camPitch = x * 0.0174532925f;
    camYaw = y * 0.0174532925f;
}

XMFLOAT3 CameraClass::GetPosition() { return m_position; }
XMFLOAT3 CameraClass::GetRotation() { return m_rotation; }
XMVECTOR CameraClass::GetPositionXM() const { return camPosition; }

void CameraClass::MoveForward(float speed) { moveBackForward += speed; }
void CameraClass::MoveRight(float speed) { moveLeftRight += speed; }
void CameraClass::AdjustYaw(float amount) { camYaw += amount; }
void CameraClass::AdjustPitch(float amount)
{
    camPitch += amount;

    // 상하 각도 제한 (라디안 단위)
    // 약 -85도 ~ +85도 정도로 제한하여 카메라가 뒤집히는 것을 방지
    float limit = XM_PIDIV2 * 0.9f; // 90도의 90% 정도까지만 허용

    if (camPitch > limit) camPitch = limit;
    if (camPitch < -limit) camPitch = -limit;
}
// TPS 파라미터 설정
void CameraClass::SetFollowParameters(float dist, float height, float offset, float smooth)
{
    m_followDist = dist;
    // height 인자는 m_targetOffset으로 대체되어 사용됩니다. (머리 높이)
    m_targetOffset = offset;
    m_smoothness = smooth;
}

// [핵심 수정] TPS 카메라 업데이트 로직
void CameraClass::UpdateFollowCamera(const XMFLOAT3& targetPos)
{
    // 1. 타겟의 피벗 포인트 설정 (플레이어 발바닥 + 오프셋 = 머리 위치)
    XMVECTOR playerPos = XMLoadFloat3(&targetPos);
    XMVECTOR focusPoint = XMVectorAdd(playerPos, XMVectorSet(0.0f, m_targetOffset, 0.0f, 0.0f));

    // 2. 회전 행렬 생성 (마우스 입력으로 제어되는 Pitch, Yaw 사용)
    // main.cpp 처럼 카메라가 타겟 주위를 공전(Orbit)하게 만듭니다.
    XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(camPitch, camYaw, 0.0f);

    // 3. 카메라 위치 계산 (타겟에서 뒤로 m_followDist 만큼 떨어진 벡터를 회전)
    // (0, 0, -dist) 벡터를 회전시켜 타겟 뒤쪽 방향을 결정합니다.
    XMVECTOR lookBackVector = XMVectorSet(0.0f, 0.0f, -m_followDist, 0.0f);
    XMVECTOR rotatedOffset = XMVector3TransformCoord(lookBackVector, rotationMatrix);

    // 4. 최종 카메라 위치 및 타겟 설정
    camPosition = XMVectorAdd(focusPoint, rotatedOffset);
    camTarget = focusPoint; // 카메라는 항상 플레이어의 머리를 바라봄

    // 멤버 변수 동기화
    XMStoreFloat3(&m_position, camPosition);

    // 5. 뷰 행렬 갱신
    UpdateViewMatrix();
}

void CameraClass::UpdateViewMatrix()
{
    // 업 벡터는 항상 Y축 (0,1,0)으로 고정하여 카메라가 기울어지는 것을 방지
    camUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    // LookAtLH 함수를 사용하여 뷰 행렬 생성 (CG-05 26p 참조)
    m_viewMatrix = XMMatrixLookAtLH(camPosition, camTarget, camUp);

    // View 행렬에서 Right, Forward 벡터 추출 (빌보드나 이동 계산용)
    // View Matrix는 World->View 변환이므로 역행렬(또는 전치행렬)의 행/열을 가져와야 함
    // 여기서는 간단히 타겟-위치 벡터로 계산
    camForward = XMVector3Normalize(XMVectorSubtract(camTarget, camPosition));
    camRight = XMVector3Normalize(XMVector3Cross(camUp, camForward));
}

void CameraClass::Render()
{
    // 이미 UpdateFollowCamera에서 갱신하므로 여기서는 뷰 행렬만 다시 계산하거나 생략 가능
    // 만약 Free Camera 모드라면 여기서 계산해야 하지만, TPS 모드에서는 UpdateFollowCamera가 주도함
    UpdateViewMatrix();
}
void CameraClass::GetViewMatrix(XMMATRIX& viewMatrix)
{
    viewMatrix = m_viewMatrix;
}
void CameraClass::Jump()
{
    // 지면에 있고 점프 가능한 상태에서만 점프 허용
    if (m_isGrounded && m_canJump)
    {
        m_verticalVelocity = m_jumpForce;  // 상향 초기 속도 설정
        m_isGrounded = false;              // 공중 상태로 변경
        m_canJump = false;                 // 연속 점프 방지
    }
}

void CameraClass::ApplyGravity(float deltaTime)
{
    // 중력 가속도를 속도에 적용
    m_verticalVelocity += m_gravity * deltaTime;

    // 속도를 위치에 적용
    m_position.y += m_verticalVelocity * deltaTime;
    camPosition = XMVectorSet(m_position.x, m_position.y, m_position.z, 0.0f);

    // 지면 충돌 검사
    CheckGroundCollision();
}

void CameraClass::CheckGroundCollision()
{
    // 카메라가 지면 아래로 떨어지는 것을 방지
    if (m_position.y <= m_groundLevel)
    {
        m_position.y = m_groundLevel;      // 지면 높이로 위치 고정
        m_verticalVelocity = 0.0f;         // 수직 속도 초기화
        m_isGrounded = true;               // 지면 접촉 상태로 변경
        m_canJump = true;                  // 점프 가능 상태로 복구

        // camPosition 벡터도 업데이트
        camPosition = XMVectorSet(m_position.x, m_position.y, m_position.z, 0.0f);
    }
}

