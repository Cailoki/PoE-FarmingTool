#pragma once
#include "Models.h"

//Current session data
class SessionData {
public:
	//Singleton instance
	static SessionData& GetInstance();
	//Getters
	const int GetRuns() const { return runs; }
	const float GetSpent() const { return spent; }
	const float GetProfit() const { return profit; }
	const float GetLastSpent() const { return lastSpent; }
	const float GetLastProfit() const { return lastProfit; }
	const std::wstring GetSessionFilename() const { return sessionFileName; }
	const bool GetIsFirstMapOpened() const { return isFirstMapOpened; }
	//Setters
	void SetIsFirstMapOpened(const bool& status) { isFirstMapOpened = status; };
	//Other
	void SubtractSpentCS();
	void SubtractSpentOnMap();
	void OnDataChanged();
	void ResetData();
	void AddSpentManual(const std::pair<std::wstring, bool>& data);
	void AddProfitManual(const float& value);
	void LoadSessionData();
	void SaveSessionData();
	void CleanLastProfit();
	void IncrementRuns() { ++runs; };
	void ResetLastRunValues();
	void AddProfit(const Item& item);
	const std::wstring MapReport();
private:
	//Private constructor
	SessionData() {  };
	//Other fields
	int runs = 0;
	float spent = 0, profit = 0, lastSpent = 0, lastProfit = 0;
	bool isFirstMapOpened = false;
	std::wstring sessionFileName = L"data\\saved_data";
	//Message shown in history window
	std::wstring message = L"No Message";
};

class Location {
public:
	Location() {};
	Location(const std::wstring& ip, const std::wstring& loc)
		:ip{ ip }, locName{ loc } {}
	//Getters
	const std::wstring& GetLocationName() const { return locName; }
	const std::wstring& GetIp() const { return ip; }
	const std::wstring& GetType() const { return type; }
	//Setters
	void SetType(const std::wstring& typ) { type = typ; }
private:
	std::wstring ip = L"0.0.0.0", locName = L"Unknown", type = L"Unknown";
};

class LocationManager {
public:
	static LocationManager& GetInstance();
	void ClassifyLocation(const Location& location);
	const Location GetCurrentLocation() const { return currentLocation; }
private:
	void OnMapOpened();
	void AddLocation(const Location& location);
	std::vector<Location> locations = {};
	Location currentLocation, mainMap, zanaMap;
	bool inMap = false, inZanaMap = false;
};

class ItemManager {
public:
	static ItemManager& GetInstance();
	void ClassifyItem(Item item);
	const Item& GetLastParsedItem() const { return lastParsedItem; }
private:
	void OnItemClassified();
	void ParseCurrency(Item& item);
	void ParseNormalItems(Item& item);
	void ParseMagicItems(Item& item);
	void ParseRareItems(Item& item);
	void ParseUniqueItems(Item& item);
	Item lastParsedItem;
};

class MapManager {
public:
	static MapManager& GetInstance();
	void ClassifyInput(const Item& item);
	const std::wstring GetLastParsedMapName() const { return lastParsedMapName; }
	void ResetManager();
	bool CheckDuplicate(const Item& item, const std::vector<Item>& data);
	//Getters
	const bool GetIsFragmentAdded() const { return isFragmentAdded; }
	const bool GetIsScarabAdded() const { return isScarabAdded; }
	const Item GetMap() const { return map; }
	const Item GetVessel() const { return vessel; }
	const int GetHistoryEntries() { return historyEntries; }
	const bool GetIsVesselAdded() { return isVesselAdded; }
	const float GetAdditionsPrice() { return additionsPrice; }
	const std::vector<Item> GetScarabs() { return scarabs; }
	const std::vector<Item> GetFragments() { return fragments; }
	//Setters
	void SetAdditionsPrice(const float& val) { additionsPrice = val; }
private:
	std::vector<Item> scarabs = {}, fragments = {};
	std::wstring lastParsedMapName = L"";
	Item map, vessel;
	float additionsPrice = 0;
	int historyEntries = 0;
	bool isMap = false, isFragmentAdded = false, isScarabAdded = false, isVesselAdded = false;
};