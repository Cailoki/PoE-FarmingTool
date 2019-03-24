#include "pch.h"
#include "UpdateData.h"
#include "DialogWindow.h"
#include <WinInet.h>

float CurrencyInfo::chiselPrice = 0, CurrencyInfo::alchPrice = 0, CurrencyInfo::vaalPrice = 0;
bool CurrencyInfo::validHTML = true;
bool CurrencyInfo::showMessage = true;
bool CurrencyInfo::currencyRatesLoaded = false;
bool CurrencyInfo::currencyRatesUpdated = false;
std::vector<CurrencyInfo::RatesStruct>CurrencyInfo::currencyRates = {}; 
std::vector<CurrencyInfo::RatesStruct>CurrencyInfo::divinationCardRates = {};
std::vector<CurrencyInfo::RatesStruct>CurrencyInfo::mapRates = {};
std::vector<CurrencyInfo::RatesStruct>CurrencyInfo::uniqueMapRates = {};
std::vector<CurrencyInfo::RatesStruct>CurrencyInfo::fragmentRates = {};
std::vector<CurrencyInfo::LinksStruct>CurrencyInfo::links = //filename, link, estimated reseved space for vector
{ 
	CurrencyInfo::LinksStruct{"data\\currency_rates", "https://poe.ninja/api/Data/GetCurrencyOverview?league=", 160},
	CurrencyInfo::LinksStruct{"data\\divination_card_rates", "https://poe.ninja/api/Data/GetDivinationCardsOverview?league=", 250},
	CurrencyInfo::LinksStruct{"data\\map_rates", "https://poe.ninja/api/Data/GetMapOverview?league=", 350}, 
	CurrencyInfo::LinksStruct{"data\\unique_map_rates", "https://poe.ninja/api/Data/GetUniqueMapOverview?league=", 25},
	CurrencyInfo::LinksStruct{"data\\fragment_rates", "https://poe.ninja/api/Data/GetFragmentOverview?league=", 80},
	CurrencyInfo::LinksStruct{"data\\essence_rates", "https://poe.ninja/api/Data/GetEssenceOverview?league=", 120},
	CurrencyInfo::LinksStruct{"data\\scarab_rates", "scarab", 40}
};

//used for reading saved rates from a file
bool CurrencyInfo::ReadRates(std::vector<CurrencyInfo::RatesStruct>& rates, const int& x) {
	std::string name, line;
	std::ifstream data{ CurrencyInfo::links[x].fileName };

	if (!data) {
		return false;
	}
	else {
		float num;
		rates.reserve(CurrencyInfo::links[x].reserveSize);
		while (std::getline(data, line)) {
			std::stringstream ss(line);
			ss >> num;
			std::getline(ss, name);
			name.erase(name.begin());
			rates.push_back(CurrencyInfo::RatesStruct{ name, num });
			if (x == 0) { //Currency data
				if (name == "Vaal Orb")
					CurrencyInfo::vaalPrice = num;
				else if (name == "Orb of Alchemy")
					CurrencyInfo::alchPrice = num;
				else if (name == "Cartographer's Chisel")
					CurrencyInfo::chiselPrice = num;
			}
		}
	}
	return true;
}

void GetHtmlData(const std::string& league, const std::string& link) {
	LPCWSTR userAgent = L"Mozilla / 5.0 (Windows NT 10.0; Win64; x64; rv:63.0) Gecko / 20100101 Firefox / 63.0";
	LPCWSTR header = L"Accept-Encoding: gzip, deflate";

	HINTERNET connect = InternetOpen(userAgent, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, NULL);
	if (!connect) {
		MessageBox(NULL, L"Connection failed or syntax error!", L"Error", MB_OK);
	}

	BOOL isDecoding = true;
	BOOL decoding = InternetSetOption(connect, INTERNET_OPTION_HTTP_DECODING, &isDecoding, sizeof(isDecoding));
	if (!decoding) {
		MessageBox(NULL, L"Connection not decoded!", L"Error", MB_OK);
	}

	std::string leagueURL ="";
	if (link == "scarab") {
		leagueURL = "https://poe.ninja/api/data/itemoverview?league=" + league + "&type=Scarab";
	}
	else
		leagueURL = link + league;
	std::wstring tempURL = std::wstring(leagueURL.begin(), leagueURL.end());
	LPCWSTR finalURL = tempURL.c_str();

	HINTERNET url = InternetOpenUrl(connect, finalURL, header, -1L, NULL, NULL);

	DWORD blocksize = 1024;
	DWORD dwTemp;
	HANDLE hFile = CreateFile(L"data\\received_html_data", GENERIC_READ | GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	DWORD dwBytesRead = 1;

	for (; dwBytesRead > 0;)
	{
		InternetReadFile(url, &blocksize, (DWORD)sizeof(blocksize), &dwBytesRead);
		WriteFile(hFile, &blocksize, dwBytesRead, &dwTemp, NULL);
	}

	CloseHandle(hFile);
	InternetCloseHandle(url);
}

void CleanHtmlData() {
	std::ifstream data{ "data\\received_html_data" };
	std::ofstream cleanDataOut{ "data\\cleaned_data" };

	while (!data.eof()) {
		char ch = data.get();
		if (ch == '{' || ch == '}' || ch == ':' || ch == '[' || ch == ']' || ch == '"' || ch == ',' || ch == ' ')
			cleanDataOut << ' ';
		else
			cleanDataOut << ch;
	}
	data.close();
	cleanDataOut.close();
}

//raw data with "currencyTypeName" keyword
void ParseCurrencyData(const int& x) {
	//working vector
	std::vector<CurrencyInfo::RatesStruct>worker;
	worker.reserve(CurrencyInfo::links[x].reserveSize);

	CleanHtmlData();

	std::vector<std::string>trash = { "Armourer's Scrap", "Scroll of Wisdom", "Orb of Transmutation", "Blacksmith's Whetstone", "Portal Scroll", "Orb of Augmentation", "Scroll of Wisdom"};

	std::ifstream cleanDataIn{ "data\\cleaned_data" };
	std::string keyword, name;
	int countValue = 0, countVector = 0;
	bool curFound = false;
	float curValue = 0;
	while (cleanDataIn >> keyword) {
		if (keyword == "currencyTypeName") {
			if (countValue == 1) {
				worker[countVector].value = curValue;
				countValue = 0;
				++countVector;
			} 
			curFound = true;
			int maxLoop = 0;
			while (maxLoop < 5) {
				cleanDataIn >> keyword;
				if (keyword == "pay")
					break;
				else {
					name += keyword + ' ';
					++maxLoop;
				}
			}
			name.erase(name.size() - 1);
			worker.emplace_back(CurrencyInfo::RatesStruct{ name, 0 });
			name = "";
			continue;
		}
		if (keyword == "value" && curFound) {
			++countValue;
			cleanDataIn >> curValue;
			if (countValue == 2) {
				if (countVector >= static_cast<int>(worker.size())) {
					MessageBox(NULL, L"Vector out of range (currency update)!", L"Error", NULL);
					return;
				}
				//filter out trash currency
				bool trashFound = false;
				for (int i = 0; i < trash.size(); ++i) {
					if (trash[i] == worker[countVector].name) {
						worker[countVector].value = 0;
						trashFound = true;
						break;
					}
				}
				if (!trashFound) {
					worker[countVector].value = curValue;
				}
				++countVector;
				countValue = 0;
				curFound = false;
			}
		}

	}
	cleanDataIn.close();

	if (worker.size() == 0) {
		MessageBox(NULL, L"Error parsing HTML file!\nCheck if league setting is valid.", L"Error", NULL);
		CurrencyInfo::validHTML = false;
		return;
	}
	else
		CurrencyInfo::validHTML = true;
	
	//add Chaos Orb, value of 1 to list
	if (x == 0)
		worker.emplace_back(CurrencyInfo::RatesStruct{std::string("Chaos Orb"), float(1.0)});
	
	//save currency rates to file, used when starting up the script / save sextant prices for later use
	std::ofstream ostRates{ CurrencyInfo::links[x].fileName };
	if (x == 0) { //currency rates
		for (int i = 0; i < static_cast<int>(worker.size()); ++i) {
			ostRates << worker[i].value << ' ' << worker[i].name << '\n';
			if (worker[i].name == "Vaal Orb")
				CurrencyInfo::vaalPrice = worker[i].value;
			else if (worker[i].name == "Orb of Alchemy")
				CurrencyInfo::alchPrice = worker[i].value;
			else if (worker[i].name == "Cartographer's Chisel")
				CurrencyInfo::chiselPrice = worker[i].value;
		}
		CurrencyInfo::currencyRates = worker;
	}
	else {
		for (int i = 0; i < static_cast<int>(worker.size()); ++i)
			ostRates << worker[i].value << ' ' << worker[i].name << '\n';
		CurrencyInfo::fragmentRates = worker;
	}
}

//raw data with "name" keyword
void ParseData(const int& x) {
	//working vector
	std::vector<CurrencyInfo::RatesStruct>tempRates;
	tempRates.reserve(CurrencyInfo::links[x].reserveSize);

	CleanHtmlData();

	//parsing cleaned data for names and prices
	std::ifstream cleanDataIn{ "data\\cleaned_data" };
	std::string keyword, name;
	int countVector = 0;
	bool nameFound = false, valueFound = false;
	//example - "name some item name icon"
	while (cleanDataIn >> keyword) {
		if (keyword == "name") {
			if (nameFound)
				continue;
			else
				nameFound = true;
			int maxLoop = 0;
			while (maxLoop < 10) { //name shouldn't be more then 10 words long, break loop if "icon" keyword not found
				cleanDataIn >> keyword;
				if (keyword == "icon")
					break;
				else {
					name += keyword + ' ';
					++maxLoop;
				}
			}
			name.erase(name.size() - 1); //remove trailing space
			tempRates.emplace_back(CurrencyInfo::RatesStruct{ name, 0 });
			++countVector;
			name = "";
			valueFound = false;
			continue;
		}
		if (keyword == "chaosValue") {
			if (valueFound) //in case of 2 consecutive "chaosValue" before reaching "name"
				continue;
			else
				valueFound = true;
			nameFound = false; //value found (reset) - look for next "name"
			float curValue;
			if (!(cleanDataIn >> curValue)) {
				tempRates.clear();
				MessageBox(NULL, L"Value not found, please re-run updater!", L"Error", NULL);
				break;
			}
			else
				tempRates[countVector - 1].value = curValue;
		}
	}
	cleanDataIn.close();

	if (x == 1)
		CurrencyInfo::divinationCardRates = tempRates;
	else if (x == 2)
		CurrencyInfo::mapRates = tempRates;
	else if (x == 3) {
		for (int i = 0; i < tempRates.size(); ++i)
			if (tempRates[i].name[0] == 'C')
				tempRates[i].name = "Caer Blaidd, Wolfpack's Den";
			else if (tempRates[i].name[0] == 'M' && tempRates[i].name != "Mao Kun")
				tempRates[i].name = "Maelström of Chaos";
		CurrencyInfo::uniqueMapRates = tempRates;
	}
	else if (x == 5) //add essences to currency list
		CurrencyInfo::currencyRates.insert(CurrencyInfo::currencyRates.end(), tempRates.begin(), tempRates.end());
	else if (x == 6) //add scarab rates to fragments list
		CurrencyInfo::fragmentRates.insert(CurrencyInfo::fragmentRates.end(), tempRates.begin(), tempRates.end());

	//save rates to a file, used when starting up script
	if (x == 5) {
		std::ofstream ostRates2{ CurrencyInfo::links[0].fileName, std::ios_base::app }; //add essence rates to document with currency (both are rarity "currency")
		for (int i = 0; i < static_cast<int>(tempRates.size()); ++i)
			ostRates2 << tempRates[i].value << ' ' << tempRates[i].name << '\n';
	}
	else if (x == 6) {
		std::ofstream ostRates3{ CurrencyInfo::links[4].fileName, std::ios_base::app }; //add scarab rates to document with fragment (both are rarity "normal")
		for (int i = 0; i < static_cast<int>(tempRates.size()); ++i)
			ostRates3 << tempRates[i].value << ' ' << tempRates[i].name << '\n';
	}
	else {
		std::ofstream ostRates{ CurrencyInfo::links[x].fileName };
		for (int i = 0; i < static_cast<int>(tempRates.size()); ++i)
			ostRates << tempRates[i].value << ' ' << tempRates[i].name << '\n';
	}
}

void CurrencyInfo::UpdateCurrencyRates(const std::string& league) {
	for (int i = 0; i < CurrencyInfo::numberOfRateGroups; ++i) {
		GetHtmlData(league, CurrencyInfo::links[i].link);
		if (i == 0 || i == 4) //does not indicate rarity "Currency", only structure of the received HTML data
			ParseCurrencyData(i);
		else
			ParseData(i);
		if (!CurrencyInfo::validHTML)
			break;
		SendMessage(DialogWindow::hProgress, PBM_STEPIT, 0, 0);
	}
	remove("data\\received_html_data");
	remove("data\\cleaned_data");
	if (CurrencyInfo::validHTML) {
		SetWindowText(DialogWindow::updateStatusWnd, L"Update Finished!");
		currencyRatesUpdated = true;
	}
	else
		SetWindowText(DialogWindow::updateStatusWnd, L"Update Failed!");
	EnableWindow(DialogWindow::updateButton, true);
}

//Read currency ratesfrom the file
void CurrencyInfo::InitCurencyRates() {
	if (!CurrencyInfo::ReadRates(CurrencyInfo::currencyRates, 0)) { //Show message if there is no file containing data
		if (showMessage) {
			MessageBox(NULL, L"Error reading currency rates, please update rates!", L"Warning", NULL);
			showMessage = false;
		}
		CurrencyInfo::currencyRatesLoaded = false;
	}
	else
		CurrencyInfo::currencyRatesLoaded = true;
	if (!CurrencyInfo::ReadRates(CurrencyInfo::divinationCardRates, 1))
		if (showMessage) {
			MessageBox(NULL, L"Error reading divination card rates, please update rates!", L"Warning", NULL);
			showMessage = false;
		}
	if (!CurrencyInfo::ReadRates(CurrencyInfo::mapRates, 2))
		if (showMessage) {
			MessageBox(NULL, L"Error reading map rates, please update rates!", L"Warning", NULL);
			showMessage = false;
		}
	if (!CurrencyInfo::ReadRates(CurrencyInfo::uniqueMapRates, 3))
		if (showMessage) {
			MessageBox(NULL, L"Error reading unique map rates, please update rates!", L"Warning", NULL);
			showMessage = false;
		}
	if (!CurrencyInfo::ReadRates(CurrencyInfo::fragmentRates, 4)) {
		if (showMessage) MessageBox(NULL, L"Error reading fragment rates, please update rates!", L"Warning", NULL);
	}
}

void Version::GetRepoData() {
	LPCWSTR userAgent = L"Mozilla / 5.0 (Windows NT 10.0; Win64; x64; rv:63.0) Gecko / 20100101 Firefox / 63.0";
	LPCWSTR header = L"Accept-Encoding: gzip, deflate";

	HINTERNET connect = InternetOpen(userAgent, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, NULL);
	if (!connect) {
		MessageBox(NULL, L"Connection failed or syntax error!", L"Error", MB_OK);
	}

	BOOL isDecoding = true;
	BOOL decoding = InternetSetOption(connect, INTERNET_OPTION_HTTP_DECODING, &isDecoding, sizeof(isDecoding));
	if (!decoding) {
		MessageBox(NULL, L"Connection not decoded!", L"Error", MB_OK);
	}

	HINTERNET url = InternetOpenUrl(connect, L"https://api.github.com/repos/Cailoki/PoE-FarmingTool/releases", header, -1L, NULL, NULL);

	DWORD blocksize = 1024;
	DWORD dwTemp;
	HANDLE hFile = CreateFile(L"data\\github_raw", GENERIC_READ | GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	DWORD dwBytesRead = 1;

	for (; dwBytesRead > 0;)
	{
		InternetReadFile(url, &blocksize, (DWORD)sizeof(blocksize), &dwBytesRead);
		WriteFile(hFile, &blocksize, dwBytesRead, &dwTemp, NULL);
	}

	CloseHandle(hFile);
	InternetCloseHandle(url);
}