#include "cameraclass.h"

CameraClass::CameraClass()
{
    m_position = { 0.0f, 0.0f, 0.0f };
    m_rotation = { 0.0f, 0.0f, 0.0f };

    camPosition = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    camTarget = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
    camUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    camRight = DefaultRight;
    camForward = DefaultForward;

    camYaw = 0.0f;
    camPitch = 0.0f;
    moveLeftRight = 0.0f;
    moveBackForward = 0.0f;
    camView = XMMatrixIdentity();

    // 점프 및 중력 관련 초기화
    m_verticalVelocity = 0.0f;
    m_gravity = -19.8f;           // 중력 가속도 (음수값)
    m_jumpForce = 13.8f;           // 점프 초기 속도
    m_groundLevel = 5.0f;         // 기본 지면 높이
    m_isGrounded = true;
    m_canJump = true;
}

CameraClass::CameraClass(const CameraClass& other) {}
CameraClass::~CameraClass() {}

void CameraClass::SetPosition(float x, float y, float z)
{
    m_position = { x, y, z };
    camPosition = XMVectorSet(x, y, z, 0.0f);
}

void CameraClass::SetRotation(float x, float y, float z)
{
    m_rotation = { x, y, z };
    camPitch = x * 0.0174532925f;
    camYaw = y * 0.0174532925f;
}

XMFLOAT3 CameraClass::GetPosition() { return m_position; }
XMFLOAT3 CameraClass::GetRotation() { return m_rotation; }
XMVECTOR CameraClass::GetPositionXM() const
{
    return camPosition; // 카메라의 현재 위치 (XMVECTOR)
}
void CameraClass::MoveForward(float speed) { moveBackForward += speed; }
void CameraClass::MoveRight(float speed) { moveLeftRight += speed; }
void CameraClass::AdjustYaw(float amount) { camYaw += amount; }
void CameraClass::AdjustPitch(float amount) { camPitch += amount; }

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

void CameraClass::UpdateCamera()
{
    // 기존 회전 및 이동 처리...
    XMMATRIX camRotationMatrix = XMMatrixRotationRollPitchYaw(camPitch, camYaw, 0);
    camTarget = XMVector3TransformCoord(DefaultForward, camRotationMatrix);
    camTarget = XMVector3Normalize(camTarget);

    XMMATRIX RotateYTempMatrix = XMMatrixRotationY(camYaw);
    camRight = XMVector3TransformCoord(DefaultRight, RotateYTempMatrix);
    camUp = XMVector3TransformCoord(camUp, RotateYTempMatrix);
    camForward = XMVector3TransformCoord(DefaultForward, RotateYTempMatrix);

    // 수평 이동만 적용 (Y축은 중력 시스템에서 처리)
    XMVECTOR horizontalMovement = moveLeftRight * camRight + moveBackForward * camForward;
    XMFLOAT3 horizontalMove;
    XMStoreFloat3(&horizontalMove, horizontalMovement);

    m_position.x += horizontalMove.x;
    m_position.z += horizontalMove.z;

    // camPosition 업데이트 (Y값은 중력 시스템에서 관리)
    camPosition = XMVectorSet(m_position.x, m_position.y, m_position.z, 0.0f);

    moveLeftRight = 0.0f;
    moveBackForward = 0.0f;

    camTarget = camPosition + camTarget;
    camView = XMMatrixLookAtLH(camPosition, camTarget, camUp);
}


void CameraClass::Render()
{
    UpdateCamera(); // Ensure camera is updated before rendering
    m_viewMatrix = camView;
}

void CameraClass::GetViewMatrix(XMMATRIX& viewMatrix)
{
    viewMatrix = m_viewMatrix;
}


