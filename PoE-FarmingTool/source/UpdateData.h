#pragma once
#include "Models.h"

class CurrencyInfo {
private:
	static void GetMapCurrency();
public:
	struct RatesStruct { std::wstring name; float value; };

	//Collection of rates separated by type and rarity
	static std::vector<RatesStruct>currencyRates;
	static std::vector<RatesStruct>divinationCardRates;
	static std::vector<RatesStruct>mapRates;
	static std::vector<RatesStruct>uniqueMapRates;
	static std::vector<RatesStruct>fragmentRates;
	static std::vector<RatesStruct>scarabRates;
	static std::vector<RatesStruct>essenceRates;
	static std::vector<RatesStruct>fossilRates;
	static std::vector<RatesStruct>resonatorRates;
	static std::vector<RatesStruct>incubatorRates;
	static std::vector<RatesStruct>prophecyRates;
	static std::vector<RatesStruct>oilRates;

	static bool ReadRates(const std::wstring& source, std::vector<CurrencyInfo::RatesStruct>& destination);
	static void UpdateRates();
	static void LoadCurencyRates();
	static void SaveAllRates();
	static float GetRate(const std::vector<RatesStruct>& rates, const Item& item);
	static void OnRatesUpdated();
	static void OnCurrencyRatesLoaded();
	static void ResetVectorData();
	static void SetCsValue(const float& val) { csPrice = val; }

	//Extracted prices of currency and average price of sextant used
	static float chiselPrice, alchPrice, vaalPrice, csPrice;

	//Used to show only one message box if something fails during loading currency rates
	static bool showMessage;
};
