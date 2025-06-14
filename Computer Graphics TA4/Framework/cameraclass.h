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
};

#endif
