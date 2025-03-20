#pragma once
#include <Windows.h>

class CShape {
protected:
    float m_x, m_y;
public:
    CShape(float x, float y) : m_x(x), m_y(y) {}
    virtual void Draw(HDC hdc) const = 0;
};