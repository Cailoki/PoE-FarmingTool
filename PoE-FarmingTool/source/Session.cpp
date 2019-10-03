#include "pch.h"
#include "Session.h"
#include "UpdateData.h"
#include "Calculator.h"
#include "Settings.h"
#include "Window.h"
#include "Utilities.h"

SessionData& SessionData::GetInstance() {
	static SessionData sessionData;
	return sessionData;
}

MapManager& MapManager::GetInstance() {
	static MapManager mapManager;
	return mapManager;
}

LocationManager& LocationManager::GetInstance() {
	static LocationManager locationManager;
	return locationManager;
}

ItemManager& ItemManager::GetInstance() {
	static ItemManager itemManager;
	return itemManager;
}

void SessionData::SubtractSpentCS() {
	spent += CurrencyInfo::csPrice;
	profit -= CurrencyInfo::csPrice;

	//Message displayed in history window
	std::wstring newVal = std::to_wstring(CurrencyInfo::csPrice);
	MainWindow::AddToHistory(L"Sextant Used! -" + newVal.erase(newVal.size() - 4, newVal.size()));

	OnDataChanged();
}

void SessionData::SubtractSpentOnMap() {
	float value = Calculator::CalculateSpentOnMap(MapManager::GetInstance().GetMap());

	//Excluding first map after startup, remove last map  value from lastProfit, generate report and reset data
	if (GetIsFirstMapOpened()) {
		lastProfit -= (MapManager::GetInstance().GetMap().GetValue() + MapManager::GetInstance().GetAdditionsPrice()); //Subtract only map value and added scarab, fragment...
		MainWindow::AddToHistory(L"- - - - - - - - - - - - - - -");
		MainWindow::AddToHistory(MapReport());
		MainWindow::AddToHistory(L"- - - - - - - - - - - - - - -");
		ResetLastRunValues();
	}

	spent += value;
	lastSpent += value;

	//On startup isFirstMapOpened is false
	if (GetIsFirstMapOpened()) {
		//1. Subtract map price from profit, because it was added on last move/parse to cancle out that action
		//2. Subtract map price with used currency on it, to show real profit
		profit -= (MapManager::GetInstance().GetMap().GetValue() + MapManager::GetInstance().GetAdditionsPrice() + value);
		lastProfit -= value;
	}
	else {
		profit -= value;
		lastProfit -= value;
		isFirstMapOpened = true;
	}

	//Message displayed in history window
	std::wstring newVal = std::to_wstring(value);
	if (MapManager::GetInstance().GetMap().GetRarity() == L"Unique")
		MainWindow::AddToHistory(MapManager::GetInstance().GetMap().GetName() + L" used! -" + newVal.erase(newVal.size() - 4, newVal.size()));
	else
		MainWindow::AddToHistory(MapManager::GetInstance().GetLastParsedMapName() + L" used! -" + newVal.erase(newVal.size() - 4, newVal.size()));
}

//Redraw and save session data
void SessionData::OnDataChanged() {
	MainWindow::RedrawData();
	SaveSessionData();
}

//Reset runs, spent and profit, convert to wchars and draw on the window
void SessionData::ResetData() {
	//Reset farming values
	runs = 0;
	spent = 0;
	lastSpent = 0;
	lastProfit = 0;
	profit = 0;

	//Stop parsing untill next map is opened
	SetIsFirstMapOpened(false);

	OnDataChanged();
}

void SessionData::AddSpentManual(const std::pair<std::wstring, bool>& data) {
	if (Utilities::CheckDigits(data.first)) {
		spent += stof(data.first);
		lastSpent += stof(data.first);
	}

	if (data.second) {
		profit -= stof(data.first);
		lastProfit -= stof(data.first);
	}

	//Message displayed in history window
	std::wstring newVal = std::to_wstring(stof(data.first));
	MainWindow::AddToHistory(L"Manually added spent +" + newVal.erase(newVal.size() - 4, newVal.size()));

	OnDataChanged();
}

void SessionData::AddProfitManual(const float& value) {
	profit += value;
	lastProfit += value;

	//Message displayed in history window
	std::wstring newVal = std::to_wstring(value);
	MainWindow::AddToHistory(L"Manually added profit +" + newVal.erase(newVal.size() - 4, newVal.size()));

	OnDataChanged();
}

void SessionData::AddProfit(const Item& item) {
	if (item.GetValue() == 0)
		return;

	//Don't add/show zero valued items
	if (Settings::GetInstance().GetIsTrash()) {
		if ((item.GetType() != L"Currency") && (item.GetValue() <= 1)) {
			MainWindow::AddToHistory(ItemManager::GetInstance().GetLastParsedItem().GetName() + L" -trash!");
			return;
		}
	}

	profit += (item.GetValue() * item.GetStack());
	lastProfit += (item.GetValue() * item.GetStack());

	//Message displayed in history window
	std::wstring newVal = std::to_wstring(item.GetValue() * item.GetStack());
	std::wstring name = ItemManager::GetInstance().GetLastParsedItem().GetBase();
	if ((name == L"Unknown") || (ItemManager::GetInstance().GetLastParsedItem().GetRarity() == L"Unique"))
		name = ItemManager::GetInstance().GetLastParsedItem().GetName();
	MainWindow::AddToHistory(std::to_wstring(ItemManager::GetInstance().GetLastParsedItem().GetStack()) + L"x" + name + L" +" + newVal.erase(newVal.size() - 4, newVal.size()));

	OnDataChanged();
}

void SessionData::LoadSessionData() {
	std::wifstream ist{ sessionFileName };
	if (ist) {
		ist >> runs >> spent >> profit;
	}
	else
		SaveSessionData(); //Save defaults
}

void SessionData::SaveSessionData() {
	std::ofstream ost{ sessionFileName };
	ost << runs << '\n'
		<< spent << '\n'
		<< profit;
}

void LocationManager::OnMapOpened() {
	SessionData::GetInstance().IncrementRuns();
	if (SessionData::GetInstance().GetIsFirstMapOpened())
		MainWindow::CleanHistory(MapManager::GetInstance().GetHistoryEntries());
	SessionData::GetInstance().SubtractSpentOnMap();
	MapManager::GetInstance().ResetManager();
	SessionData::GetInstance().OnDataChanged();
}

void OnUnspecifiedMapOpened() {
	MainWindow::CleanHistory(MapManager::GetInstance().GetHistoryEntries());
	if (SessionData::GetInstance().GetIsFirstMapOpened())
		SessionData::GetInstance().CleanLastProfit();

	//Show last map data
	MainWindow::AddToHistory(L"- - - - - - - - - - - - - - -");
	MainWindow::AddToHistory(SessionData::GetInstance().MapReport());
	MainWindow::AddToHistory(L"- - - - - - - - - - - - - - -");

	//Stop adding profit untill specified map is opened
	SessionData::GetInstance().SetIsFirstMapOpened(false);
	SessionData::GetInstance().ResetLastRunValues();
	SessionData::GetInstance().OnDataChanged();
}

//Clean profit added for the last parsed map which was opened, and is different from map user specified
void SessionData::CleanLastProfit() {
	if (isFirstMapOpened) {
		float value = ItemManager::GetInstance().GetLastParsedItem().GetValue();
		profit -= value;
		lastProfit -= value;
	}

	OnDataChanged();
}

//Show last run values in history before reset
const std::wstring SessionData::MapReport() {
	std::wstring strProfit = std::to_wstring(SessionData::GetInstance().GetLastProfit());
	strProfit.erase(strProfit.size() - 4, strProfit.size());
	std::wstring strSpent = std::to_wstring(SessionData::GetInstance().GetLastSpent());
	strSpent.erase(strSpent.size() - 4, strSpent.size());
	return std::wstring(L"Map finished! Spent: " + strSpent + L" - Profit: " + strProfit);
}

void LocationManager::ClassifyLocation(const Location& location) {
	//Map saved in settings
	bool isSpecificMap = (Settings::GetInstance().GetMapName().find(location.GetLocationName()) != std::wstring::npos);
	//Last parsed map
	bool isMap = false;
	if (MapManager::GetInstance().GetMap().GetRarity() == L"Unique") { //Search unique by name
		isMap = (MapManager::GetInstance().GetMap().GetName().find(location.GetLocationName()) != std::wstring::npos);
	}
	else //Search non-unique maps by base
		isMap = (MapManager::GetInstance().GetMap().GetBase().find(location.GetLocationName()) != std::wstring::npos);
	//Hideout
	bool isHideout = (location.GetLocationName().find(L"Hideout") != std::wstring::npos);

	//Set current location
	currentLocation = location;

	//Show location name on the main window
	MainWindow::DrawLocation(location.GetLocationName());

	//Check if location is Hideout
	if (isHideout) {
		inMap = false;
		inZanaMap = false;
		return;
	}

	if (Settings::GetInstance().GetIsGeneral()) {
		//Check if location is Zana's Map
		if (inMap) {
			zanaMap = location;
			inZanaMap = true;
			inMap = false;
		}
		//Check if moved from Zana's map to main map
		else if (inZanaMap && (location.GetIp() == mainMap.GetIp())) {
			inZanaMap = false;
			inMap = true;
		}
		//Check if location is map opened trough map device
		else if (isMap && (location.GetIp() != mainMap.GetIp())) {
			mainMap = location;
			inMap = true;

			OnMapOpened();
		}
	}
	else {
		if (inMap) {
			zanaMap = location;
			inZanaMap = true;
			inMap = false;
		}
		else if (inZanaMap && (location.GetIp() == mainMap.GetIp())) {
			inZanaMap = false;
			inMap = true;
		}
		else if (isSpecificMap && (location.GetIp() != mainMap.GetIp())) {
			mainMap = location;
			inMap = true;

			OnMapOpened();
		}
		else if (isMap) {
			OnUnspecifiedMapOpened();
		}
	}
}

void LocationManager::AddLocation(const Location& location) {
	if (locations.size() < 10) {
		locations.push_back(location);
	}
	else {
		locations.erase(locations.begin());
		locations.push_back(location);
	}
}

void MapManager::ClassifyInput(const Item& item) {
	if (item.GetType() == L"Map") {
		lastParsedMapName = item.GetBase();
		isMap = true;
		map = item;
		isScarabAdded = false;
		scarabs.clear();
		isFragmentAdded = false;
		fragments.clear();
		isVesselAdded = false;
		historyEntries = 1;
	}
	else if ((item.GetType() == L"Scarab") && isMap) {
		isScarabAdded = true;
		if (!CheckDuplicate(item, scarabs))
			scarabs.push_back(item);
		historyEntries++;
	}
	else if ((item.GetType() == L"Fragment") && isMap) {
		isFragmentAdded = true;
		if (!CheckDuplicate(item, fragments))
			fragments.push_back(item);
		historyEntries++;
	}
	else if ((item.GetName() == L"Divine Vessel") && isMap) {
		isVesselAdded = true;
		vessel = item;
		historyEntries++;
	}
	else
		ResetManager();
}

void MapManager::ResetManager() {
	lastParsedMapName = L"";
	isMap = false;
	isScarabAdded = false;
	scarabs.clear();
	isFragmentAdded = false;
	fragments.clear();
	isVesselAdded = false;
	historyEntries = 0;
}

bool MapManager::CheckDuplicate(const Item& item, const std::vector<Item>& data) {
	for (size_t i = 0; i < data.size(); ++i)
		if (data[i].GetName() == item.GetName())
			return true;
	return false;
}

void SessionData::ResetLastRunValues() {
	lastProfit = 0;
	lastSpent = 0;

	OnDataChanged();
}

void ItemManager::OnItemClassified() {
	//Set item value
	lastParsedItem.SetValue(Calculator::CalculateItemValue(lastParsedItem));

	//Check if item is a map, fragment or scarab used with a map
	MapManager::GetInstance().ClassifyInput(lastParsedItem);

	//Parse only if map is opened or if no condition is enabled
	if (Settings::GetInstance().GetIsParseAll()) {
		SessionData::GetInstance().AddProfit(lastParsedItem);
	}
	else if (SessionData::GetInstance().GetIsFirstMapOpened())
		SessionData::GetInstance().AddProfit(lastParsedItem);
}

void ItemManager::ClassifyItem(Item item) {
	if (item.GetRarity() == L"Currency") {
		ParseCurrency(item);
	}
	else if (item.GetRarity() == L"Divination Card") {
		//No special classification
	}
	else if (item.GetRarity() == L"Normal") {
		ParseNormalItems(item);
	}
	else if (item.GetRarity() == L"Magic") {
		ParseMagicItems(item);
	}
	else if (item.GetRarity() == L"Rare") {
		ParseRareItems(item);
	}
	else if (item.GetRarity() == L"Unique") {
		ParseUniqueItems(item);
	}

	lastParsedItem = item;

	OnItemClassified();
}

//Set additional fields based on item's name, could be Fossil, Resonator...
void ItemManager::ParseCurrency(Item& item) {
	if (item.GetName().find(L"Fossil") != std::wstring::npos) {
		item.SetType(L"Fossil");
	}
	else if (item.GetName().find(L"Resonator") != std::wstring::npos) {
		item.SetType(L"Resonator");
	}
	else if (item.GetName().find(L"Oil") != std::wstring::npos) {
		item.SetType(L"Oil");
	}
	//Essences and special condition "Remnant of Corruption", which is also type of essence but does not contain keyword
	else if (item.GetName().find(L"Essence") != std::wstring::npos || item.GetName().find(L"Remnant") != std::wstring::npos) {
		item.SetType(L"Essence");
	}
	else
		item.SetType(L"Currency");
}

//Populate additional fields based on item's name, could be Map, Scarab...
void ItemManager::ParseNormalItems(Item& item) {
	//Set item Name and Type
	if (item.GetName()[item.GetName().length() - 3] == 'M') { //Is item Normal Map? (Superior Example Map)

		//Set type
		item.SetType(L"Map");

		std::wstringstream stream{ item.GetName() };
		std::wstring prefix;
		stream >> prefix;

		//Set new map name without prefix
		if (prefix == L"Superior") {
			std::wstring newName;
			while (stream >> prefix)
				newName += prefix + L' ';
			item.SetBase(Utilities::RemoveTrailingChar(newName)); //Map with removed prefix
		}
	}
	else if (item.GetName().find(L"Scarab") != std::wstring::npos) {
		item.SetType(L"Scarab");
	}
	else if (item.GetName().find(L"Sacrifice") != std::wstring::npos) {
		item.SetType(L"Fragment");
	}
	else if (item.GetName().find(L"Incubator") != std::wstring::npos) {
		item.SetType(L"Incubator");
	}
}

//Clean item name (prefix/suffix), and set it as a base
void ItemManager::ParseMagicItems(Item& item) {
	//Return if passed item data is not a map
	if (!(item.GetName().find(L"Map") != std::wstring::npos))
		return;
	else {
		item.SetType(L"Map");

		//Remove map suffix (Slithering Bone Crypt Map of Endurance -> Slithering Bone Crypt Map)
		std::vector<std::wstring> nameParts; //Used to store map name name without suffix and removing prefix if present
		bool isSpecial = false; //If map has special prefix we dont have too look for other prefixes
		std::wistringstream nameStream{ item.GetName() };//Make stream out of name
		std::wstring word;

		while (nameStream >> word) {
			//"Superior" prefix will show up ONLY if map is unidentified
			if (word == L"Superior" || word == L"Zana's") {
				isSpecial = true;
				continue;
			}
			else if (word == L"Map") { //Break on "Map" keyword to lose "of Endurance" suffix
				nameParts.push_back(word);
				break;
			}
			else
				nameParts.push_back(word); //Push word until "Map" keyword
		}

		if (isSpecial) {
			//Prefix already found, set new name
			item.SetBase(Utilities::VectorToString(nameParts, ' '));
		}
		else {
			//Remove prefix if found
			for (std::wstring part : ItemMods::mapPrefixModifiers)
			{
				if (nameParts[0] == part)
					nameParts.erase(nameParts.begin());
			}
			//Set final map name cleaned of prefix and suffix
			item.SetBase(Utilities::VectorToString(nameParts, ' '));
		}
	}
}

//Clean item name (prefix/suffix), and set it as a base
void ItemManager::ParseRareItems(Item& item) {
	//Unidentified map
	if (item.GetName()[item.GetName().length() - 3] == 'M') {
		std::wstringstream nameStream{ item.GetName() };

		//Set Type
		item.SetType(L"Map");

		//Look for prefix and remove it
		std::wstring word;
		nameStream >> word;
		if (word == L"Superior" || word == L"Zana's") {
			std::wstring newName;
			while (nameStream >> word)
				newName += word + L' ';
			item.SetBase(Utilities::RemoveTrailingChar(newName));
		}
		else //Unidentified map without prefix 
			item.SetBase(item.GetName()); //Name is actually map's base
	}
	else if (item.GetBase()[item.GetBase().length() - 3] == 'M') { //Identified rare map
		//Set Type
		item.SetType(L"Map");
	}
}

//Parse item based on its name and base
void ItemManager::ParseUniqueItems(Item& item) {
	if (item.GetName()[item.GetName().length() - 3] == 'M') { //Unidentified map
		//Set Type
		item.SetType(L"Map");

		std::wstringstream nameStream{ item.GetName() };
		std::wstring word, mapBase;
		nameStream >> word;
		if (word == L"Superior") { //Unindentified might have "Superior" prefix
			while (nameStream >> word)
				mapBase += word + L' ';
			item.SetBase(Utilities::RemoveTrailingChar(mapBase));
		}
		else
			item.SetBase(item.GetName()); //Name is actually map's base

		//Map base is "Underground Sea Map", we need indentified name
		for (auto uniqueMap : Maps::uniqueMaps) {
			if (item.GetBase() == uniqueMap.base) {
				item.SetName(uniqueMap.name); //Map name is "Oba's Cursed Trove"
				break; //Stop looking for map
			}
		}
		//Unique map found
		return;
	}
	else if (item.GetBase()[item.GetBase().length() - 3] == 'M') { //Item base is identified map
		//SPECIAL CASE: "Caer Blaidd, Wolfpack's Den" -> is without comma in downloaded data from api
		if (item.GetName().find(L"Caer") != std::wstring::npos) {
			item.SetName(L"Caer Blaidd Wolfpack's Den");
		}
		//Set Type
		item.SetType(L"Map");
		return;
	}
}