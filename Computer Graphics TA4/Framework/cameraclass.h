#ifndef _CAMERACLASS_H_
#define _CAMERACLASS_H_

#include <directxmath.h>
#include "AlignedAllocationPolicy.h"

using namespace DirectX;

class CameraClass : public AlignedAllocationPolicy<16>
{
public:
    CameraClass();
    CameraClass(const CameraClass&);
    ~CameraClass();

    void SetPosition(float, float, float);
    void SetRotation(float, float, float);
    XMFLOAT3 GetPosition();
    XMFLOAT3 GetRotation();
    XMVECTOR GetPositionXM() const;

    void MoveForward(float);
    void MoveRight(float);
    void AdjustYaw(float);
    void AdjustPitch(float);

    void UpdateCamera();
    void Render();
    void GetViewMatrix(XMMATRIX&);

    // 점프 관련 함수 추가
    void Jump();
    void ApplyGravity(float deltaTime);
    void CheckGroundCollision();
    bool IsGrounded() const { return m_isGrounded; }
    void SetGroundLevel(float groundLevel) { m_groundLevel = groundLevel; }

private:
    XMFLOAT3 m_position;
    XMFLOAT3 m_rotation;
    XMMATRIX m_viewMatrix;

    XMVECTOR camPosition, camTarget, camUp, camRight, camForward;
    XMMATRIX camView;

    float camYaw, camPitch;
    float moveLeftRight, moveBackForward;

    const XMVECTOR DefaultForward = XMVectorSet(0, 0, 1, 0);
    const XMVECTOR DefaultRight = XMVectorSet(1, 0, 0, 0);

    // 점프 관련 변수
    float m_verticalVelocity;      // Y축 속도
    float m_gravity;               // 중력 가속도
    float m_jumpForce;             // 점프 초기 속도
    float m_groundLevel;           // 지면 높이
    bool m_isGrounded;             // 지면 접촉 여부
    bool m_canJump;                // 점프 가능 여부
};

#endif
