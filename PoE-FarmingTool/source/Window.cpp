#include "pch.h"
#include "Window.h"

//declaration of static members
HMENU MainWindow::hMenu = NULL;
HWND MainWindow::mapNameWnd = NULL;
HWND MainWindow::runsWnd = NULL;
HWND MainWindow::IIQWnd = NULL;
HWND MainWindow::CSWnd = NULL;
HWND MainWindow::spentWnd = NULL;
HWND MainWindow::profitWnd = NULL;
HWND MainWindow::itemMovedWnd = NULL;
HWND MainWindow::itemStatusWnd = NULL;
HWND MainWindow::locWnd = NULL;
HWND MainWindow::mapPriceWnd = NULL;
HWND MainWindow::leagueWnd = NULL;
HWND MainWindow::patreonWnd = NULL;
HWND MainWindow::gitWnd = NULL;
HWND MainWindow::dicordWnd = NULL;
HWND MainWindow::overlayButton = NULL;
HWND MainWindow::lastProfitWnd = NULL;
HWND MainWindow::lastSpentWnd = NULL;
HWND MainWindow::historyWnd = NULL;
HWND MainWindow::profitButton = NULL;
HWND MainWindow::spentButton = NULL;
HWND MainWindow::updateButton = NULL;
bool MainWindow::overlayOn = false;

//global
HFONT hFontTitle = CreateFont(18, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, TEXT("Tahoma"));

void MainWindow::ShowLabels(HDC hdc) {
	//1. Row
	TextOut(hdc, 20, 40, L"Map:", static_cast<int>(wcslen(L"Map:")));
	TextOut(hdc, 255, 40, L"League:", static_cast<int>(wcslen(L"League:")));
	//2. Row
	TextOut(hdc, 20, 70, L"Map price:", static_cast<int>(wcslen(L"Map price:")));
	TextOut(hdc, 155, 70, L"IIQ:", static_cast<int>(wcslen(L"IIQ:")));
	TextOut(hdc, 245, 70, L"CS:", static_cast<int>(wcslen(L"CS:")));
	//3. Row
	TextOut(hdc, 20, 100, L"Runs:", static_cast<int>(wcslen(L"Runs:")));
	TextOut(hdc, 127, 100, L"Spent:", static_cast<int>(wcslen(L"Spent:")));
	TextOut(hdc, 245, 100, L"Profit:", static_cast<int>(wcslen(L"Profit:")));
	//4. Row
	TextOut(hdc, 20, 130, L"Last run ------------>", static_cast<int>(wcslen(L"Last run ------------>")));
	TextOut(hdc, 127, 130, L"Spent:", static_cast<int>(wcslen(L"Spent:")));
	TextOut(hdc, 245, 130, L"Profit:", static_cast<int>(wcslen(L"Profit:")));
	//5. Row
	TextOut(hdc, 20, 160, L"Item moved:", static_cast<int>(wcslen(L"Item moved:")));
	//6. Row
	TextOut(hdc, 20, 190, L"Status:", static_cast<int>(wcslen(L"Status:")));
	TextOut(hdc, 215, 190, L"Location:", static_cast<int>(wcslen(L"Location:")));
	//7. Row
	TextOut(hdc, 20, 220, L"History:", static_cast<int>(wcslen(L"History:")));
	//Title
	SelectObject(hdc, hFontTitle);
	TextOut(hdc, 140, 10, L"PoE - Farming Tool", static_cast<int>(wcslen(L"PoE - Farming Tool")));
}

void MainWindow::ShowUpdatingText(HWND hWnd) {
	DWORD dwStyleText = WS_CHILD | WS_VISIBLE | ES_CENTER | WS_BORDER;
	DWORD dwStyleButton = WS_CHILD | WS_VISIBLE | ES_CENTER | BS_FLAT | BS_TEXT;
	DWORD dwStyleImage = WS_CHILD | WS_VISIBLE | SS_BITMAP | WS_BORDER | SS_NOTIFY;
	//1.Row
	mapNameWnd = CreateWindow(L"Static", NULL, dwStyleText, 55, 40, 180, 20, hWnd, NULL, NULL, NULL);
	leagueWnd = CreateWindow(L"Static", NULL, dwStyleText, 305, 40, 95, 20, hWnd, NULL, NULL, NULL);
	//2. Row
	mapPriceWnd = CreateWindow(L"Static", NULL, dwStyleText, 85, 70, 50, 20, hWnd, NULL, NULL, NULL);
	IIQWnd= CreateWindow(L"Static", NULL, dwStyleText, 177, 70, 50, 20, hWnd, NULL, NULL, NULL);
	CSWnd = CreateWindow(L"Static", NULL, dwStyleText, 270, 70, 70, 20, hWnd, NULL, NULL, NULL);
	overlayButton = CreateWindow(L"Button", L"Overlay", dwStyleButton, 350, 70, 50, 20, hWnd, NULL, NULL, NULL);
	//3. Row
	runsWnd = CreateWindow(L"Static", NULL, dwStyleText, 57, 100, 50, 20, hWnd, NULL, NULL, NULL);
	spentWnd = CreateWindow(L"Static", NULL, dwStyleText, 167, 100, 60, 20, hWnd, NULL, NULL, NULL);
	profitWnd = CreateWindow(L"Static", NULL, dwStyleText, 280, 100, 60, 20, hWnd, NULL, NULL, NULL);
	profitButton = CreateWindow(L"Button", L"+Profit", dwStyleButton, 350, 100, 50, 20, hWnd, NULL, NULL, NULL);
	//4. Row
	lastSpentWnd = CreateWindow(L"Static", NULL, dwStyleText, 167, 130, 60, 20, hWnd, NULL, NULL, NULL);
	lastProfitWnd = CreateWindow(L"Static", NULL, dwStyleText, 280, 130, 60, 20, hWnd, NULL, NULL, NULL);
	spentButton = CreateWindow(L"Button", L"+Spent", dwStyleButton, 350, 130, 50, 20, hWnd, NULL, NULL, NULL);
	//5. Row
	itemMovedWnd = CreateWindow(L"Static", NULL, dwStyleText, 90, 160, 310, 20, hWnd, NULL, NULL, NULL);
	//6. Row
	itemStatusWnd = CreateWindow(L"Static", NULL, dwStyleText, 60, 190, 140, 20, hWnd, NULL, NULL, NULL);
	locWnd = CreateWindow(L"Static", NULL, dwStyleText, 270, 190, 130, 20, hWnd, NULL, NULL, NULL);
	//7. Row (Lisbox/History)
	historyWnd = CreateWindow(L"Listbox", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL, 20, 240, 380, 120, hWnd, NULL, NULL, NULL);
	//8. Row
	patreonWnd = CreateWindow(L"Static", NULL, dwStyleImage, 20, 370, 32, 32, hWnd, NULL, NULL, NULL);
	HBITMAP hImagePatreon = (HBITMAP)LoadImage(NULL, L"resources\\Patreon_Mark_Black.bmp", IMAGE_BITMAP, 32, 32, LR_LOADFROMFILE);
	SendMessage(patreonWnd, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hImagePatreon); 

	gitWnd = CreateWindow(L"Static", NULL, dwStyleImage, 62, 370, 32, 32, hWnd, NULL, NULL, NULL);
	HBITMAP hImageGit = (HBITMAP)LoadImage(NULL, L"resources\\GitHub-Mark-32px.bmp", IMAGE_BITMAP, 32, 32, LR_LOADFROMFILE);
	SendMessage(gitWnd, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hImageGit);

	dicordWnd = CreateWindow(L"Static", NULL, dwStyleImage, 104, 370, 32, 32, hWnd, NULL, NULL, NULL);
	HBITMAP hImageDiscord = (HBITMAP)LoadImage(NULL, L"resources\\Discord-Logo-Color.bmp", IMAGE_BITMAP, 32, 32, LR_LOADFROMFILE);
	SendMessage(dicordWnd, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hImageDiscord);

	updateButton = CreateWindow(L"Button", L"Update Available!", WS_CHILD | ES_CENTER | BS_FLAT | BS_TEXT, 280, 370, 120, 32, hWnd, NULL, NULL, NULL);
}

void MainWindow::AddMenus(HWND hWnd) {
	hMenu = CreateMenu();
	HMENU hFileMenu = CreateMenu();
	HMENU hSetMenu = CreateMenu();
	HMENU hCurrencyMenu = CreateMenu();
	HMENU hHelpMenu = CreateMenu();
	AppendMenu(hSetMenu, MF_STRING, SETTINGS_MENU_MAP, L"Map");
	AppendMenu(hSetMenu, MF_STRING, SETTINGS_MENU_IIQ, L"IIQ");
	AppendMenu(hSetMenu, MF_STRING, SETTINGS_MENU_CS, L"Sextants");
	AppendMenu(hSetMenu, MF_STRING, SETTINGS_MENU_LEAGUE, L"League");
	AppendMenu(hSetMenu, MF_STRING, SETTINGS_MENU_OVERLAY, L"Overlay");
	AppendMenu(hCurrencyMenu, MF_STRING, CURRENCY_MENU_UPDATE, L"Update Rates");
	AppendMenu(hFileMenu, MF_STRING, FILE_MENU_RESET, L"Reset Data");
	AppendMenu(hFileMenu, MF_STRING, FILE_MENU_EXIT, L"Exit");
	AppendMenu(hHelpMenu, MF_STRING, HELP_TIPS, L"Tips");
	AppendMenu(hHelpMenu, MF_STRING, HELP_ABOUT, L"About");
	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, L"File");
	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hSetMenu, L"Settings");
	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hCurrencyMenu, L"Currency");
	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hHelpMenu, L"Help");
	SetMenu(hWnd, hMenu);
}