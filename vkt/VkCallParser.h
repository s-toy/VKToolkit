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
		std::map<std::string, std::pair<std::string, std::string>> ParameterInfo; //NOTE: ParameterInfo: <Name, {Type, Value}>
	};

	class VKT_DECLSPEC CVkCallParser
	{
	public:
		bool parse(const std::string& vFileName);
		const std::vector<SVKCallInfo>& getVKCallInfoAt(int vThread, int vFrame) const { return m_ParseResult.at({vThread, vFrame}); }

	private:
		std::pair<int, int> __parseThreadAndFrameID(const std::string& vLine);
		std::pair<std::string, std::string> __parseFunctionNameAndReturnValue(const std::string& vLine);
		std::map<std::string, std::pair<std::string, std::string>> __parseParameterInfo(const std::vector<std::string>& vLines);

		int __computeIndentation(const std::string& vInput);
		const std::string& __trim(std::string& vInput);

	private:
		std::map<std::pair<int, int>, std::vector<SVKCallInfo>> m_ParseResult;
	};
}