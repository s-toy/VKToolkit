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
#define _THROW_RUNTINE_ERROR(e)		throw std::runtime_error(e);

#define _DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName &) = delete; \
    TypeName &operator =(const TypeName &) = delete;

namespace hiveVKT
{
	struct SDisplayInfo
	{
		SDisplayInfo() {  }

		int WindowWidth = 0, WindowHeight = 0;
		int WindowPosX = 0, WindowPosY = 0;
		std::string WindowTitle = "";
		bool IsWindowFullScreen = false;
		bool IsWindowResizable = false;

		bool isValid() const { return (WindowWidth > 0 && WindowHeight > 0); }	//TODO:
	};
}