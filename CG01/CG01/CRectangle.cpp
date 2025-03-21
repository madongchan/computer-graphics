#include "CRectangle.h"

CRectangle::CRectangle(float x, float y, float w, float h) : CShape(x, y), m_w(w), m_h(h) {}

void CRectangle::Draw(HDC hdc) const {
	Rectangle(hdc, m_x, m_y, m_w, m_h);
}
