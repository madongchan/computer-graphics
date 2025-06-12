#ifndef _CAMERACLASS_H_
#define _CAMERACLASS_H_

#include <directxmath.h>
#include "AlignedAllocationPolicy.h"

using namespace DirectX;

class CameraClass : public AlignedAllocationPolicy<16>
{
public:
    CameraClass();
    ~CameraClass();

    void SetPosition(float x, float y, float z);
    void SetRotation(float x, float y, float z);
    XMFLOAT3 GetPosition();
    XMFLOAT3 GetRotation();

    void SetYawPitch(float yaw, float pitch); // First Person ����
    void Move(float leftRight, float forward); // FPS �̵� ó��

    void Jump();         // ���� ����
    void UpdateJump(float deltaTime);  // �ð� ��� ����
    bool IsJumping();    // ���� ������ ����

    XMVECTOR GetLookVector();
    XMVECTOR GetRightVector();
    XMVECTOR GetUpVector();

    void Render();
    void GetViewMatrix(XMMATRIX&);

private:
    XMFLOAT3 m_position;
    XMFLOAT3 m_rotation;
    XMMATRIX m_viewMatrix;

    float camYaw;
    float camPitch;
    // ���� ����
    bool m_isJumping = false;
    float m_jumpVelocity = 0.0f;
    const float GRAVITY = 0.02f;
    const float JUMP_POWER = 0.4f;

};

#endif
