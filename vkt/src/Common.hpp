#pragma once

#if (defined(_DEBUG) || defined(DEBUG))
#	define _ENABLE_VK_DEBUG_UTILS
#endif

#define _SAFE_DELETE(p) { delete (p); (p) = nullptr; }