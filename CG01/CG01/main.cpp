// Main.cpp
#pragma once
#include <windows.h>
#include "CShape.h"
#include "CRectangle.h"
#include "CCircle.h"
LRESULT WINAPI WndProc(HWND, UINT, WPARAM, LPARAM);

// 전역 도형 배열 선언
CShape* shapes[5] = { NULL };

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpszCmdLine, _In_ int nCmdShow)
{
	WNDCLASS wc;
	HWND hwnd;
	MSG msg;
	wc.style = 0;
	wc.lpfnWndProc = (WNDPROC)WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = L"MyWndClass";

	RegisterClass(&wc);
	hwnd = CreateWindow(
		L"MyWndClass", // WNDCLASS name
		L"SDK Application", // Window title
		WS_OVERLAPPEDWINDOW,// Window style
		CW_USEDEFAULT, // Horizontal position
		CW_USEDEFAULT, // Vertical position
		600, // Initial width
		600, // Initial height
		HWND_DESKTOP, // Handle of parent window
		NULL, // Menu handle
		hInstance, // Application's instance handle
		NULL // Window-creation data
	);

	// 도형 배열 초기화
	shapes[0] = new CCircle(100, 100, 50);
	shapes[1] = new CRectangle(300, 300, 100, 100);
	shapes[2] = new CRectangle(200, 100, 50, 150);
	shapes[3] = new CCircle(100, 300, 150);
	shapes[4] = new CRectangle(200, 200, 300, 300);

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	// 메모리 반환
	for (int i = 0; i < 5; ++i)
	{
		delete shapes[i];
		shapes[i] = NULL;

	}
	return (int)msg.wParam;
}

// main.cpp 파일 맨 아래에 WndProc 구현 추가
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps; // a structure need to paint the client area of a window
	HDC hdc; // a device context need for drawing on a window
	switch (message) {

	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps); // start of drawing objects
		for (int i = 0; i < 5; ++i)
		{
			shapes[i]->Draw(hdc); // 다형성을 이용한 도형 그리기
		}
		EndPaint(hwnd, &ps); // end of drawing objects
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}
