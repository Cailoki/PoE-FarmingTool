#pragma once

//By declaring a function member as static, you make it independent of any particular object of the class.
//A static member function can be called even if no objects of the class exist and the static functions 
//are accessed using only the class name and the scope resolution operator ::
class CurrencyInfo {
public:
	struct RatesStruct { std::string name; float value; };
	struct LinksStruct { std::string fileName, link; int reserveSize; };
	static std::vector<RatesStruct>currencyRates; //0
	static std::vector<RatesStruct>divinationCardRates; //1
	static std::vector<RatesStruct>mapRates; //2
	static std::vector<RatesStruct>uniqueMapRates; //3
	static std::vector<RatesStruct>fragmentRates; //4
	static std::vector<LinksStruct>links;
	static void UpdateCurrencyRates(const std::string& league);
	static void InitCurencyRates();
	static bool ReadRates(std::vector<CurrencyInfo::RatesStruct>& rates, const int& x);
	static float chiselPrice, alchPrice, vaalPrice;
	static bool validHTML, showMessage, currencyRatesLoaded, currencyRatesUpdated;
	static const int numberOfRateGroups = 7; //used for few for loops and progress bar steps
};

class Version {
public:
	void GetRepoData();
};