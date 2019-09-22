#include "pch.h"
#include "Parser.h"
#include "Utilities.h"

//Parses data from clipboard and constructs item object
Item ClipParser::ParseClipboard() {
	//Empty clipboard
	if (data.size() == 0)
		return Item();

	Item item; //Item populated during parsing
	std::wistringstream dataStream{ data };
	std::wistringstream temp; //Temp for streaming lines
	std::wstring stringLine, word;
	
	//First line example -> "Rarity: Currency"
	dataStream.seekg(8, std::ios_base::cur);
	std::getline(dataStream, stringLine);
	item.SetRarity(Utilities::RemoveTrailingChar(stringLine)); //Set rarity for item

	//Second line example -> "Orb of Scouring" or "Rotting Shadows"
	std::getline(dataStream, stringLine);
	item.SetName(Utilities::RemoveTrailingChar(stringLine)); //Set item's name

	//Third line example -> "Reef Map" or "--------" or "Midnight Blade"...
	std::getline(dataStream, stringLine);
	stringLine = Utilities::RemoveTrailingChar(stringLine);
	//If third line isn't separator line then set that line as item base
	if (stringLine != L"--------") {
		item.SetBase(stringLine);
		dataStream >> word; //Consume separator
	}

	//After first separator, possible lines "Stack Size: 4/30", "Quality: +20%", "Corrupted"
	while (std::getline(dataStream, stringLine)) {
		if (stringLine.find(L"Stack") != std::wstring::npos) {
			int stack;
			temp = std::wistringstream(stringLine);
			temp >> word; //"Stack"
			temp >> word; //"Size:"
			temp >> stack;
			item.SetStack(stack);
		}
		else if (stringLine.find(L"Quality:") != std::wstring::npos) {
			temp = std::wistringstream(stringLine); //"Quality: +20%"
			temp.seekg(10);
			int quality;
			temp >> quality;
			item.SetQuality(quality);
		}
		else if (stringLine.find(L"Corrupted") != std::wstring::npos) {
			item.SetCorrupted(true);
		}
	}

	return item;
}