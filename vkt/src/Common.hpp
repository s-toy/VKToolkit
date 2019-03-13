#pragma once

namespace hiveVKT
{
#if (defined(_DEBUG) || defined(DEBUG))
#	define _ENABLE_VK_DEBUG_UTILS
#endif

#define _SAFE_DELETE(p)				{ delete (p); (p) = nullptr; }

#define _OUTPUT_EVENT(e)			std::cout << (e) << std::endl;
#define _OUTPUT_WARNING(e)			std::cerr << (e) << std::endl;
#define _THROW_RUNTINE_ERROR(e)		throw std::runtime_error(e);
}