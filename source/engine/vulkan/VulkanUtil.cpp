#include "stdheader.h"
#include "vulkan/vulkan.hpp"
#include "vulkan/vulkan_win32.h"
#include "VulkanUtil.h"

namespace Aurora
{

	PFN_vkCreateDebugUtilsMessengerEXT CreateDebugUtilsMessengerEXT;
	PFN_vkDestroyDebugUtilsMessengerEXT DestroyDebugUtilsMessengerEXT;
	PFN_vkSubmitDebugUtilsMessageEXT SubmitDebugUtilsMessageEXT;
	PFN_vkCmdBeginDebugUtilsLabelEXT CmdBeginDebugUtilsLabelEXT;
	PFN_vkCmdEndDebugUtilsLabelEXT CmdEndDebugUtilsLabelEXT;
	PFN_vkCmdInsertDebugUtilsLabelEXT CmdInsertDebugUtilsLabelEXT;
	PFN_vkSetDebugUtilsObjectNameEXT SetDebugUtilsObjectNameEXT;
	VkDebugUtilsMessengerEXT dbg_messenger;



const char* string_VkObjectType(VkObjectType input_value)
{
	switch ((VkObjectType)input_value)
	{
	case VK_OBJECT_TYPE_QUERY_POOL:
		return "VK_OBJECT_TYPE_QUERY_POOL";
	case VK_OBJECT_TYPE_OBJECT_TABLE_NVX:
		return "VK_OBJECT_TYPE_OBJECT_TABLE_NVX";
	case VK_OBJECT_TYPE_SAMPLER_YCBCR_CONVERSION:
		return "VK_OBJECT_TYPE_SAMPLER_YCBCR_CONVERSION";
	case VK_OBJECT_TYPE_SEMAPHORE:
		return "VK_OBJECT_TYPE_SEMAPHORE";
	case VK_OBJECT_TYPE_SHADER_MODULE:
		return "VK_OBJECT_TYPE_SHADER_MODULE";
	case VK_OBJECT_TYPE_SWAPCHAIN_KHR:
		return "VK_OBJECT_TYPE_SWAPCHAIN_KHR";
	case VK_OBJECT_TYPE_SAMPLER:
		return "VK_OBJECT_TYPE_SAMPLER";
	case VK_OBJECT_TYPE_INDIRECT_COMMANDS_LAYOUT_NVX:
		return "VK_OBJECT_TYPE_INDIRECT_COMMANDS_LAYOUT_NVX";
	case VK_OBJECT_TYPE_DEBUG_REPORT_CALLBACK_EXT:
		return "VK_OBJECT_TYPE_DEBUG_REPORT_CALLBACK_EXT";
	case VK_OBJECT_TYPE_IMAGE:
		return "VK_OBJECT_TYPE_IMAGE";
	case VK_OBJECT_TYPE_UNKNOWN:
		return "VK_OBJECT_TYPE_UNKNOWN";
	case VK_OBJECT_TYPE_DESCRIPTOR_POOL:
		return "VK_OBJECT_TYPE_DESCRIPTOR_POOL";
	case VK_OBJECT_TYPE_COMMAND_BUFFER:
		return "VK_OBJECT_TYPE_COMMAND_BUFFER";
	case VK_OBJECT_TYPE_BUFFER:
		return "VK_OBJECT_TYPE_BUFFER";
	case VK_OBJECT_TYPE_SURFACE_KHR:
		return "VK_OBJECT_TYPE_SURFACE_KHR";
	case VK_OBJECT_TYPE_INSTANCE:
		return "VK_OBJECT_TYPE_INSTANCE";
	case VK_OBJECT_TYPE_VALIDATION_CACHE_EXT:
		return "VK_OBJECT_TYPE_VALIDATION_CACHE_EXT";
	case VK_OBJECT_TYPE_IMAGE_VIEW:
		return "VK_OBJECT_TYPE_IMAGE_VIEW";
	case VK_OBJECT_TYPE_DESCRIPTOR_SET:
		return "VK_OBJECT_TYPE_DESCRIPTOR_SET";
	case VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT:
		return "VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT";
	case VK_OBJECT_TYPE_COMMAND_POOL:
		return "VK_OBJECT_TYPE_COMMAND_POOL";
	case VK_OBJECT_TYPE_PHYSICAL_DEVICE:
		return "VK_OBJECT_TYPE_PHYSICAL_DEVICE";
	case VK_OBJECT_TYPE_DISPLAY_KHR:
		return "VK_OBJECT_TYPE_DISPLAY_KHR";
	case VK_OBJECT_TYPE_BUFFER_VIEW:
		return "VK_OBJECT_TYPE_BUFFER_VIEW";
	case VK_OBJECT_TYPE_DEBUG_UTILS_MESSENGER_EXT:
		return "VK_OBJECT_TYPE_DEBUG_UTILS_MESSENGER_EXT";
	case VK_OBJECT_TYPE_FRAMEBUFFER:
		return "VK_OBJECT_TYPE_FRAMEBUFFER";
	case VK_OBJECT_TYPE_DESCRIPTOR_UPDATE_TEMPLATE:
		return "VK_OBJECT_TYPE_DESCRIPTOR_UPDATE_TEMPLATE";
	case VK_OBJECT_TYPE_PIPELINE_CACHE:
		return "VK_OBJECT_TYPE_PIPELINE_CACHE";
	case VK_OBJECT_TYPE_PIPELINE_LAYOUT:
		return "VK_OBJECT_TYPE_PIPELINE_LAYOUT";
	case VK_OBJECT_TYPE_DEVICE_MEMORY:
		return "VK_OBJECT_TYPE_DEVICE_MEMORY";
	case VK_OBJECT_TYPE_FENCE:
		return "VK_OBJECT_TYPE_FENCE";
	case VK_OBJECT_TYPE_QUEUE:
		return "VK_OBJECT_TYPE_QUEUE";
	case VK_OBJECT_TYPE_DEVICE:
		return "VK_OBJECT_TYPE_DEVICE";
	case VK_OBJECT_TYPE_RENDER_PASS:
		return "VK_OBJECT_TYPE_RENDER_PASS";
	case VK_OBJECT_TYPE_DISPLAY_MODE_KHR:
		return "VK_OBJECT_TYPE_DISPLAY_MODE_KHR";
	case VK_OBJECT_TYPE_EVENT:
		return "VK_OBJECT_TYPE_EVENT";
	case VK_OBJECT_TYPE_PIPELINE:
		return "VK_OBJECT_TYPE_PIPELINE";
	default:
		return "Unhandled VkObjectType";
	}
}





bool in_callback = false;

VKAPI_ATTR VkBool32 VKAPI_CALL debug_messenger_callback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData) {
	char prefix[64] = "";
	char* message = (char*)malloc(strlen(pCallbackData->pMessage) + 5000);
	assert(message);
	struct demo* demo = (struct demo*)pUserData;


	if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
		strcat(prefix, "VERBOSE : ");
	}
	else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
		strcat(prefix, "INFO : ");
	}
	else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
		strcat(prefix, "WARNING : ");
	}
	else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
		strcat(prefix, "ERROR : ");
	}

	if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT) {
		strcat(prefix, "GENERAL");
	}
	else {
		if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) {
			strcat(prefix, "VALIDATION");
			//validation_error = 1;
		}
		if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT) {
			if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) {
				strcat(prefix, "|");
			}
			strcat(prefix, "PERFORMANCE");
		}
	}

	sprintf(message, "%s - Message Id Number: %d | Message Id Name: %s\n\t%s\n", prefix, pCallbackData->messageIdNumber,
		pCallbackData->pMessageIdName, pCallbackData->pMessage);
	if (pCallbackData->objectCount > 0) {
		char tmp_message[500];
		sprintf(tmp_message, "\n\tObjects - %d\n", pCallbackData->objectCount);
		strcat(message, tmp_message);
		for (uint32_t object = 0; object < pCallbackData->objectCount; ++object) {
			if (NULL != pCallbackData->pObjects[object].pObjectName && strlen(pCallbackData->pObjects[object].pObjectName) > 0) {
				sprintf(tmp_message, "\t\tObject[%d] - %s, Handle %p, Name \"%s\"\n", object,
					string_VkObjectType(pCallbackData->pObjects[object].objectType),
					(void*)(pCallbackData->pObjects[object].objectHandle), pCallbackData->pObjects[object].pObjectName);
			}
			else {
				sprintf(tmp_message, "\t\tObject[%d] - %s, Handle %p\n", object,
					string_VkObjectType(pCallbackData->pObjects[object].objectType),
					(void*)(pCallbackData->pObjects[object].objectHandle));
			}
			strcat(message, tmp_message);
		}
	}
	if (pCallbackData->cmdBufLabelCount > 0) {
		char tmp_message[500];
		sprintf(tmp_message, "\n\tCommand Buffer Labels - %d\n", pCallbackData->cmdBufLabelCount);
		strcat(message, tmp_message);
		for (uint32_t cmd_buf_label = 0; cmd_buf_label < pCallbackData->cmdBufLabelCount; ++cmd_buf_label) {
			sprintf(tmp_message, "\t\tLabel[%d] - %s { %f, %f, %f, %f}\n", cmd_buf_label,
				pCallbackData->pCmdBufLabels[cmd_buf_label].pLabelName, pCallbackData->pCmdBufLabels[cmd_buf_label].color[0],
				pCallbackData->pCmdBufLabels[cmd_buf_label].color[1], pCallbackData->pCmdBufLabels[cmd_buf_label].color[2],
				pCallbackData->pCmdBufLabels[cmd_buf_label].color[3]);
			strcat(message, tmp_message);
		}
	}

#ifdef _WIN32

	in_callback = true;
	//if (!demo->suppress_popups) MessageBox(NULL, message, "Alert", MB_OK);
	GLog->Info(message);
	in_callback = false;

#elif defined(ANDROID)

	if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
		__android_log_print(ANDROID_LOG_INFO, APP_SHORT_NAME, "%s", message);
	}
	else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
		__android_log_print(ANDROID_LOG_WARN, APP_SHORT_NAME, "%s", message);
	}
	else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
		__android_log_print(ANDROID_LOG_ERROR, APP_SHORT_NAME, "%s", message);
	}
	else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
		__android_log_print(ANDROID_LOG_VERBOSE, APP_SHORT_NAME, "%s", message);
	}
	else {
		__android_log_print(ANDROID_LOG_INFO, APP_SHORT_NAME, "%s", message);
	}

#else

	printf("%s\n", message);
	fflush(stdout);

#endif

	free(message);

	// Don't bail out, but keep going.
	return false;
}



}
