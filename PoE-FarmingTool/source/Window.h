#pragma once

class MainWindow {
public:
	//Main window elements
	static void ShowLabels(const HDC&);
	static void AddMenus(const HWND&);
	static void ShowUpdatingText(const HWND&);
	//Elements control
	static void RedrawData();
	static void RedrawSettings();
	static void DrawLocation(const std::wstring& loc);
	//History control
	static void AddToHistory(const std::wstring& data);
	static void CleanHistory(const int& count);
	//Fields
	static HMENU hMenu;
	static bool overlayOn;
	static HWND mapNameWnd, runsWnd, spentWnd, profitWnd, itemMovedWnd, itemStatusWnd, locWnd,
		IIQWnd, CSWnd, mapPriceWnd, leagueWnd, patreonWnd, gitWnd, discordWnd, overlayButton,
		lastSpentWnd, lastProfitWnd, historyWnd, profitButton, spentButton, newVerAvailableButton;
};