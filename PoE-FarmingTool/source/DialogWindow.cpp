#include "pch.h"
#include "DialogWindow.h"

//definition of static members
HWND DialogWindow::setMapWnd = NULL;
HWND DialogWindow::setIIQWnd = NULL;
HWND DialogWindow::setCSWnd = NULL;
HWND DialogWindow::hProgress = NULL;
HWND DialogWindow::updateStatusWnd = NULL;
HWND DialogWindow::setMapPriceWnd = NULL;
HWND DialogWindow::generalCheckbox = NULL;
HWND DialogWindow::setXPosWnd = NULL;
HWND DialogWindow::setYPosWnd = NULL;
HWND DialogWindow::plusProfitWnd = NULL;
HWND DialogWindow::plusSpentWnd = NULL;
HWND DialogWindow::setZanaModPrice = NULL;
HWND DialogWindow::logLoc = NULL;
HWND DialogWindow::comboBoxWnd = NULL;
HWND DialogWindow::updateButton = NULL;
bool DialogWindow::isGeneral = true;

//global styles
DWORD dwStyleText = WS_CHILD | WS_VISIBLE | ES_CENTER;
DWORD dwStyleButton = WS_CHILD | WS_VISIBLE | ES_CENTER | BS_FLAT | BS_TEXT;

//map farming type
void DialogWindow::SetIsGeneral(bool status) {
	isGeneral = status;
}

void DialogWindow::OpenMapDialog(HWND hWnd, wchar_t mapNameWnd[50], wchar_t mapPriceWnd[10], wchar_t zanaModPrice[10]) {
	RECT rect;
	GetWindowRect(hWnd, &rect);
	HWND hDlg = CreateWindowEx(NULL, L"myDialogClass", L"Set Map", WS_BORDER | WS_VISIBLE | WS_OVERLAPPEDWINDOW, rect.left + 25, rect.top + 25, 260, 255, NULL, NULL, NULL, NULL);
	CreateWindow(L"Static", L"Enter name of the map:", dwStyleText, 20, 10, 200, 20, hDlg, NULL, NULL, NULL);
	setMapWnd = CreateWindow(L"Edit", mapNameWnd, WS_CHILD | WS_VISIBLE | ES_CENTER | WS_BORDER, 20, 35, 200, 20, hDlg, NULL, NULL, NULL);
	CreateWindow(L"Static", L"(If price is set to 0 use online price.)", WS_CHILD | WS_VISIBLE, 20, 60, 200, 20, hDlg, NULL, NULL, NULL);
	CreateWindow(L"Static", L"Map price:", WS_CHILD | WS_VISIBLE, 20, 85, 80, 20, hDlg, NULL, NULL, NULL);
	setMapPriceWnd = CreateWindow(L"Edit", mapPriceWnd, WS_CHILD | WS_VISIBLE | ES_CENTER | WS_BORDER | ES_NUMBER, 85, 85, 40, 20, hDlg, NULL, NULL, NULL);
	CreateWindow(L"Static", L"Zana mod price:", WS_CHILD | WS_VISIBLE, 20, 110, 90, 20, hDlg, NULL, NULL, NULL);
	setZanaModPrice = CreateWindow(L"Edit", zanaModPrice, WS_CHILD | WS_VISIBLE | ES_CENTER | WS_BORDER | ES_NUMBER, 115, 110, 40, 20, hDlg, NULL, NULL, NULL);
	CreateWindow(L"Static", L"General map farming:", WS_CHILD | WS_VISIBLE, 20, 135, 130, 18, hDlg, NULL, NULL, NULL);
	generalCheckbox = CreateWindow(L"BUTTON", NULL, WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 150, 135, 20, 20, hDlg, (HMENU)DIALOG_WINDOW_CHECKBOX_GENERAL, NULL, NULL);
	CreateWindow(L"Button", L"Confirm", dwStyleButton, 40, 165, 70, 30, hDlg, (HMENU)DIALOG_WINDOW_SAVE_MAP, NULL, NULL);
	CreateWindow(L"Button", L"Cancel", dwStyleButton, 130, 165, 70, 30, hDlg, (HMENU)DIALOG_WINDOW_CANCEL, NULL, NULL);

	if (isGeneral) {
		SendMessage(generalCheckbox, BM_SETCHECK, BST_CHECKED, 0);
		SetWindowText(setMapWnd, L"General");
		EnableWindow(setMapWnd, false);
		EnableWindow(setMapPriceWnd, false);
	}
}

void DialogWindow::OpenIIQDialog(HWND hWnd, wchar_t iiq[5]) {
	RECT rect;
	GetWindowRect(hWnd, &rect);
	HWND hDlg = CreateWindowEx(NULL, L"myDialogClass", L"Set IIQ", WS_BORDER | WS_VISIBLE, rect.left + 25, rect.top + 25, 260, 180, NULL, NULL, NULL, NULL);
	CreateWindow(L"Static", L"Enter total IIQ on gear:", dwStyleText, 20, 10, 200, 20, hDlg, NULL, NULL, NULL);
	setIIQWnd = CreateWindow(L"Edit", iiq, WS_CHILD | WS_VISIBLE | ES_CENTER | WS_BORDER | ES_NUMBER, 20, 35, 200, 20, hDlg, NULL, NULL, NULL);
	CreateWindow(L"Button", L"Confirm", dwStyleButton, 40, 90, 70, 30, hDlg, (HMENU)DIALOG_WINDOW_SAVE_IIQ, NULL, NULL);
	CreateWindow(L"Button", L"Cancel", dwStyleButton, 130, 90, 70, 30, hDlg, (HMENU)DIALOG_WINDOW_CANCEL, NULL, NULL);
}

void DialogWindow::OpenSextantDialog(HWND hWnd, wchar_t wCS[10]) {
	RECT rect;
	GetWindowRect(hWnd, &rect);
	HWND hDlg = CreateWindowEx(NULL, L"myDialogClass", L"Set CS", WS_BORDER | WS_VISIBLE, rect.left + 25, rect.top + 25, 260, 190, NULL, NULL, NULL, NULL);
	CreateWindow(L"Static", L"Set combination of Sextants Apprentice-Journey-Master", dwStyleText, 20, 10, 200, 35, hDlg, NULL, NULL, NULL);
	setCSWnd = CreateWindow(L"Edit", wCS, WS_CHILD | WS_VISIBLE | ES_CENTER | WS_BORDER, 20, 55, 200, 20, hDlg, NULL, NULL, NULL);
	CreateWindow(L"Button", L"Confirm", dwStyleButton, 40, 100, 70, 30, hDlg, (HMENU)DIALOG_WINDOW_SAVE_CS, NULL, NULL);
	CreateWindow(L"Button", L"Cancel", dwStyleButton, 130, 100, 70, 30, hDlg, (HMENU)DIALOG_WINDOW_CANCEL, NULL, NULL);
}

void DialogWindow::OpenLeagueDialog(HWND hWnd) {
	RECT rect;
	GetWindowRect(hWnd, &rect);
	HWND hDlg = CreateWindowEx(NULL, L"myDialogClass", L"Set CS", WS_BORDER | WS_VISIBLE, rect.left + 25, rect.top + 25, 260, 180, NULL, NULL, NULL, NULL);
	CreateWindow(L"Static", L"Set League", dwStyleText, 20, 10, 200, 20, hDlg, NULL, NULL, NULL);
	comboBoxWnd = CreateWindow(L"ComboBox", NULL, WS_CHILD | WS_VISIBLE | CBS_DROPDOWN | SS_CENTER, 65, 35, 120, 60, hDlg, NULL, NULL, NULL); //SS_CENTER doesn't center text but makes it a bit cleaner when selecting options
	SendMessage(comboBoxWnd, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(L"Synthesis"));
	SendMessage(comboBoxWnd, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(L"HC Synthesis"));
	SendMessage(comboBoxWnd, CB_SETCURSEL, 0, NULL);
	CreateWindow(L"Button", L"Confirm", dwStyleButton, 40, 90, 70, 30, hDlg, (HMENU)DIALOG_WINDOW_SAVE_LEAGUE, NULL, NULL);
	CreateWindow(L"Button", L"Cancel", dwStyleButton, 130, 90, 70, 30, hDlg, (HMENU)DIALOG_WINDOW_CANCEL, NULL, NULL);
}

void DialogWindow::UpdateCurrencyDialog(HWND hWnd, int steps) {
	RECT rect;
	GetWindowRect(hWnd, &rect);
	HWND hDlg = CreateWindowEx(NULL, L"myDialogClass", L"Updating Currency", WS_BORDER | WS_VISIBLE, rect.left + 25, rect.top + 25, 260, 200, NULL, NULL, NULL, NULL);
	updateStatusWnd = CreateWindow(L"Static", L"Update Currency Rates", dwStyleText, 20, 20, 200, 20, hDlg, NULL, NULL, NULL);
	hProgress = CreateWindowEx(0, PROGRESS_CLASS, (LPTSTR)NULL, WS_CHILD | WS_VISIBLE | PBS_SMOOTH, 20, 50, 200, 30, hDlg, NULL, NULL, NULL);
	updateButton = CreateWindow(L"Button", L"Update", dwStyleButton, 40, 100, 70, 30, hDlg, (HMENU)DIALOG_WINDOW_UPDATE_CURRENCY, NULL, NULL);
	CreateWindow(L"Button", L"Close", dwStyleButton, 130, 100, 70, 30, hDlg, (HMENU)DIALOG_WINDOW_CANCEL, NULL, NULL);
	SendMessage(hProgress, PBM_SETRANGE, 0, MAKELPARAM(0, steps));
	SendMessage(hProgress, PBM_SETSTEP, (WPARAM)1, 0);
}

void DialogWindow::OpenOverlayDialog(HWND hWnd,const float& x, const float& y) {
	RECT rect;
	GetWindowRect(hWnd, &rect);
	wchar_t xp[5], yp[5]; //used to show last saved position of the overlay window
	StringCbPrintfW(xp, 10, L"%.2f", x);
	StringCbPrintfW(yp, 10, L"%.2f", y);
	HWND hDlg = CreateWindowEx(NULL, L"myDialogClass", L"Overlay Settings", WS_BORDER | WS_VISIBLE, rect.left + 25, rect.top + 25, 260, 180, NULL, NULL, NULL, NULL);
	CreateWindow(L"Static", L"Set xPos 0.00% - 0.95%", dwStyleText, 20, 10, 150, 20, hDlg, NULL, NULL, NULL);
	setXPosWnd = CreateWindow(L"Edit", xp, WS_CHILD | WS_VISIBLE | ES_CENTER | WS_BORDER, 170, 10, 50, 20, hDlg, NULL, NULL, NULL);
	CreateWindow(L"Static", L"Set yPos 0.00% - 0.95%", dwStyleText, 20, 35, 150, 20, hDlg, NULL, NULL, NULL);
	setYPosWnd = CreateWindow(L"Edit", yp, WS_CHILD | WS_VISIBLE | ES_CENTER | WS_BORDER, 170, 35, 50, 20, hDlg, NULL, NULL, NULL);
	CreateWindow(L"Button", L"Confirm", dwStyleButton, 40, 80, 70, 30, hDlg, (HMENU)DIALOG_WINDOW_SAVE_OVERLAY_SETTINGS, NULL, NULL);
	CreateWindow(L"Button", L"Cancel", dwStyleButton, 130, 80, 70, 30, hDlg, (HMENU)DIALOG_WINDOW_CANCEL, NULL, NULL);
}

void DialogWindow::OpenPlusProfitDialog(HWND hWnd) {
	RECT rect;
	GetWindowRect(hWnd, &rect);
	HWND hDlg = CreateWindowEx(NULL, L"myDialogClass", L"+Profit", WS_BORDER | WS_VISIBLE, rect.left + 25, rect.top + 25, 260, 180, NULL, NULL, NULL, NULL);
	CreateWindow(L"Static", L"Add value to profit(+/-):", dwStyleText, 20, 10, 200, 20, hDlg, NULL, NULL, NULL);
	plusProfitWnd = CreateWindow(L"Edit", L"0.00", WS_CHILD | WS_VISIBLE | ES_CENTER | WS_BORDER, 20, 35, 200, 20, hDlg, NULL, NULL, NULL);
	CreateWindow(L"Button", L"Confirm", dwStyleButton, 40, 90, 70, 30, hDlg, (HMENU)DIALOG_WINDOW_PLUS_PROFIT, NULL, NULL);
	CreateWindow(L"Button", L"Cancel", dwStyleButton, 130, 90, 70, 30, hDlg, (HMENU)DIALOG_WINDOW_CANCEL, NULL, NULL);
}

void DialogWindow::OpenPlusSpentDialog(HWND hWnd) {
	RECT rect;
	GetWindowRect(hWnd, &rect);
	HWND hDlg = CreateWindowEx(NULL, L"myDialogClass", L"+Spent", WS_BORDER | WS_VISIBLE, rect.left + 25, rect.top + 25, 260, 180, NULL, NULL, NULL, NULL);
	CreateWindow(L"Static", L"Add value to spent(+/-):", dwStyleText, 20, 10, 200, 20, hDlg, NULL, NULL, NULL);
	plusSpentWnd = CreateWindow(L"Edit", L"0.00", WS_CHILD | WS_VISIBLE | ES_CENTER | WS_BORDER, 20, 35, 200, 20, hDlg, NULL, NULL, NULL);
	CreateWindow(L"Static", L"NOTE: changing spent value also changes profit by the same amount", dwStyleText, 20, 60, 200, 40, hDlg, NULL, NULL, NULL);
	CreateWindow(L"Button", L"Confirm", dwStyleButton, 40, 100, 70, 30, hDlg, (HMENU)DIALOG_WINDOW_PLUS_SPENT, NULL, NULL);
	CreateWindow(L"Button", L"Cancel", dwStyleButton, 130, 100, 70, 30, hDlg, (HMENU)DIALOG_WINDOW_CANCEL, NULL, NULL);
}

void DialogWindow::OpenTipsDialog(HWND hWnd) {
	RECT rect;
	GetWindowRect(hWnd, &rect);
	HWND hDlg = CreateWindowEx(NULL, L"myDialogClass", L"Tips", WS_BORDER | WS_VISIBLE | WS_OVERLAPPEDWINDOW, rect.left + 25, rect.top + 25, 300, 200, NULL, NULL, NULL, NULL);
	CreateWindow(L"Static", L"Tips", WS_VISIBLE | WS_CHILD | SS_CENTER, 40, 5, 200, 150, hDlg, NULL, NULL, NULL);
	CreateWindow(L"Static", L"Shortcuts:\nCtrl+LMB - Parse items\nCtrl+RMB - Disable/Enable item parsing", WS_VISIBLE | WS_CHILD, 20, 25, 250, 80, hDlg, NULL, NULL, NULL);
}

void DialogWindow::OpenAboutDialog(HWND hWnd) {
	RECT rect;
	GetWindowRect(hWnd, &rect);
	HWND hDlg = CreateWindowEx(NULL, L"myDialogClass", L"About", WS_BORDER | WS_VISIBLE | WS_OVERLAPPEDWINDOW, rect.left + 25, rect.top + 25, 260, 240, NULL, NULL, NULL, NULL);
	CreateWindow(L"Static", L"PoE Farming Tool", WS_VISIBLE | WS_CHILD | SS_CENTER, 20, 20, 200, 150, hDlg, NULL, NULL, NULL);
	CreateWindow(L"Static", L"Author: Cailoki", WS_VISIBLE | WS_CHILD, 20, 60, 200, 150, hDlg, NULL, NULL, NULL);
	CreateWindow(L"Static", L"Home page: github.com/Cailoki", WS_VISIBLE | WS_CHILD, 20, 80, 300, 150, hDlg, NULL, NULL, NULL);
	CreateWindow(L"Static", L"License: GNU GPL v3.0", WS_VISIBLE | WS_CHILD, 20, 100, 200, 150, hDlg, NULL, NULL, NULL);
	CreateWindow(L"Static", L"Version: 1.0.0", WS_VISIBLE | WS_CHILD, 20, 120, 200, 150, hDlg, NULL, NULL, NULL);
}

void DialogWindow::OpenSetLogLocation() {
	RECT wr = { 0, 0, 420, 120 }; //exact client area
	AdjustWindowRect(&wr, WS_BORDER | WS_VISIBLE | WS_OVERLAPPEDWINDOW, TRUE);
	HWND hDlg = CreateWindow(L"myDialogClass", L"Set Map", WS_BORDER | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, wr.right - wr.left, wr.bottom - wr.top, NULL, NULL, NULL, NULL);
	CreateWindow(L"Static", L"Client.txt not found, please edit the \"logs\" folder location below!", WS_CHILD | WS_VISIBLE | SS_CENTER, 10, 10, 410, 20, hDlg, NULL, NULL, NULL);
	logLoc = CreateWindow(L"Edit", L"C:\\Program Files (x86)\\Steam\\steamapps\\common\\Path of Exile\\logs", WS_CHILD | WS_VISIBLE | SS_CENTER | ES_MULTILINE, 10, 35, 400, 40, hDlg, NULL, NULL, NULL);
	CreateWindow(L"Button", L"Confirm", dwStyleButton, 175, 90, 70, 30, hDlg, (HMENU)DIALOG_WINDOW_SAVELOC, NULL, NULL);
}

void DialogWindow::RegisterDialogClass() {
	WNDCLASSEX dialog;

	dialog.cbSize = sizeof(WNDCLASSEXW);
	dialog.style = NULL;
	dialog.lpfnWndProc = DialogProcedure;
	dialog.cbClsExtra = 0;
	dialog.cbWndExtra = 0;
	dialog.hInstance = NULL;
	dialog.hIcon = NULL;
	dialog.hCursor = NULL;
	dialog.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	dialog.lpszMenuName = NULL;
	dialog.lpszClassName = L"myDialogClass";
	dialog.hIconSm = NULL;

	RegisterClassEx(&dialog);
}