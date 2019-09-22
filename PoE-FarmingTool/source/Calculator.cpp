#include "pch.h"
#include "Calculator.h"
#include "UpdateData.h"
#include "Settings.h"
#include "Session.h"
#include "Utilities.h"

//Calculate the profit of an item based on its rarity or type
const float Calculator::CalculateItemValue(const Item& item){
	if (item.GetType() == L"Currency") {
		return CurrencyInfo::GetRate(CurrencyInfo::currencyRates, item);
	}
	else if (item.GetType() == L"Fossil") {
		return CurrencyInfo::GetRate(CurrencyInfo::fossilRates, item);
	}
	else if (item.GetType() == L"Resonator") {
		return CurrencyInfo::GetRate(CurrencyInfo::resonatorRates, item);
	}
	else if (item.GetType() == L"Essence") {
		return CurrencyInfo::GetRate(CurrencyInfo::essenceRates, item);
	}
	else if (item.GetRarity() == L"Divination Card") {
		return CurrencyInfo::GetRate(CurrencyInfo::divinationCardRates, item);
	}
	else if (item.GetRarity() == L"Unique" && item.GetType() == L"Map") { //Parse unique maps
		return CurrencyInfo::GetRate(CurrencyInfo::uniqueMapRates, item);
	}
	else if (item.GetType() == L"Map" && item.GetRarity() != L"Unique") { //Parse non-unique maps
		return CurrencyInfo::GetRate(CurrencyInfo::mapRates, item);
	}
	else if (item.GetType() == L"Scarab") {
		return CurrencyInfo::GetRate(CurrencyInfo::scarabRates, item);
	}
	else if (item.GetType() == L"Incubator") {
		return CurrencyInfo::GetRate(CurrencyInfo::incubatorRates, item);
	}
	else if (item.GetType() == L"Oil") {
		return CurrencyInfo::GetRate(CurrencyInfo::oilRates, item);
	}
	else if (item.GetRarity() == L"Normal") {
		float itemVal = 0;
		//If item can't be classified by type, search through other rate lists which are rarity "Normal"
		itemVal = CurrencyInfo::GetRate(CurrencyInfo::fragmentRates, item);
		if (itemVal == 0)
			itemVal = CurrencyInfo::GetRate(CurrencyInfo::prophecyRates, item);
		return itemVal;
	}
	return 0;
}

//Calculate the average price of the cartographer sextants (from user setting 0-0-0)
const float Calculator::CalculateCSPrice(const std::wstring& cs){
	//Extract values from string
	float appPrice = 0, jouPrice = 0, masPrice = 0;
	for (size_t i = 0; i < CurrencyInfo::currencyRates.size(); ++i) {
		if (L"Apprentice Cartographer's Sextant" == CurrencyInfo::currencyRates[i].name)
			appPrice = CurrencyInfo::currencyRates[i].value;
		else if (L"Journeyman Cartographer's Sextant" == CurrencyInfo::currencyRates[i].name)
			jouPrice = CurrencyInfo::currencyRates[i].value;
		else if (L"Master Cartographer's Sextant" == CurrencyInfo::currencyRates[i].name)
			masPrice = CurrencyInfo::currencyRates[i].value;
	}

	//String example of cartographer sextant setting "0-0-0" (convert char '9' to int 9 with ('9' - '0') = (57 - 48) = 9)
	int appCount = cs[0] - '0', jouCount = cs[2] - '0', masCount = cs[4] - '0';
	int totalCount = appCount + jouCount + masCount;
	if (totalCount == 0)
		return 0;

	//Average price used for increasing spent value when parser detects used sextant
	return ((appPrice * appCount + jouPrice * jouCount + masPrice * masCount) / totalCount);
}

//Return chisels used, based on quality amount
const int Calculator::GetChiselCount(const int& quality) {
	//Assume player would use a chisel on a map with 17 quality, but not if it has 18 or 19 quality
	if (quality > 16)
		return (int)floor(quality/5);
	else
		return 0;
}

const float Calculator::CalculateSpentOnMap(const Item& item) {
	float spent = 0;
	//Map worth
	if (Settings::GetInstance().GetIsGeneral() || (Settings::GetInstance().GetMapPrice() == 0)) {
		spent += item.GetValue();
	}
	else
		spent += Settings::GetInstance().GetMapPrice(); //Price set by user
	//Add zana mod price
	spent += Settings::GetInstance().GetModPrice();
	//Alch spent for rare map
	if(item.GetRarity() == L"Rare")
		spent += CurrencyInfo::alchPrice;
	//Spent chisels for map quality
	spent += (GetChiselCount(item.GetQuality()) * CurrencyInfo::chiselPrice);
	//Spent vaal orb for corruption
	if (item.GetCorrupted())
		spent += CurrencyInfo::vaalPrice;

	float temp = spent;

	//Add fragment if used
	if (MapManager::GetInstance().GetIsFragmentAdded())
		for (size_t i = 0; i < MapManager::GetInstance().GetFragments().size(); ++i)
			spent += MapManager::GetInstance().GetFragments()[i].GetValue();
	//Add scarab if used
	if (MapManager::GetInstance().GetIsScarabAdded())
		for(size_t i = 0; i < MapManager::GetInstance().GetScarabs().size(); ++i)
			spent += MapManager::GetInstance().GetScarabs()[i].GetValue();
	//Add vessel if used
	if (MapManager::GetInstance().GetIsVesselAdded()) {
		spent += MapManager::GetInstance().GetVessel().GetValue();
	}

	//Save additions prices total value
	MapManager::GetInstance().SetAdditionsPrice(spent - temp);

	return spent;
}