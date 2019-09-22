#pragma once
#include "Models.h"

class Calculator {
public:
	static const float CalculateItemValue(const Item& item);
	static const float CalculateCSPrice(const std::wstring& cs);
	static const float CalculateSpentOnMap(const Item& item);
	static const int GetChiselCount(const int& quality);
};