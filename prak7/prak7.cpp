// prak7.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "prak7.h"
#include <string>

#define MAX_LOADSTRING 100

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[] = L"Title";                  // Текст строки заголовка
WCHAR szWindowClass[] = L"prak7";            // имя класса главного окна

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void DoHeap(HWND hwnd);
void DoVirtual(HWND hwnd);
void UseSystemInfo(HWND hwnd);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MyRegisterClass(hInstance);

	// Выполнить инициализацию приложения:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PRAK7));

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
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PRAK7));
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

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

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
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
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
		DoHeap(hWnd);
		DoVirtual(hWnd);
		UseSystemInfo(hWnd);
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

void DoHeap(HWND hwnd)
{
	HANDLE hHeap = HeapCreate(0, 0x01000, 0);
	if (hHeap != NULL)
	{
		void* allocated = HeapAlloc(hHeap, HEAP_ZERO_MEMORY, 1024);
		if (allocated != NULL) {
			std::wstring str = L"Куча создана по адресу ";
			str += (std::to_wstring((INT64)allocated));
			MessageBox(hwnd, str.c_str(), L"Удачно", IDOK);
			*(int*)(allocated) = 20;
			str = L"Занесённое значение: ";
			str += (std::to_wstring(*(int*)allocated));
			MessageBox(hwnd, str.c_str(), L"Удачно", IDOK);
			if (HeapFree(hHeap, 0, allocated) == FALSE)
			{
				MessageBox(hwnd, L"Ошибка освобождения", L"Ошибка ", IDOK);
			}
		}
		if (HeapDestroy(hHeap) == FALSE)
		{
			MessageBox(hwnd, L"Ошибка удаления", L"Ошибка ", IDOK);
		}
		else { MessageBox(hwnd, L"Создали и разрушили кучу удачно", L"Удачно ", IDOK); }
	}
	else { MessageBox(hwnd, L"Не удалось создать кучу", L"Ошибка ", IDOK); }
}

void DoVirtual(HWND hwnd)
{
	LPVOID allocated = VirtualAlloc(NULL, 4096, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if (allocated)
	{
		std::wstring str = L"Выделена виртуальная память с адресом ";
		str += (std::to_wstring((INT64)allocated));
		MessageBox(hwnd, str.c_str(), L"Удачно", IDOK);
		*(int*)(allocated) = 55;
		str = L"Занесённое значение: ";
		str += (std::to_wstring(*(int*)allocated));
		MessageBox(hwnd, str.c_str(), L"Удачно", IDOK);
		if (VirtualLock(allocated, 4096))
		{
			MessageBox(hwnd, L"Lock", L"Привязали память", IDOK);
		}
		else
		{
			MessageBox(hwnd, L"Ошибка блокировки", L"Виртуальная память", IDOK);
		}
		if (VirtualUnlock(allocated, 4096))
		{
			MessageBox(hwnd, L"Память разблокирована", L"Виртуальная память", IDOK);
		}
		else
		{
			MessageBox(hwnd, L"Ошибка разблокировки", L"Виртуальная память", IDOK);
		}
		if (VirtualFree(allocated, 0, MEM_RELEASE))
		{
			MessageBox(hwnd, L"Освободили", L"Виртуальная память", IDOK);
		}
		else
		{
			MessageBox(hwnd, L"Не Освободили", L"Виртуальная память", IDOK);
		}
	}
	else
	{
		MessageBox(hwnd, L"Не удалось выделить виртуальную память", L"Ошибка", IDOK);
	}

}

void UseSystemInfo(HWND hwnd)
{
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);

	MessageBox(hwnd, std::to_wstring(sysInfo.dwPageSize).c_str(),
		L"Размер страницы памяти", IDOK);
}