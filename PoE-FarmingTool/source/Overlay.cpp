#include "pch.h"
#include "Overlay.h"
#include "Settings.h"
#include "Session.h"
#include "Utilities.h"

HWND OverlayWindow::hWndOverlay = NULL;
HWND OverlayWindow::runsWnd = NULL;
HWND OverlayWindow::spentWnd = NULL;
HWND OverlayWindow::profitWnd = NULL;
HWND OverlayWindow::spentLastWnd = NULL;
HWND OverlayWindow::profitLastWnd = NULL;
HWND OverlayWindow::disableWnd = NULL;

void OverlayWindow::OpenOverlay(bool status) {
	DWORD dwExStyle = WS_EX_COMPOSITED | WS_EX_LAYERED | WS_EX_NOACTIVATE | WS_EX_TOPMOST | WS_EX_TRANSPARENT;
	DWORD dwStyle = WS_POPUP | WS_VISIBLE;

	int sizeX = GetSystemMetrics(SM_CXSCREEN); //Width of display
	int sizeY = GetSystemMetrics(SM_CYSCREEN); //Height of display
	RECT wr = { 0, 0, 210, 30 }; //Used for calculating exact client area
	AdjustWindowRect(&wr, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, TRUE); //Calculate window size
	hWndOverlay = CreateWindowEx(dwExStyle, L"myOverlayClass", NULL, dwStyle, (int)(sizeX* Settings::GetInstance().GetOverlayXPos()), (int)(sizeY* Settings::GetInstance().GetOverlayYPos()), wr.right - wr.left, wr.bottom - wr.top, 0, 0, NULL, 0);

	COLORREF RRR = RGB(255, 0, 255);
	SetLayeredWindowAttributes(hWndOverlay, RRR, (BYTE)0, LWA_COLORKEY);

	//1.Row
	CreateWindow(L"Static", L"Runs", WS_CHILD | WS_VISIBLE | SS_CENTER, 50, 0, 35, 20, hWndOverlay, NULL, NULL, NULL);
	CreateWindow(L"Static", L"Spent", WS_CHILD | WS_VISIBLE | SS_CENTER, 100, 0, 55, 20, hWndOverlay, NULL, NULL, NULL);
	CreateWindow(L"Static", L"Profit", WS_CHILD | WS_VISIBLE | SS_CENTER, 170, 0, 55, 20, hWndOverlay, NULL, NULL, NULL);

	//2.Row
	CreateWindow(L"Static", L"Total:", WS_CHILD | WS_VISIBLE | SS_CENTER, 0, 20, 35, 20, hWndOverlay, NULL, NULL, NULL);
	runsWnd = CreateWindow(L"Static", Converter<int>::ToWstring(SessionData::GetInstance().GetRuns()).c_str(), WS_CHILD | WS_VISIBLE | SS_CENTER, 50, 20, 35, 20, hWndOverlay, NULL, NULL, NULL);
	spentWnd = CreateWindow(L"Static", Converter<float>::ToWstring(SessionData::GetInstance().GetSpent()).c_str(), WS_CHILD | WS_VISIBLE | SS_CENTER, 100, 20, 55, 20, hWndOverlay, NULL, NULL, NULL);
	profitWnd = CreateWindow(L"Static", Converter<float>::ToWstring(SessionData::GetInstance().GetProfit()).c_str(), WS_CHILD | WS_VISIBLE | SS_CENTER, 170, 20, 55, 20, hWndOverlay, NULL, NULL, NULL);

	//3.Row
	CreateWindow(L"Static", L"Last:", WS_CHILD | WS_VISIBLE | SS_LEFT, 0, 40, 70, 20, hWndOverlay, NULL, NULL, NULL);
	spentLastWnd = CreateWindow(L"Static", Converter<float>::ToWstring(SessionData::GetInstance().GetLastSpent()).c_str(), WS_CHILD | WS_VISIBLE | SS_CENTER, 100, 40, 55, 20, hWndOverlay, NULL, NULL, NULL);
	profitLastWnd = CreateWindow(L"Static", Converter<float>::ToWstring(SessionData::GetInstance().GetLastProfit()).c_str(), WS_CHILD | WS_VISIBLE | SS_CENTER, 170, 40, 55, 20, hWndOverlay, NULL, NULL, NULL);

	//Show tool status when reopening overlay window
	std::wstring disabled = L"";
	if (status == true)
		disabled = L"*** DISABLED ***";

	//4.Row
	disableWnd = CreateWindow(L"Static", disabled.c_str(), WS_CHILD | WS_VISIBLE | SS_CENTER, 0, 60, 210, 30, hWndOverlay, NULL, NULL, NULL);
}

void OverlayWindow::CloseOverlay(){
	PostMessage(hWndOverlay, WM_CLOSE, NULL, NULL);
}

void OverlayWindow::StatusDisabled(){
	RedrawWindow(disableWnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE);
	SetWindowText(disableWnd, L"*** DISABLED ***");
}

void OverlayWindow::StatusEnabled(){
	RedrawWindow(disableWnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE);
	SetWindowText(disableWnd, L"");
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