#pragma once

struct ParsedData {
	std::string name = "";
	float value = 0;
};

//Used to download specific item price from poe.trade (with custom POST request)
void GetHTML(const std::string& name, const std::string& league);
void ParseHTML(ParsedData& pData);
void ParseLine(const std::string&, ParsedData& pData);