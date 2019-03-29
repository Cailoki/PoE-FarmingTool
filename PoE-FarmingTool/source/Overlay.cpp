#include "pch.h"
#include "Overlay.h"

HWND OverlayWindow::hWndOverlay = NULL;
HWND OverlayWindow::runsWnd = NULL;
HWND OverlayWindow::spentWnd = NULL;
HWND OverlayWindow::profitWnd = NULL;
HWND OverlayWindow::spentLastWnd = NULL;
HWND OverlayWindow::profitLastWnd = NULL;
HWND OverlayWindow::disableWnd = NULL;
float OverlayWindow::xPos = 0;
float OverlayWindow::yPos = 0;

void OverlayWindow::SetPosition(float x, float y) {
	xPos = x;
	yPos = y;
}

void OverlayWindow::SetOverlayWindow(wchar_t runs[5], wchar_t spent[20], wchar_t profit[20]) {
	DWORD dwExStyle = WS_EX_COMPOSITED | WS_EX_LAYERED | WS_EX_NOACTIVATE | WS_EX_TOPMOST | WS_EX_TRANSPARENT;
	DWORD dwStyle = WS_POPUP | WS_VISIBLE;

	int sizeX = GetSystemMetrics(SM_CXSCREEN); //width of display
	int sizeY = GetSystemMetrics(SM_CYSCREEN); //height of display
	RECT wr = { 0, 0, 210, 25 }; //used for calculating exact client area
	AdjustWindowRect(&wr, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, TRUE); //calculate window size
	hWndOverlay = CreateWindowEx(dwExStyle, L"myOverlayClass", NULL, dwStyle, sizeX*xPos, sizeY*yPos, wr.right - wr.left, wr.bottom - wr.top, 0, 0, NULL, 0);

	COLORREF RRR = RGB(255, 0, 255);
	SetLayeredWindowAttributes(hWndOverlay, RRR, (BYTE)0, LWA_COLORKEY);

	//1.Row
	CreateWindow(L"Static", L"Runs", WS_CHILD | WS_VISIBLE | SS_CENTER, 50, 0, 35, 20, hWndOverlay, NULL, NULL, NULL);
	CreateWindow(L"Static", L"Spent", WS_CHILD | WS_VISIBLE | SS_CENTER, 100, 0, 55, 20, hWndOverlay, NULL, NULL, NULL);
	CreateWindow(L"Static", L"Profit", WS_CHILD | WS_VISIBLE | SS_CENTER, 170, 0, 55, 20, hWndOverlay, NULL, NULL, NULL);

	//2.Row
	CreateWindow(L"Static", L"Total:", WS_CHILD | WS_VISIBLE | SS_CENTER, 0, 20, 35, 20, hWndOverlay, NULL, NULL, NULL);
	runsWnd = CreateWindow(L"Static", runs, WS_CHILD | WS_VISIBLE | SS_CENTER, 50, 20, 35, 20, hWndOverlay, NULL, NULL, NULL);
	spentWnd = CreateWindow(L"Static", spent, WS_CHILD | WS_VISIBLE | SS_CENTER, 100, 20, 55, 20, hWndOverlay, NULL, NULL, NULL);
	profitWnd = CreateWindow(L"Static", profit, WS_CHILD | WS_VISIBLE | SS_CENTER, 170, 20, 55, 20, hWndOverlay, NULL, NULL, NULL);

	//3.Row
	CreateWindow(L"Static", L"Last run:", WS_CHILD | WS_VISIBLE | SS_LEFT, 0, 40, 70, 20, hWndOverlay, NULL, NULL, NULL);
	spentLastWnd = CreateWindow(L"Static", L"0.00", WS_CHILD | WS_VISIBLE | SS_CENTER, 100, 40, 55, 20, hWndOverlay, NULL, NULL, NULL);
	profitLastWnd = CreateWindow(L"Static", L"0.00", WS_CHILD | WS_VISIBLE | SS_CENTER, 170, 40, 55, 20, hWndOverlay, NULL, NULL, NULL);

	//4.Row
	disableWnd = CreateWindow(L"Static", L"", WS_CHILD | WS_VISIBLE | SS_CENTER, 0, 60, 210, 20, hWndOverlay, NULL, NULL, NULL);
}

void OverlayWindow::RegisterOverlayClass() {
	WNDCLASSEX overlay;

	overlay.cbSize = sizeof(WNDCLASSEXW);
	overlay.style = NULL;
	overlay.lpfnWndProc = OverlayProcedure;
	overlay.cbClsExtra = 0;
	overlay.cbWndExtra = 0;
	overlay.hInstance = NULL;
	overlay.hIcon = NULL;
	overlay.hCursor = NULL;
	overlay.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	overlay.lpszMenuName = NULL;
	overlay.lpszClassName = L"myOverlayClass";
	overlay.hIconSm = NULL;

	RegisterClassEx(&overlay);
}