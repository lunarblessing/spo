// Prak1.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "Prak1.h"
#include <vector>
#include <string>

#define MAX_LOADSTRING 100

// Глобальные переменные:
HINSTANCE hInst;
HWND hwndMainWindow, hwndChildWindow1, hwndChildWindow2, hwndCreateFile, hwndReadFile;
WCHAR titleMainWindow[] = L"Main Window";
WCHAR classMainWindow[] = L"MainWindowClass";
WCHAR titleChild1[] = L"Child window 1";
WCHAR classChild1[] = L"ChildWindowClass1";
WCHAR titleChild2[] = L"Child window 2";
WCHAR classChild2[] = L"ChildWindowClass2";
bool moveFirst = true;

HBRUSH mainBrush;
char* fileContent = new char[1024]{ 0 };
int fileContentIndex = 0;

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance, const WCHAR className[], HBRUSH brush);
BOOL                CreateWindows(int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void				SelectRGB(HWND hDlg);
void				FileCreate(HWND hWnd);
INT_PTR CALLBACK	CreateFileDialogProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void				FileRead(HWND hWnd);
INT_PTR CALLBACK ReadFileDialogProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	hInst = hInstance;
	mainBrush = CreateSolidBrush(RGB(250, 250, 250));
	MyRegisterClass(hInst, classMainWindow, mainBrush); //Основное окно
	MyRegisterClass(hInst, classChild1, CreateSolidBrush(RGB(0, 0, 220)));
	MyRegisterClass(hInst, classChild2, CreateSolidBrush(RGB(50, 0, 170)));
	if (!CreateWindows(nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PRAK1));
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

ATOM MyRegisterClass(HINSTANCE hInstance, const WCHAR className[], HBRUSH brush)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PRAK1));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = brush;
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_PRAK1);
	wcex.lpszClassName = className;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
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
	hwndChildWindow1 = CreateWindowW(classChild1, titleChild1, WS_OVERLAPPEDWINDOW | WS_CHILD,
		200, 150, 400, 400, hwndMainWindow, nullptr, hInst, nullptr);
	if (!hwndChildWindow1)
	{
		return FALSE;
	}
	ShowWindow(hwndChildWindow1, nCmdShow);
	UpdateWindow(hwndChildWindow1);

	//Второе окно
	hwndChildWindow2 = CreateWindowW(classChild2, titleChild2, WS_CHILD,
		700, 400, 200, 200, hwndMainWindow, nullptr, hInst, nullptr);
	if (!hwndChildWindow2)
	{
		return FALSE;
	}
	ShowWindow(hwndChildWindow2, nCmdShow);
	UpdateWindow(hwndChildWindow2);

	return TRUE;
}

//
//
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
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case IDM_COLOR:
			SelectRGB(hWnd);
			InvalidateRect(hWnd, NULL, 0);
			break;
		case IDM_CREATEFILE:
			FileCreate(hWnd);
			break;
		case IDM_READ_FILE:
			FileRead(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		if (hWnd == hwndMainWindow) {
			FillRect(hdc, &ps.rcPaint, mainBrush);
		}
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
	{
		if (hWnd == hwndMainWindow) {
			PostQuitMessage(0);
		}
		else {
			DestroyWindow(hWnd);
		}
	}
	break;
	case WM_KEYDOWN:
	{
		if (wParam == '1' && GetKeyState(VK_CONTROL) < 0) {
			moveFirst = true;
		}
		else if (wParam == '2' && GetKeyState(VK_CONTROL) < 0) {
			moveFirst = false;
		}
		else {
			HWND movingWnd = moveFirst ? hwndMainWindow : hwndChildWindow1;
			RECT rect;
			GetWindowRect(movingWnd, &rect);
			if (movingWnd != hwndMainWindow) {
				MapWindowPoints(HWND_DESKTOP, hwndMainWindow, (LPPOINT)&rect, 2);
			}
			if (wParam == VK_LEFT) {
				SetWindowPos(movingWnd, HWND_TOP, rect.left - 3, rect.top, 0, 0, SWP_NOSIZE);
			}
			else if (wParam == VK_RIGHT) {
				SetWindowPos(movingWnd, HWND_TOP, rect.left + 3, rect.top, 0, 0, SWP_NOSIZE);
			}
			else if (wParam == VK_UP) {
				SetWindowPos(movingWnd, HWND_TOP, rect.left, rect.top - 3, 0, 0, SWP_NOSIZE);
			}
			else if (wParam == VK_DOWN) {
				SetWindowPos(movingWnd, HWND_TOP, rect.left, rect.top + 3, 0, 0, SWP_NOSIZE);
			}
		}
	}
	break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
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

void SelectRGB(HWND hWnd)
{
	DWORD dColors[3] = { 0,0,0 };

	CHOOSECOLOR cc;

	cc.Flags = CC_RGBINIT | CC_FULLOPEN;
	cc.hInstance = NULL;
	cc.hwndOwner = hWnd;
	cc.lCustData = 0L;
	cc.lpCustColors = dColors;
	cc.lpfnHook = NULL;
	cc.lpTemplateName = NULL;
	cc.lStructSize = sizeof(cc);
	cc.rgbResult = RGB(100, 255, 128);

	if (ChooseColor(&cc) == TRUE)
	{
		mainBrush = CreateSolidBrush(cc.rgbResult);
	}
}

void FileCreate(HWND hWnd)
{
	hwndCreateFile = CreateDialog(hInst, MAKEINTRESOURCE(IDD_CREATEFILEDIALOG), hWnd, CreateFileDialogProc);
	ShowWindow(hwndCreateFile, SW_SHOWNORMAL);
	return;
}

INT_PTR CALLBACK CreateFileDialogProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message)
	{
	case WM_INITDIALOG:
	{
		CheckRadioButton(hWnd, IDC_RADIO_BYTE, IDC_RADIO_BOOL, IDC_RADIO_BYTE);
	}
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_ADD_ELEMENT:
		{
			HWND textBox = GetDlgItem(hWnd, IDC_ELEMENT_TEXTBOX);
			WCHAR text[300]{ 0 };
			GetWindowText(textBox, text, 300);
			if (IsDlgButtonChecked(hWnd, IDC_RADIO_STRING)) {
				int length = lstrlenW(text);
				memcpy(&fileContent[fileContentIndex], text, length * 2);
				fileContentIndex += length * 2 + 2;
			}
			else if (IsDlgButtonChecked(hWnd, IDC_RADIO_INT)) {
				int value = _wtoi(text);
				memcpy(&fileContent[fileContentIndex], &value, sizeof(int));
				fileContentIndex += sizeof(int);
			}
			else if (IsDlgButtonChecked(hWnd, IDC_RADIO_BYTE)) {
				int value = _wtoi(text);
				memcpy(&fileContent[fileContentIndex], &value, sizeof(char));
				fileContentIndex += sizeof(char);
			}
			else if (IsDlgButtonChecked(hWnd, IDC_RADIO_BOOL)) {
				BOOL value;
				if (lstrcmp(text, L"true")) {
					value = 1;
				}
				else if (lstrcmp(text, L"false")) {
					value = 0;
				}
				else {
					value = _wtoi(text);
					if (value != 0) {
						value = 1;
					}
				}
				memcpy(&fileContent[fileContentIndex], &value, sizeof(BOOL));
				fileContentIndex += sizeof(BOOL);
			}
			break;
		}
		case IDC_CREATE_FILE_BUTTON:
		{
			WCHAR fileName[260]{ 0 };
			OPENFILENAME openFile;
			ZeroMemory(&openFile, sizeof(openFile));
			openFile.lStructSize = sizeof(openFile);
			openFile.hwndOwner = hWnd;
			openFile.lpstrFile = fileName;
			openFile.nMaxFile = MAX_PATH;
			openFile.lpstrDefExt = L".txt";
			if (GetSaveFileName(&openFile)) {
				HANDLE hFile;
				hFile = CreateFile(fileName, GENERIC_WRITE, FILE_SHARE_READ, NULL,
					CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
				DWORD written = 0;
				WriteFile(hFile, fileContent, fileContentIndex, &written, NULL);
			}
			else {
				openFile.Flags = 0;
			}
			break;
		}
			
		case IDCANCEL:
			DestroyWindow(hWnd);
			hwndCreateFile = NULL;
			return TRUE;
		}
	}
	return FALSE;
}

void FileRead(HWND hWnd) {
	hwndReadFile = CreateDialog(hInst, MAKEINTRESOURCE(IDD_READ_FILE_DIALOG), hWnd, ReadFileDialogProc);
	ShowWindow(hwndReadFile, SW_SHOWNORMAL);
	return;
}

INT_PTR CALLBACK ReadFileDialogProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message)
	{
	case WM_INITDIALOG:
	{
		CheckRadioButton(hWnd, IDC_RADIO_BYTE, IDC_RADIO_BOOL, IDC_RADIO_BYTE);
	}
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_OPEN_FILE:
		{
			WCHAR fileName[260]{ 0 };
			OPENFILENAME openFile;
			ZeroMemory(&openFile, sizeof(openFile));
			openFile.lStructSize = sizeof(openFile);
			openFile.hwndOwner = hWnd;
			openFile.lpstrFile = fileName;
			openFile.nMaxFile = MAX_PATH;
			if (GetOpenFileName(&openFile)) {
				HANDLE hFile = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ, NULL,
					OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
				DWORD read = 0;
				char buffer[1024]{ 0 };
				ReadFile(hFile, buffer, 1024, &read, NULL);
				HWND dump = GetDlgItem(hWnd, IDC_DUMP_FORMAT);
				HWND value = GetDlgItem(hWnd, IDC_VALUE_FORMAT);
				std::wstring dumpText = L"";
				for (int i = 0; i < read; i++)
				{
					WCHAR buf[4]{ 0 };
					_itow_s(buffer[i], buf, 16);
					dumpText.append(buf);
					dumpText.append(L" ");
				}
				SetWindowText(dump, dumpText.c_str());
				if (IsDlgButtonChecked(hWnd, IDC_RADIO_STRING)) {
					for (int i = 0; i < read; i += 2)
					{
						if (*(WCHAR*)&buffer[i] == 0) {
							*(WCHAR*)&buffer[i] = L' ';
						}
					}
					SetWindowText(value, (WCHAR*)buffer);
				}
				else if (IsDlgButtonChecked(hWnd, IDC_RADIO_INT)) {
					std::wstring text;
					for (int i = 0; i < read - read % sizeof(int); i += sizeof(int))
					{
						int ival;
						memcpy(&ival, &buffer[i], sizeof(int));
						WCHAR buf[12]{ 0 };
						_itow_s(ival, buf, 10);
						text.append(buf).append(L" ");
					}
					SetWindowText(value, text.c_str());
				}
				else if (IsDlgButtonChecked(hWnd, IDC_RADIO_BYTE)) {
					std::wstring text;
					for (int i = 0; i < read; i++)
					{
						char ival;
						memcpy(&ival, &buffer[i], 1);
						WCHAR buf[5]{ 0 };
						_itow_s(ival, buf, 10);
						text.append(buf).append(L" ");
					}
					SetWindowText(value, text.c_str());
				}
				else if (IsDlgButtonChecked(hWnd, IDC_RADIO_INT)) {
					std::wstring text;
					for (int i = 0; i < read - read % sizeof(BOOL); i += sizeof(BOOL))
					{
						int ival;
						memcpy(&ival, &buffer[i], sizeof(BOOL));
						if(ival != 0) {
							text.append(L"true").append(L" ");
						}
						else {
							text.append(L"false").append(L" ");
						}
					}
					SetWindowText(value, text.c_str());
				}
			}
			break;
		}

		case IDCANCEL:
			DestroyWindow(hWnd);
			hwndReadFile = NULL;
			return TRUE;
		}
	}
	return FALSE;
}