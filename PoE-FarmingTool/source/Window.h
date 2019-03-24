#pragma once

#define SETTINGS_MENU_MAP 1
#define SETTINGS_MENU_IIQ 2
#define FILE_MENU_EXIT 3
#define SETTINGS_MENU_CS 4
#define CURRENCY_MENU_UPDATE 5
#define SETTINGS_MENU_LEAGUE 6
#define SETTINGS_MENU_OVERLAY 7
#define FILE_MENU_RESET 19
#define HELP_ABOUT 20
#define HELP_TIPS 21

class MainWindow {
public:
	static void ShowLabels(HDC);
	static void AddMenus(HWND);
	static void ShowUpdatingText(HWND);
	static bool overlayOn;
	static HMENU hMenu;
	static HWND mapNameWnd, runsWnd, spentWnd, profitWnd, itemMovedWnd, itemStatusWnd, locWnd,
				IIQWnd, CSWnd, mapPriceWnd, leagueWnd, patreonWnd, gitWnd, dicordWnd, overlayButton,
				lastSpentWnd, lastProfitWnd, historyWnd, profitButton, spentButton, updateButton;
};