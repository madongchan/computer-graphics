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

void CameraClass::UpdateCamera()
{
    XMMATRIX camRotationMatrix = XMMatrixRotationRollPitchYaw(camPitch, camYaw, 0);
    camTarget = XMVector3TransformCoord(DefaultForward, camRotationMatrix);
    camTarget = XMVector3Normalize(camTarget);

    XMMATRIX RotateYTempMatrix = XMMatrixRotationY(camYaw);
    camRight = XMVector3TransformCoord(DefaultRight, RotateYTempMatrix);
    camUp = XMVector3TransformCoord(camUp, RotateYTempMatrix);
    camForward = XMVector3TransformCoord(DefaultForward, RotateYTempMatrix);

    camPosition += moveLeftRight * camRight;
    camPosition += moveBackForward * camForward;
    moveLeftRight = 0.0f;
    moveBackForward = 0.0f;

    camTarget = camPosition + camTarget;
    camView = XMMatrixLookAtLH(camPosition, camTarget, camUp);

    XMStoreFloat3(&m_position, camPosition);
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
