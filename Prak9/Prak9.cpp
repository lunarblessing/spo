// Prak9.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "Prak9.h"
#include <string>

#define MAX_LOADSTRING 100

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна
HWND hWnd;
LPCTSTR slotName = TEXT("\\\\.\\mailslot\\sample_mailslot");
LPCTSTR pipeName = TEXT("\\\\.\\pipe\\pipename");
HANDLE pipeHandle;

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

void DoFileMapping();
void WriteToMailslot();
void WriteToPipe();
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
	LoadStringW(hInstance, IDC_PRAK9, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Выполнить инициализацию приложения:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PRAK9));

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
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PRAK9));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_PRAK9);
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
		DoFileMapping();
		WriteToMailslot();
		WriteToPipe();
		ReadPipe();
	}
	break;
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		break;
	}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void DoFileMapping() {
	MessageBox(hWnd, L"FileMapping", NULL, IDOK);
	HANDLE hFile = CreateFile(L"C:\\Users\\Dima\\Desktop\\file.txt", GENERIC_READ, 0, nullptr,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	DWORD dwFileSize = GetFileSize(hFile, nullptr);
	HANDLE hMapping = CreateFileMapping(hFile, nullptr, PAGE_READONLY, 0, 0, nullptr);
	unsigned char* dataPtr = (unsigned char*)MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, dwFileSize);
	std::wstring str((WCHAR*)dataPtr);
	MessageBox(hWnd, str.c_str(), L"Содержание файла", IDOK);
	UnmapViewOfFile(dataPtr);
	CloseHandle(hMapping);
	CloseHandle(hFile);
}

void WriteToMailslot() {
	HANDLE hFile;
	hFile = CreateFile(slotName, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	BOOL fResult;
	DWORD cbWritten;
	fResult = WriteFile(hFile, L"Message", (DWORD)(lstrlen(L"Message") + 1) * sizeof(TCHAR),
		&cbWritten, NULL);
	MessageBox(hWnd, L"Slot written", L"Mailslot", 0);
	CloseHandle(hFile);
}

void WriteToPipe() {
	MessageBox(hWnd, L"Отправить пайп?", L"Pipe", 0);
	pipeHandle = CreateFile(pipeName, GENERIC_WRITE | GENERIC_READ, 0,
		NULL, OPEN_EXISTING, 0, NULL);
	DWORD cbWritten;
	WriteFile(pipeHandle, L"Pipe message 1", (DWORD)(lstrlen(L"Pipe message 1") + 1) * sizeof(TCHAR),
		&cbWritten, NULL);
}

void ReadPipe() {
	MessageBox(hWnd, L"Принять сообщение Pipe?", L"Pipe", 0);
	WCHAR buffer[1024];
	DWORD written;
	ReadFile(pipeHandle, buffer, 1024, &written, NULL);
	std::wstring str(buffer);
	MessageBox(hWnd, str.c_str(), L"Pipe", 0);
	CloseHandle(pipeHandle);
}
