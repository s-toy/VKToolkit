#include "Common.h"
#include <fstream>

using namespace DeferredShading;

//************************************************************************************
//Function:
std::vector<char> DeferredShading::ReadFile(const std::string& vFileName)
{
	std::ifstream File(vFileName, std::ios::ate | std::ios::binary);

	if (!File.is_open())
		throw std::runtime_error(("Failed to open file [" + vFileName + "]!").c_str());

	rsize_t FileSize = static_cast<rsize_t>(File.tellg());
	std::vector<char> Buffer(FileSize);

	File.seekg(0);
	File.read(Buffer.data(), FileSize);

	File.close();

	return Buffer;
}
