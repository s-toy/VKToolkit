#pragma once
#include <string>
#include <fstream>
#include <vector>
#include "Common.h"

namespace hiveVKT
{
	template <class ... Args>
	static std::string format(const char *vFormat, Args... args)
	{
		if (nullptr == vFormat) return std::string{};

		int n = snprintf(nullptr, 0, vFormat, args...);
		if (0 == n) return std::string{};

		std::string Ret(n, '\0');
		snprintf(&*Ret.begin(), n + 1, vFormat, args...);

		return Ret;
	}

	template <class T>
	static bool readFile(const std::string& vFilename, T& voBuffer)
	{
		std::ifstream Fin(vFilename, std::ios::ate | std::ios::binary);
		if (!Fin.is_open()) return false;

		size_t FileSize = (size_t)Fin.tellg();

		using value_type = typename T::value_type;
		size_t ValueSize = sizeof(value_type);
		size_t BufferSize = (FileSize + ValueSize - 1) / ValueSize;
		voBuffer.resize(BufferSize);

		Fin.seekg(0);
		Fin.read(reinterpret_cast<char*>(voBuffer.data()), FileSize);
		Fin.close();

		return true;
	}
}