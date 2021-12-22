#pragma once

namespace Aurora
{

	extern PFN_vkCreateDebugUtilsMessengerEXT CreateDebugUtilsMessengerEXT;
	extern VkDebugUtilsMessengerEXT dbg_messenger;

	

	const char* string_VkObjectType(VkObjectType input_value);
	VKAPI_ATTR VkBool32 VKAPI_CALL debug_messenger_callback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData);

}