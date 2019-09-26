#pragma once

class Utilities {
public:
	static const std::wstring VectorToString(const std::vector<std::wstring>& source, const wchar_t& delimiter);
	static void GetHtmlData(const std::wstring& APIurl, const std::wstring& path);
	static void CleanHtmlData(const std::wstring& source, const std::wstring& dest);
	static void CheckVersion(const HWND& hWnd);
	static const std::wstring WchartToString(const wchar_t source[]);
	static const bool CheckDigits(const std::wstring& value);
	static std::wstring RemoveTrailingChar(std::wstring line);
};

class Version {
public:
	const bool Check() const;
	const std::wstring& GetVersion() const { return version; }
private:
	const std::wstring version = L"v1.1.1";
	const std::wstring githubApiURL = L"https://api.github.com/repos/Cailoki/PoE-FarmingTool/releases";
};

//Template class
template <class T>
class Converter {};
template <>
class Converter<int> {
public:
	static const std::wstring ToWstring(const int& value);
};
template <>
class Converter<float> {
public:
	static const std::wstring ToWstring(const float& value);
};