#include "VkCallParser.h"
#include <fstream>
#include "common/UtilityInterface.h"
#include "Utility.h"

using namespace hiveVKT;

//***********************************************************************************************
//FUNCTION:
TParseResult CVkCallParser::parse(const std::string& vFileName)
{
	_ASSERTE(!vFileName.empty());

	std::ifstream ifs(vFileName);
	_ASSERTE(ifs.is_open());
	std::string FileContent((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));

	std::vector<std::string> VKCallStrSet;
	hiveUtility::hiveSplitLine(FileContent, "", false, -1, VKCallStrSet);

	TParseResult Result;

	for (auto VKCallStr : VKCallStrSet)
	{
		std::vector<std::string> Lines;
		hiveUtility::hiveSplitLine(VKCallStr, "", false, -1, Lines);
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

	std::string l, r;
	hiveUtility::hiveSplitLine(vLine, "returns", false, l, r);

	std::string l1, l2;
	hiveUtility::hiveSplitLine(l, "(", false, l1, l2);
	VKCallInfo.FunctionName = l1;

	std::vector<std::string> rs;
	hiveUtility::hiveSplitLine(r, " ", false, -1, rs);
	VKCallInfo.ReturnValue = rs[1];

	return VKCallInfo;
}