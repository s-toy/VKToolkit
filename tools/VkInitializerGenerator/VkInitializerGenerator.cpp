#include "pch.h"
#include <fstream>
#include <cstdlib>
#include <regex>

int main()
{
	std::string FilePath = std::getenv("VULKAN") + std::string("\\include\\vulkan\\vulkan.hpp");
	std::ifstream ifs(FilePath);
	_ASSERTE(ifs.is_open());

	std::string Content((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));

	std::smatch Match;
	std::regex Regex("struct");

	return 0;
}