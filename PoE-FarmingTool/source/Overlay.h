#pragma once

class OverlayWindow {
public:
	static void RegisterOverlayClass();
	static void SetOverlayWindow(wchar_t runs[5], wchar_t spent[20], wchar_t profit[20]);
	static HWND hWndOverlay, runsWnd, profitWnd, spentWnd, profitLastWnd, spentLastWnd, disableWnd;
	static void SetPosition(float x, float y);
	static float GetXPos() { return xPos; }
	static float GetYPos() { return yPos; }
private:
	static float xPos, yPos;
};

LRESULT CALLBACK OverlayProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);