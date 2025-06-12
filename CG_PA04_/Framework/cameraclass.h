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

    void SetYawPitch(float yaw, float pitch); // First Person 전용
    void Move(float leftRight, float forward); // FPS 이동 처리

    void Jump();         // 점프 시작
    void UpdateJump(float deltaTime);  // 시간 기반 점프
    bool IsJumping();    // 점프 중인지 여부

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
    // 점프 관련
    bool m_isJumping = false;
    float m_jumpVelocity = 0.0f;
    const float GRAVITY = 0.02f;
    const float JUMP_POWER = 0.4f;

};

#endif
