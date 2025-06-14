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

    // ���� �� �߷� ���� �ʱ�ȭ
    m_verticalVelocity = 0.0f;
    m_gravity = -19.8f;           // �߷� ���ӵ� (������)
    m_jumpForce = 30.8f;           // ���� �ʱ� �ӵ�
    m_groundLevel = 5.0f;         // �⺻ ���� ����
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
    return camPosition; // ī�޶��� ���� ��ġ (XMVECTOR)
}
void CameraClass::MoveForward(float speed) { moveBackForward += speed; }
void CameraClass::MoveRight(float speed) { moveLeftRight += speed; }
void CameraClass::AdjustYaw(float amount) { camYaw += amount; }
void CameraClass::AdjustPitch(float amount) { camPitch += amount; }

void CameraClass::Jump()
{
    // ���鿡 �ְ� ���� ������ ���¿����� ���� ���
    if (m_isGrounded && m_canJump)
    {
        m_verticalVelocity = m_jumpForce;  // ���� �ʱ� �ӵ� ����
        m_isGrounded = false;              // ���� ���·� ����
        m_canJump = false;                 // ���� ���� ����
    }
}

void CameraClass::ApplyGravity(float deltaTime)
{
    // �߷� ���ӵ��� �ӵ��� ����
    m_verticalVelocity += m_gravity * deltaTime;

    // �ӵ��� ��ġ�� ����
    m_position.y += m_verticalVelocity * deltaTime;
    camPosition = XMVectorSet(m_position.x, m_position.y, m_position.z, 0.0f);

    // ���� �浹 �˻�
    CheckGroundCollision();
}

void CameraClass::CheckGroundCollision()
{
    // ī�޶� ���� �Ʒ��� �������� ���� ����
    if (m_position.y <= m_groundLevel)
    {
        m_position.y = m_groundLevel;      // ���� ���̷� ��ġ ����
        m_verticalVelocity = 0.0f;         // ���� �ӵ� �ʱ�ȭ
        m_isGrounded = true;               // ���� ���� ���·� ����
        m_canJump = true;                  // ���� ���� ���·� ����

        // camPosition ���͵� ������Ʈ
        camPosition = XMVectorSet(m_position.x, m_position.y, m_position.z, 0.0f);
    }
}

void CameraClass::UpdateCamera()
{
    // ���� ȸ�� �� �̵� ó��...
    XMMATRIX camRotationMatrix = XMMatrixRotationRollPitchYaw(camPitch, camYaw, 0);
    camTarget = XMVector3TransformCoord(DefaultForward, camRotationMatrix);
    camTarget = XMVector3Normalize(camTarget);

    XMMATRIX RotateYTempMatrix = XMMatrixRotationY(camYaw);
    camRight = XMVector3TransformCoord(DefaultRight, RotateYTempMatrix);
    camUp = XMVector3TransformCoord(camUp, RotateYTempMatrix);
    camForward = XMVector3TransformCoord(DefaultForward, RotateYTempMatrix);

    // ���� �̵��� ���� (Y���� �߷� �ý��ۿ��� ó��)
    XMVECTOR horizontalMovement = moveLeftRight * camRight + moveBackForward * camForward;
    XMFLOAT3 horizontalMove;
    XMStoreFloat3(&horizontalMove, horizontalMovement);

    m_position.x += horizontalMove.x;
    m_position.z += horizontalMove.z;

    // camPosition ������Ʈ (Y���� �߷� �ý��ۿ��� ����)
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


