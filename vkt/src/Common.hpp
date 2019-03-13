#pragma once

#if (defined(_DEBUG) || defined(DEBUG))
#	define _ENABLE_DEBUG_UTILS
#endif

#define _SAFE_DELETE(p)				{ delete (p); (p) = nullptr; }

#define _OUTPUT_EVENT(e)			std::cout << (e) << std::endl;
#define _OUTPUT_WARNING(e)			std::cerr << (e) << std::endl;
#define _THROW_RUNTINE_ERROR(e)		throw std::runtime_error(e);

#define _DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName &) = delete; \
    TypeName &operator =(const TypeName &) = delete;

namespace hiveVKT
{
	struct SDisplayInfo
	{
		int WindowWidth = 0, WindowHeight = 0;
		int WindowPosX = 0, WindowPosY = 0;
		std::string WindowTitle = "";
		bool IsWindowFullScreen = false;
		bool IsWindowResizable = false;

		bool isValid() const { return (WindowWidth > 0 && WindowHeight > 0); }
	};
}