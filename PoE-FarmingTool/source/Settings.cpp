#include "pch.h"
#include "Settings.h"
#include "Utilities.h"
#include "Window.h"

Settings& Settings::GetInstance() {
	static Settings settings;
	return settings;
}

//Get data from dialog window
void Settings::SaveNewLogLocation(const std::wstring& data) {
	std::wstring usableLoc; //Location must have this format c://Program Files (x86)//Steam//
	//Swap backslash with double forwardslash
	for (size_t i = 0; i < data.size(); ++i) {
		//Escape sequences are used to represent certain special characters within string literals and character literals. Backslash needs to be escaped
		if (data[i] == '\\')
			usableLoc += L"//";
		else
			usableLoc += data[i];
	}

	//Set new location
	clientLogLocation = (usableLoc + L"//Client.txt");

	OnSettingsChanged();
}

bool Settings::SetCS(const std::wstring& newCS){
	//Check if user entered correct format (0-0-0)
	if (newCS.size() != 5 || !isdigit(newCS[0]) || !isdigit(newCS[2]) || !isdigit(newCS[4]) || isdigit(newCS[1]) || isdigit(newCS[3])) {
		MessageBox(NULL, L"Wrong format of set cartographer's sextants, please save them in the correct format (0-0-0)!", L"Warning", NULL);
		return false;
	}
	cs = newCS;

	OnSettingsChanged();

	return true;
}

void Settings::SetLeague(const std::wstring& newLeague) {
	selectedLeague = newLeague;

	OnSettingsChanged();
}

void Settings::SetIIQ(const int& quantity){
	iiq = quantity;

	OnSettingsChanged();
}

void Settings::SetOverlayPosition(const std::pair<float, float>& xy){
	xPosOverlay = xy.first;
	yPosOverlay = xy.second;

	OnSettingsChanged();
}

void Settings::OnSettingsChanged() {
	MainWindow::RedrawSettings();
	SaveSettings();
}

void Settings::SaveMapFarmingSettings(const MapSettings& mapSettings) {
	mapName = mapSettings.mapName;
	mapPrice = mapSettings.mapPrice;
	zanaModPrice = mapSettings.zanaModPrice;
	isGeneral = mapSettings.isGeneral;
	isTrash = mapSettings.isTrash;
	isParseAll = mapSettings.isParseAll;

	OnSettingsChanged();
}

void Settings::SaveSettings(){
	std::wofstream ost{ settingsFileName };
	ost << clientLogLocation << '\n'
		<< mapName << '\n'
		<< selectedLeague << '\n'
		<< cs << '\n'
		<< iiq << '\n'
		<< mapPrice << '\n'
		<< zanaModPrice << '\n'
		<< isGeneral << '\n'
		<< isTrash << '\n'
		<< isParseAll << '\n'
		<< xPosOverlay << '\n'
		<< yPosOverlay;
}

void Settings::LoadSettings() {
	std::wifstream sSettingsStream{ settingsFileName };
	if (sSettingsStream) {
		std::getline(sSettingsStream, clientLogLocation);
		std::getline(sSettingsStream, mapName);
		std::getline(sSettingsStream, selectedLeague);
		sSettingsStream >> cs
			>> iiq
			>> mapPrice
			>> zanaModPrice
			>> isGeneral
			>> isTrash
			>> isParseAll
			>> xPosOverlay
			>> yPosOverlay;
	}
	else
		SaveSettings(); //Save defaults
}