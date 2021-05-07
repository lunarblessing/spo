// Prak9Client.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "Prak9Client.h"
#include <string>

#define MAX_LOADSTRING 100

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна
HWND hWnd;
HANDLE hSlot;
HANDLE hPipe;
LPCTSTR slotName = TEXT("\\\\.\\mailslot\\sample_mailslot");
LPCTSTR pipeName = TEXT("\\\\.\\pipe\\pipename");

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void MakeSlot(LPCTSTR lpszSlotName);
void ReadSlot();
void MakePipe(LPCTSTR pipeName);
void ReadPipe();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Разместите код здесь.

	// Инициализация глобальных строк
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_PRAK9CLIENT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Выполнить инициализацию приложения:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PRAK9CLIENT));

	MSG msg;

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

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PRAK9CLIENT));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_PRAK9CLIENT);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

	hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, 600, 400, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
	{
		PostMessage(hWnd, WM_USER + 100, 0, 0);
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	break;
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Разобрать выбор в меню:
		switch (wmId)
		{
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_USER + 100:
	{
		MakeSlot(slotName);
		MakePipe(pipeName);
		ReadPipe();
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void MakeSlot(LPCTSTR lpszSlotName)
{
	hSlot = CreateMailslot(lpszSlotName, 0, MAILSLOT_WAIT_FOREVER, NULL);
	MessageBox(hWnd, L"Принимать сообщения по mailslot?", L"", 0);
	ReadSlot();
}

void ReadSlot()
{
	WCHAR buffer[1024];
	DWORD written;
	ReadFile(hSlot, buffer, 1024, &written, NULL);
	std::wstring str(buffer);
	if (str != L"") {
		MessageBox(hWnd, str.c_str(), L"Сообщение", 0);
	}
	else {
		MessageBox(hWnd, L"Сообщения нет", L"Сообщение", 0);
	}
	CloseHandle(hSlot);
}

void MakePipe(LPCTSTR pipeName) {
	MessageBox(hWnd, L"Создать пайп?", L"Сообщение", 0);
	hPipe = CreateNamedPipe(pipeName, PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, 4, 1024, 1024, 3000, NULL);
	ConnectNamedPipe(hPipe, NULL);
	MessageBox(hWnd, L"Принять сообщение?", L"Сообщение", 0);
}

void ReadPipe() {
	WCHAR buffer[1024];
	DWORD written;
	ReadFile(hPipe, buffer, 1024, &written, NULL);
	std::wstring str(buffer);
	MessageBox(hWnd, str.c_str(), L"Pipe", 0);
	DWORD cbWritten;
	WriteFile(hPipe, L"Pipe message 2", (DWORD)(lstrlen(L"Pipe message 2") + 1) * sizeof(TCHAR),
		&cbWritten, NULL);
}