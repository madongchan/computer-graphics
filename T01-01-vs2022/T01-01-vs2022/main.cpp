#include "CShape.h"
#include "CRectangle.h"
#include "CCircle.h"

// 윈도우 프로시저 선언
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// 전역 도형 배열 선언
CShape* shapes[5] = { NULL };

// WinMain 함수
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow) {
    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"GraphicsWindow";

    RegisterClass(&wc);

    HWND hwnd = CreateWindow(L"GraphicsWindow", L"Windows API Graphics",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 500, 500,
        NULL, NULL, hInstance, NULL);

    // 도형 배열 초기화
    shapes[0] = new CCircle(100, 100, 50);
    shapes[1] = new CRectangle(300, 300, 100, 100);
    shapes[2] = new CRectangle(200, 100, 50, 150);
    shapes[3] = new CCircle(100, 300, 150);
    shapes[4] = new CRectangle(200, 200, 300, 300);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    for (CShape* shape : shapes) {
        delete shape;
    }
    return (int)msg.wParam;
}

// 윈도우 프로시저 정의
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        
        for (CShape* shape : shapes) {
			shape->Draw(hdc); // 다형성을 이용한 도형 그리기
        }

        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}
