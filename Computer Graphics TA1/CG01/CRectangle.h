#pragma once
#include "CShape.h"
class CRectangle : public CShape
{
protected:
	float m_w, m_h;
public:
	CRectangle(float x, float y, float w, float h);
	void Draw(HDC hdc) const override;
};

