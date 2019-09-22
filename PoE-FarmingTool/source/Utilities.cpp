#include "pch.h"
#include "Utilities.h"
#include <WinInet.h>

//Template class specialization definition for integer
const std::wstring Converter<int>::ToWstring(const int& value) {
	return std::to_wstring(value);
}

//Template class specialization definition for float
const std::wstring Converter<float>::ToWstring(const float& value) {
	wchar_t test[10];
	std::swprintf(test, 10, L"%.2f", value);
	std::wstring r(test);
	return r;
}

//Check if user put numerical value in the edit box
const bool Utilities::CheckDigits(const std::wstring& value) {
	bool dotFound = false;
	for (size_t i = 0; i < value.size(); ++i) //Example -50.25 / bad example -50.2.5
		if (!isdigit(value[i])) {
			if (value[i] == '-' && i == 0) //First character
				continue;
			else if (value[i] == '-' && i != 0) {
				MessageBox(NULL, L"Invalid entry, only numerical values allowed (0.00)!", L"Error", NULL);
				return false;
			}
			if (value[i] != '.' || dotFound) { //Reject all non 'dot'-s / bad example -50*25
				MessageBox(NULL, L"Invalid entry, only numerical values allowed (0.00)!", L"Error", NULL);
				return false;
			}
			else
				dotFound = true;
		}
	return true;
}

std::wstring Utilities::RemoveTrailingChar(std::wstring data) {
	try {
		return data.erase(data.size() - 1);
	}
	catch (...) {
		return L"error"; //Invalid string position
	}
}

//Convert vector data to string
const std::wstring Utilities::VectorToString(const std::vector<std::wstring>& source, const wchar_t& delimiter) {
	std::wstring returnValue;
	for (size_t i = 0; i < source.size(); i++)
	{
		returnValue += source[i] + delimiter;
	}
	return Utilities::RemoveTrailingChar(returnValue);
}

//Write given URL data to a file
void Utilities::GetHtmlData(const std::wstring& APIurl, const std::wstring& path) {
	LPCWSTR userAgent = L"Mozilla / 5.0 (Windows NT 10.0; Win64; x64; rv:63.0) Gecko / 20100101 Firefox / 63.0";
	LPCWSTR header = L"Accept-Encoding: gzip, deflate";

	HINTERNET connect = InternetOpen(userAgent, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, NULL);
	if (!connect) {
		MessageBox(NULL, L"Connection failed or syntax error!", L"Error", MB_OK);
		return;
	}

	BOOL isDecoding = true;
	BOOL decoding = InternetSetOption(connect, INTERNET_OPTION_HTTP_DECODING, &isDecoding, sizeof(isDecoding));
	if (!decoding) {
		MessageBox(NULL, L"Connection not decoded!", L"Error", MB_OK);
		return;
	}

	std::wstring tempURL = std::wstring(APIurl.begin(), APIurl.end());
	LPCWSTR finalURL = tempURL.c_str();
	HINTERNET url = InternetOpenUrl(connect, finalURL, header, (DWORD)-1L, NULL, INTERNET_FLAG_NO_CACHE_WRITE);

	if (!url) {
		MessageBox(NULL, L"Error opening URL!", L"Error", MB_OK);
		return;
	}

	std::wstring tempPath = std::wstring(path.begin(), path.end());
	LPCWSTR finalPath = tempPath.c_str();
	HANDLE hFile = CreateFile(finalPath, GENERIC_READ | GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	DWORD blocksize = 1024;
	DWORD dwTemp;
	DWORD dwBytesRead = 1024;

	for (; dwBytesRead > 0;)
	{
		InternetReadFile(url, &blocksize, (DWORD)sizeof(blocksize), &dwBytesRead);
		WriteFile(hFile, &blocksize, dwBytesRead, &dwTemp, NULL);
	}

	CloseHandle(hFile);
	InternetCloseHandle(url);
}

//Clean downloaded file(JSON original) from special charaters ('{',':','['...)
void Utilities::CleanHtmlData(const std::wstring& source, const std::wstring& dest) {
	std::wifstream data{ source };
	std::ofstream cleanDataOut{ dest };

	while (!data.eof()) {
		char ch = (char)data.get();
		if (ch == '{' || ch == '}' || ch == ':' || ch == '[' || ch == ']' || ch == '"' || ch == ',' || ch == ' ')
			cleanDataOut << ' ';
		else
			cleanDataOut << ch;
	}
}

//Change "update available" buttons state
void Utilities::CheckVersion(const HWND& hWnd) {
	Version newVer;
	if (newVer.Check()) //Returns true if new version is found
		ShowWindow(hWnd, SW_SHOW);
}

//Convert wchar_t to string
const std::wstring Utilities::WchartToString(const wchar_t source[]) {
	std::wstring mystring;
	while (*source)
		mystring += (char)* source++;
	return mystring;
}

//Check if app version is different from the repository version
const bool Version::Check() const {
	std::wstring path = L"data\\github_raw";
	Utilities::GetHtmlData(githubApiURL, path);

	std::wifstream dataIn{ path };
	if (!dataIn) {
		MessageBox(NULL, L"New version check \"raw\" file not found!", L"Error", NULL);
		return false;
	}

	std::wstring keyword, ver;
	while (dataIn >> keyword) {
		if (keyword == L"\"tag_name\":") { //Example - "tag_name": "v1.0.0",
			dataIn >> ver;
			ver.erase(ver.begin());
			ver.erase(ver.end() - 2, ver.end());
			break;
		}
	}

	//Cleanup
	dataIn.close();
	remove("data\\github_raw");

	//Returned state will change "Update Button" visibility
	if (ver != version)
		return true;
	else
		return false;
}