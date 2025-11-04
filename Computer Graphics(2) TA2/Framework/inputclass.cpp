////////////////////////////////////////////////////////////////////////////////
// Filename: inputclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "inputclass.h"


InputClass::InputClass()
{
}


InputClass::InputClass(const InputClass& other)
{
}


InputClass::~InputClass()
{
}


void InputClass::Initialize()
{
	int i;

	// Initialize all the keys to being released and not pressed.
	for (i = 0; i < 256; i++)
	{
		m_keys[i] = false;
		m_prevKeys[i] = false; // 이전 키 상태도 초기화
	}

	return;
}


void InputClass::KeyDown(unsigned int input)
{
	// If a key is pressed then save that state in the key array.
	m_keys[input] = true;
	return;
}


void InputClass::KeyUp(unsigned int input)
{
	// If a key is released then clear that state in the key array.
	m_keys[input] = false;
	return;
}


bool InputClass::IsKeyDown(unsigned int key)
{
	// Return what state the key is in (pressed/not pressed).
	return m_keys[key];
}


bool InputClass::IsKeyPressed(unsigned int key)
{
	// 현재 프레임에서 눌려있고, 이전 프레임에서 안 눌려있었는지 체크
	return m_keys[key] && !m_prevKeys[key];
}


void InputClass::Update()
{
	// 현재 프레임의 키 상태를 이전 프레임 상태로 복사
	for (int i = 0; i < 256; ++i)
	{
		m_prevKeys[i] = m_keys[i];
	}
}