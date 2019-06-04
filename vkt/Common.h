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

#define _DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName &) = delete; \
    TypeName &operator =(const TypeName &) = delete;

namespace hiveVKT
{
	enum class EResult
	{
		eSuccess = VK_SUCCESS,
		eNotReady = VK_NOT_READY,
		eTimeout = VK_TIMEOUT,
		eEventSet = VK_EVENT_SET,
		eEventReset = VK_EVENT_RESET,
		eIncomplete = VK_INCOMPLETE,
		eErrorOutOfHostMemory = VK_ERROR_OUT_OF_HOST_MEMORY,
		eErrorOutOfDeviceMemory = VK_ERROR_OUT_OF_DEVICE_MEMORY,
		eErrorInitializationFailed = VK_ERROR_INITIALIZATION_FAILED,
		eErrorDeviceLost = VK_ERROR_DEVICE_LOST,
		eErrorMemoryMapFailed = VK_ERROR_MEMORY_MAP_FAILED,
		eErrorLayerNotPresent = VK_ERROR_LAYER_NOT_PRESENT,
		eErrorExtensionNotPresent = VK_ERROR_EXTENSION_NOT_PRESENT,
		eErrorFeatureNotPresent = VK_ERROR_FEATURE_NOT_PRESENT,
		eErrorIncompatibleDriver = VK_ERROR_INCOMPATIBLE_DRIVER,
		eErrorTooManyObjects = VK_ERROR_TOO_MANY_OBJECTS,
		eErrorFormatNotSupported = VK_ERROR_FORMAT_NOT_SUPPORTED,
		eErrorFragmentedPool = VK_ERROR_FRAGMENTED_POOL,
		eErrorOutOfPoolMemory = VK_ERROR_OUT_OF_POOL_MEMORY,
		eErrorOutOfPoolMemoryKHR = VK_ERROR_OUT_OF_POOL_MEMORY,
		eErrorInvalidExternalHandle = VK_ERROR_INVALID_EXTERNAL_HANDLE,
		eErrorInvalidExternalHandleKHR = VK_ERROR_INVALID_EXTERNAL_HANDLE,
		eErrorSurfaceLostKHR = VK_ERROR_SURFACE_LOST_KHR,
		eErrorNativeWindowInUseKHR = VK_ERROR_NATIVE_WINDOW_IN_USE_KHR,
		eSuboptimalKHR = VK_SUBOPTIMAL_KHR,
		eErrorOutOfDateKHR = VK_ERROR_OUT_OF_DATE_KHR,
		eErrorIncompatibleDisplayKHR = VK_ERROR_INCOMPATIBLE_DISPLAY_KHR,
		eErrorValidationFailedEXT = VK_ERROR_VALIDATION_FAILED_EXT,
		eErrorInvalidShaderNV = VK_ERROR_INVALID_SHADER_NV,
		eErrorInvalidDrmFormatModifierPlaneLayoutEXT = VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT,
		eErrorFragmentationEXT = VK_ERROR_FRAGMENTATION_EXT,
		eErrorNotPermittedEXT = VK_ERROR_NOT_PERMITTED_EXT,
		eErrorInvalidDeviceAddressEXT = VK_ERROR_INVALID_DEVICE_ADDRESS_EXT,
		eErrorInvalidParameters
	};
}