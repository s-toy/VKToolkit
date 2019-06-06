#include "VkCallParser.h"
#include <fstream>
#include "common/UtilityInterface.h"
#include "Utility.h"
#include "VkUtility.h"

using namespace hiveVKT;

//***********************************************************************************************
//FUNCTION:
TParseResult CVkCallParser::parse(const std::string& vFileName)
{
	_ASSERTE(!vFileName.empty());

	std::ifstream ifs(vFileName);
	_ASSERTE(ifs.is_open());
	std::string FileContent((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));

	std::vector<std::string> VKCallStrSet = hiveVKT::splitBySpaceLine(FileContent);

	TParseResult Result;

	for (auto VKCallStr : VKCallStrSet)
	{
		std::vector<std::string> Lines;
		hiveUtility::hiveSplitLine(VKCallStr, "\n", false, -1, Lines);
		_ASSERTE(Lines.size() >= 2);

		auto IDPair = __parseThreadAndFrameID(Lines[0]);
		auto VKCallInfo = __parseVKCallInfo(Lines[1]);
		Result[IDPair].push_back(VKCallInfo);
	}

	return Result;
}

//***********************************************************************************************
//FUNCTION:
std::pair<int, int> CVkCallParser::__parseThreadAndFrameID(const std::string& vLine)
{
	_ASSERTE(!vLine.empty());

	std::pair<int, int> FrameIDs;

	std::string l, r;
	hiveUtility::hiveSplitLine(vLine, ",", false, l, r);

	std::string l1, l2;
	hiveUtility::hiveSplitLine(l, " ", false, l1, l2);
	FrameIDs.first = atoi(l2.c_str());

	std::string r1, r2;
	hiveUtility::hiveSplitLine(r, " ", false, r1, r2);
	FrameIDs.second = atoi(r2.c_str());

	return FrameIDs;
}

//***********************************************************************************************
//FUNCTION:
SVKCallInfo CVkCallParser::__parseVKCallInfo(const std::string& vLine)
{
	SVKCallInfo VKCallInfo = {};

	std::vector<std::string> StrSet;
	hiveVKT::splitByStr(vLine, "returns", StrSet);

	std::vector<std::string> SubStrSet1;
	hiveUtility::hiveSplitLine(StrSet[0], "(", false, -1, SubStrSet1);
	VKCallInfo.FunctionName = SubStrSet1[0];

	std::vector<std::string> SubStrSet2;
	hiveUtility::hiveSplitLine(StrSet[1], " ", false, -1, SubStrSet2);
	VKCallInfo.ReturnValue = SubStrSet2[1];

	return VKCallInfo;
}