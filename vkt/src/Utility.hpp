#pragma once
#include <string>
#include <fstream>
#include <vector>
#include "Common.hpp"

namespace hiveVKT
{
	template <class ... Args>
	std::string format(const char *vFormat, Args... args)
	{
		int n = snprintf(nullptr, 0, vFormat, args...);
		std::string Ret(n, '\0');
		snprintf(&*Ret.begin(), n + 1, vFormat, args...);
		return Ret;
	}

	static std::vector<char> readFile(const std::string& vFilename)
	{
		std::ifstream Fin(vFilename, std::ios::ate | std::ios::binary);

		if (!Fin.is_open()) _THROW_RUNTINE_ERROR("Failed to open file!");

		size_t FileSize = (size_t)Fin.tellg();
		std::vector<char> Buffer(FileSize);

		Fin.seekg(0);
		Fin.read(Buffer.data(), FileSize);
		Fin.close();

		return Buffer;
	}
}