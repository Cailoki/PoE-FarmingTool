#pragma once
#include "Models.h"

class ClipParser {
public:
	explicit ClipParser(const std::wstring& text) :data(text) { }
	Item ParseClipboard();
private:
	std::wstring data;
};