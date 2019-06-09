#pragma once
#include <string>
#include <vector>
#include <map>
#include "Export.h"

namespace hiveVKT
{
	struct SVKCallInfo
	{
		std::string FunctionName;
		std::string ReturnValue;
	};

	class VKT_DECLSPEC CVkCallParser
	{
	public:
		bool parse(const std::string& vFileName);
		const std::vector<SVKCallInfo>& getVKCallInfoAt(int vThread, int vFrame) const { return m_ParseResult.at({vThread, vFrame}); }

	private:
		std::pair<int, int> __parseThreadAndFrameID(const std::string& vLine);
		SVKCallInfo __parseVKCallInfo(const std::string& vLine);

	private:
		std::map<std::pair<int, int>, std::vector<SVKCallInfo>> m_ParseResult;
	};
}