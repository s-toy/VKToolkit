#include "Utility.h"
#include <boost/regex.hpp>

//***********************************************************************************************
//FUNCTION:
std::vector<std::string> hiveVKT::splitByEnter(std::string vInput)
{
	std::vector<std::string> Result;
	boost::regex e("\r\n", boost::regbase::normal | boost::regbase::icase);
	boost::regex_split(std::back_inserter(Result), vInput, e);

	return Result;
}

//***********************************************************************************************
//FUNCTION:
std::vector<std::string> hiveVKT::splitBySpaceLine(std::string vInput)
{
	std::vector<std::string> Result;
	boost::regex e("\r\n\r\n", boost::regbase::normal | boost::regbase::icase);
	boost::regex_split(std::back_inserter(Result), vInput, e);

	return Result;
}