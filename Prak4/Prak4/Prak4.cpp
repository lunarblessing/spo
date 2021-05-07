// Prak4.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "Prak4.h"

#define MAX_LOADSTRING 100

// Глобальные переменные:
HINSTANCE hInst;
WCHAR titleMainWindow[] = L"Prak4";
WCHAR classMainWindow[] = L"Prak4";
WCHAR titleChild[] = L"Child";
WCHAR classChild[] = L"Prak4child";
HWND hwndMainWindow, hwndChild;

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance, const WCHAR className[], HBRUSH brush);
BOOL                CreateWindows(int nCmdShow);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	hInst = hInstance;
	MyRegisterClass(hInstance, classMainWindow, CreateSolidBrush(RGB(250, 250, 250)));
	MyRegisterClass(hInstance, classChild, CreateSolidBrush(RGB(250, 210, 210)));

	// Выполнить инициализацию приложения:
	if (!CreateWindows(SW_SHOWDEFAULT))
	{
		return FALSE;
	}
	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PRAK4));
	MSG msg;

	SetTimer(hwndMainWindow, 1, 2000, NULL);

	// Цикл основного сообщения:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance, const WCHAR className[], HBRUSH brush)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PRAK4));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = brush;
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = className;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

BOOL CreateWindows(int nCmdShow)
{
	//Основное окно
	hwndMainWindow = CreateWindowW(classMainWindow, titleMainWindow, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, 1000, 700, nullptr, nullptr, hInst, nullptr);
	if (!hwndMainWindow)
	{
		return FALSE;
	}
	ShowWindow(hwndMainWindow, nCmdShow);
	UpdateWindow(hwndMainWindow);

	//Первое окно
	LPRECT rect = new tagRECT();
	GetWindowRect(hwndMainWindow, rect);
	hwndChild = CreateWindowW(classChild, titleChild, WS_OVERLAPPEDWINDOW | WS_CHILD,
		200, 150, 400, 400, hwndMainWindow, nullptr, hInst, nullptr);
	if (!hwndChild)
	{
		return FALSE;
	}
	//ShowWindow(hwndChild, nCmdShow);
	//UpdateWindow(hwndChild);
	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_TIMER:
	{
		ShowWindow(hwndChild, SW_SHOWDEFAULT);
		UpdateWindow(hwndChild);
		KillTimer(hwndMainWindow, 1);
		break;
	}
	case WM_LBUTTONDOWN:
	{
		if (hWnd == hwndChild) {
			DestroyWindow(hwndChild);
		}
		break;
	}
	case WM_KEYDOWN:
	{
		if (!IsWindowVisible(hwndChild)) {
			break;
		}
		RECT rect;
		GetWindowRect(hwndChild, &rect);
		MapWindowPoints(HWND_DESKTOP, hwndMainWindow, (LPPOINT)&rect, 2);
		if (wParam == VK_LEFT) {
			SetWindowPos(hwndChild, NULL, rect.left - 3, rect.top, NULL, NULL, SWP_NOSIZE | SWP_NOZORDER);
		}
		else if (wParam == VK_RIGHT) {
			SetWindowPos(hwndChild, NULL, rect.left + 3, rect.top, NULL, NULL, SWP_NOSIZE | SWP_NOZORDER);
		}
		else if (wParam == VK_UP) {
			SetWindowPos(hwndChild, NULL, rect.left, rect.top - 3, NULL, NULL, SWP_NOSIZE | SWP_NOZORDER);
		}
		else if (wParam == VK_DOWN) {
			SetWindowPos(hwndChild, NULL, rect.left, rect.top + 3, NULL, NULL, SWP_NOSIZE | SWP_NOZORDER);
		}
		break;
	}
	case WM_DESTROY:
		if (hWnd == hwndMainWindow) {
			PostQuitMessage(0);
		}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

