#pragma once

//Used to return map settings from the dialog window
struct MapSettings {
	std::wstring mapName = L"";
	float mapPrice = 0.0f;
	int zanaModPrice = 0;
	bool isGeneral = true;
	bool isTrash = false;
};

//Global app settings
class Settings {
public:
	//Singleton instance
	static Settings& GetInstance();
	//Getters
	const std::wstring GetClientLogLocation() const { return clientLogLocation; }
	const std::wstring GetCS() const { return cs; }
	const bool GetIsGeneral() const { return isGeneral; }
	const bool GetIsTrash() const { return isTrash; }
	const std::wstring GetLeague() const { return league; }
	const int GetIIQ() const { return iiq; }
	const float GetMapPrice() const { return mapPrice; }
	const std::wstring GetMapName() const { return mapName; }
	const int GetModPrice() const { return zanaModPrice; }
	const std::wstring GetLeagueHC() const { return leagueHC; }
	const float GetOverlayXPos() const { return xPosOverlay; }
	const float GetOverlayYPos() const { return yPosOverlay; }
	const int GetProgressBarSteps() const { return progressBarSteps; }
	const std::wstring GetSettingsFilename() const { return settingsFileName; }
	//Setters
	bool SetCS(const std::wstring& cs);
	void SetLeague(const std::wstring& league);
	void SetIIQ(const int& quantity);
	void SetOverlayPosition(const std::pair<float, float>& xy);
	//Other
	void SaveNewLogLocation(const std::wstring& data);
	void OnSettingsChanged();
	void SaveMapFarmingSettings(const MapSettings& mapSettings);
	void SaveSettings();
	void LoadSettings();
private:
	//Private constructor
	Settings() {};
	//Other settings
	std::wstring clientLogLocation = L"c://Program Files (x86)//Steam//steamapps//common//Path of Exile//logs//Client.txt";
	std::wstring cs = L"0-0-0";
	int iiq = 0, progressBarSteps = 12;
	//League group -> change on every new league update (MAKE SURE THERE ARE NO TYPOS!)
	std::wstring league = L"Blight", leagueHC = L"HC Blight", leagueHardcore = L"Hardcore Blight";
	//Map settings group
	std::wstring mapName = L"General";
	float mapPrice = 0.0f;
	int zanaModPrice = 0;
	bool isGeneral = true, isTrash = false;
	//Overlay
	float xPosOverlay = 0.0f, yPosOverlay = 0.0f;
	//Filename
	std::wstring settingsFileName = L"data\\saved_settings";
};