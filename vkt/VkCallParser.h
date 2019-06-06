#pragma once
#include <string>
#include <vector>
#include <map>

namespace hiveVKT
{
	struct SVKCallInfo
	{
		std::string FunctionName;
		std::string ReturnValue;
	};

	using TParseResult = std::map<std::pair<int, int>, std::vector<SVKCallInfo>>;

	class CVkCallParser
	{
	public:
		TParseResult parse(const std::string& vFileName);

	private:
		std::pair<int, int> __parseThreadAndFrameID(const std::string& vLine);
		SVKCallInfo __parseVKCallInfo(const std::string& vLine);
	};
}