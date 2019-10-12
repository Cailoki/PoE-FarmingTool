#pragma once

class ItemMods {
public:
	static const std::vector<std::wstring> mapPrefixModifiers;
};

class Maps {
public:
	struct UniqueMap { std::wstring name, base; };
	static const std::vector<Maps::UniqueMap>uniqueMaps;
};

class Item {
public:
	//Constructors
	Item() {};
	//Getters
	const std::wstring GetRarity() const { return rarity; }
	const std::wstring GetType() const { return type; }
	const std::wstring GetName() const { return name; }
	const int GetStack() const { return stack; }
	const int GetQuality() const { return quality; }
	const bool GetCorrupted() const { return isCorrupted; }
	const bool GetIdentified() const { return isIdentified; }
	const float GetValue() const { return value; }
	const std::wstring GetBase() const{ return base; }
	//Setters
	void SetRarity(const std::wstring& rar) { rarity = rar; }
	void SetType(const std::wstring& typ) { type = typ; }
	void SetName(const std::wstring& nm) { name = nm; }
	void SetStack(const int& stck) { stack = stck; }
	void SetQuality(const int& qlty) { quality = qlty; }
	void SetCorrupted(bool corrupted) { isCorrupted = corrupted; }
	void SetValue(const float& val) { value = val; }
	void SetBase(const std::wstring& val) { base = val; }
	void SetIdentified(bool identified) { isIdentified = identified; }
private:
	std::wstring rarity = L"Unknown"; //Currency, Normal, Unique...
	std::wstring type = L"Unknown"; //Map, Scarab, Essence...
	std::wstring name = L"Unknown"; //Ahn's Might, Rotting Shadows, Orb of Scouring
	std::wstring base = L"Unknown"; //Midnight Blade, Reef Map
	float value = 0;
	int stack = 1;
	int quality = 0;
	bool isCorrupted = false, isIdentified = true;
};