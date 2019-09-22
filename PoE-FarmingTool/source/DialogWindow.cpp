#include "pch.h"
#include "DialogWindow.h"
#include "Constants.h"
#include "Utilities.h"
#include "Settings.h"

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
HWND DialogWindow::trashCheckbox = NULL;
HWND DialogWindow::updateButton = NULL;

//global styles
DWORD dwStyleText = WS_CHILD | WS_VISIBLE | ES_CENTER;
DWORD dwStyleButton = WS_CHILD | WS_VISIBLE | ES_CENTER | BS_FLAT | BS_TEXT;

void DialogWindow::OpenMapDialog(HWND hWnd) {
	RECT rect;
	GetWindowRect(hWnd, &rect); //Parent window
	HWND hDlg = CreateWindowEx(NULL, L"myDialogClass", L"Set Map", WS_BORDER | WS_VISIBLE | WS_OVERLAPPEDWINDOW, rect.left + 25, rect.top + 25, 260, 280, NULL, NULL, NULL, NULL);
	CreateWindow(L"Static", L"Enter name of the map:", dwStyleText, 20, 10, 200, 20, hDlg, NULL, NULL, NULL);
	setMapWnd = CreateWindow(L"Edit", Settings::GetInstance().GetMapName().c_str(), WS_CHILD | WS_VISIBLE | ES_CENTER | WS_BORDER, 20, 35, 200, 20, hDlg, NULL, NULL, NULL);
	CreateWindow(L"Static", L"(If price is set to 0 use online price.)", WS_CHILD | WS_VISIBLE, 20, 60, 200, 20, hDlg, NULL, NULL, NULL);
	CreateWindow(L"Static", L"Map price:", WS_CHILD | WS_VISIBLE, 20, 85, 80, 20, hDlg, NULL, NULL, NULL);
	setMapPriceWnd = CreateWindow(L"Edit", Converter<float>::ToWstring(Settings::GetInstance().GetMapPrice()).c_str(), WS_CHILD | WS_VISIBLE | ES_CENTER | WS_BORDER | ES_NUMBER, 85, 85, 40, 20, hDlg, NULL, NULL, NULL);
	CreateWindow(L"Static", L"Zana mod price:", WS_CHILD | WS_VISIBLE, 20, 110, 90, 20, hDlg, NULL, NULL, NULL);
	setZanaModPrice = CreateWindow(L"Edit", Converter<int>::ToWstring(Settings::GetInstance().GetModPrice()).c_str(), WS_CHILD | WS_VISIBLE | ES_CENTER | WS_BORDER | ES_NUMBER, 115, 110, 40, 20, hDlg, NULL, NULL, NULL);
	CreateWindow(L"Static", L"General map farming:", WS_CHILD | WS_VISIBLE, 20, 135, 130, 18, hDlg, NULL, NULL, NULL);
	generalCheckbox = CreateWindow(L"BUTTON", NULL, WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 150, 135, 20, 20, hDlg, (HMENU)DIALOG_WINDOW_CHECKBOX_GENERAL, NULL, NULL);
	CreateWindow(L"Static", L"Ignore trash?", WS_CHILD | WS_VISIBLE, 20, 150, 130, 18, hDlg, NULL, NULL, NULL);
	trashCheckbox = CreateWindow(L"BUTTON", NULL, WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 150, 150, 20, 20, hDlg, (HMENU)DIALOG_WINDOW_CHECKBOX_TRASH, NULL, NULL);
	CreateWindow(L"Button", L"Confirm", dwStyleButton, 40, 190, 70, 30, hDlg, (HMENU)DIALOG_WINDOW_SAVE_MAP, NULL, NULL);
	CreateWindow(L"Button", L"Cancel", dwStyleButton, 130, 190, 70, 30, hDlg, (HMENU)DIALOG_WINDOW_CANCEL, NULL, NULL);

	if (Settings::GetInstance().GetIsTrash()) {
		SendMessage(trashCheckbox, BM_SETCHECK, BST_CHECKED, 0);
	}

	if (Settings::GetInstance().GetIsGeneral()) {
		SendMessage(generalCheckbox, BM_SETCHECK, BST_CHECKED, 0);
		SetWindowText(setMapWnd, L"General");
		EnableWindow(setMapWnd, false);
		EnableWindow(setMapPriceWnd, false);
	}
}

void DialogWindow::OpenIIQDialog(HWND hWnd) {
	RECT rect;
	GetWindowRect(hWnd, &rect);
	HWND hDlg = CreateWindowEx(NULL, L"myDialogClass", L"Set IIQ", WS_BORDER | WS_VISIBLE, rect.left + 25, rect.top + 25, 260, 180, NULL, NULL, NULL, NULL);
	CreateWindow(L"Static", L"Enter total IIQ on gear:", dwStyleText, 20, 10, 200, 20, hDlg, NULL, NULL, NULL);
	setIIQWnd = CreateWindow(L"Edit", Converter<int>::ToWstring(Settings::GetInstance().GetIIQ()).c_str(), WS_CHILD | WS_VISIBLE | ES_CENTER | WS_BORDER | ES_NUMBER, 20, 35, 200, 20, hDlg, NULL, NULL, NULL);
	CreateWindow(L"Button", L"Confirm", dwStyleButton, 40, 90, 70, 30, hDlg, (HMENU)DIALOG_WINDOW_SAVE_IIQ, NULL, NULL);
	CreateWindow(L"Button", L"Cancel", dwStyleButton, 130, 90, 70, 30, hDlg, (HMENU)DIALOG_WINDOW_CANCEL, NULL, NULL);
}

void DialogWindow::OpenSextantDialog(HWND hWnd) {
	RECT rect;
	GetWindowRect(hWnd, &rect);
	HWND hDlg = CreateWindowEx(NULL, L"myDialogClass", L"Set CS", WS_BORDER | WS_VISIBLE, rect.left + 25, rect.top + 25, 260, 190, NULL, NULL, NULL, NULL);
	CreateWindow(L"Static", L"Set combination of Sextants Apprentice-Journey-Master", dwStyleText, 20, 10, 200, 35, hDlg, NULL, NULL, NULL);
	setCSWnd = CreateWindow(L"Edit", Settings::GetInstance().GetCS().c_str(), WS_CHILD | WS_VISIBLE | ES_CENTER | WS_BORDER, 20, 55, 200, 20, hDlg, NULL, NULL, NULL);
	CreateWindow(L"Button", L"Confirm", dwStyleButton, 40, 100, 70, 30, hDlg, (HMENU)DIALOG_WINDOW_SAVE_CS, NULL, NULL);
	CreateWindow(L"Button", L"Cancel", dwStyleButton, 130, 100, 70, 30, hDlg, (HMENU)DIALOG_WINDOW_CANCEL, NULL, NULL);
}

void DialogWindow::OpenLeagueDialog(HWND hWnd) {
	RECT rect;
	GetWindowRect(hWnd, &rect);
	HWND hDlg = CreateWindowEx(NULL, L"myDialogClass", L"Set League", WS_BORDER | WS_VISIBLE, rect.left + 25, rect.top + 25, 260, 180, NULL, NULL, NULL, NULL);
	CreateWindow(L"Static", L"Set League", dwStyleText, 20, 10, 200, 20, hDlg, NULL, NULL, NULL);
	//SS_CENTER doesn't center text but makes it a bit cleaner when selecting options
	comboBoxWnd = CreateWindow(L"ComboBox", NULL, WS_CHILD | WS_VISIBLE | CBS_DROPDOWN | SS_CENTER, 65, 35, 120, 60, hDlg, NULL, NULL, NULL); 
	SendMessage(comboBoxWnd, CB_ADDSTRING, 0, (LPARAM)(Settings::GetInstance().GetLeague()).c_str());
	SendMessage(comboBoxWnd, CB_ADDSTRING, 0, (LPARAM)(Settings::GetInstance().GetLeagueHC()).c_str());
	SendMessage(comboBoxWnd, CB_SETCURSEL, 0, NULL);
	CreateWindow(L"Button", L"Confirm", dwStyleButton, 40, 90, 70, 30, hDlg, (HMENU)DIALOG_WINDOW_SAVE_LEAGUE, NULL, NULL);
	CreateWindow(L"Button", L"Cancel", dwStyleButton, 130, 90, 70, 30, hDlg, (HMENU)DIALOG_WINDOW_CANCEL, NULL, NULL);
}

//Get data from league dialog 
std::wstring DialogWindow::GetNewLeague() {
	//Get value from window and convert it to string
	wchar_t wLeague[50];
	GetWindowText(DialogWindow::comboBoxWnd, wLeague, 15);

	return Utilities::WchartToString(wLeague);
}

std::wstring DialogWindow::GetCS(){
	wchar_t wCS[20];
	GetWindowText(DialogWindow::setCSWnd, wCS, 10);

	return Utilities::WchartToString(wCS);
}

std::wstring DialogWindow::GetIIQ(){
	wchar_t wIIQ[5];
	GetWindowText(DialogWindow::setIIQWnd, wIIQ, 5);

	return Utilities::WchartToString(wIIQ);
}

std::wstring DialogWindow::GetLogLocation() {
	//Get text that user typed
	wchar_t log[250];
	GetWindowText(logLoc, log, 200);

	//Convert data to string
	return Utilities::WchartToString(log); //Example - C:\Program Files (x86)\Steam
}

MapSettings DialogWindow::GetMapSettings(const HWND& hWnd){
	wchar_t wMap[200], wMapPrice[6], wZanaModPrice[6];
	//Get text from dialog window
	GetWindowText(DialogWindow::setMapWnd, wMap, (sizeof(wMap) / 2));
	GetWindowText(DialogWindow::setMapPriceWnd, wMapPrice, (sizeof(wMapPrice) / 2));
	GetWindowText(DialogWindow::setZanaModPrice, wZanaModPrice, (sizeof(wZanaModPrice) / 2));

	//Populate map settings struct
	MapSettings mapSettings;
	mapSettings.mapName = Utilities::WchartToString(wMap);
	mapSettings.mapPrice = stof(Utilities::WchartToString(wMapPrice));
	mapSettings.zanaModPrice = stoi(Utilities::WchartToString(wZanaModPrice));

	//Check status of checkbox for general farming
	if (IsDlgButtonChecked(hWnd, DIALOG_WINDOW_CHECKBOX_GENERAL)) {
		mapSettings.isGeneral = true;
	}
	else {
		mapSettings.isGeneral = false;
	}

	//Check status of checkbox for ignoring trash values
	if (IsDlgButtonChecked(hWnd, DIALOG_WINDOW_CHECKBOX_TRASH)) {
		mapSettings.isTrash = true;
	}
	else {
		mapSettings.isTrash = false;
	}

	return mapSettings;
}

std::pair<float, float> DialogWindow::GetOverlaySettings(){
	//Get data from window
	wchar_t xP[5], yP[5];
	GetWindowText(DialogWindow::setXPosWnd, xP, (sizeof(xP) / 2));
	GetWindowText(DialogWindow::setYPosWnd, yP, (sizeof(yP) / 2));

	//Convert data to string
	std::wstring x = Utilities::WchartToString(xP);
	std::wstring y = Utilities::WchartToString(yP);

	//Check if data can be made into float
	if (!Utilities::CheckDigits(x))
		return { 0.0f, 0.0f };
	if (!Utilities::CheckDigits(y))
		return { 0.0f, 0.0f };

	//Save data to overlay settings
	return std::pair<float, float> {stof(x), stof(y)};
}

void DialogWindow::OnGeneralCheckboxChecked(const HWND& hWnd){
	//Minor visual change to indicate that specific map name and price won't be used when parsing values
	if (IsDlgButtonChecked(hWnd, DIALOG_WINDOW_CHECKBOX_GENERAL)) {
		EnableWindow(setMapWnd, false);
		EnableWindow(setMapPriceWnd, false);
		SetWindowText(setMapPriceWnd, L"0");
		SetWindowText(setMapWnd, L"General");
	}
	else {
		EnableWindow(setMapWnd, true);
		EnableWindow(setMapPriceWnd, true);
		SetWindowText(setMapWnd, L"Maze of the Minotaur");
		SetWindowText(setMapPriceWnd, Converter<float>::ToWstring(Settings::GetInstance().GetMapPrice()).c_str());
	}
}

float DialogWindow::GetSpentValue(){
	wchar_t wSpent[20];
	GetWindowText(plusSpentWnd, wSpent, (sizeof(wSpent) / 2));

	std::wstring spent = Utilities::WchartToString(wSpent);

	if (!Utilities::CheckDigits(spent))
		return 0.0f;

	return stof(spent);
}

float DialogWindow::GetProfitValue(){
	wchar_t wProfit[20];
	GetWindowText(plusProfitWnd, wProfit, (sizeof(wProfit) / 2));

	std::wstring profit = Utilities::WchartToString(wProfit);

	if (!Utilities::CheckDigits(profit))
		return 0.0f;

	return stof(profit);
}

void DialogWindow::UpdateCurrencyDialog(HWND hWnd) {
	RECT rect;
	GetWindowRect(hWnd, &rect);
	HWND hDlg = CreateWindowEx(NULL, L"myDialogClass", L"Updating Currency", WS_BORDER | WS_VISIBLE, rect.left + 25, rect.top + 25, 260, 200, NULL, NULL, NULL, NULL);
	updateStatusWnd = CreateWindow(L"Static", L"Update Currency Rates", dwStyleText, 20, 20, 200, 20, hDlg, NULL, NULL, NULL);
	hProgress = CreateWindowEx(0, PROGRESS_CLASS, (LPTSTR)NULL, WS_CHILD | WS_VISIBLE | PBS_SMOOTH, 20, 50, 200, 30, hDlg, NULL, NULL, NULL);
	updateButton = CreateWindow(L"Button", L"Update", dwStyleButton, 40, 100, 70, 30, hDlg, (HMENU)DIALOG_WINDOW_UPDATE_CURRENCY, NULL, NULL);
	CreateWindow(L"Button", L"Close", dwStyleButton, 130, 100, 70, 30, hDlg, (HMENU)DIALOG_WINDOW_CANCEL, NULL, NULL);
	SendMessage(hProgress, PBM_SETRANGE, 0, MAKELPARAM(0, Settings::GetInstance().GetProgressBarSteps()));
	SendMessage(hProgress, PBM_SETSTEP, (WPARAM)1, 0);
}

void DialogWindow::OpenOverlaySettingsDialog(HWND hWnd) {
	RECT rect;
	GetWindowRect(hWnd, &rect);
	HWND hDlg = CreateWindowEx(NULL, L"myDialogClass", L"Overlay Settings", WS_BORDER | WS_VISIBLE, rect.left + 25, rect.top + 25, 260, 180, NULL, NULL, NULL, NULL);
	CreateWindow(L"Static", L"Set xPos 0.00% - 0.95%", dwStyleText, 20, 10, 150, 20, hDlg, NULL, NULL, NULL);
	setXPosWnd = CreateWindow(L"Edit", Converter<float>::ToWstring(Settings::GetInstance().GetOverlayXPos()).c_str(), WS_CHILD | WS_VISIBLE | ES_CENTER | WS_BORDER, 170, 10, 50, 20, hDlg, NULL, NULL, NULL);
	CreateWindow(L"Static", L"Set yPos 0.00% - 0.95%", dwStyleText, 20, 35, 150, 20, hDlg, NULL, NULL, NULL);
	setYPosWnd = CreateWindow(L"Edit", Converter<float>::ToWstring(Settings::GetInstance().GetOverlayYPos()).c_str(), WS_CHILD | WS_VISIBLE | ES_CENTER | WS_BORDER, 170, 35, 50, 20, hDlg, NULL, NULL, NULL);
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
	CreateWindow(L"Button", L"Confirm", dwStyleButton, 40, 90, 70, 30, hDlg, (HMENU)DIALOG_WINDOW_PLUS_SPENT, NULL, NULL);
	CreateWindow(L"Button", L"Cancel", dwStyleButton, 130, 90, 70, 30, hDlg, (HMENU)DIALOG_WINDOW_CANCEL, NULL, NULL);
}

void DialogWindow::OpenAboutDialog(HWND hWnd) {
	RECT rect;
	GetWindowRect(hWnd, &rect);
	Version ver;
	HWND hDlg = CreateWindowEx(NULL, L"myDialogClass", L"About", WS_BORDER | WS_VISIBLE | WS_OVERLAPPEDWINDOW, rect.left + 25, rect.top + 25, 260, 200, NULL, NULL, NULL, NULL);
	CreateWindow(L"Static", L"PoE Farming Tool", WS_VISIBLE | WS_CHILD | SS_CENTER, 20, 20, 200, 150, hDlg, NULL, NULL, NULL);
	CreateWindow(L"Static", L"Author: Cailoki", WS_VISIBLE | WS_CHILD, 30, 60, 200, 150, hDlg, NULL, NULL, NULL);
	CreateWindow(L"Static", L"Home page: github.com/Cailoki", WS_VISIBLE | WS_CHILD, 30, 80, 300, 150, hDlg, NULL, NULL, NULL);
	CreateWindow(L"Static", L"License: GNU GPL v3.0", WS_VISIBLE | WS_CHILD, 30, 100, 200, 150, hDlg, NULL, NULL, NULL);
	CreateWindow(L"Static", (L"Version: " + ver.GetVersion()).c_str(), WS_VISIBLE | WS_CHILD, 30, 120, 200, 150, hDlg, NULL, NULL, NULL);
}

void DialogWindow::OpenSetLogLocation() {
	RECT wr = { 0, 0, 420, 120 }; //exact client area
	AdjustWindowRect(&wr, WS_BORDER | WS_VISIBLE | WS_OVERLAPPEDWINDOW, TRUE);
	HWND hDlg = CreateWindow(L"myDialogClass", L"Set Map", WS_BORDER | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, wr.right - wr.left, wr.bottom - wr.top, NULL, NULL, NULL, NULL);
	CreateWindow(L"Static", L"Client.txt not found, please copy the \"logs\" folder location below!", WS_CHILD | WS_VISIBLE | SS_CENTER, 10, 10, 410, 20, hDlg, NULL, NULL, NULL);
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