#include "StringUtils.h"
#include <Windows.h>

std::string WStringToString(const std::wstring& wstr)
{
	if (wstr.empty())
	{
		return std::string();
	}

	int size_need = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), nullptr, 0, nullptr, nullptr);
	std::string str(size_need, 0);
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), &str[0], size_need, nullptr, nullptr);
	return str;
}