#include "pch.h"
#include "UpdateData.h"
#include "DialogWindow.h"
#include <WinInet.h>
#include "Utilities.h"
#include "Calculator.h"
#include "Settings.h"

//Declaration of CurrencyInfo class static members
float CurrencyInfo::chiselPrice = 0, CurrencyInfo::alchPrice = 0, CurrencyInfo::vaalPrice = 0, CurrencyInfo::csPrice = 0;
bool CurrencyInfo::showMessage = true;
std::vector<CurrencyInfo::RatesStruct>CurrencyInfo::currencyRates = {};
std::vector<CurrencyInfo::RatesStruct>CurrencyInfo::divinationCardRates = {};
std::vector<CurrencyInfo::RatesStruct>CurrencyInfo::mapRates = {};
std::vector<CurrencyInfo::RatesStruct>CurrencyInfo::uniqueMapRates = {};
std::vector<CurrencyInfo::RatesStruct>CurrencyInfo::fragmentRates = {};
std::vector<CurrencyInfo::RatesStruct>CurrencyInfo::scarabRates = {};
std::vector<CurrencyInfo::RatesStruct>CurrencyInfo::essenceRates = {};
std::vector<CurrencyInfo::RatesStruct>CurrencyInfo::fossilRates = {};
std::vector<CurrencyInfo::RatesStruct>CurrencyInfo::resonatorRates = {};
std::vector<CurrencyInfo::RatesStruct>CurrencyInfo::incubatorRates = {};
std::vector<CurrencyInfo::RatesStruct>CurrencyInfo::prophecyRates = {};
std::vector<CurrencyInfo::RatesStruct>CurrencyInfo::oilRates = {};

//Read saved rates from a file and populate working vector
bool CurrencyInfo::ReadRates(const std::wstring& sourceFile, std::vector<CurrencyInfo::RatesStruct>& destinationVector) {
	std::wstring name, line;
	std::wifstream data{ sourceFile };

	if (!data) {
		return false;
	}
	else {
		float num;
		destinationVector.reserve(400);

		while (std::getline(data, line)) {
			std::wstringstream ss(line);
			ss >> num;
			std::getline(ss, name);
			name.erase(name.begin());
			destinationVector.emplace_back(CurrencyInfo::RatesStruct{ name, num });
		}
		destinationVector.shrink_to_fit();
	}
	return true;
}

//Parse raw data with "currencyTypeName" keyword
void ParseCurrencyData(const std::wstring& input, std::vector<CurrencyInfo::RatesStruct>& destination) {
	std::wstring word;
	std::vector<CurrencyInfo::RatesStruct> temp;
	temp.reserve(400); //Reserving space should prevent unnecessary copying when adding data to vector
	std::wifstream dataIn{ input };

	if (!dataIn)
		MessageBox(NULL, L"ParseCurrencyData failed, input not found!", L"Error", NULL);

	//Example - "currencyTypeName Mirror of Kalandra pay"
	while (dataIn >> word) {
		if (word == L"currencyTypeName") {
		currencyFound:
			std::wstring name;

			//Build currency name
			while (dataIn >> word) {
				if (word == L"pay")
					break;
				else {
					name += word + L' ';
				}
			}

			//Look for currency value in Chaos Orbs
			//Value one is Chaos Orb worth in that currency, value two is how much found currency is worth in Chaos Orbs
			//Example - Exalted Orb first value is 0.0082, second value is 118 (Exalted Orb is 118 Chaos Orbs)
			float curValue = 0;
			int countValue = 0;
			while (dataIn >> word) {
				if (word == L"value") { //Two "value" keywords for some currency names
					++countValue; //Count "value" occurrences
					dataIn >> curValue; //Store each "value"
					if (countValue == 2) { //Stop looping after second "value" and store it into vector
						//Save currency name and value to vector
						temp.emplace_back(CurrencyInfo::RatesStruct{ Utilities::RemoveTrailingChar(name), curValue });
						break;
					}
				}
				else if (word == L"currencyTypeName") { //Loop didnt break because second "value" was not found, and next currency name was found
					//Save currency name and value to vector
					temp.emplace_back(CurrencyInfo::RatesStruct{ Utilities::RemoveTrailingChar(name), curValue });
					goto currencyFound;
				}
			}
		}
		else if (word == L"currencyDetails") //After this keyword, file containt lots of unnecessary data
			break;
	}
	temp.shrink_to_fit(); //Shrink excess reserved space
	destination = temp; //Assign parsed data to vector for later use
}

//Raw data with "name" keyword
void ParseData(const std::wstring& input, std::vector<CurrencyInfo::RatesStruct>& destination) {
	std::wifstream dataIn{ input };
	if (!dataIn)
		MessageBox(NULL, L"ParseData failed, input not found!", L"Error", NULL);

	std::wstring word;
	std::vector<CurrencyInfo::RatesStruct> temp;
	temp.reserve(400); //Reserving space should prevent unnecessary copying when adding data to vector

	//Example - "name The Perandus Manor icon"
	while (dataIn >> word) {
		if (word == L"name") {
			std::wstring name;

			//Build currency name
			while (dataIn >> word) {
				if (word == L"icon")
					break;
				else {
					name += word + L' ';
				}
			}

			//Look for currency value in Chaos Orbs
			float curValue = 0;
			while (dataIn >> word) {
				if (word == L"chaosValue") {
					dataIn >> curValue;
					break;
				}
			}

			//Save currency name and value to vector
			temp.emplace_back(CurrencyInfo::RatesStruct{ Utilities::RemoveTrailingChar(name), curValue });
		}
	}
	temp.shrink_to_fit(); //Shrink excess reserved space
	destination = temp; //Assign parsed data to vector for later use
}

//Save rates to file
void SaveRates(std::vector<CurrencyInfo::RatesStruct>& input, const std::wstring& outPath) {
	std::wofstream ost{ outPath };
	for (size_t i = 0; i < input.size(); ++i)
		ost << input[i].value << ' ' << input[i].name << '\n';
}

void CurrencyInfo::SaveAllRates() {
	SaveRates(CurrencyInfo::currencyRates, L"data\\currency_rates");
	SaveRates(CurrencyInfo::divinationCardRates, L"data\\divination_card_rates");
	SaveRates(CurrencyInfo::mapRates, L"data\\map_rates");
	SaveRates(CurrencyInfo::uniqueMapRates, L"data\\unique_map_rates");
	SaveRates(CurrencyInfo::fragmentRates, L"data\\fragment_rates");
	SaveRates(CurrencyInfo::scarabRates, L"data\\scarab_rates");
	SaveRates(CurrencyInfo::essenceRates, L"data\\essence_rates");
	SaveRates(CurrencyInfo::fossilRates, L"data\\fossil_rates");
	SaveRates(CurrencyInfo::resonatorRates, L"data\\resonator_rates");
	SaveRates(CurrencyInfo::incubatorRates, L"data\\incubator_rates");
	SaveRates(CurrencyInfo::prophecyRates, L"data\\prophecy_rates");
	SaveRates(CurrencyInfo::oilRates, L"data\\oil_rates");
}

void CurrencyInfo::OnRatesUpdated() {
	//Make update button clickable
	EnableWindow(DialogWindow::updateButton, true);

	//Change status message
	SetWindowText(DialogWindow::updateStatusWnd, L"Update Finished!");

	//Calculate cartographers sextant price
	csPrice = Calculator::CalculateCSPrice(Settings::GetInstance().GetCS());

	GetMapCurrency();

	//Save rates to a file
	SaveAllRates();
}

//Store rates of vaal, alchemy and chisel for later use
void CurrencyInfo::GetMapCurrency() {
	for (size_t i = 0; i < CurrencyInfo::currencyRates.size(); ++i) {
		if (CurrencyInfo::currencyRates[i].name == L"Vaal Orb")
			CurrencyInfo::vaalPrice = CurrencyInfo::currencyRates[i].value;
		else if (CurrencyInfo::currencyRates[i].name == L"Orb of Alchemy")
			CurrencyInfo::alchPrice = CurrencyInfo::currencyRates[i].value;
		else if (CurrencyInfo::currencyRates[i].name == L"Cartographer's Chisel")
			CurrencyInfo::chiselPrice = CurrencyInfo::currencyRates[i].value;
	}
}

void CurrencyInfo::OnCurrencyRatesLoaded() {
	GetMapCurrency();
	csPrice = Calculator::CalculateCSPrice(Settings::GetInstance().GetCS());
}

//Clear vector data, otherwise, vector will fill up indefinitely when updating rates
void CurrencyInfo::ResetVectorData() {
	CurrencyInfo::currencyRates.clear();
	CurrencyInfo::divinationCardRates.clear();
	CurrencyInfo::mapRates.clear();
	CurrencyInfo::uniqueMapRates.clear();
	CurrencyInfo::fragmentRates.clear();
	CurrencyInfo::scarabRates.clear();
	CurrencyInfo::essenceRates.clear();
	CurrencyInfo::fossilRates.clear();
	CurrencyInfo::resonatorRates.clear();
	CurrencyInfo::incubatorRates.clear();
	CurrencyInfo::prophecyRates.clear();
	CurrencyInfo::oilRates.clear();
}

void CurrencyInfo::UpdateRates() {
	std::wstring tempFile = L"data\\received_html_data";
	std::wstring cleanedHTML = L"data\\cleaned_data";

	ResetVectorData();

	//Get league name
	std::wstring league = Settings::GetInstance().GetLeague();

	//Currency Rates
	Utilities::GetHtmlData((L"https://poe.ninja/api/Data/GetCurrencyOverview?league=" + league), tempFile);
	Utilities::CleanHtmlData(tempFile, cleanedHTML);
	ParseCurrencyData(cleanedHTML, CurrencyInfo::currencyRates);
	CurrencyInfo::currencyRates.emplace_back(RatesStruct{ L"Chaos Orb", 1.0f }); //Manually add chaos orb to the list
	OnCurrencyRatesLoaded();
	SendMessage(DialogWindow::hProgress, PBM_STEPIT, 0, 0);

	//Divination Card Rates
	Utilities::GetHtmlData((L"https://poe.ninja/api/Data/GetDivinationCardsOverview?league=" + league), tempFile);
	Utilities::CleanHtmlData(tempFile, cleanedHTML);
	ParseData(cleanedHTML, CurrencyInfo::divinationCardRates);
	SendMessage(DialogWindow::hProgress, PBM_STEPIT, 0, 0);

	//Map Rates
	Utilities::GetHtmlData((L"https://poe.ninja/api/Data/GetMapOverview?league=" + league), tempFile);
	Utilities::CleanHtmlData(tempFile, cleanedHTML);
	ParseData(cleanedHTML, CurrencyInfo::mapRates);
	SendMessage(DialogWindow::hProgress, PBM_STEPIT, 0, 0);

	//Unique Map Rates
	Utilities::GetHtmlData((L"https://poe.ninja/api/Data/GetUniqueMapOverview?league=" + league), tempFile);
	Utilities::CleanHtmlData(tempFile, cleanedHTML);
	ParseData(cleanedHTML, CurrencyInfo::uniqueMapRates);
	SendMessage(DialogWindow::hProgress, PBM_STEPIT, 0, 0);

	//Fragment Rates
	Utilities::GetHtmlData((L"https://poe.ninja/api/Data/GetFragmentOverview?league=" + league), tempFile);
	Utilities::CleanHtmlData(tempFile, cleanedHTML);
	ParseCurrencyData(cleanedHTML, CurrencyInfo::fragmentRates);
	SendMessage(DialogWindow::hProgress, PBM_STEPIT, 0, 0);

	//Essence Rates
	Utilities::GetHtmlData((L"https://poe.ninja/api/Data/GetEssenceOverview?league=" + league), tempFile);
	Utilities::CleanHtmlData(tempFile, cleanedHTML);
	ParseData(cleanedHTML, CurrencyInfo::essenceRates);
	SendMessage(DialogWindow::hProgress, PBM_STEPIT, 0, 0);

	//Scarab Rates
	Utilities::GetHtmlData((L"https://poe.ninja/api/data/itemoverview?league=" + league + L"&type=Scarab"), tempFile);
	Utilities::CleanHtmlData(tempFile, cleanedHTML);
	ParseData(cleanedHTML, CurrencyInfo::scarabRates);
	SendMessage(DialogWindow::hProgress, PBM_STEPIT, 0, 0);

	//Fossil Rates
	Utilities::GetHtmlData((L"https://poe.ninja/api/data/itemoverview?league=" + league + L"&type=Fossil"), tempFile);
	Utilities::CleanHtmlData(tempFile, cleanedHTML);
	ParseData(cleanedHTML, CurrencyInfo::fossilRates);
	SendMessage(DialogWindow::hProgress, PBM_STEPIT, 0, 0);

	//Resonator Rates
	Utilities::GetHtmlData((L"https://poe.ninja/api/data/itemoverview?league=" + league + L"&type=Resonator"), tempFile);
	Utilities::CleanHtmlData(tempFile, cleanedHTML);
	ParseData(cleanedHTML, CurrencyInfo::resonatorRates);
	SendMessage(DialogWindow::hProgress, PBM_STEPIT, 0, 0);

	//Incubator Rates
	Utilities::GetHtmlData((L"https://poe.ninja/api/data/itemoverview?league=" + league + L"&type=Incubator"), tempFile);
	Utilities::CleanHtmlData(tempFile, cleanedHTML);
	ParseData(cleanedHTML, CurrencyInfo::incubatorRates);
	SendMessage(DialogWindow::hProgress, PBM_STEPIT, 0, 0);

	//Prophecy Rates
	Utilities::GetHtmlData((L"https://poe.ninja/api/data/itemoverview?league=" + league + L"&type=Prophecy"), tempFile);
	Utilities::CleanHtmlData(tempFile, cleanedHTML);
	ParseData(cleanedHTML, CurrencyInfo::prophecyRates);
	SendMessage(DialogWindow::hProgress, PBM_STEPIT, 0, 0);

	//Oil Rates
	Utilities::GetHtmlData((L"https://poe.ninja/api/data/itemoverview?league=" + league + L"&type=Oil"), tempFile);
	Utilities::CleanHtmlData(tempFile, cleanedHTML);
	ParseData(cleanedHTML, CurrencyInfo::oilRates);
	SendMessage(DialogWindow::hProgress, PBM_STEPIT, 0, 0);

	//Remove files (cleanup)
	remove("data\\received_html_data");
	remove("data\\cleaned_data");

	OnRatesUpdated();
}

//Read currency rates from file
void CurrencyInfo::LoadCurencyRates() {

	//Show message if there is no file containing data
	if (!CurrencyInfo::ReadRates(L"data\\currency_rates", CurrencyInfo::currencyRates)) {
		if (showMessage) {
			MessageBox(NULL, L"Error reading currency rates, please update rates!", L"Warning", NULL);
			showMessage = false;
		}
	}

	if (!CurrencyInfo::ReadRates(L"data\\divination_card_rates", CurrencyInfo::divinationCardRates))
		if (showMessage) {
			MessageBox(NULL, L"Error reading divination card rates, please update rates!", L"Warning", NULL);
			showMessage = false;
		}

	if (!CurrencyInfo::ReadRates(L"data\\map_rates", CurrencyInfo::mapRates))
		if (showMessage) {
			MessageBox(NULL, L"Error reading map rates, please update rates!", L"Warning", NULL);
			showMessage = false;
		}

	if (!CurrencyInfo::ReadRates(L"data\\unique_map_rates", CurrencyInfo::uniqueMapRates))
		if (showMessage) {
			MessageBox(NULL, L"Error reading unique map rates, please update rates!", L"Warning", NULL);
			showMessage = false;
		}

	if (!CurrencyInfo::ReadRates(L"data\\fragment_rates", CurrencyInfo::fragmentRates))
		if (showMessage) {
			MessageBox(NULL, L"Error reading fragment rates, please update rates!", L"Warning", NULL);
			showMessage = false;
		}

	if (!CurrencyInfo::ReadRates(L"data\\essence_rates", CurrencyInfo::essenceRates))
		if (showMessage) {
			MessageBox(NULL, L"Error reading essence rates, please update rates!", L"Warning", NULL);
			showMessage = false;
		}

	if (!CurrencyInfo::ReadRates(L"data\\fossil_rates", CurrencyInfo::fossilRates))
		if (showMessage) {
			MessageBox(NULL, L"Error reading fossil rates, please update rates!", L"Warning", NULL);
			showMessage = false;
		}

	if (!CurrencyInfo::ReadRates(L"data\\resonator_rates", CurrencyInfo::resonatorRates))
		if (showMessage) {
			MessageBox(NULL, L"Error reading resonator rates, please update rates!", L"Warning", NULL);
			showMessage = false;
		}

	if (!CurrencyInfo::ReadRates(L"data\\incubator_rates", CurrencyInfo::incubatorRates))
		if (showMessage) {
			MessageBox(NULL, L"Error reading incubator rates, please update rates!", L"Warning", NULL);
			showMessage = false;
		}

	if (!CurrencyInfo::ReadRates(L"data\\prophecy_rates", CurrencyInfo::prophecyRates))
		if (showMessage) {
			MessageBox(NULL, L"Error reading prophecy rates, please update rates!", L"Warning", NULL);
			showMessage = false;
		}

	if (!CurrencyInfo::ReadRates(L"data\\oil_rates", CurrencyInfo::oilRates))
		if (showMessage) {
			MessageBox(NULL, L"Error reading oil rates, please update rates!", L"Warning", NULL);
			showMessage = false;
		}

	if (!CurrencyInfo::ReadRates(L"data\\scarab_rates", CurrencyInfo::scarabRates)) {
		if (showMessage) MessageBox(NULL, L"Error reading scarab rates, please update rates!", L"Warning", NULL);
	}

	OnCurrencyRatesLoaded();
}

//Get item value from list of rates, based on item name or base
float CurrencyInfo::GetRate(const std::vector<CurrencyInfo::RatesStruct>& rates, const Item& item) {
	for (size_t i = 0; i < rates.size(); i++) {
		if ((item.GetName() == rates[i].name) || (item.GetBase() == rates[i].name)) {
			return rates[i].value;
		}
	}
	return 0.0f;
}