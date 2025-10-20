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
		m_prevKeys[i] = false; // ���� Ű ���µ� �ʱ�ȭ
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
	// ���� �����ӿ��� �����ְ�, ���� �����ӿ��� �� �����־����� üũ
	return m_keys[key] && !m_prevKeys[key];
}


void InputClass::Update()
{
	// ���� �������� Ű ���¸� ���� ������ ���·� ����
	for (int i = 0; i < 256; ++i)
	{
		m_prevKeys[i] = m_keys[i];
	}
}