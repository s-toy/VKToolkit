#include "VkCallParser.h"
#include <fstream>
#include "common/UtilityInterface.h"
#include "Utility.h"
#include "VkUtility.h"

using namespace hiveVKT;

//***********************************************************************************************
//FUNCTION:
bool CVkCallParser::parse(const std::string& vFileName)
{
	if (vFileName.empty()) return false;

	std::ifstream ifs(vFileName);
	if (!ifs.is_open()) return false;
	std::string FileContent((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));

	std::vector<std::string> VKCallStrSet;
	hiveUtility::hiveSplitLineByRegexPattern(FileContent, "\n\n", false, VKCallStrSet);

	for (auto VKCallStr : VKCallStrSet)
	{
		std::vector<std::string> Lines;
		hiveUtility::hiveSplitLineByRegexPattern(VKCallStr, "\n", false, Lines);
		if (Lines.size() < 2 || Lines[0].empty(), Lines[1].empty()) return false;

		SVKCallInfo VKCallInfo = {};
		auto IDPair = __parseThreadAndFrameID(Lines[0]);
		auto FuncPair = __parseFunctionNameAndReturnValue(Lines[1]);
		auto ParameterInfo = __parseParameterInfo(Lines);
		VKCallInfo.FunctionName = FuncPair.first;
		VKCallInfo.ReturnValue  = FuncPair.second;
		VKCallInfo.ParameterInfo = ParameterInfo;

		m_ParseResult[IDPair].push_back(VKCallInfo);
	}

	return true;
}

//***********************************************************************************************
//FUNCTION:
std::pair<int, int> CVkCallParser::__parseThreadAndFrameID(const std::string& vLine)
{
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
std::pair<std::string, std::string> CVkCallParser::__parseFunctionNameAndReturnValue(const std::string& vLine)
{
	std::vector<std::string> StrSet;
	hiveVKT::splitByStr(vLine, "returns", StrSet);

	std::vector<std::string> SubStrSet1;
	hiveUtility::hiveSplitLine(StrSet[0], "(", false, -1, SubStrSet1);
	auto FunctionName = SubStrSet1[0];

	std::vector<std::string> SubStrSet2;
	hiveUtility::hiveSplitLine(StrSet[1], " ", false, -1, SubStrSet2);
	auto ReturnValue = SubStrSet2.size() <= 1 ? "void" : SubStrSet2[1];

	return { FunctionName, ReturnValue };
}

//***********************************************************************************************
//FUNCTION:
std::map<std::string, std::pair<std::string, std::string>> CVkCallParser::__parseParameterInfo(const std::vector<std::string>& vLines)
{
	std::map<std::string, std::pair<std::string, std::string>> ParameterInfo;

	auto LastIndent = 0; std::string LastParameterName;
	for (size_t i = 2; i < vLines.size(); ++i) //NOTE: skip the first two lines
	{
		auto Line = vLines[i];
		auto CurIndent = __computeIndentation(Line);
		std::vector<std::string> SplitParameterSet;
		hiveUtility::hiveSplitLineByRegexPattern(Line, "[:=(\\|)]", false, SplitParameterSet);

		_ASSERT(SplitParameterSet.size() >= 3);
		const auto& Name  = __trim(SplitParameterSet[0]);
		const auto& Type  = __trim(SplitParameterSet[1]);
		const auto& Value = __trim(SplitParameterSet[2]);

		std::string FullName;
		if (LastIndent == 0) FullName = Name;
		else 
		{
			if (CurIndent > LastIndent)
				FullName += LastParameterName + "|" + Name;
			else
			{
				std::string Input = LastParameterName;
				std::vector<std::string> Result;
				hiveUtility::hiveSplitLineByRegexPattern(Input, "\\|", false, Result);

				auto Back = (LastIndent - CurIndent) / 4 + 1; //NOTE: »ØËÝ²ã¼¶
				for (auto k = 0; k < Result.size() - Back; ++k)
				{
					FullName += Result[k]; FullName += "|";
				}
				FullName += SplitParameterSet[0];
			}
		}

		LastIndent = CurIndent;
		LastParameterName = FullName;
		ParameterInfo[FullName] = { Type, Value };
	}

	return ParameterInfo;
}

//***********************************************************************************************
//FUNCTION:
int CVkCallParser::__computeIndentation(const std::string& vInput)
{
	auto Indentation = 0;
	for (auto Iter = vInput.begin(); Iter != vInput.end(); ++Iter)
	{
		if (*Iter != ' ') break;
		Indentation++;
	}
	return Indentation;
}

//***********************************************************************************************
//FUNCTION:
const std::string& CVkCallParser::__trim(std::string& vInput)
{
	if (vInput.empty()) return vInput;

	vInput.erase(0, vInput.find_first_not_of(" "));
	vInput.erase(vInput.find_last_not_of(" ") + 1);

	return vInput;
}