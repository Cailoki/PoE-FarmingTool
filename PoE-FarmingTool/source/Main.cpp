#include "pch.h"
#include "Window.h"
#include "DialogWindow.h"
#include "Overlay.h"
#include "UpdateData.h"
#include "Utilities.h"
#include "Parser.h"
#include "Models.h"
#include "Settings.h"
#include "Session.h"
#include "Constants.h"
#include "Calculator.h"

//Globals
static HHOOK hookHandleMouse;
LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam);
HFONT hFont = CreateFont(15, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
	DEFAULT_QUALITY, DEFAULT_PITCH, TEXT("Microsoft Sans Serif"));

bool CALLBACK SetFont(HWND child, LPARAM font) {
	SendMessage(child, WM_SETFONT, font, true);
	return true;
}

class Actions {
private:
	bool globalDisable = false;
public:
	void FlipGlobalDisable();
	bool GetGlobalDisable() { return globalDisable; }
}actions;

class LogParser {
private:
	static bool parserInitiated;
	static void ParseLog();
public:
	static void RunLogParser();
};

bool LogParser::parserInitiated = false;

//Assumes CTRL is already pressed, send virtual c-key press
void SendCopyCommand() {
	INPUT ip;

	ip.type = INPUT_KEYBOARD;
	ip.ki.wScan = 0; // hardware scan code for key
	ip.ki.time = 0;
	ip.ki.dwExtraInfo = 0;

	//Press C Key
	ip.ki.wVk = 0x43; // virtual-key code for C-key
	ip.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &ip, sizeof(INPUT));

	// Release C key
	ip.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
	SendInput(1, &ip, sizeof(INPUT));
}

//Return clipboard contents (if formated as text(UNICODE))
std::wstring GetClipboardData() {
	if (IsClipboardFormatAvailable(CF_UNICODETEXT)) {
		HANDLE clip;
		OpenClipboard(NULL);
		clip = GetClipboardData(CF_UNICODETEXT);
		EmptyClipboard(); //Missclick would return old data
		CloseClipboard();
		return (wchar_t*)clip;
	}
	return L"";
}

void OnButtonClicked(HWND hWnd, LPARAM lParam) {
	//Open links in browser
	if (MainWindow::patreonWnd == (HWND)lParam)
		ShellExecute(NULL, L"open", L"https://www.patreon.com/cailoki", NULL, NULL, SW_SHOWNORMAL);
	else if (MainWindow::gitWnd == (HWND)lParam)
		ShellExecute(NULL, L"open", L"https://github.com/Cailoki/PoE-FarmingTool", NULL, NULL, SW_SHOWNORMAL);
	else if (MainWindow::discordWnd == (HWND)lParam)
		ShellExecute(NULL, L"open", L"https://discord.gg/D2m7Kpw", NULL, NULL, SW_SHOWNORMAL);
	else if (MainWindow::newVerAvailableButton == (HWND)lParam)
		ShellExecute(NULL, L"open", L"https://github.com/Cailoki/PoE-FarmingTool/releases", NULL, NULL, SW_SHOWNORMAL);

	//Overlay
	else if (MainWindow::overlayButton == (HWND)lParam)
		if (MainWindow::overlayOn) {
			MainWindow::overlayOn = false;
			OverlayWindow::CloseOverlay();
		}
		else {
			MainWindow::overlayOn = true;
			OverlayWindow::OpenOverlay(actions.GetGlobalDisable());
		}

	//Other buttons
	else if (MainWindow::profitButton == (HWND)lParam)
		DialogWindow::OpenPlusProfitDialog(hWnd);
	else if (MainWindow::spentButton == (HWND)lParam)
		DialogWindow::OpenPlusSpentDialog(hWnd);
}

void ResetData(HWND hWnd) {
	int code = MessageBox(hWnd, L"Reset runs, spent and profit?", L"Reset data", MB_YESNO);
	if (code == 6) //The Yes button was selected. 
		SessionData::GetInstance().ResetData();
}

//If Ctrl + LMB is excuted on the left side of the screen(game client, not desktop) -> moving items from stash, right side -> moving items to stash
bool IsMovingToStash() {
	HWND hwnd = GetForegroundWindow();
	RECT rect;
	POINT p;

	GetWindowRect(hwnd, &rect);
	GetCursorPos(&p);
	ScreenToClient(hwnd, &p); //Converts the screen coordinates of a specified point on the screen to client-area coordinates

	if (p.x < ((rect.right - rect.top) / 2)) //Left side, moving items from stash (don't execute copy command)
		return false;
	else
		return true; //Right side, moving items to stash
}

//Initialize data, settings, currency...
void InitializeApp() {
	SessionData::GetInstance().LoadSessionData();
	Settings::GetInstance().LoadSettings();
	CurrencyInfo::LoadCurencyRates();
}

/////////////////////////////////////////////
//Procedures - Main Window, LL Mouse, Dialog, Overlay
/////////////////////////////////////////////
LRESULT CALLBACK OverlayProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	RECT rect;
	static HBRUSH hbrBackground;

	switch (uMsg)
	{
	case WM_LBUTTONDOWN:
		InvalidateRect(hWnd, NULL, true);
		return 0;
	case WM_ERASEBKGND:
		GetClientRect(hWnd, &rect);
		FillRect((HDC)wParam, &rect, CreateSolidBrush(RGB(255, 0, 255))); //make background transparent
		break;
	case WM_CREATE:
		hbrBackground = CreateSolidBrush(TRANSPARENT);
		break;
	case WM_PAINT:
		BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	case WM_CTLCOLORDLG:
		return (ULONG_PTR)hbrBackground;
	case WM_CTLCOLORSTATIC: {
		HDC hdcStatic = (HDC)wParam;
		SetTextColor(hdcStatic, RGB(255, 255, 255));
		SetBkMode(hdcStatic, TRANSPARENT);
		return (LRESULT)GetStockObject(NULL_BRUSH);
	}
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
		break;
	}
	return 0;
}

LRESULT CALLBACK DialogProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	static HBRUSH hbrBackground;

	switch (uMsg) {
	case WM_COMMAND:
		switch (wParam) {
		case DIALOG_WINDOW_PLUS_PROFIT: {
			SessionData::GetInstance().AddProfitManual(DialogWindow::GetProfitValue());
			DestroyWindow(hWnd);
			break;
		}
		case DIALOG_WINDOW_PLUS_SPENT: {
			SessionData::GetInstance().AddSpentManual(DialogWindow::GetSpentValue());
			DestroyWindow(hWnd);
			break;
		}
		case DIALOG_WINDOW_CHECKBOX_GENERAL: {
			DialogWindow::OnGeneralCheckboxChecked(hWnd);
			break;
		}
		case DIALOG_WINDOW_SAVE_OVERLAY_SETTINGS: {
			Settings::GetInstance().SetOverlayPosition(DialogWindow::GetOverlaySettings());
			DestroyWindow(hWnd);
			break;
		}
		case DIALOG_WINDOW_SAVE_MAP: {
			Settings::GetInstance().SaveMapFarmingSettings(DialogWindow::GetMapSettings(hWnd));
			DestroyWindow(hWnd);
			break;
		}
		case DIALOG_WINDOW_SAVE_IIQ: {
			Settings::GetInstance().SetIIQ(stoi(DialogWindow::GetIIQ()));
			DestroyWindow(hWnd);
			break;
		}
		case DIALOG_WINDOW_SAVE_CS: {
			//Save new Cartographer's Sextant value (returns false if not saved)
			if (Settings::GetInstance().SetCS(DialogWindow::GetCS())) {
				//Recalculate average price of Cartographer's Sextant
				CurrencyInfo::SetCsValue(Calculator::CalculateCSPrice(Settings::GetInstance().GetCS()));
				DestroyWindow(hWnd);
			}
			break;
		}
		case DIALOG_WINDOW_SAVE_LEAGUE: {
			//Save new league
			Settings::GetInstance().SetLeague(DialogWindow::GetNewLeague());
			//Show update currency dialog on league change
			DialogWindow::UpdateCurrencyDialog(hWnd);
			MessageBox(NULL, L"League changed, please update currency rates!", L"Warning!", NULL);
			DestroyWindow(hWnd);
			break;
		}
		case DIALOG_WINDOW_SAVELOC: {
			//Save new log location
			Settings::GetInstance().SaveNewLogLocation(DialogWindow::GetLogLocation());
			DestroyWindow(hWnd);

			//Rerun log parser
			LogParser parser;
			parser.RunLogParser();
			break;
		}
		case DIALOG_WINDOW_UPDATE_CURRENCY: {
			//Disable update button so it can't be pressed while updating data
			EnableWindow(DialogWindow::updateButton, false);
			SetWindowText(DialogWindow::updateStatusWnd, L"Please Wait...");
			std::thread update(CurrencyInfo::UpdateRates);
			update.detach();
			break;
		}
		case DIALOG_WINDOW_CANCEL: {
			DestroyWindow(hWnd);
			break;
		}
		}
		break;
	case WM_PAINT: {
		BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		EnumChildWindows(hWnd, (WNDENUMPROC)SetFont, (LPARAM)hFont);
		break;
	}
	case WM_CTLCOLORDLG:
		return (ULONG_PTR)hbrBackground;
	case WM_CTLCOLORSTATIC: {
		HDC hdcStatic = (HDC)wParam;
		SetBkMode(hdcStatic, TRANSPARENT);
		return (ULONG_PTR)hbrBackground;
	}
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode == HC_ACTION) { //HC_ACTION == 0, The wParam and lParam parameters contain information about a keystroke message.
		switch (wParam) {
		case WM_LBUTTONDOWN:
			if (GetKeyState(VK_CONTROL) < 0) { //Parse hotkey (CTRL + LMB) activated
				//Left/Right side of the focused window, parse items only if moved from the right side and parsing is enabled
				if (!actions.GetGlobalDisable() && IsMovingToStash()) {
					SendCopyCommand(); //Press virtual C-key, needed for Ctrl+C
					std::this_thread::sleep_for(std::chrono::milliseconds(10)); //Wait a bit to be sure that data made it to the clipboard
					ItemManager::GetInstance().ClassifyItem(ClipParser(GetClipboardData()).ParseClipboard());
				}
			}
			break;
		case WM_RBUTTONDOWN:
			if (GetKeyState(VK_CONTROL) < 0) { //CTRL + RMB clicked
				actions.FlipGlobalDisable(); //Disable tool
			}
			break;
		}
	}
	return CallNextHookEx(hookHandleMouse, nCode, wParam, lParam);
}

LRESULT CALLBACK WndProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	static HBRUSH hbrBackground;

	switch (uMsg)
	{
	case WM_COMMAND:
		switch (wParam) {
		case STN_CLICKED: //Notification code is sent when user clicks a static control with SS_NOTIFY style
			OnButtonClicked(hWnd, lParam);
			break;
		case SETTINGS_MENU_MAP:
			DialogWindow::OpenMapDialog(hWnd);
			break;
		case SETTINGS_MENU_IIQ:
			DialogWindow::OpenIIQDialog(hWnd);
			break;
		case SETTINGS_MENU_CS:
			DialogWindow::OpenSextantDialog(hWnd);
			break;
		case SETTINGS_MENU_LEAGUE:
			DialogWindow::OpenLeagueDialog(hWnd);
			break;
		case SETTINGS_MENU_OVERLAY:
			DialogWindow::OpenOverlaySettingsDialog(hWnd);
			break;
		case FILE_MENU_RESET:
			ResetData(hWnd);
			break;
		case HELP_ABOUT:
			DialogWindow::OpenAboutDialog(hWnd);
			break;
		case FILE_MENU_EXIT:
			DestroyWindow(hWnd);
			break;
		case CURRENCY_MENU_UPDATE:
			DialogWindow::UpdateCurrencyDialog(hWnd);
			break;
		}
	case WM_CTLCOLORDLG:
		return (ULONG_PTR)hbrBackground;
	case WM_CTLCOLORSTATIC: {
		HDC hdcStatic = (HDC)wParam;
		SetBkMode(hdcStatic, TRANSPARENT);
		return (ULONG_PTR)hbrBackground;
	}
	case WM_CREATE: {
		MainWindow::AddMenus(hWnd);
		MainWindow::ShowUpdatingText(hWnd); //Profit, spent... settings(league, map...)
		EnumChildWindows(hWnd, (WNDENUMPROC)SetFont, (LPARAM)hFont);
		hbrBackground = CreateSolidBrush(RGB(255, 255, 255)); //White background for "updating text" windows
	}
					break;
	case WM_PAINT: {
		hdc = BeginPaint(hWnd, &ps);
		SetBkMode(hdc, TRANSPARENT);
		SelectObject(hdc, hFont);
		MainWindow::ShowLabels(hdc);
		EndPaint(hWnd, &ps);
	}
				   break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}

//hPrevInstance, lpCmdLine and nCmdShow removed because not used, clears warning: unreferenced formal parameter
int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = NULL; //CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = NULL;
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"DesktopApp";
	wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	//Register window classes
	RegisterClassEx(&wcex);
	DialogWindow::RegisterDialogClass();
	OverlayWindow::RegisterOverlayClass();

	//Initialize app before droawing windows so they coul pick up loaded settings
	InitializeApp();

	//Create/Draw main window
	RECT wr = { 0, 0, 420, 420 }; //Exact client area
	AdjustWindowRect(&wr, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, TRUE); //Calculate window size out of the client area and other styles using title/menu bar
	CreateWindowW(L"DesktopApp", L"PoE Farming Tool", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, wr.right - wr.left, wr.bottom - wr.top, NULL, NULL, hInstance, NULL);

	//Check for new version (second param is a button which might change visibility)
	std::thread check(Utilities::CheckVersion, MainWindow::newVerAvailableButton);
	check.detach();

	//Start log parser
	LogParser::RunLogParser();

	hookHandleMouse = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, NULL, 0);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	UnhookWindowsHookEx(hookHandleMouse); //Unhook mouse handle before exiting app
	return (int)msg.wParam;
}

//Disables all parsing, Ctrl+LMB won't go trough
void Actions::FlipGlobalDisable() {
	MessageBeep(MB_ICONSTOP);
	if (globalDisable) {
		globalDisable = false;
		SetWindowText(MainWindow::itemMovedWnd, L"");
		if (MainWindow::overlayOn) {
			OverlayWindow::StatusEnabled();
		}
	}
	else {
		globalDisable = true;
		SetWindowText(MainWindow::itemMovedWnd, L"*** TOOL DISABLED ***");
		if (MainWindow::overlayOn) {
			OverlayWindow::StatusDisabled();
		}
	}
}

//Parsing log for zone changes (running in another thread indefinitely)
void LogParser::ParseLog() {
	std::wifstream ist{ Settings::GetInstance().GetClientLogLocation() };
	if (!ist) {
		MessageBox(NULL, L"Unable to open Client.txt!", L"Error", NULL);
		return;
	}
	ist.seekg(0, ist.end);
	std::wstring lastEntry, word, ip, location;
next_line: while (getline(ist, lastEntry)) {
	std::wistringstream lineStream{ lastEntry };
	lineStream.seekg(50); //Skip some data

	while (lineStream >> word) {
		if (actions.GetGlobalDisable())
			break;
		if (word == L"Connecting") { //get ip address of location
			while (lineStream >> word)
				if (isdigit(word[0]))
					ip = word;
			break;
		}
		if (word == L"entered") { //get entered location name
			while (lineStream >> word)
				location += word + L' ';
			location.erase(location.size() - 2, location.size() - 1);
			LocationManager::GetInstance().ClassifyLocation(Location{ ip, location });
			ip.clear(); //Slight performance gain from ip = "" to ip.clear();
			location.clear();
			break;
		}
		if (word == L"mediocre") { //The Shaper: The mediocre turned remarkable (line which shaper says on used sextant)
			SessionData::GetInstance().SubtractSpentCS(); //Cartographer's Sextant used
			break;
		}
	}
}
		   if (ist.eof()) {
			   Sleep(1000);
			   ist.clear();
			   goto next_line;
		   }
}

//Runs parser if Client.txt is found, otherwise prompt for user input
void LogParser::RunLogParser() {
	//Check if Client.txt exists, if not, let user enter a path to the file
	std::wifstream ist{ Settings::GetInstance().GetClientLogLocation() };
	if (!ist) {
		DialogWindow::OpenSetLogLocation(); //Open dialog window so user can enter new location
		return;
	}

	//Prevent multiple instances of parser being run
	if (!parserInitiated) {
		std::thread parser(LogParser::ParseLog);
		parser.detach();
		parserInitiated = true;
	}
}