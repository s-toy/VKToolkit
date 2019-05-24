#pragma once
#include "common/CommonMicro.h"
#include "common/UtilityInterface.h"

#define FORCE_ENABLE_DEBUG_UTILS

#if (defined(_DEBUG) || defined(DEBUG) || defined(FORCE_ENABLE_DEBUG_UTILS))
#	define _ENABLE_DEBUG_UTILS
#endif

#if (defined(_MSC_VER) || defined(__CYGWIN__) || defined(__MINGW32__))
#	define _WINDOWS
#endif

//TODO: some of these micros should be moved to hiveCommon

#define _OUTPUT_EVENT(e)			hiveUtility::hiveOutputEvent(e);
#define _OUTPUT_WARNING(e)			hiveUtility::hiveOutputWarning(__EXCEPTION_SITE__, (e));
#define _THROW_RUNTIME_ERROR(e)		throw std::runtime_error(e);

#define _DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName &) = delete; \
    TypeName &operator =(const TypeName &) = delete;

#define _CALLBACK_0(Selector, Target, ...) std::bind(&Selector, Target, ##__VA_ARGS__)
#define _CALLBACK_1(Selector, Target, ...) std::bind(&Selector, Target, std::placeholders::_1, ##__VA_ARGS__)
#define _CALLBACK_2(Selector, Target, ...) std::bind(&Selector, Target, std::placeholders::_1, std::placeholders::_2, ##__VA_ARGS__)
#define _CALLBACK_3(Selector, Target, ...) std::bind(&Selector, Target, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, ##__VA_ARGS__)
#define _CALLBACK_4(Selector, Target, ...) std::bind(&Selector, Target, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, ##__VA_ARGS__)

namespace hiveVKT
{
	struct SDisplayInfo
	{
		uint32_t WindowWidth	= 800;
		uint32_t WindowHeight	= 600;
		uint32_t WindowPosX		= 100;
		uint32_t WindowPosY		= 100;
		std::string WindowTitle = "HIVE APPLICATION";
		bool IsWindowFullScreen = false;
		bool IsWindowResizable	= false;

		bool isValid() const { return (WindowWidth > 0 && WindowHeight > 0); }	//TODO:
	};
}