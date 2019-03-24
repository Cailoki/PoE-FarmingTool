#pragma once

#define DIALOG_WINDOW_CANCEL 8
#define DIALOG_WINDOW_SAVE_MAP 9
#define DIALOG_WINDOW_SAVE_IIQ 10
#define DIALOG_WINDOW_SAVE_CS 11
#define DIALOG_WINDOW_SAVE_LEAGUE 12
#define DIALOG_WINDOW_UPDATE_CURRENCY 13
#define DIALOG_WINDOW_CHECKBOX_GENERAL 14
#define DIALOG_WINDOW_SAVE_OVERLAY_SETTINGS 15
#define DIALOG_WINDOW_PLUS_PROFIT 16
#define DIALOG_WINDOW_PLUS_SPENT 17
#define DIALOG_WINDOW_SAVELOC 18

class DialogWindow {
public:
	static void OpenMapDialog(HWND, wchar_t[50], wchar_t[10], wchar_t[10]);
	static void OpenIIQDialog(HWND, wchar_t[5]);
	static void OpenSextantDialog(HWND, wchar_t[10]);
	static void OpenLeagueDialog(HWND);
	static void OpenOverlayDialog(HWND, const float&, const float&);
	static void OpenPlusProfitDialog(HWND);
	static void OpenPlusSpentDialog(HWND);
	static void OpenTipsDialog(HWND);
	static void OpenAboutDialog(HWND);
	static void OpenSetLogLocation();
	static void UpdateCurrencyDialog(HWND, int);
	static void RegisterDialogClass();
	static void SetIsGeneral(bool); //farming type "general"
	static bool GetIsGeneral() { return isGeneral; } //farming type "general"
	static HWND setMapWnd, setIIQWnd, setCSWnd, hProgress, updateStatusWnd, setMapPriceWnd, generalCheckbox, 
		setXPosWnd, setYPosWnd, plusProfitWnd, plusSpentWnd, setZanaModPrice, logLoc, comboBoxWnd, updateButton;
private:
	static bool isGeneral; //farming type "general"
};

LRESULT CALLBACK DialogProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);