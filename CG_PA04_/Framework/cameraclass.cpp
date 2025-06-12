#include "cameraclass.h"

CameraClass::CameraClass()
{
    m_position = XMFLOAT3(0.0f, 0.0f, 0.0f);
    m_rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
    camYaw = 0.0f;
    camPitch = 0.0f;
}

CameraClass::~CameraClass() {}

void CameraClass::SetPosition(float x, float y, float z)
{
    m_position = XMFLOAT3(x, y, z);
}

void CameraClass::SetRotation(float x, float y, float z)
{
    m_rotation = XMFLOAT3(x, y, z);
}

XMFLOAT3 CameraClass::GetPosition() { return m_position; }
XMFLOAT3 CameraClass::GetRotation() { return m_rotation; }

void CameraClass::SetYawPitch(float yaw, float pitch)
{
    camYaw = yaw;
    camPitch = pitch;
}

void CameraClass::Move(float leftRight, float forward)
{
    XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(camPitch, camYaw, 0);

    XMVECTOR forwardDir = XMVector3TransformCoord(XMVectorSet(0, 0, 1, 0), rotationMatrix);
    XMVECTOR rightDir = XMVector3TransformCoord(XMVectorSet(1, 0, 0, 0), rotationMatrix);

    XMVECTOR position = XMLoadFloat3(&m_position);
    position += leftRight * rightDir;
    position += forward * forwardDir;

    XMStoreFloat3(&m_position, position);
}

void CameraClass::Jump()
{
    if (!m_isJumping)
    {
        m_isJumping = true;
        m_jumpVelocity = JUMP_POWER;
    }
}

void CameraClass::UpdateJump(float deltaTime)
{
    if (m_isJumping)
    {
        // 속도 단위: units/sec → 프레임 보정
        m_position.y += m_jumpVelocity * deltaTime;
        m_jumpVelocity += GRAVITY * deltaTime;

        if (m_position.y <= 0.0f)
        {
            m_position.y = 0.0f;
            m_isJumping = false;
            m_jumpVelocity = 0.0f;
        }
    }
}


bool CameraClass::IsJumping()
{
    return m_isJumping;
}


XMVECTOR CameraClass::GetLookVector()
{
    XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(camPitch, camYaw, 0);
    return XMVector3TransformCoord(XMVectorSet(0, 0, 1, 0), rotationMatrix);
}

XMVECTOR CameraClass::GetRightVector()
{
    XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(camPitch, camYaw, 0);
    return XMVector3TransformCoord(XMVectorSet(1, 0, 0, 0), rotationMatrix);
}

XMVECTOR CameraClass::GetUpVector()
{
    XMVECTOR look = GetLookVector();
    XMVECTOR right = GetRightVector();
    return XMVector3Cross(look, right);
}


void CameraClass::Render()
{
    XMVECTOR up = XMVectorSet(0, 1, 0, 0);
    XMVECTOR position = XMLoadFloat3(&m_position);
    XMVECTOR lookAt = XMVectorSet(0, 0, 1, 0);

    XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(camPitch, camYaw, 0);
    lookAt = XMVector3TransformCoord(lookAt, rotationMatrix);
    up = XMVector3TransformCoord(up, rotationMatrix);
    lookAt = position + lookAt;

    m_viewMatrix = XMMatrixLookAtLH(position, lookAt, up);
}

void CameraClass::GetViewMatrix(XMMATRIX& viewMatrix)
{
    viewMatrix = m_viewMatrix;
}
