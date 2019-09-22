#pragma once

class OverlayWindow {
public:
	static void RegisterOverlayClass();
	static void OpenOverlay(bool status);
	static void CloseOverlay();
	static void StatusDisabled();
	static void StatusEnabled();
	static HWND hWndOverlay, runsWnd, profitWnd, spentWnd, profitLastWnd, spentLastWnd, disableWnd;
};

LRESULT CALLBACK OverlayProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);