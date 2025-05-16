#pragma once
#include <Windows.h>

class CShape {
protected:
    float m_x, m_y;
public:
    CShape(float x, float y);
    virtual void Draw(HDC hdc) const = 0;
};