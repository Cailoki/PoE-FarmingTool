#include "pch.h"

#include "Window.h"
#include "DialogWindow.h"
#include "Overlay.h"
#include "UpdateData.h"
//#include "GetPrice.h"

//Globals
static HHOOK hookHandleMouse;
LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam);
HFONT hFont = CreateFont(15, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, TEXT("Microsoft Sans Serif"));
const std::vector<std::string>magicMapPrefixes = { "Ceremonial","Antagonist's","Anarchic","Skeletal","Capricious","Slithering","Undead","Emanant","Feral","Demonic","Bipedal","Solar",
"Lunar","Haunting","Feasting","Multifarious","Chaining","Otherworldly","Twinned","Splitting","Abhorrent","Unstoppable","Conflagrating","Unwavering","Armoured","Fecund","Savage",
"Burning","Freezing","Shocking","Fleet","Punishing","Mirrored","Overlord's","Titan's","Hexwarded","Resistant","Impervious","Hexproof","Empowered" };
const std::string version = "v1.0.1";

//Convert to wchar_t template (used for the window text variables)
template<class T>
void convert_to_wchar_t(wchar_t wch[], T value)
{
	if (typeid(T) == typeid(int)) {
		StringCbPrintfW(wch, 50, L"%d", value);
	}
	else if (typeid(T) == typeid(float)) {
		StringCbPrintfW(wch, 50, L"%.2f", value);
	}
	else {
		MessageBox(NULL, L"Unknown type in template function	convert_to_wchar_t", L"Error", NULL);
	}
}
template<>
void convert_to_wchar_t<std::string>(wchar_t wch[], std::string value) {
	char buf[100];
	strncpy_s(buf, value.c_str(), ARRAYSIZE(buf));
	StringCbPrintfW(wch, 100, L"%S", buf);
}

//Current session data
class SessionData {
public:
	//calculate profit/spent
	bool CalculateProfit(const std::string&, const int&, const std::vector<CurrencyInfo::RatesStruct>&);
	void CalculateSpent(const std::string&);
	//display calculated data
	void DisplayProfit(const std::string&, const float&, const int&);
	void DrawData() const; //Draws runs, spent and profit values on the window
	//Get/Set variables for spent currency and added fragment/scarab
	const float& GetSpentCurrencyOnMap() const { return spentCurrencyOnMap; }
	void SetSpentCurrencyOnMap(const float& value); //Add values of chisel, vaal, alch
	const float& GetMapPriceExtra() const { return mapPriceExtra; }
	void SetMapPriceExtra(const float& value); //Add values of fragment or scarab, 0 resets changing variable

	const bool GetIsMapEntered() const { return isMapOpened; }
	void ClearIsMapEntered() { isMapOpened = false; }
	void SetCsPrice(const float& value) { csPrice = value; }
	void ResetData();
	void UpdateLocation(const std::string&, const std::string&);
	void SaveData();
	void OpenOverlay();
	void DisplayHistory(const std::string&, const bool&);
	void ManualData(const float&, const float&);
	void InitData(); //initialize existing data from a file, and populate private members
	void IncrementRuns();
	wchar_t wRuns[5] = L"0", wSpent[20] = L"0.00", wProfit[20] = L"0.00", w_location[50] = L"", wItemMoved[100] = L"", w_status[50] = L"", wLastProfit[10] = L"", wLastSpent[10] = L"", wZanaModPrice[10] = L"0";
private:
	std::vector<std::string>history;
	bool isMapOpened = false; //false on script startup, used every run to block parsing until first map is opened/entered
	bool isSpecificMapOpened = false;
	int runs = 0, iiq = 0;
	float spentCurrencyOnMap = 0; //Currency used on a map (chisel, vaal, alch)
	float mapPriceExtra = 0; //fragment and/or scarab
	float spent = 0, profit = 0, csPrice = 0, lastProfit = 0, lastSpent = 0;
	const int mapTextSize = 25, iiqTextSize = 5, csTextSize = 10, priceTextSize = 5, leagueTextSize = 15;
} sessionData;

class Settings {
public:
	void InitSettings();
	const void DrawSettings() const; //Draw settings on the window
	void SaveSettings();
	const std::string& GetLogLocation() const { return clientLogLocation; }
	void SetLogLocation(const std::string& value) { clientLogLocation = value; }
	//League
	const std::string& GetLeague() const { return league; }
	void SetLeague(const std::string& value) { league = value; }
	//Cartographer's Sextants (CS)
	void SetCS(const std::string& value) { cs = value; }
	void ParseCS() const; //Parse cs and pass calculated average price to data instance
	//IIQ
	void SetIIQ(const int& value) { iiq = value; }
	//Map name, price and zana mod
	void UpdateMapSettings(HWND);
	void SetMapName(const std::string& name) { mapName = name; }
	const std::string& GetMapName() { return mapName; }
	const float GetMapPrice()const { return mapPrice; }
	const int GetZanaModPrice() const { return zanaModPrice; }
	//Global disable hotkey/option - activated with Shift+RMB
	void SetGlobalDisable();
	const bool GetGlobalDisable() const { return globalDisable; }
	//Format for text used on windows
	wchar_t wMap[50] = L"General", wMapPrice[6] = L"0", wZanaModPrice[6] = L"0", wIIQ[5] = L"0.00", wCS[10] = L"0-0-0", wLeague[15] = L"Synthesis", wX[5] = L"0.00";
private:
	int iiq = 0, zanaModPrice = 0;
	bool globalDisable = false;
	float mapPrice = 0;
	std::string mapName = "General", league = "Synthesis", cs = "0-0-0", mapFarmingType = "false"; //CS - Cartographer's Sextant
	std::string clientLogLocation = "c://Program Files (x86)//Steam//steamapps//common//Path of Exile//logs//Client.txt";
}settings;

class Maps {
public:
	struct UniqueMap { std::string name, base; };
	std::vector<UniqueMap>uniqueMaps = {
		{"Acton's Nightmare", "Overgrown Shrine Map"} , {"Caer Blaidd, Wolfpack's Den", "Underground River Map"}, {"Death and Taxes", "Necropolis Map"}, {"Doryani's Machinarium", "Maze Map"},
		{"Hall of Grandmasters", "Promenade Map"}, {"Hallowed Ground", "Cemetery Map"}, {"Maelström of Chaos", "Atoll Map"}, {"Mao Kun","Shore Map"}, {"Oba's Cursed Trove", "Underground Sea Map"},
		{"Olmec's Sanctum","Bone Crypt Map"}, {"Pillars of Arun", "Dunes Map"}, {"Poorjoy's Asylum","Temple Map"}, {"The Beachhead", "The Coward's Trial"}, {"The Perandus Manor", "Chateau Map"},
		{"The Putrid Cloister", "Museum Map"}, {"The Twilight Temple", "Moon Temple Map"}, {"The Vinktar Square", "Courtyard Map"}, {"Vaults of Atziri", "Vaal Pyramid Map"}, {"Whakawairua Tuahu", "Strand Map"}
	};
}mapData;

void SessionData::SetMapPriceExtra(const float& value) {
	if (value == 0) mapPriceExtra = 0;
	else mapPriceExtra += value;
}

void SessionData::SetSpentCurrencyOnMap(const float& value) {
	if (value == 0) spentCurrencyOnMap = 0;
	else spentCurrencyOnMap += value;
}

struct InstanceData {
	std::string location = "", ip = "";
	int portals = 0;
	bool inMap = false;
}mainMap, zanaMap;

struct LastParsedMap {
	std::string name = "";
	float price = 0;
}lastParsedMap;

//Reset runs, spent and profit, convert to wchars and draw on the window
void SessionData::ResetData() {
	SendMessage(MainWindow::historyWnd, LB_RESETCONTENT, NULL, NULL); //Reset history window
	isMapOpened = false; //Reset map status as if script was just started
	runs = 0;
	spent = 0;
	profit = 0;
	lastProfit = 0;
	lastSpent = 0;
	convert_to_wchar_t(wRuns, runs);
	convert_to_wchar_t(wSpent, spent);
	convert_to_wchar_t(wProfit, profit);
	convert_to_wchar_t(wLastProfit, lastProfit);
	convert_to_wchar_t(wLastSpent, lastSpent);
	SetWindowText(MainWindow::itemMovedWnd, L"");
	SetWindowText(MainWindow::itemStatusWnd, L"");
	DrawData();
	SaveData();
}

//Used to keep track of used fragment or scarab when opening a map
struct MapAdditions {
	bool isMap = false; //parsed item was a map
	bool isFragment = false;
	bool isScarab = false;
}mapAdditionsCurrent;

//Open overlay with current session data
void SessionData::OpenOverlay() {
	OverlayWindow::SetOverlayWindow(wRuns, wSpent, wProfit);
}

//manually add value to spent or profit via buttons on main window 
void SessionData::ManualData(const float& spentManual, const float& profitManual) {
	spent += spentManual;
	profit -= spentManual;
	profit += profitManual;
	convert_to_wchar_t(wSpent, spent);
	convert_to_wchar_t(wProfit, profit);
	SetWindowText(MainWindow::profitWnd, wProfit);
	SetWindowText(MainWindow::spentWnd, wSpent);
	RedrawWindow(OverlayWindow::profitWnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE);
	SetWindowText(OverlayWindow::profitWnd, wProfit);
	RedrawWindow(OverlayWindow::spentWnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE);
	SetWindowText(OverlayWindow::spentWnd, wSpent);
	SaveData();
}

//Draw values after init or on every paint call
void SessionData::DrawData() const {
	SetWindowText(MainWindow::profitWnd, wProfit);
	SetWindowText(MainWindow::spentWnd, wSpent);
	SetWindowText(MainWindow::runsWnd, wRuns);
	SetWindowText(MainWindow::lastSpentWnd, wLastSpent);
	SetWindowText(MainWindow::lastProfitWnd, wLastProfit);
	if (MainWindow::overlayOn) {
		RedrawWindow(OverlayWindow::profitWnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE);
		SetWindowText(OverlayWindow::profitWnd, wProfit);
		RedrawWindow(OverlayWindow::spentWnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE);
		SetWindowText(OverlayWindow::spentWnd, wSpent);
		RedrawWindow(OverlayWindow::spentLastWnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE);
		SetWindowText(OverlayWindow::spentLastWnd, wLastSpent);
		RedrawWindow(OverlayWindow::profitLastWnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE);
		SetWindowText(OverlayWindow::profitLastWnd, wLastProfit);
		SetWindowText(OverlayWindow::runsWnd, wRuns);
	}
	SetWindowText(MainWindow::locWnd, w_location);
}

void SessionData::IncrementRuns() {
	++runs;
	convert_to_wchar_t(wRuns, runs);
	SetWindowText(MainWindow::runsWnd, wRuns);
	if (MainWindow::overlayOn) {
		SetWindowText(OverlayWindow::runsWnd, wRuns);
	}
	SaveData();
}

void SessionData::SaveData() {
	std::ofstream ost{ "data\\saved_data" };
	ost << runs << '\n' << spent << '\n' << profit;
}

//Takes in type/name and value(price)
void SessionData::CalculateSpent(const std::string& type) {
	float val = 0;
	std::string message = "";
	if (type == "CS") { //Cartographer's Sextant
		//If rates were updated, recalculate average CS price and set status to false until next update
		if (CurrencyInfo::currencyRatesUpdated) {
			settings.ParseCS();
			CurrencyInfo::currencyRatesUpdated = false;
		}
		spent += csPrice;
		profit -= csPrice;
		val = csPrice;
		message = "Sextant used!";
	}
	else if (type == "Map") { //Map set in settings
		if (isMapOpened) { //If map first map was opened after script startup
			if (settings.GetMapPrice() == 0) { //Use online price
				val = lastParsedMap.price + mapPriceExtra + spentCurrencyOnMap + settings.GetZanaModPrice();
				profit -= (lastParsedMap.price * 2 + mapPriceExtra + spentCurrencyOnMap + settings.GetZanaModPrice());
			}
			else { //Price set in settings
				val = settings.GetMapPrice() + mapPriceExtra + spentCurrencyOnMap + settings.GetZanaModPrice();
				profit -= (lastParsedMap.price + settings.GetMapPrice() + mapPriceExtra + spentCurrencyOnMap + settings.GetZanaModPrice());
			}
			spent += val;

			//Display last run data in the history
			lastProfit -= lastParsedMap.price;
			std::string strSpent = std::to_string(lastSpent);
			strSpent.erase(strSpent.size() - 4, strSpent.size());
			std::string strProfit = std::to_string(lastProfit);
			strProfit.erase(strProfit.size() - 4, strProfit.size());
			DisplayHistory("Map finished! Spent: " + strSpent + " Profit: " + strProfit, true);
			DisplayHistory("--------------------", false);

			//Reset last run data
			lastSpent = 0;
			lastProfit = 0;
		}
		else {
			if (settings.GetMapPrice() == 0) //Use online price
				val = lastParsedMap.price + mapPriceExtra + spentCurrencyOnMap + settings.GetZanaModPrice();
			else //Price set in settings
				val = settings.GetMapPrice() + mapPriceExtra + spentCurrencyOnMap + settings.GetZanaModPrice();
			spent += val;
			profit -= val;
		}
		lastSpent += val;
		lastProfit -= val;
		message = settings.GetMapName() + " used!";
	}
	else { //any map
		val = lastParsedMap.price + mapPriceExtra + spentCurrencyOnMap + settings.GetZanaModPrice();//new spent value
		spent += lastParsedMap.price + mapPriceExtra + spentCurrencyOnMap + settings.GetZanaModPrice(); //map opened, add spent value
		//next line: "*2" because first ctrl+click will transfer map to the device, parse it and add as profit
		//if opened, profit must be subtracted to cancel first action, then subtract again to calculate actual profit decrease 
		if (isMapOpened) { //false on script startup
			profit -= (lastParsedMap.price * 2 + mapPriceExtra + spentCurrencyOnMap + settings.GetZanaModPrice());

			//Display last run data in the history
			lastProfit -= lastParsedMap.price;
			std::string strSpent = std::to_string(lastSpent);
			strSpent.erase(strSpent.size() - 4, strSpent.size());
			std::string strProfit = std::to_string(lastProfit);
			strProfit.erase(strProfit.size() - 4, strProfit.size());
			DisplayHistory("Map finished! Spent: " + strSpent + " Profit: " + strProfit, true);
			DisplayHistory("--------------------", false);
			
			//Reset last run data
			lastSpent = 0;
			lastProfit = 0;
		}
		else {
			profit -= val;
		}
		lastSpent += val;
		lastProfit -= val;
		message = type + " used!";
	}
	//Clear last parsed map
	lastParsedMap.name = "";
	lastParsedMap.price = 0;

	//Convert data to format used by windows
	convert_to_wchar_t(wSpent, spent);
	convert_to_wchar_t(wProfit, profit);
	convert_to_wchar_t(wLastProfit, lastProfit);
	convert_to_wchar_t(wLastSpent, lastSpent);
	convert_to_wchar_t(wItemMoved, message);
	SetWindowText(MainWindow::itemMovedWnd, wItemMoved); //Display message
	std::string status = "-" + std::to_string(val); //How much currency was spent in string format (-1.23)
	status.erase(status.size() - 4, status.size()); //hacky way to show 2 decimal points 12.34
	convert_to_wchar_t(w_status, status);
	SetWindowText(MainWindow::itemStatusWnd, w_status); //Display value of spent currency
	std::string hist = message + ' ' + status; //Message shown in history window
	DisplayHistory(hist, false);
	DrawData();
	SaveData();
}

void SessionData::DisplayHistory(const std::string& data, const bool& removeLast) {
	//If last parsed item was map which was opened, remove it's entry in the history
	if (removeLast) {
		if (GetListBoxInfo(MainWindow::historyWnd) > 0)
			SendMessage(MainWindow::historyWnd, LB_DELETESTRING, GetListBoxInfo(MainWindow::historyWnd)-1, NULL);
	}
	//Add an entry to the list
	wchar_t wData[200];
	convert_to_wchar_t(wData, data);
	SendMessage(MainWindow::historyWnd, LB_ADDSTRING, NULL, (LPARAM)wData);
	SendMessage(MainWindow::historyWnd, WM_VSCROLL, SB_BOTTOM, NULL);
	//Keep number of items on the list at 100
	if (GetListBoxInfo(MainWindow::historyWnd) > 200)
		SendMessage(MainWindow::historyWnd, LB_DELETESTRING, 0, NULL);
}

//Display profit on the window and SAVE data
void SessionData::DisplayProfit(const std::string& name, const float& value, const int& stack) {
	convert_to_wchar_t(wItemMoved, name);
	SetWindowText(MainWindow::itemMovedWnd, wItemMoved);
	convert_to_wchar_t(wProfit, profit);
	SetWindowText(MainWindow::profitWnd, wProfit);
	RedrawWindow(OverlayWindow::profitWnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE);
	SetWindowText(OverlayWindow::profitWnd, wProfit);
	convert_to_wchar_t(wLastProfit, lastProfit);
	SetWindowText(MainWindow::lastProfitWnd, wLastProfit);
	RedrawWindow(OverlayWindow::profitLastWnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE);
	SetWindowText(OverlayWindow::profitLastWnd, wLastProfit);
	std::string status = "+" + std::to_string(value);
	status.erase(status.size() - 4, status.size()); //hacky way to show 2 decimal points 12.34
	convert_to_wchar_t(w_status, status);
	SetWindowText(MainWindow::itemStatusWnd, w_status);

	//show data in history window
	std::string hist = std::to_string(stack) + 'x' + ' ' + name + ' ' + status;
	DisplayHistory(hist, false); //All profit will be show in the history window
	SaveData();
}

//Takes in an item name, stack size and vector where to look for it
bool SessionData::CalculateProfit(const std::string& name, const int& stack, const std::vector<CurrencyInfo::RatesStruct>& rates) {
	for (int i = 0; i < static_cast<int>(rates.size()); ++i) {
		if (name == rates[i].name) {
			std::vector<std::string>vec; //Used to split name into parts and looking for "Scarab" and "Sacrifice"
			float addProfit = stack * rates[i].value; //Calculate profit
			//if passed name is a map, store its value (if map price in settings is set to 0, use price from last update(price found here))
			if (name[name.size() - 3] == 'M' || rates.size() < 25) { //Normal maps end with "Map", unique have specific name and vector size below 25
				lastParsedMap.name = name;
				lastParsedMap.price = addProfit;
				mapAdditionsCurrent.isMap = true;
			}
			//Check for scarab/fragemt, if not break chain and reset lastParsedMap and mapPriceExtra
			//Never add profit for scarabs and fragments if they were parsed after map
			else if (mapAdditionsCurrent.isMap) {
				std::stringstream ss{ name };
				std::string name;
				vec.reserve(5);
				while (ss >> name)
					vec.push_back(name);
				if (vec[vec.size()-1] == "Scarab") { //example: Gilded Divination Scarab
					SetMapPriceExtra(addProfit);
					mapAdditionsCurrent.isScarab = true;
					return true;
				}
				else if (vec[0] == "Sacrifice") { //example: Sacrifice at Noon
					SetMapPriceExtra(addProfit);
					mapAdditionsCurrent.isFragment = true;
					return true;
				}
				else {
					mapAdditionsCurrent.isMap = false;
					lastParsedMap.name = "";
					lastParsedMap.price = 0;
					SetMapPriceExtra(0);
				}
			}

			if (isMapOpened) { //false on script startup, won't calculate and display data until opening one map
				if (!DialogWindow::GetIsGeneral() && !isSpecificMapOpened) {
					return true;
				}
				else {
					profit += addProfit;
					lastProfit += addProfit;
					DisplayProfit(name, addProfit, stack);
					return true;
				}
			}
			else
				return true; //first map not opened, don't parse data
		}
	}
	return false; //nothing found
}

//Read saved data -> Runs, Spent, Profit
void SessionData::InitData() {
	std::ifstream s_data_stream{ "data\\saved_data" };
	if (s_data_stream) {
		s_data_stream >> runs >> spent >> profit;
	}
	s_data_stream.close();
	//Convert data to format used by windows
	convert_to_wchar_t(wProfit, profit);
	convert_to_wchar_t(wSpent, spent);
	convert_to_wchar_t(wRuns, runs);
	DrawData(); //Draw runs, spent and profit values on the screen
}

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

//Parse location and IP, reset data and calculate spent if map is new, display location on main window
void SessionData::UpdateLocation(const std::string& location, const std::string& ip) {
	//Specific map farming
	if (!DialogWindow::GetIsGeneral()) {
		if ((location + " Map" == settings.GetMapName() || location == settings.GetMapName())) { //specific map(regular or unique)
			if (mainMap.ip == ip) {
				if (zanaMap.inMap) { //If entering main map from zanas map
					zanaMap.inMap = false;
					mainMap.inMap = true;
				}
				else {
					mainMap.inMap = true;
					mainMap.portals -= 1;
				}
			}
			else { //Specified map opened
				mainMap.location = location;
				mainMap.ip = ip;
				mainMap.portals = 5;
				mainMap.inMap = true;
				IncrementRuns();
				CalculateSpent("Map");
				isMapOpened = true; //false on script startup, used every run to block parsing until first map is opened/entered
				isSpecificMapOpened = true;
			}
		}
		else if (zanaMap.ip == ip) { //Re-enter zanas map
			zanaMap.inMap = true;
			zanaMap.portals -= 1;
			mainMap.inMap = false;
		}
		else {
			bool isMap = false;
			for (int i = 0; i < CurrencyInfo::mapRates.size(); ++i) { //look trough regular maps
				if (CurrencyInfo::mapRates[i].name == location + " Map") { //Opened map which was not set in settings
					if (mainMap.inMap) { //in zanas map
						zanaMap.location = location;
						zanaMap.ip = ip;
						zanaMap.portals = 5;
						zanaMap.inMap = true;
						mainMap.inMap = false;
					}
					else { //Opened map which is not specified in settings, reset main map data and set isSpecificMapOpened to false(stops item parsing)
						mainMap.ip = "";
						mainMap.location = "";
						mainMap.portals = 0;
						isSpecificMapOpened = false;
						profit -= lastParsedMap.price;
						lastProfit -= lastParsedMap.price;
						convert_to_wchar_t(wProfit, profit);
						convert_to_wchar_t(wLastProfit, lastProfit);
						DrawData();
						DisplayHistory("Used unspecified map, removed from the history!", true);
						lastParsedMap.name = "";
						lastParsedMap.price = 0;
					}
					isMap = true;
					break;
				}
			}
			if (!isMap) { //look trough unique maps
				for (int i = 0; i < CurrencyInfo::uniqueMapRates.size(); ++i) {
					if (CurrencyInfo::mapRates[i].name == location + " Map") {
						if (mainMap.inMap) {
							zanaMap.location = location;
							zanaMap.ip = ip;
							zanaMap.portals = 5;
							zanaMap.inMap = true;
							mainMap.inMap = false;
						}
						else { //Opened map which is not specified in settings, reset main map data
							mainMap.inMap = false;
							mainMap.ip = "";
							mainMap.location = "";
							mainMap.portals = 0;
							isSpecificMapOpened = false;
							profit -= lastParsedMap.price;
							lastProfit -= lastParsedMap.price;
							convert_to_wchar_t(wProfit, profit);
							convert_to_wchar_t(wLastProfit, lastProfit);
							DrawData();
							DisplayHistory("Used unspecified map, removed from the history!", true);
							lastParsedMap.name = "";
							lastParsedMap.price = 0;
						}
						isMap = true;
						break;
					}
				}
			}
			if (!isMap) { //Non map zone
				mainMap.inMap = false;
			}
		}
	}
	else if (DialogWindow::GetIsGeneral()) { 	//General map farming
		if (ip == mainMap.ip) { //Entered map from hideout
			mainMap.portals -= 1;
			mainMap.inMap = true;
		}
		else if (ip == zanaMap.ip) { //Enter zana's map from main map
			zanaMap.portals -= 1;
			zanaMap.inMap = true;
			mainMap.inMap = false;
		}
		else if ((ip == mainMap.ip) && zanaMap.inMap) { //Left zana's map
			zanaMap.inMap = false;
			mainMap.inMap = true;
		}
		else {
			bool mapOpened = false;
			if (location + " Map" == lastParsedMap.name || location == lastParsedMap.name) { //New map opened (regular map or unique)
				mapOpened = true;
				mainMap.location = location;
				mainMap.ip = ip;
				mainMap.portals = 5;
				mainMap.inMap = true;
				IncrementRuns();
				CalculateSpent(location + " Map");
				mapAdditionsCurrent.isMap = false; //Map opened, reset chain (map->fragment<->scarab), used for calculating spent currency(NOTE: Must be executed after CalculateSpent)
				isMapOpened = true; //after script startup this is set to false, which prevents parsing of currency (NOTE: Must be executed after CalculateSpent)
			}
			else if (mainMap.inMap) {
				//Regular Maps
				for (int i = 0; i < static_cast<int>(CurrencyInfo::mapRates.size()); ++i) { //look only for maps, not leveling zones
					if (location + " Map" == CurrencyInfo::mapRates[i].name) {//Entered location is a map within map (zana found)
						zanaMap.location = location;
						zanaMap.ip = ip;
						zanaMap.portals = 5;
						zanaMap.inMap = true;
						mainMap.inMap = false;
						mapOpened = true;
						break;
					}
				}
				//Unique maps
				for (int i = 0; i < static_cast<int>(CurrencyInfo::uniqueMapRates.size()); ++i) {
					if (location == CurrencyInfo::uniqueMapRates[i].name) {
						zanaMap.location = location;
						zanaMap.ip = ip;
						zanaMap.portals = 5;
						zanaMap.inMap = true;
						mainMap.inMap = false;
						mapOpened = true;
						break;
					}
				}
			}
			if (!mapOpened) //If moved to hideout mapOpened is set to false
				mainMap.inMap = false;
		}
	}
	convert_to_wchar_t(w_location, location);
	SetWindowText(MainWindow::locWnd, w_location);
}

//Parsing log for zone changes (running in another thread indefinitely)
void ParseLog() {
	std::ifstream ist{ settings.GetLogLocation() };
	if (!ist) {
		MessageBox(NULL, L"Unable to open Client.txt!", L"Error", NULL);
		return;
	}
	ist.seekg(0, ist.end);
	std::string lastEntry, word, ip;
next_line: while (getline(ist, lastEntry)) {
	std::istringstream lineStream{ lastEntry };
	lineStream.seekg(50);

	while (lineStream >> word) {
		if (settings.GetGlobalDisable())
			continue;
		if (word == "Connecting") { //get ip address of location
			while (lineStream >> word)
				if (isdigit(word[0]))
					ip = word;
		}
		if (word == "entered") { //get entered location name
			std::string location;
			while (lineStream >> word)
				location += word + ' ';
			location.erase(location.size() - 2, location.size() - 1);
			sessionData.UpdateLocation(location, ip);
		}
		if (word == "mediocre") { //The Shaper: The mediocre turned remarkable (line which shaper says on used sextant)
			sessionData.CalculateSpent("CS"); //Cartographer's Sextant
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

//Get item rarity, name and parse its values(stack size, quality, corruption)
void ParseClipboard(std::istringstream& clipStream) {
	std::string stringLine, name;
	std::getline(clipStream, stringLine); //first line -> Rarity: Something

	//parse currency
	if (stringLine[8] == 'C') {
		std::getline(clipStream, name); //get currency name
		name.erase(name.size() - 1); //remove '\r'
		std::getline(clipStream, stringLine); //skip ----------
		std::getline(clipStream, stringLine); //get stack size
		std::istringstream stackStream{ stringLine };
		//sLine example -> Stack Size: 15/20
		stackStream.seekg(12);
		int stackCurrent, stackMax;
		char delimiter;
		stackStream >> stackCurrent >> delimiter >> stackMax;
		sessionData.CalculateProfit(name, stackCurrent, CurrencyInfo::currencyRates);
	}

	//parse Divination Card
	else if (stringLine[8] == 'D') {
		std::getline(clipStream, name); //get card name
		name.erase(name.size() - 1); //remove '\r'
		std::getline(clipStream, stringLine); //skip ----------
		std::getline(clipStream, stringLine); //get stack size
		std::istringstream stackStream{ stringLine };
		stackStream.seekg(12);
		int stackCurrent;
		stackStream >> stackCurrent;
		sessionData.CalculateProfit(name, stackCurrent, CurrencyInfo::divinationCardRates);
	}

	//parse normal items
	else if (stringLine[8] == 'N') { //Rarity: Normal
		std::getline(clipStream, name); //get item name
		name.erase(name.size() - 1); //remove '\r'
		if (name[name.length() - 3] == 'M') { //is item Normal Map? (Example Map)
			sessionData.SetSpentCurrencyOnMap(0); //Reset
			sessionData.SetMapPriceExtra(0); //reset
			std::stringstream removePrefix{ name };
			std::string prefix, newName;
			removePrefix >> prefix;
			if (prefix == "Superior") {
				while (removePrefix >> prefix)
					newName += prefix + ' ';
				newName.erase(newName.size() - 1);
				sessionData.CalculateProfit(newName, 1, CurrencyInfo::mapRates);
			}
			else
				sessionData.CalculateProfit(name, 1, CurrencyInfo::mapRates);
		}
		else
			sessionData.CalculateProfit(name, 1, CurrencyInfo::fragmentRates); //Fragments and scarabs, both are Rarity: Normal
	}

	//parse magic items
	else if (stringLine[8] == 'M') { //Rarity: Magic
		std::getline(clipStream, name); //get item name
		name.erase(name.size() - 1); //remove '\r'
		std::istringstream nameStream{ name };
		std::string word;
		std::vector<std::string> streamParts;
		streamParts.reserve(10);
		bool isMap = false;
		//Get map name without suffix (Slithering Bone Crypt Map of Endurance -> Slithering Bone Crypt Map)
		while (nameStream >> word) {
			if (word == "Superior" || word == "Zana's")
				continue;
			if (word == "Map") { //Magic item is a map, break on "Map" keyword
				streamParts.push_back("Map");
				isMap = true;
				break;
			}
			else
				streamParts.push_back(word); //Push word until "Map" keyword
		}
		if (isMap) {
			std::string mapName;
			bool lookForPrefix = true;
			sessionData.SetSpentCurrencyOnMap(0); //Reset
			sessionData.SetMapPriceExtra(0); //reset
			//Construct full map name without prefix (Slithering Bone Crypt Map -> Bone Crypt Map)
			for (int i = 0; i < static_cast<int>(streamParts.size()); ++i) {
				if (lookForPrefix) {
					for (int b = 0; b < magicMapPrefixes.size(); ++b) {
						if (streamParts[i] == magicMapPrefixes[b]) {
							++i;
							mapName += streamParts[i] + ' ';
							lookForPrefix = false;
							break;
						}
					}
					if (lookForPrefix) { //if still looking for prefix, passed data doesn't contanin one, set it to false and continue constructing map name
						mapName += streamParts[i] + ' '; //No prefix found, start constructing map name
						lookForPrefix = false; //Prefix can only be at the beginning, stop looking for it 
					}
				}
				else
					mapName += streamParts[i] + ' '; //Construct full map name
			}
			mapName.erase(mapName.size() - 1); //remove trailing space
			sessionData.CalculateProfit(mapName, 1, CurrencyInfo::mapRates);
		}
	}

	//parse rare items
	else if (stringLine[8] == 'R') { //Rarity: Rare
		//Steel Zone - Name
		//Cage Map - Base
		std::string itemName, itemBase, word;
		std::getline(clipStream, itemName); //get name
		itemName.erase(itemName.size() - 1); //remove \r
		bool isMap = false;
		if (itemName[itemName.length() - 3] == 'M') { //unidentified map
			std::stringstream removePrefix{ itemName };
			std::string prefix, newName;
			removePrefix >> prefix;
			if (prefix == "Superior") {
				while (removePrefix >> prefix)
					newName += prefix + ' ';
				newName.erase(newName.size() - 1);
				sessionData.CalculateProfit(newName, 1, CurrencyInfo::mapRates);
			}
			else
				sessionData.CalculateProfit(itemName, 1, CurrencyInfo::mapRates);
			isMap = true;
		}
		else {
			getline(clipStream, itemBase); //get base
			itemBase.erase(itemBase.size() - 1); //remove \r
			if (itemBase[itemBase.length() - 3] == 'M') { //identified map
				isMap = true;
				sessionData.CalculateProfit(itemBase, 1, CurrencyInfo::mapRates);
			}
		}
		//Look for other mods, quality and corruption and add them to mapPriceExtra variable
		if (isMap) {
			sessionData.SetSpentCurrencyOnMap(0); //Reset
			sessionData.SetMapPriceExtra(0); //reset
			sessionData.SetSpentCurrencyOnMap(1 * CurrencyInfo::alchPrice); //add alchemy spent for rare map
			while (clipStream >> word) {
				if (word == "Quality:") {
					clipStream.seekg(2, std::ios_base::cur);
					std::string quality;
					clipStream >> quality;
					quality.erase(quality.size() - 1);
					int qualityNumber = stoi(quality);
					if (qualityNumber >= 17 && qualityNumber <= 19) //add 3xChisel to spent map (case: foud map with 3 or 4 quality, not spending 4th chisel for few missing quality)
						sessionData.SetSpentCurrencyOnMap(3 * CurrencyInfo::chiselPrice);
					else if (qualityNumber == 20)
						sessionData.SetSpentCurrencyOnMap(4 * CurrencyInfo::chiselPrice);
				}
				if (word == "Corrupted") //add vaal orb spent on map
					sessionData.SetSpentCurrencyOnMap(1 * CurrencyInfo::vaalPrice);
			}
		}
	}

	//parse unique maps
	else if (stringLine[8] == 'U') {
		std::string itemName, itemBase;
		std::getline(clipStream, itemName); //get name
		itemName.erase(itemName.size() - 1); //remove \r
		if (itemName[itemName.length() - 3] == 'M') { //unidentified map
			std::stringstream line{ itemName };
			std::string prefix;
			line >> prefix;
			if (prefix == "Superior") {
				itemName = "";
				while (line >> prefix)
					itemName += prefix + ' ';
				itemName.erase(itemName.size() - 1);
			}
			for (int i = 0; i < mapData.uniqueMaps.size(); ++i)
				if (itemName == mapData.uniqueMaps[i].base) {
					sessionData.SetSpentCurrencyOnMap(0); //Reset
					sessionData.SetMapPriceExtra(0); //reset
					sessionData.CalculateProfit(mapData.uniqueMaps[i].name, 1, CurrencyInfo::uniqueMapRates);
					break;
				}
		}
		else {
			getline(clipStream, itemBase); //get base
			//itemBase.erase(itemBase.size() - 1); //remove \r
			if (itemBase[itemBase.length() - 4] == 'M') { //identified map (-4 if \\r is not removed)
				sessionData.SetSpentCurrencyOnMap(0); //Reset
				sessionData.SetMapPriceExtra(0); //reset
				sessionData.CalculateProfit(itemName, 1, CurrencyInfo::uniqueMapRates); //pass item name not base
			}
		}
	}
}

//Read clipboard contents(only if formated as text) into the stream and parse that data
void GetClipboardData() {
	if (IsClipboardFormatAvailable(CF_TEXT)) {
		HANDLE clip;
		OpenClipboard(NULL);
		clip = GetClipboardData(CF_TEXT);
		EmptyClipboard(); //fast clicking can parse same item multiple times
		CloseClipboard();
		std::string clipContents = (char*)clip;
		std::istringstream sStream{ clipContents };
		ParseClipboard(sStream);
	}
}

//If Ctrl + LMB is excuted on the left side of the screen(game client, not desktop) -> moving items from stash, right side -> moving items to stash
bool GetMousePosition() {
	HWND hwnd = GetForegroundWindow();
	RECT rect;
	GetWindowRect(hwnd, &rect);
	POINT p;
	GetCursorPos(&p);
	ScreenToClient(hwnd, &p);
	if (p.x < ((rect.right - rect.top) / 2)) //left side, moving items from stash (don't execute copy command)
		return false;
	else
		return true; //right side, moving items to stash
}

//Runs parser if Client.txt is found, otherwise prompts for user input
void RunLogParser() {
	//Check if Client.txt exists, if not, let use enter path to the file
	std::string test = settings.GetLogLocation();
	std::ifstream ist{ test };
	if (!ist) {
		DialogWindow::OpenSetLogLocation();
		return;
	}
	std::thread parser(ParseLog);
	parser.detach();
}

//Check if user put numerical value in the edit box
bool CheckDigits(std::string value) {
	bool dotFound = false;
	for (int i = 0; i < value.size(); ++i)
		if (!isdigit(value[i])) {
			if (value[i] == '-' && i == 0)
				continue;
			else if (value[i] == '-' && i != 0) {
				MessageBox(NULL, L"Invalid entry, only numerical values allowed (0.00)!", L"Error", NULL);
				return false;
			}
			if (value[i] != '.' || dotFound) {
				MessageBox(NULL, L"Invalid entry, only numerical values allowed (0.00)!", L"Error", NULL);
				return false;
			}
			else
				dotFound = true;
		}
	return true;
}

bool CALLBACK SetFont(HWND child, LPARAM font) {
	SendMessage(child, WM_SETFONT, font, true);
	return true;
}

void CheckVersion() {
	GetRepoData();
	std::ifstream dataIn{ "data\\github_raw" };
	std::string keyword, ver;
	while (dataIn >> keyword) {
		if (keyword == "\"tag_name\":") { //Example - "tag_name": "v1.0.0",
			dataIn >> ver;
			ver.erase(ver.begin());
			ver.erase(ver.end() - 2, ver.end());
			if (ver != version) //Version is a global variable
				ShowWindow(MainWindow::updateButton, SW_SHOW);
			break;
		}
	}
	dataIn.close();
	remove("data\\github_raw");
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
			wchar_t wProfit[20];
			GetWindowText(DialogWindow::plusProfitWnd, wProfit, (sizeof(wProfit) / 2));
			std::wstring wsProfit(wProfit);
			std::string strProfit(wsProfit.begin(), wsProfit.end());
			if (CheckDigits(strProfit)) {
				sessionData.ManualData(0, stof(strProfit));
				std::string hist = "Manually added profit +" + strProfit;
				sessionData.DisplayHistory(hist, false);
			}
			else break;

			DestroyWindow(hWnd);
			break;
		}
		case DIALOG_WINDOW_PLUS_SPENT: {
			wchar_t wSpent[20];
			GetWindowText(DialogWindow::plusSpentWnd, wSpent, (sizeof(wSpent) / 2));
			std::wstring wsSpent(wSpent);
			std::string strSpent(wsSpent.begin(), wsSpent.end());
			if (CheckDigits(strSpent)) {
				sessionData.ManualData(stof(strSpent), 0);
				std::string hist = "Manually added spent +" + strSpent;
				sessionData.DisplayHistory(hist, false);
			}
			else break;

			DestroyWindow(hWnd);
			break;
		}
		case DIALOG_WINDOW_CHECKBOX_GENERAL:
			//Minor visual change to indicate that specific map name and price won't be used when parsing values
			if (IsDlgButtonChecked(hWnd, DIALOG_WINDOW_CHECKBOX_GENERAL)) {
				EnableWindow(DialogWindow::setMapWnd, false);
				EnableWindow(DialogWindow::setMapPriceWnd, false);
				SetWindowText(DialogWindow::setMapPriceWnd, L"0");
				SetWindowText(DialogWindow::setMapWnd, L"General");
			}
			else {
				EnableWindow(DialogWindow::setMapWnd, true);
				EnableWindow(DialogWindow::setMapPriceWnd, true);
				SetWindowText(DialogWindow::setMapWnd, L"Example Map");
				SetWindowText(DialogWindow::setMapPriceWnd, settings.wMapPrice);
			}
			break;
		case DIALOG_WINDOW_SAVE_OVERLAY_SETTINGS: {
			wchar_t xP[5], yP[5];
			GetWindowText(DialogWindow::setXPosWnd, xP, (sizeof(xP) / 2));
			GetWindowText(DialogWindow::setYPosWnd, yP, (sizeof(yP) / 2));
			std::wstring ws_xp(xP), ws_yp(yP);
			std::string str_xp(ws_xp.begin(), ws_xp.end()), str_yp(ws_yp.begin(), ws_yp.end());
			if (!CheckDigits(str_xp))
				break;
			if (!CheckDigits(str_yp))
				break;
			OverlayWindow::SetPosition(stof(str_xp), stof(str_yp));
			settings.SaveSettings(); //save settings after updating overlay coordinates
			DestroyWindow(hWnd);
			break;
		}
		case DIALOG_WINDOW_SAVE_MAP:
			settings.UpdateMapSettings(hWnd);
			DestroyWindow(hWnd);
			break;
		case DIALOG_WINDOW_SAVE_IIQ: {
			GetWindowText(DialogWindow::setIIQWnd, settings.wIIQ, 5);
			SetWindowText(MainWindow::IIQWnd, settings.wIIQ);
			std::wstring wsIIQ(settings.wIIQ);
			std::string strIIQ(wsIIQ.begin(), wsIIQ.end());
			settings.SetIIQ(stoi(strIIQ));
			settings.SaveSettings();
			DestroyWindow(hWnd);
			break;
		}
		case DIALOG_WINDOW_SAVE_CS:
			if (CurrencyInfo::currencyRatesLoaded || CurrencyInfo::currencyRatesUpdated) {
				GetWindowText(DialogWindow::setCSWnd, settings.wCS, 10);
				SetWindowText(MainWindow::CSWnd, settings.wCS);
				std::wstring wsCS(settings.wCS);
				std::string strCS(wsCS.begin(), wsCS.end());
				settings.SetCS(strCS);
				settings.ParseCS();
				settings.SaveSettings();
			}
			else
				MessageBox(NULL, L"Currency rates didn't load or update properly, can't parse CS average price!", L"Warning", NULL);
			DestroyWindow(hWnd);
			break;
		case DIALOG_WINDOW_SAVE_LEAGUE: {
			GetWindowText(DialogWindow::comboBoxWnd, settings.wLeague, 15);
			SetWindowText(MainWindow::leagueWnd, settings.wLeague);
			std::wstring wsLeague(settings.wLeague);
			std::string strLeague(wsLeague.begin(), wsLeague.end());
			if (strLeague == "HC Synthesis")
				strLeague = "Hardcore Synthesis";
			if (!(settings.GetLeague() == strLeague)) {
				settings.SetLeague(strLeague);
				settings.SaveSettings();
				DialogWindow::UpdateCurrencyDialog(hWnd, CurrencyInfo::numberOfRateGroups);
				MessageBox(NULL, L"League changed, please update currency rates!", L"Warning!", NULL);
			}
			DestroyWindow(hWnd);
			break;
		}
		case DIALOG_WINDOW_SAVELOC: {
			wchar_t log[200];
			GetWindowText(DialogWindow::logLoc, log, 200);
			std::wstring ws_log(log);
			std::string sLog(ws_log.begin(), ws_log.end()); // Example - C:\Program Files (x86)\Steam
			std::string usableLoc; //location must have this format c://Program Files (x86)//Steam//
			for (int i = 0; i < static_cast<int>(sLog.size()); ++i) {
				if (sLog[i] == '\\') // Escape sequences are used to represent certain special characters within string literals and character literals. Backslash needs to be escaped
					usableLoc += "//";
				else
					usableLoc += sLog[i];
			}
			usableLoc += "//Client.txt";
			settings.SetLogLocation(usableLoc);
			settings.SaveSettings();
			RunLogParser();
			DestroyWindow(hWnd);
			break;
		}
		case DIALOG_WINDOW_UPDATE_CURRENCY: {
			EnableWindow(DialogWindow::updateButton, false);
			SetWindowText(DialogWindow::updateStatusWnd, L"Please Wait...");
			std::thread update(CurrencyInfo::UpdateCurrencyRates, settings.GetLeague());
			update.detach();
			break;
		}
		case DIALOG_WINDOW_CANCEL:
			DestroyWindow(hWnd);
			break;
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
			if (GetKeyState(VK_CONTROL) < 0) { //Parse hotkey activated
				if (!settings.GetGlobalDisable() && GetMousePosition()) {//Left/Right side of the focused window, parse items only if moved from right and parsing is enabled
					SendCopyCommand(); //press virtual C-key, needed for Ctrl+C
					std::this_thread::sleep_for(std::chrono::milliseconds(30)); //wait a bit to be sure that data made it to the clipboard
					GetClipboardData();
					sessionData.SaveData(); //save parsed values (runs, spent, profit..)
				}
			}
			break;
		case WM_RBUTTONDOWN:
			if (GetKeyState(VK_CONTROL) < 0) { //Disable parse hotkey activated
				settings.SetGlobalDisable(); //if activated disable parsing of items
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
		case STN_CLICKED:
			if (MainWindow::patreonWnd == (HWND)lParam)
				ShellExecute(NULL, L"open", L"https://www.patreon.com/cailoki", NULL, NULL, SW_SHOWNORMAL);
			else if (MainWindow::gitWnd == (HWND)lParam)
				ShellExecute(NULL, L"open", L"https://github.com/Cailoki/PoE-FarmingTool", NULL, NULL, SW_SHOWNORMAL);
			else if (MainWindow::dicordWnd == (HWND)lParam)
				ShellExecute(NULL, L"open", L"https://discord.gg/D2m7Kpw", NULL, NULL, SW_SHOWNORMAL);
			else if (MainWindow::overlayButton == (HWND)lParam)
				if (MainWindow::overlayOn) {
					MainWindow::overlayOn = false;
					PostMessage(OverlayWindow::hWndOverlay, WM_CLOSE, NULL, NULL);
				}
				else {
					MainWindow::overlayOn = true;
					sessionData.OpenOverlay();
				}
			else if (MainWindow::profitButton == (HWND)lParam)
				DialogWindow::OpenPlusProfitDialog(hWnd);
			else if (MainWindow::spentButton == (HWND)lParam)
				DialogWindow::OpenPlusSpentDialog(hWnd);
			else if (MainWindow::updateButton == (HWND)lParam)
				ShellExecute(NULL, L"open", L"https://github.com/Cailoki/PoE-FarmingTool/releases", NULL, NULL, SW_SHOWNORMAL);
			break;
		case SETTINGS_MENU_MAP:
			DialogWindow::OpenMapDialog(hWnd, settings.wMap, settings.wMapPrice, settings.wZanaModPrice);
			break;
		case SETTINGS_MENU_IIQ:
			DialogWindow::OpenIIQDialog(hWnd, settings.wIIQ);
			break;
		case SETTINGS_MENU_CS:
			DialogWindow::OpenSextantDialog(hWnd, settings.wCS);
			break;
		case SETTINGS_MENU_LEAGUE:
			DialogWindow::OpenLeagueDialog(hWnd);
			break;
		case SETTINGS_MENU_OVERLAY:
			DialogWindow::OpenOverlayDialog(hWnd, OverlayWindow::GetXPos(), OverlayWindow::GetYPos());
			break;
		case FILE_MENU_RESET: {
			int code = MessageBox(hWnd, L"Reset runs, spent and profit?", L"Reset data", MB_YESNO);
			if (code == 6) //The Yes button was selected. 
				sessionData.ResetData();
		}
			break;
		case HELP_TIPS:
			DialogWindow::OpenTipsDialog(hWnd);
			break;
		case HELP_ABOUT:
			DialogWindow::OpenAboutDialog(hWnd);
			break;
		case FILE_MENU_EXIT:
			DestroyWindow(hWnd);
			break;
		case CURRENCY_MENU_UPDATE:
			DialogWindow::UpdateCurrencyDialog(hWnd, CurrencyInfo::numberOfRateGroups); //dialog with progress bar
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
		MainWindow::ShowUpdatingText(hWnd); //profit, spent... settings(league, map...)
		EnumChildWindows(hWnd, (WNDENUMPROC)SetFont, (LPARAM)hFont);
		hbrBackground = CreateSolidBrush(RGB(255, 255, 255)); //white bacground for updating text windows
	}
		break;
	case WM_PAINT: {
		hdc = BeginPaint(hWnd, &ps);
		SetBkMode(hdc, TRANSPARENT);
		SelectObject(hdc, hFont);
		MainWindow::ShowLabels(hdc);
		sessionData.DrawData();	  //repaint data (runs, profit, spent) when closing the dialog window
		settings.DrawSettings(); //repaint settings when closing the dialog window
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

int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_ HINSTANCE, _In_ LPSTR, _In_ int) //hPrevInstance, lpCmdLine and nCmdShow removed because not used, clears warning: unreferenced formal parameter
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

	RegisterClassEx(&wcex);
	DialogWindow::RegisterDialogClass();
	OverlayWindow::RegisterOverlayClass();

	RECT wr = { 0, 0, 420, 420 }; //exact client area
	AdjustWindowRect(&wr, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, TRUE); //calculate window size out of the client area and other styles using title/menu bar
	CreateWindowW(L"DesktopApp", L"PoE Farming Tool", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, wr.right - wr.left, wr.bottom - wr.top, NULL, NULL, hInstance, NULL);

	//initialize data, settings, currency, log parser and mouse hook
	std::thread check(CheckVersion);
	check.detach();
	CurrencyInfo::InitCurencyRates(); //must be called before InitSettings so ParseCS can calculate price
	settings.InitSettings();
	sessionData.InitData();
	RunLogParser();
	hookHandleMouse = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, NULL, 0);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	UnhookWindowsHookEx(hookHandleMouse); //unhook mouse handle before exiting app
	return (int)msg.wParam;
}

//Read saved settings -> Map, IIQ, Sextant combination, league, coordinates...
void Settings::InitSettings() {
	std::ifstream sSettingsStream{ "data\\saved_settings" };
	if (sSettingsStream) {
		//mapname, iiq, cs, mapPrice, league, mapFarmingType(true/false), x coord(overlay), y coord(overlay), zanaModPrice, clientLogLocation
		std::getline(sSettingsStream, mapName);
		sSettingsStream >> iiq >> cs >> mapPrice;
		sSettingsStream.ignore(); //Ignore newline character \n
		std::getline(sSettingsStream, league);
		float xP, yP; //Overlay coordinates
		sSettingsStream >> mapFarmingType >> xP >> yP >> zanaModPrice;
		sSettingsStream.ignore(); //Ignore newline character \n
		getline(sSettingsStream, clientLogLocation);
		//Initiate overlay position
		OverlayWindow::SetPosition(xP, yP);
		//Initiate general farming
		if (mapFarmingType == "true") {
			DialogWindow::SetIsGeneral(true);
		}
	}
	sSettingsStream.close();
	ParseCS(); //works only if InitSettings is called after InitCurrencyRates!

	//Convert data to format used by windows
	convert_to_wchar_t(wMap, mapName);
	convert_to_wchar_t(wMapPrice, mapPrice);
	convert_to_wchar_t(wZanaModPrice, zanaModPrice);
	convert_to_wchar_t(wIIQ, iiq);
	convert_to_wchar_t(wCS, cs);
	//Write short "HC Synthesis" on the window
	if (league == "Hardcore Synthesis") {
		std::string temp = "HC Synthesis";
		convert_to_wchar_t(wLeague, temp);
	}
	else
		convert_to_wchar_t(wLeague, league);

	DrawSettings(); //Draw settings on the window
}

//Save setting to a file
void Settings::SaveSettings() {
	//mapname, iiq, cs, mapPrice, league, mapFarmingType(true/false), x coord(overlay), y coord(overlay), zanaModPrice, clientLogLocation
	std::ofstream ost{ "data\\saved_settings" };
	ost << mapName << '\n' << iiq << '\n' << cs << '\n' << mapPrice << '\n' << league << '\n' << mapFarmingType << '\n' << OverlayWindow::GetXPos() << '\n' << OverlayWindow::GetYPos() << '\n' << zanaModPrice << '\n' << clientLogLocation;
}

//Draw values after init or on every paint call
const void Settings::DrawSettings() const {
	SetWindowText(MainWindow::mapNameWnd, wMap);
	SetWindowText(MainWindow::mapPriceWnd, wMapPrice);
	SetWindowText(MainWindow::IIQWnd, wIIQ);
	SetWindowText(MainWindow::CSWnd, wCS);
	SetWindowText(MainWindow::leagueWnd, wLeague);
}

//Calculate ave0rage price of the cartographer sextants which is set in the settings (this price will be added to the spent value when log shows used sextant)
void Settings::ParseCS() const {
	//Check if user entered correct format (0-0-0)
	if (cs.size() != 5 || !isdigit(cs[0]) || !isdigit(cs[2]) || !isdigit(cs[4]) || isdigit(cs[1]) || isdigit(cs[3])) {
		MessageBox(NULL, L"Wrong format of set cartographer's sextants, please save them in the correct format (0-0-0)!", L"Warning", NULL);
		return;
	}

	float appPrice = 0, jouPrice = 0, masPrice = 0;
	for (int i = 0; i < static_cast<int>(CurrencyInfo::currencyRates.size()); ++i) {
		if ("Apprentice Cartographer's Sextant" == CurrencyInfo::currencyRates[i].name)
			appPrice = CurrencyInfo::currencyRates[i].value;
		else if ("Journeyman Cartographer's Sextant" == CurrencyInfo::currencyRates[i].name)
			jouPrice = CurrencyInfo::currencyRates[i].value;
		else if ("Master Cartographer's Sextant" == CurrencyInfo::currencyRates[i].name)
			masPrice = CurrencyInfo::currencyRates[i].value;
	}
	int appCount = cs[0] - '0', jouCount = cs[2] - '0', masCount = cs[4] - '0'; //string example of cartographer sextant setting "0-0-0" (convert char '9' to int 9 with ('9' - '0'))
	int totalCount = appCount + jouCount + masCount;
	if (totalCount == 0)
		return;
	sessionData.SetCsPrice((appPrice*appCount + jouPrice * jouCount + masPrice * masCount) / totalCount); //average price used for increasing spent value when parser detects used sextant
}

//Get values from map settings dialog (name, price, zana mod)
void Settings::UpdateMapSettings(HWND hWnd) {
	//Get text from dialog window
	GetWindowText(DialogWindow::setMapWnd, wMap, (sizeof(wMap) / 2));
	GetWindowText(DialogWindow::setMapPriceWnd, wMapPrice, (sizeof(wMapPrice) / 2));
	GetWindowText(DialogWindow::setZanaModPrice, wZanaModPrice, (sizeof(wZanaModPrice) / 2));
	//Display received text on the main window
	SetWindowText(MainWindow::mapPriceWnd, wMapPrice);
	SetWindowText(MainWindow::mapNameWnd, wMap);
	//Check status of checkbox for general farming
	if (IsDlgButtonChecked(hWnd, DIALOG_WINDOW_CHECKBOX_GENERAL)) {
		DialogWindow::SetIsGeneral(true);
		mapFarmingType = "true"; //Used to save value so it can be reloaded on startup
		SetWindowText(MainWindow::mapNameWnd, L"General");
		SetWindowText(MainWindow::mapPriceWnd, wMapPrice);
		sessionData.ClearIsMapEntered(); //Reset status "start over" (parse only maps until new map i opened)
	}
	else {
		mapFarmingType = "false"; //Used to save value so it can be reloaded on startup
		sessionData.ClearIsMapEntered();
		DialogWindow::SetIsGeneral(false);
	}
	//Convert wchar_t to usable format and assign to local members for later use
	std::wstring ws_mp(wMap), ws_pr(wMapPrice), ws_mod(wZanaModPrice);
	std::string map(ws_mp.begin(), ws_mp.end()), price(ws_pr.begin(), ws_pr.end()), modPrice(ws_mod.begin(), ws_mod.end());
	mapName = map;
	mapPrice = stof(price);
	zanaModPrice = stoi(modPrice);

	SaveSettings();
}

//Disables all parsing, Ctrl+LMB won't go trough
void Settings::SetGlobalDisable() {
	MessageBeep(MB_ICONSTOP);
	if (globalDisable) {
		globalDisable = false;
		RedrawWindow(OverlayWindow::disableWnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE);
		SetWindowText(OverlayWindow::disableWnd, L"");
		SetWindowText(MainWindow::itemMovedWnd, L"");
	}
	else {
		globalDisable = true;
		RedrawWindow(OverlayWindow::disableWnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE);
		SetWindowText(OverlayWindow::disableWnd, L"*** DISABLED ***");
		SetWindowText(MainWindow::itemMovedWnd, L"*** TOOL DISABLED ***");
	}
}