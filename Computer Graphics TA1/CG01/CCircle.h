#pragma once
#include "CShape.h"
class CCircle : public CShape
{
protected:
	float m_r;
public:
	CCircle(float x, float y, float r);
	void Draw(HDC hdc) const override;
};

