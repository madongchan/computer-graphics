////////////////////////////////////////////////////////////////////////////////
// Filename: systemclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "systemclass.h"


SystemClass::SystemClass()
{
	m_Input = 0;
	m_Graphics = 0;
	m_Sound = 0;
	m_Timer = 0;

	m_screenWidth = 0;
	m_screenHeight = 0;
	m_posX = 0;
	m_posY = 0;
}


SystemClass::SystemClass(const SystemClass& other)
{
}


SystemClass::~SystemClass()
{
}


bool SystemClass::Initialize()
{
	bool result;


	// Initialize the width and height of the screen to zero before sending the variables into the function.
	m_screenWidth = 0;
	m_screenHeight = 0;

	// Initialize the windows api.
	InitializeWindows(m_screenWidth, m_screenHeight);

	// Create the input object.  This object will be used to handle reading the keyboard input from the user.
	m_Input = new InputClass;
	if(!m_Input)
	{
		return false;
	}

	// Initialize the input object.
	result = m_Input->Initialize(m_hinstance, m_hwnd, m_screenWidth, m_screenHeight);

	// Create the graphics object.  This object will handle rendering all the graphics for this application.
	m_Graphics = new GraphicsClass;
	if(!m_Graphics)
	{
		return false;
	}

	// Initialize the graphics object.
	result = m_Graphics->Initialize(m_screenWidth, m_screenHeight, m_hwnd);
	if(!result)
	{
		return false;
	}

	// Create the sound object.
	m_Sound = new SoundClass;
	if (!m_Sound)
	{
		return false;
	}

	// Initialize the sound object.
	result = m_Sound->Initialize(m_hwnd);
	if (!result)
	{
		MessageBox(m_hwnd, L"Could not initialize Direct Sound.", L"Error", MB_OK);
		return false;
	}


	m_Timer = new TimerClass;
	if (!m_Timer)
	{
		return false;
	}
	m_Timer->Initialize();

	m_isCameraControlMode = true;
	return true;
}


void SystemClass::Shutdown()
{
	// Release the graphics object.
	if(m_Graphics)
	{
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = 0;
	}

	// Release the input object.
	if(m_Input)
	{
		m_Input->Shutdown();
		delete m_Input;
		m_Input = 0;
	}
	if(m_Timer)
	{
		delete m_Timer;
		m_Timer = 0;
	}
	// Shutdown the window.
	ShutdownWindows();
	
	return;
}


void SystemClass::Run()
{
	MSG msg;
	bool done, result;


	// Initialize the message structure.
	ZeroMemory(&msg, sizeof(MSG));
	
	// Loop until there is a quit message from the window or the user.
	done = false;
	while(!done)
	{
		// Handle the windows messages.
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// If windows signals to end the application then exit out.
		if(msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			double deltaTime = m_Timer->GetDeltaTime();
			// Otherwise do the frame processing.
			result = Frame(deltaTime);
			if(!result)
			{
				done = true;
			}
		}
		// 매 프레임 마우스 커서를 창 중앙으로 고정
		// (m_posX, m_posY는 InitializeWindows에서 계산된 윈도우의 '화면' 좌표)
		// 카메라 모드일 때만 마우스를 화면 중앙으로 강제 이동
		if (m_isCameraControlMode)
		{
			SetCursorPos(m_posX + (m_screenWidth / 2), m_posY + (m_screenHeight / 2));
		}
	}
	return;
}


bool SystemClass::Frame(double deltaTime)
{
	bool result;

	// 1. (입력) DirectInput으로 키보드/마우스 상태 읽기
	result = m_Input->Frame();
	if (!result)
	{
		return false;
	}
	// 2. (입력 처리) ESC 키 확인
	if (m_Input->IsEscapePressed())
	{
		return false;
	}
	if (m_Input->IsKeyToggle(DIK_F1))
	{
		m_isCameraControlMode = !m_isCameraControlMode;

		if (m_isCameraControlMode)
		{
			// 게임 모드: 커서 숨기기 (카운트가 0보다 작아질 때까지)
			while (ShowCursor(false) >= 0);
			// 즉시 중앙으로 이동
			SetCursorPos(m_posX + (m_screenWidth / 2), m_posY + (m_screenHeight / 2));
		}
		else
		{
			// UI 모드: 커서 보이기
			while (ShowCursor(true) < 0);
		}
	}

	// 3. (로직/렌더링) Graphics 객체 프레임 처리 (deltaTime 전달)
	result = m_Graphics->Frame(m_Input, deltaTime);
	if (!result)
	{
		return false;
	}

	return true;

	return true;
}


LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch(umsg)
	{
		// Any other messages send to the default message handler as our application won't make use of them.
		default:
		{
			return DefWindowProc(hwnd, umsg, wparam, lparam);
		}
	}
}


void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	// 로컬 변수 posX, posY 제거 (멤버 m_posX, m_posY 사용)
	// int posX, posY;

	// Get an external pointer to this object.	
	ApplicationHandle = this;

	// Get the instance of this application.
	m_hinstance = GetModuleHandle(NULL);

	// Give the application a name.
	m_applicationName = L"Engine";

	// Setup the windows class with default settings.
	wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc   = WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = m_hinstance;
	wc.hIcon		 = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm       = wc.hIcon;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize        = sizeof(WNDCLASSEX);
	
	// Register the window class.
	RegisterClassEx(&wc);

	// Determine the resolution of the clients desktop screen.
	m_screenWidth = GetSystemMetrics(SM_CXSCREEN);
	m_screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Setup the screen settings depending on whether it is running in full screen or in windowed mode.
	if(FULL_SCREEN)
	{
		// If full screen set the screen to maximum size of the users desktop and 32bit.
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize       = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth  = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;			
		dmScreenSettings.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Change the display settings to full screen.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// Set the position of the window to the top left corner.
		m_posX = m_posY = 0;
	}
	else
	{
		// If windowed then set it to 800x600 resolution.
		screenWidth  = 800;
		screenHeight = 600;

		// Place the window in the middle of the screen.
		m_posX = (GetSystemMetrics(SM_CXSCREEN) - m_screenWidth) / 2;
		m_posY = (GetSystemMetrics(SM_CYSCREEN) - m_screenHeight) / 2;
	}

	// Create the window with the screen settings and get the handle to it.
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName, 
						    WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
							m_posX, m_posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	// Bring the window up on the screen and set it as main focus.
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	// Hide the mouse cursor.
	ShowCursor(false);

	screenWidth = m_screenWidth;
	screenHeight = m_screenHeight;

	return;
}


void SystemClass::ShutdownWindows()
{
	// Show the mouse cursor.
	ShowCursor(true);

	// Fix the display settings if leaving full screen mode.
	if(FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// Remove the window.
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	// Remove the application instance.
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	// Release the pointer to this class.
	ApplicationHandle = NULL;

	return;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch(umessage)
	{
		// Check if the window is being destroyed.
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}

		// Check if the window is being closed.
		case WM_CLOSE:
		{
			PostQuitMessage(0);		
			return 0;
		}

		// All other messages pass to the message handler in the system class.
		default:
		{
			return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
		}
	}
}