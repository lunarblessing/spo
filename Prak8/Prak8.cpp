// Prak8.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "Prak8.h"
#include <string>
#include <iostream>
#include <process.h>

#define MAX_LOADSTRING 100
#define MAX_ARRAY 10

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[] = L"Title";                  // Текст строки заголовка
WCHAR szWindowClass[] = L"prak8";            // имя класса главного окна

HWND hWndMainWindow;

CRITICAL_SECTION critsect;
int	arr[MAX_ARRAY];
int mutexArr[80];
int semaphoreArr[80];
int mutexIndex = 0;
int semaphoreIndex = 0;

HANDLE mutex;
HANDLE semaphore;

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
void StartCriticalSection();
void DoMutex();
void EmptyArray(void*);
void PrintArray(void*);
void FullArray(void*);
void PrintMutexArray();
void MutexFill(void* args);
void DoSemaphore();
void SemaphoreFill(void* args);
void PrintSemaphone();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	MyRegisterClass(hInstance);
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}
	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PRAK8));
	MSG msg;
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
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PRAK8));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

	hWndMainWindow = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWndMainWindow)
	{
		return FALSE;
	}

	ShowWindow(hWndMainWindow, nCmdShow);
	UpdateWindow(hWndMainWindow);

	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
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
	case WM_CREATE:
	{
		PostMessage(hWnd, WM_USER + 100, 0, 0);
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	break;
	case WM_USER + 100:
	{
		StartCriticalSection();
		DoMutex();
		DoSemaphore();
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

// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

void StartCriticalSection()
{
	InitializeCriticalSection(&critsect);
	for (int i = 0; i < MAX_ARRAY; i++) {
		arr[i] = 5 + i;
	}
	_beginthread(PrintArray, 1024, NULL);
	_beginthread(FullArray, 1024, NULL);
	_beginthread(PrintArray, 1024, NULL);
	_beginthread(EmptyArray, 1024, NULL);
	_beginthread(PrintArray, 1024, NULL);
	Sleep(1000);
	MessageBox(hWndMainWindow, L"Мьютекс", L"", IDOK);
}

void EmptyArray(void*)
{
	EnterCriticalSection(&critsect);
	for (int i = 0; i < MAX_ARRAY; i++) {
		arr[i] = 0;
	}
	LeaveCriticalSection(&critsect);
	_endthread();
}

void PrintArray(void*)
{
	EnterCriticalSection(&critsect);
	std::wstring str = L"";
	for (int i = 0; i < MAX_ARRAY; i++) {
		str += std::to_wstring(arr[i]);
	}
	MessageBox(hWndMainWindow, str.c_str(), L"Тестовая строка ", IDOK);
	LeaveCriticalSection(&critsect);
	_endthread();
}

void FullArray(void*)
{
	EnterCriticalSection(&critsect);
	for (int i = 0; i < MAX_ARRAY; i++) {
		arr[i] = i;
	}
	LeaveCriticalSection(&critsect);
	_endthread();
}

void PrintMutexArray() {
	std::wstring str = L"";
	for (int i = 0; i < 80; i++) {
		str += std::to_wstring(mutexArr[i]);
	}
	MessageBox(hWndMainWindow, str.c_str(), L"Тестовая строка ", IDOK);
}

void MutexFill(void* args) {
	DWORD result = WaitForSingleObject(mutex, -1);
	if (result == WAIT_OBJECT_0) {
		int index = *(int*)args;
		for (int j = 0; j < 10; j++)
		{
			mutexArr[j + mutexIndex] = index;
			Sleep(15);
		}
		mutexIndex += 10;
		delete args;
		ReleaseMutex(mutex);
	}
	_endthread();
}

void DoMutex()
{
	for (int i = 0; i < 80; i++)
	{
		mutexArr[i] = -i;
	}
	mutex = CreateMutex(NULL, FALSE, L"Мьютекс");
	for (int i = 0; i < 8; i++)
	{
		_beginthread(MutexFill, 1024, new int(i));
	}
	Sleep(2500);
	PrintMutexArray();
	CloseHandle(mutex);
	
}

void SemaphoreFill(void* args) {
	int index = *(int*)args;
	if (WaitForSingleObject(semaphore, -1) == WAIT_OBJECT_0) {
		for (int j = 0; j < 10; j++)
		{
			semaphoreArr[semaphoreIndex++] = index;
			Sleep(10);
		}
		delete args;
		ReleaseSemaphore(semaphore, 1, NULL);
	}
	_endthread();
}

void DoSemaphore() {
	MessageBox(hWndMainWindow, L"Семафор", NULL, IDOK);
	for (int i = 0; i < 80; i++)
	{
		semaphoreArr[i] = -i;
	}
	semaphore = CreateSemaphore(NULL, 2, 2, NULL);
	for (int i = 0; i < 8; i++)
	{
		_beginthread(SemaphoreFill, 1024, new int(i));
	}
	Sleep(3000);
	PrintSemaphone();
	CloseHandle(semaphore);
}

void PrintSemaphone() {
	std::wstring str = L"";
	for (int i = 0; i < 77; i++) {
		str += std::to_wstring(semaphoreArr[i]);
	}
	MessageBox(hWndMainWindow, str.c_str(), L"Тестовая строка ", IDOK);
}