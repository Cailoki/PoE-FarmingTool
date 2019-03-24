#include "pch.h"
#include "GetPrice.h"
#include "wininet.h"

void ParseLine(const std::string& line, ParsedData& pData) {
	std::stringstream ss{ line };
	float value;
	std::string name;
	ss.seekg(13);
	ss >> value >> name;
	name.erase(name.size() - 1);
	if (name == "chaos")
		pData.name = "Chaos Orb";
	else if (name == "exalted")
		pData.name = "Exalted Orb";

	pData.value = value;
}

void ParseHTML(ParsedData& pData) {
	std::ifstream ist{ "data\\search_result_html" };
	if (!ist) {
		MessageBox(NULL, L"search_result_html not found!", L"Error!", NULL);
		return;
	}

	ist.seekg(77000); //magic number, jump closer to important data
	std::string line, validLine;
	int counter = 1;
	bool found = false;
	while (getline(ist, line)) {
		if ((line.size() > 5) && (line[5] == 'b')) {
			validLine = line;
			if (counter > 4) {
				found = true;
				ParseLine(line, pData);
				break;
			}
			else
				++counter;
		}
	}

	if (!found && validLine != "") { //if less then 5 items on sale, use last valid entry (may return unrealistic value)
		ParseLine(validLine, pData);
	}

	ist.close();
	remove("data\\search_result_html");
}

//Get specific item prices from poe.trade
void GetHTML(const std::string& name, const std::string& league) {
	//example request "league=Betrayal&online=x&buyout_min=1&buyout_currency=alteration&name=The+Endless+Darkness"
	std::string part1 = "league=", part2 = "&online=x&buyout_min=1&buyout_currency=alteration&name=", part3, finalUrl, word;
	std::istringstream ss{ name };
	while (ss >> word)
		part3 += word + '+';
	part3.erase(part3.size() - 1);
	finalUrl = part1 + league + part2 + part3; //remove extra '+' at the end

	CHAR szReq[130];
	int i = 0;
	for (i; i < static_cast<int>(finalUrl.size()); ++i) //fill array with finalUrl character
		szReq[i] = finalUrl[i];
	for (i; i < 130; ++i) //set unused characters to \0
		szReq[i] = '\0';

	LPCWSTR header = L"Accept-Encoding: gzip, deflate";
	LPCWSTR userAgent = L"Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:64.0) Gecko/20100101 Firefox/64.0";

	HINTERNET open = InternetOpen(userAgent, INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, NULL);
	
	HINTERNET connect = InternetConnect(open, L"poe.trade", 80, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
	//importat position in code for decoding option relative to the handle it needs, if using "connect" handle, put decoding part direcly below
	BOOL isDecoding = true;
	InternetSetOption(connect, INTERNET_OPTION_HTTP_DECODING, &isDecoding, sizeof(isDecoding));

	HINTERNET request = HttpOpenRequest(connect, L"POST", L"/search", L"HTTP/1.1", NULL, NULL, NULL, NULL);
	HttpSendRequest(request, header, (DWORD)(wcslen(header)), szReq, DWORD(strlen(szReq)));

	//saving html data to a file
	char* Buffer = new char[1024];
	DWORD BytesWritten;
	std::ofstream ost{ "data\\search_result_html" };
	while (InternetReadFile(request, Buffer, 1024, &BytesWritten) && BytesWritten) {
		ost << Buffer;
	}

	delete Buffer;
	InternetCloseHandle(open);
}