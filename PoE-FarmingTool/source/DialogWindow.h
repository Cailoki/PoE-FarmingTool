#pragma once
#include "Settings.h"

class DialogWindow {
public:
	static std::wstring GetNewLeague();
	static std::wstring GetCS();
	static std::wstring GetIIQ();
	static std::wstring GetLogLocation();
	static MapSettings GetMapSettings(const HWND& hWnd);
	static std::pair<float, float> GetOverlaySettings();
	static void OnGeneralCheckboxChecked(const HWND& hWnd);
	static std::pair<std::wstring, bool> GetAddSpentWindowValues(HWND hWnd);
	static float GetProfitValue();

	static void OpenMapDialog(HWND);
	static void OpenIIQDialog(HWND);
	static void OpenSextantDialog(HWND);
	static void OpenLeagueDialog(HWND);
	static void OpenOverlaySettingsDialog(HWND);
	static void OpenPlusProfitDialog(HWND);
	static void OpenPlusSpentDialog(HWND);
	static void OpenAboutDialog(HWND);
	static void OpenSetLogLocation();
	static void UpdateCurrencyDialog(HWND);
	static void RegisterDialogClass();
	static HWND setMapWnd, setIIQWnd, setCSWnd, hProgress, updateStatusWnd, setMapPriceWnd, generalCheckbox, trashCheckbox, 
		setXPosWnd, setYPosWnd, plusProfitWnd, plusSpentWnd, setZanaModPrice, logLoc, comboBoxWnd, updateButton, subtractProfitCheckbox, parseAllCheckbox;
};

LRESULT CALLBACK DialogProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);