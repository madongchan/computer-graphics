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
    float GetYaw() const { return camYaw; }

    // 이동 및 회전 입력 처리
    void MoveForward(float);
    void MoveRight(float);
    void AdjustYaw(float);
    void AdjustPitch(float);

    // 렌더링 및 행렬 생성
    void Render();
    void GetViewMatrix(XMMATRIX&);

    // TPS 카메라 파라미터 설정
    void SetFollowParameters(float dist, float height, float offset, float smooth);

    // TPS 카메라 업데이트 (인자 수정: Yaw는 내부 멤버 변수 camYaw를 사용하므로 제거)
    void UpdateFollowCamera(const XMFLOAT3& targetPos);

    // 물리(점프) 관련 (기존 유지)
    void Jump();
    void ApplyGravity(float deltaTime);
    void CheckGroundCollision();
    bool IsGrounded() const { return m_isGrounded; }
    void SetGroundLevel(float groundLevel) { m_groundLevel = groundLevel; }

private:
    void UpdateViewMatrix(); // 내부 헬퍼 함수

private:
    XMFLOAT3 m_position;
    XMFLOAT3 m_rotation;
    XMMATRIX m_viewMatrix;

    // 카메라 벡터
    XMVECTOR camPosition;
    XMVECTOR camTarget;
    XMVECTOR camUp;
    XMVECTOR camRight;
    XMVECTOR camForward;

    // 회전 및 이동 제어 변수
    float camYaw;
    float camPitch;
    float moveLeftRight;
    float moveBackForward;

    const XMVECTOR DefaultForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
    const XMVECTOR DefaultRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

    // 물리 변수 (기존 유지)
    float m_verticalVelocity;
    float m_gravity;
    float m_jumpForce;
    float m_groundLevel;
    bool m_isGrounded;
    bool m_canJump;

    // TPS 추적 변수
    float m_followDist;     // 카메라와 타겟 사이의 거리
    float m_targetOffset;   // 타겟(플레이어)의 Y축 오프셋 (머리/가슴 높이)
    float m_smoothness;     // 보간 속도 (Camera Lag 효과)
};

#endif