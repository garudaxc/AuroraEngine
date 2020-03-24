#include "stdHeader.h"
#include "Renderer.h"
#include "Console.h"
#include "Shader.h"
#include "Log.h"
#include "Util.h"
#include "RendererObject.h"
#include "Pipeline.h"
#include "RenderState.h"
#include "stb/stb_image.h"
#include "FileSystem.h"
#include "DDSTextureLoader.h"
#include "vulkan/vulkan.hpp"
#include "vulkan/vulkan_win32.h"

#pragma comment (lib, "vulkan-1.lib") 
#pragma comment (lib, "VkLayer_utils.lib") 



#ifndef NDEBUG
#define VERIFY(x) assert(x)
#else
#define VERIFY(x) ((void)(x))
#endif


#define FRAME_LAG 2

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

#ifdef _WIN32
#define ERR_EXIT(err_msg, err_class)                                          \
    do {   																	  \
		GLog->Error(err_msg);												  \
        if (!suppress_popups) MessageBox(nullptr, err_msg, err_class, MB_OK); \
        exit(1);                                                              \
    } while (0)
#else
#define ERR_EXIT(err_msg, err_class) \
    do {                             \
        printf("%s\n", err_msg);     \
        fflush(stdout);              \
        exit(1);                     \
    } while (0)
#endif


#define GET_INSTANCE_PROC_ADDR(inst, entrypoint)                                                              \
    {                                                                                                         \
        demo->fp##entrypoint = (PFN_vk##entrypoint)vkGetInstanceProcAddr(inst, "vk" #entrypoint);             \
        if (demo->fp##entrypoint == NULL) {                                                                   \
            ERR_EXIT("vkGetInstanceProcAddr failed to find vk" #entrypoint, "vkGetInstanceProcAddr Failure"); \
        }                                                                                                     \
    }

static PFN_vkGetDeviceProcAddr g_gdpa = NULL;

#define GET_DEVICE_PROC_ADDR(dev, entrypoint)                                                                    \
    {                                                                                                            \
        if (!g_gdpa) g_gdpa = (PFN_vkGetDeviceProcAddr)vkGetInstanceProcAddr(demo->inst, "vkGetDeviceProcAddr"); \
        demo->fp##entrypoint = (PFN_vk##entrypoint)g_gdpa(dev, "vk" #entrypoint);                                \
        if (demo->fp##entrypoint == NULL) {                                                                      \
            ERR_EXIT("vkGetDeviceProcAddr failed to find vk" #entrypoint, "vkGetDeviceProcAddr Failure");        \
        }                                                                                                        \
    }

namespace Aurora
{
	   

	// for debug
	enum RESOURCE_FLAG:int32
	{
		RESOURCE_FLAG_SHADER	= 0x01000000,
		RESOURCE_FLAG_BUFFER	= 0x02000000,
		RESOURCE_FLAG_TEXTURE	= 0x04000000,
		RESOURCE_FLAG_SAMPLER	= 0x08000000
	};

	enum SHADER_FLAG :int32
	{
		SHADER_FLAG_VERTEX		= 0x00010000,
		SHADER_FLAG_PIXEL		= 0x00020000,
		SHADER_FLAG_GEOMETRY	= 0x00040000,
	};

	class CRendererVulkan : public IRenderDevice
	{
	public:
		

		virtual Handle  CreateShader(const ShaderCode& code);

		virtual Texture* CreateTexture(File* file);

		virtual Texture* CreateTexture(Texture::Type type, const Texture::Desc& desc);

		virtual RenderTarget* CreateRenderTarget(const RenderTarget::Desc& desc);


		virtual void		Clear(int clearMask, const Color& clearColor = Color::BLUE, float fDepth = 1.0f, uint8 nStencil = 0);
		virtual void		BeginScene();
		virtual void		EndScene();
		virtual void		Present();

		RenderTarget* GetFrameBuffer();
		RenderTarget* GetDepthStecil();

		virtual void		SetVertexDesc(VertexLayout* pVertDesc);

		virtual void		SetVertexShader(Shader* pShader);
		virtual void		SetPixelShader(Shader* pShader);


		virtual void		SetRenderTarget(uint idx, RenderTarget* pRenderTarget);
		virtual void		SetDepthStencil(RenderTarget* pDepthStencil);

		virtual void		SetRenderTarget(uint nRTs, RenderTarget** pRenderTargets, RenderTarget* pDepthStencil);

		virtual void		ExecuteOperator(const RenderOperator& op);

		virtual void		OnReset();
		virtual void		OnLost();
		virtual void		GetFrameBufferSize(uint& nWidth, uint& nHeight);

		virtual void		Reset(int nWidth, int nHeight);
		virtual void		RestoreFrameBuffer();


	};
	


	extern HWND	MainHWnd;
	extern HINSTANCE ModuleInstance;
	static CRendererVulkan			RendererVulkan;
	IRenderDevice* GRenderDevice = &RendererVulkan;

	vk::Instance vkInst_;
	vk::PhysicalDevice gpu;
	vk::Device device;




	ResourceBufferMapper::ResourceBufferMapper(Handle handle)
	{
	}

	ResourceBufferMapper::~ResourceBufferMapper()
	{
	}

	void* ResourceBufferMapper::Ptr()
	{
		return nullptr;
	}



	PFN_vkCreateDebugUtilsMessengerEXT CreateDebugUtilsMessengerEXT;
	PFN_vkDestroyDebugUtilsMessengerEXT DestroyDebugUtilsMessengerEXT;
	PFN_vkSubmitDebugUtilsMessageEXT SubmitDebugUtilsMessageEXT;
	PFN_vkCmdBeginDebugUtilsLabelEXT CmdBeginDebugUtilsLabelEXT;
	PFN_vkCmdEndDebugUtilsLabelEXT CmdEndDebugUtilsLabelEXT;
	PFN_vkCmdInsertDebugUtilsLabelEXT CmdInsertDebugUtilsLabelEXT;
	PFN_vkSetDebugUtilsObjectNameEXT SetDebugUtilsObjectNameEXT;
	VkDebugUtilsMessengerEXT dbg_messenger;



	static inline const char* string_VkObjectType(VkObjectType input_value)
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



	struct texture_object {
		vk::Sampler sampler;

		vk::Image image;
		vk::Buffer buffer;
		vk::ImageLayout imageLayout{ vk::ImageLayout::eUndefined };

		vk::MemoryAllocateInfo mem_alloc;
		vk::DeviceMemory mem;
		vk::ImageView view;

		int32_t tex_width{ 0 };
		int32_t tex_height{ 0 };
	};


	static int validation_error = 0;

	struct vkcube_vs_uniform {
		// Must start with MVP
		float mvp[4][4];
		float position[12 * 3][4];
		float color[12 * 3][4];
	};



	typedef struct {
		vk::Image image;
		vk::CommandBuffer cmd;
		vk::CommandBuffer graphics_to_present_cmd;
		vk::ImageView view;
		vk::Buffer uniform_buffer;
		vk::DeviceMemory uniform_memory;
		void* uniform_memory_ptr;
		vk::Framebuffer framebuffer;
		vk::DescriptorSet descriptor_set;
	} SwapchainImageResources;



	vk::SurfaceKHR surface;
	bool prepared;
	bool use_staging_buffer;



	vk::Queue graphics_queue;
	vk::Queue present_queue;
	uint32_t graphics_queue_family_index;
	uint32_t present_queue_family_index;
	vk::Semaphore image_acquired_semaphores[FRAME_LAG];
	vk::Semaphore draw_complete_semaphores[FRAME_LAG];
	vk::Semaphore image_ownership_semaphores[FRAME_LAG];
	std::unique_ptr<vk::QueueFamilyProperties[]> queue_props;
	vk::PhysicalDeviceMemoryProperties memory_properties;


	vk::PhysicalDeviceProperties gpu_props;
	vk::PhysicalDeviceFeatures physDevFeatures;



	uint32_t width;
	uint32_t height;
	vk::Format format;
	vk::ColorSpaceKHR color_space;

	uint32_t swapchainImageCount;
	vk::SwapchainKHR swapchain;
	std::unique_ptr<SwapchainImageResources[]> swapchain_image_resources;
	vk::PresentModeKHR presentMode = vk::PresentModeKHR::eFifo;
	vk::Fence fences[FRAME_LAG];
	uint32_t frame_index;

	vk::CommandPool cmd_pool;
	vk::CommandPool present_cmd_pool;

	//vk::ShaderModule vert_shader_module;
	//vk::ShaderModule frag_shader_module;

	vk::DescriptorPool desc_pool;
	vk::DescriptorSet desc_set;


	bool quit;
	uint32_t curFrame;
	uint32_t frameCount;
	bool validate = true;
	bool use_break;
	bool suppress_popups;

	uint32_t current_buffer;
	uint32_t queue_family_count;




	struct {
		vk::Format format;
		vk::Image image;
		vk::MemoryAllocateInfo mem_alloc;
		vk::DeviceMemory mem;
		vk::ImageView view;
	} depth;

	static int32_t const texture_count = 1;
	texture_object textures[texture_count];
	texture_object staging_texture;

	struct {
		vk::Buffer buf;
		vk::MemoryAllocateInfo mem_alloc;
		vk::DeviceMemory mem;
		vk::DescriptorBufferInfo buffer_info;
	} uniform_data;

	vk::CommandBuffer cmd;  // Buffer for initialization commands
	vk::PipelineLayout pipeline_layout;
	vk::DescriptorSetLayout desc_layout;
	vk::PipelineCache pipelineCache;
	vk::RenderPass render_pass;
	vk::Pipeline pipeline;

	uint32_t enabled_extension_count;
	uint32_t enabled_layer_count;
	char const* extension_names[64];
	char const* enabled_layers[64];

	vk::Bool32 check_layers(uint32_t check_count, char const* const* const check_names, 
							uint32_t layer_count, vk::LayerProperties* layers) {
		for (uint32_t i = 0; i < check_count; i++) {
			vk::Bool32 found = VK_FALSE;
			for (uint32_t j = 0; j < layer_count; j++) {
				if (!strcmp(check_names[i], layers[j].layerName)) {
					found = VK_TRUE;
					break;
				}
			}
			if (!found) {
				GLog->Error("Cannot find layer: %s\n", check_names[i]);
				return 0;
			}
		}
		return VK_TRUE;
	}

	bool memory_type_from_properties(uint32_t typeBits, vk::MemoryPropertyFlags requirements_mask, uint32_t* typeIndex);



	void build_image_ownership_cmd(uint32_t const& i) {
		auto const cmd_buf_info = vk::CommandBufferBeginInfo().setFlags(vk::CommandBufferUsageFlagBits::eSimultaneousUse);
		auto result = swapchain_image_resources[i].graphics_to_present_cmd.begin(&cmd_buf_info);
		VERIFY(result == vk::Result::eSuccess);

		auto const image_ownership_barrier =
			vk::ImageMemoryBarrier()
			.setSrcAccessMask(vk::AccessFlags())
			.setDstAccessMask(vk::AccessFlags())
			.setOldLayout(vk::ImageLayout::ePresentSrcKHR)
			.setNewLayout(vk::ImageLayout::ePresentSrcKHR)
			.setSrcQueueFamilyIndex(graphics_queue_family_index)
			.setDstQueueFamilyIndex(present_queue_family_index)
			.setImage(swapchain_image_resources[i].image)
			.setSubresourceRange(vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));

		swapchain_image_resources[i].graphics_to_present_cmd.pipelineBarrier(
			vk::PipelineStageFlagBits::eBottomOfPipe, vk::PipelineStageFlagBits::eBottomOfPipe, vk::DependencyFlagBits(), 0, nullptr, 0,
			nullptr, 1, &image_ownership_barrier);

		swapchain_image_resources[i].graphics_to_present_cmd.end();
	}




	extern const char* ENGINE_NAME;

	void init_vk() {
		uint32_t instance_extension_count = 0;
		uint32_t instance_layer_count = 0;
		char const* const instance_validation_layers[] = { "VK_LAYER_KHRONOS_validation" };
		enabled_extension_count = 0;
		enabled_layer_count = 0;

		// Look for validation layers
		vk::Bool32 validation_found = VK_FALSE;
		if (validate) {
			auto result = vk::enumerateInstanceLayerProperties(&instance_layer_count, static_cast<vk::LayerProperties*>(nullptr));
			VERIFY(result == vk::Result::eSuccess);

			if (instance_layer_count > 0) {
				std::unique_ptr<vk::LayerProperties[]> instance_layers(new vk::LayerProperties[instance_layer_count]);
				result = vk::enumerateInstanceLayerProperties(&instance_layer_count, instance_layers.get());
				VERIFY(result == vk::Result::eSuccess);

				validation_found = check_layers(ARRAY_SIZE(instance_validation_layers), instance_validation_layers,
					instance_layer_count, instance_layers.get());
				if (validation_found) {
					enabled_layer_count = ARRAY_SIZE(instance_validation_layers);
					enabled_layers[0] = "VK_LAYER_KHRONOS_validation";
				}
			}

			if (!validation_found) {
				ERR_EXIT(
					"vkEnumerateInstanceLayerProperties failed to find required validation layer.\n\n"
					"Please look at the Getting Started guide for additional information.\n",
					"vkCreateInstance Failure");
			}
		}

		/* Look for instance extensions */
		vk::Bool32 surfaceExtFound = VK_FALSE;
		vk::Bool32 platformSurfaceExtFound = VK_FALSE;
		memset(extension_names, 0, sizeof(extension_names));

		auto result = vk::enumerateInstanceExtensionProperties(nullptr, &instance_extension_count,
			static_cast<vk::ExtensionProperties*>(nullptr));
		VERIFY(result == vk::Result::eSuccess);

		if (instance_extension_count > 0) {
			std::unique_ptr<vk::ExtensionProperties[]> instance_extensions(new vk::ExtensionProperties[instance_extension_count]);
			result = vk::enumerateInstanceExtensionProperties(nullptr, &instance_extension_count, instance_extensions.get());
			VERIFY(result == vk::Result::eSuccess);

			for (uint32_t i = 0; i < instance_extension_count; i++) {
				if (!strcmp(VK_KHR_SURFACE_EXTENSION_NAME, instance_extensions[i].extensionName)) {
					surfaceExtFound = 1;
					extension_names[enabled_extension_count++] = VK_KHR_SURFACE_EXTENSION_NAME;
				}
#if defined(VK_USE_PLATFORM_WIN32_KHR)
				if (!strcmp(VK_KHR_WIN32_SURFACE_EXTENSION_NAME, instance_extensions[i].extensionName)) {
					platformSurfaceExtFound = 1;
					extension_names[enabled_extension_count++] = VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
				}
#endif

				if (!strcmp(instance_extensions[i].extensionName, VK_EXT_DEBUG_UTILS_EXTENSION_NAME)) {
					extension_names[enabled_extension_count++] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
				}
				assert(enabled_extension_count < 64);
			}
		}

		if (!surfaceExtFound) {
			ERR_EXIT("vkEnumerateInstanceExtensionProperties failed to find the " VK_KHR_SURFACE_EXTENSION_NAME
				" extension.\n\n"
				"Do you have a compatible Vulkan installable client driver (ICD) installed?\n"
				"Please look at the Getting Started guide for additional information.\n",
				"vkCreateInstance Failure");
		}

		if (!platformSurfaceExtFound) {
#if defined(VK_USE_PLATFORM_WIN32_KHR)
			ERR_EXIT("vkEnumerateInstanceExtensionProperties failed to find the " VK_KHR_WIN32_SURFACE_EXTENSION_NAME
				" extension.\n\n"
				"Do you have a compatible Vulkan installable client driver (ICD) installed?\n"
				"Please look at the Getting Started guide for additional information.\n",
				"vkCreateInstance Failure");
#elif defined(VK_USE_PLATFORM_DISPLAY_KHR)
			ERR_EXIT("vkEnumerateInstanceExtensionProperties failed to find the " VK_KHR_DISPLAY_EXTENSION_NAME
				" extension.\n\n"
				"Do you have a compatible Vulkan installable client driver (ICD) installed?\n"
				"Please look at the Getting Started guide for additional information.\n",
				"vkCreateInstance Failure");
#endif
			
		}
		auto const app = vk::ApplicationInfo()
			.setPApplicationName(ENGINE_NAME)
			.setApplicationVersion(0)
			.setPEngineName(ENGINE_NAME)
			.setEngineVersion(0)
			.setApiVersion(VK_API_VERSION_1_1);
		auto const inst_info = vk::InstanceCreateInfo()
			.setPApplicationInfo(&app)
			.setEnabledLayerCount(enabled_layer_count)
			.setPpEnabledLayerNames(instance_validation_layers)
			.setEnabledExtensionCount(enabled_extension_count)
			.setPpEnabledExtensionNames(extension_names) ;


		result = vk::createInstance(&inst_info, nullptr, &vkInst_);
		if (result == vk::Result::eErrorIncompatibleDriver) {
			ERR_EXIT(
				"Cannot find a compatible Vulkan installable client driver (ICD).\n\n"
				"Please look at the Getting Started guide for additional information.\n",
				"vkCreateInstance Failure");
		}
		else if (result == vk::Result::eErrorExtensionNotPresent) {
			ERR_EXIT(
				"Cannot find a specified extension library.\n"
				"Make sure your layers path is set appropriately.\n",
				"vkCreateInstance Failure");
		}
		else if (result != vk::Result::eSuccess) {
			ERR_EXIT(
				"vkCreateInstance failed.\n\n"
				"Do you have a compatible Vulkan installable client driver (ICD) installed?\n"
				"Please look at the Getting Started guide for additional information.\n",
				"vkCreateInstance Failure");
		}

		/* Make initial call to query gpu_count, then second call for gpu info*/
		uint32_t gpu_count;
		result = vkInst_.enumeratePhysicalDevices(&gpu_count, static_cast<vk::PhysicalDevice*>(nullptr));
		VERIFY(result == vk::Result::eSuccess);

		if (gpu_count > 0) {
			std::unique_ptr<vk::PhysicalDevice[]> physical_devices(new vk::PhysicalDevice[gpu_count]);
			result = vkInst_.enumeratePhysicalDevices(&gpu_count, physical_devices.get());
			VERIFY(result == vk::Result::eSuccess);
			/* For cube demo we just grab the first physical device */
			gpu = physical_devices[0];
		}
		else {
			ERR_EXIT(
				"vkEnumeratePhysicalDevices reported zero accessible devices.\n\n"
				"Do you have a compatible Vulkan installable client driver (ICD) installed?\n"
				"Please look at the Getting Started guide for additional information.\n",
				"vkEnumeratePhysicalDevices Failure");
		}

		/* Look for device extensions */
		uint32_t device_extension_count = 0;
		vk::Bool32 swapchainExtFound = VK_FALSE;
		enabled_extension_count = 0;
		memset(extension_names, 0, sizeof(extension_names));

		result =
			gpu.enumerateDeviceExtensionProperties(nullptr, &device_extension_count, static_cast<vk::ExtensionProperties*>(nullptr));
		VERIFY(result == vk::Result::eSuccess);

		if (device_extension_count > 0) {
			std::unique_ptr<vk::ExtensionProperties[]> device_extensions(new vk::ExtensionProperties[device_extension_count]);
			result = gpu.enumerateDeviceExtensionProperties(nullptr, &device_extension_count, device_extensions.get());
			VERIFY(result == vk::Result::eSuccess);

			for (uint32_t i = 0; i < device_extension_count; i++) {
				if (!strcmp(VK_KHR_SWAPCHAIN_EXTENSION_NAME, device_extensions[i].extensionName)) {
					swapchainExtFound = 1;
					extension_names[enabled_extension_count++] = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
				}
				assert(enabled_extension_count < 64);
			}
		}

		if (!swapchainExtFound) {
			ERR_EXIT("vkEnumerateDeviceExtensionProperties failed to find the " VK_KHR_SWAPCHAIN_EXTENSION_NAME
				" extension.\n\n"
				"Do you have a compatible Vulkan installable client driver (ICD) installed?\n"
				"Please look at the Getting Started guide for additional information.\n",
				"vkCreateInstance Failure");
		}
			   

		if (validate) {

			VkDebugUtilsMessengerCreateInfoEXT dbg_messenger_create_info;
			// VK_EXT_debug_utils style
			dbg_messenger_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
			dbg_messenger_create_info.pNext = NULL;
			dbg_messenger_create_info.flags = 0;
			dbg_messenger_create_info.messageSeverity =
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
			dbg_messenger_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
			dbg_messenger_create_info.pfnUserCallback = debug_messenger_callback;
			dbg_messenger_create_info.pUserData = nullptr;
			

			// Setup VK_EXT_debug_utils function pointers always (we use them for
			// debug labels and names).
			CreateDebugUtilsMessengerEXT =
				(PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vkInst_, "vkCreateDebugUtilsMessengerEXT");
			//DestroyDebugUtilsMessengerEXT =
			//	(PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vkInst_, "vkDestroyDebugUtilsMessengerEXT");
			//SubmitDebugUtilsMessageEXT =
			//	(PFN_vkSubmitDebugUtilsMessageEXT)vkGetInstanceProcAddr(vkInst_, "vkSubmitDebugUtilsMessageEXT");
			//CmdBeginDebugUtilsLabelEXT =
			//	(PFN_vkCmdBeginDebugUtilsLabelEXT)vkGetInstanceProcAddr(vkInst_, "vkCmdBeginDebugUtilsLabelEXT");
			//CmdEndDebugUtilsLabelEXT =
			//	(PFN_vkCmdEndDebugUtilsLabelEXT)vkGetInstanceProcAddr(vkInst_, "vkCmdEndDebugUtilsLabelEXT");
			//CmdInsertDebugUtilsLabelEXT =
			//	(PFN_vkCmdInsertDebugUtilsLabelEXT)vkGetInstanceProcAddr(vkInst_, "vkCmdInsertDebugUtilsLabelEXT");
			//SetDebugUtilsObjectNameEXT =
			//	(PFN_vkSetDebugUtilsObjectNameEXT)vkGetInstanceProcAddr(vkInst_, "vkSetDebugUtilsObjectNameEXT");
			//if (NULL == CreateDebugUtilsMessengerEXT || NULL == DestroyDebugUtilsMessengerEXT ||
			//	NULL == SubmitDebugUtilsMessageEXT || NULL == CmdBeginDebugUtilsLabelEXT ||
			//	NULL == CmdEndDebugUtilsLabelEXT || NULL == CmdInsertDebugUtilsLabelEXT ||
			//	NULL == SetDebugUtilsObjectNameEXT) {
			//	ERR_EXIT("GetProcAddr: Failed to init VK_EXT_debug_utils\n", "GetProcAddr: Failure");
			//}

			if (CreateDebugUtilsMessengerEXT == nullptr) {
				ERR_EXIT("GetProcAddr: Failed to init VK_EXT_debug_utils\n", "GetProcAddr: Failure");
			}

			auto vkResult = CreateDebugUtilsMessengerEXT(vkInst_, &dbg_messenger_create_info, NULL, &dbg_messenger);
			VERIFY(vkResult == VK_SUCCESS);
		}

		gpu.getProperties(&gpu_props);

		/* Call with nullptr data to get count */
		gpu.getQueueFamilyProperties(&queue_family_count, static_cast<vk::QueueFamilyProperties*>(nullptr));
		assert(queue_family_count >= 1);

		queue_props.reset(new vk::QueueFamilyProperties[queue_family_count]);
		gpu.getQueueFamilyProperties(&queue_family_count, queue_props.get());

		// Query fine-grained feature support for this device.
		//  If app has specific feature requirements it should check supported
		//  features based on this query
		gpu.getFeatures(&physDevFeatures);
	}

	   
	void init_vk_swapchain() {

		// Create a WSI surface for the window:
#if defined(VK_USE_PLATFORM_WIN32_KHR)
		{
			auto const createInfo = vk::Win32SurfaceCreateInfoKHR().setHinstance(ModuleInstance).setHwnd(MainHWnd);

			auto result = vkInst_.createWin32SurfaceKHR(&createInfo, nullptr, &surface);
			VERIFY(result == vk::Result::eSuccess);
		}
#endif


		// Iterate over each queue to learn whether it supports presenting:
		std::unique_ptr<vk::Bool32[]> supportsPresent(new vk::Bool32[queue_family_count]);
		for (uint32_t i = 0; i < queue_family_count; i++) {
			gpu.getSurfaceSupportKHR(i, surface, &supportsPresent[i]);
		}

		uint32_t graphicsQueueFamilyIndex = UINT32_MAX;
		uint32_t presentQueueFamilyIndex = UINT32_MAX;
		for (uint32_t i = 0; i < queue_family_count; i++) {
			if (queue_props[i].queueFlags & vk::QueueFlagBits::eGraphics) {
				if (graphicsQueueFamilyIndex == UINT32_MAX) {
					graphicsQueueFamilyIndex = i;
				}

				if (supportsPresent[i] == VK_TRUE) {
					graphicsQueueFamilyIndex = i;
					presentQueueFamilyIndex = i;
					break;
				}
			}
		}

		if (presentQueueFamilyIndex == UINT32_MAX) {
			// If didn't find a queue that supports both graphics and present,
			// then
			// find a separate present queue.
			for (uint32_t i = 0; i < queue_family_count; ++i) {
				if (supportsPresent[i] == VK_TRUE) {
					presentQueueFamilyIndex = i;
					break;
				}
			}
		}

		// Generate error if could not find both a graphics and a present queue
		if (graphicsQueueFamilyIndex == UINT32_MAX || presentQueueFamilyIndex == UINT32_MAX) {
			ERR_EXIT("Could not find both graphics and present queues\n", "Swapchain Initialization Failure");
		}

		graphics_queue_family_index = graphicsQueueFamilyIndex;
		present_queue_family_index = presentQueueFamilyIndex;
		VERIFY(graphics_queue_family_index == present_queue_family_index);


		float const priorities[1] = { 0.0 };

		vk::DeviceQueueCreateInfo queues[2];
		queues[0].setQueueFamilyIndex(graphics_queue_family_index);
		queues[0].setQueueCount(1);
		queues[0].setPQueuePriorities(priorities);

		auto deviceInfo = vk::DeviceCreateInfo()
			.setQueueCreateInfoCount(1)
			.setPQueueCreateInfos(queues)
			.setEnabledLayerCount(0)
			.setPpEnabledLayerNames(nullptr)
			.setEnabledExtensionCount(enabled_extension_count)
			.setPpEnabledExtensionNames((const char* const*)extension_names)
			.setPEnabledFeatures(nullptr);

		auto result = gpu.createDevice(&deviceInfo, nullptr, &device);
		VERIFY(result == vk::Result::eSuccess);

		device.getQueue(graphics_queue_family_index, 0, &graphics_queue);
		present_queue = graphics_queue;

		// Get the list of VkFormat's that are supported:
		uint32_t formatCount;
		result = gpu.getSurfaceFormatsKHR(surface, &formatCount, static_cast<vk::SurfaceFormatKHR*>(nullptr));
		VERIFY(result == vk::Result::eSuccess);

		std::unique_ptr<vk::SurfaceFormatKHR[]> surfFormats(new vk::SurfaceFormatKHR[formatCount]);
		result = gpu.getSurfaceFormatsKHR(surface, &formatCount, surfFormats.get());
		VERIFY(result == vk::Result::eSuccess);

		// If the format list includes just one entry of VK_FORMAT_UNDEFINED,
		// the surface has no preferred format.  Otherwise, at least one
		// supported format will be returned.
		if (formatCount == 1 && surfFormats[0].format == vk::Format::eUndefined) {
			format = vk::Format::eB8G8R8A8Unorm;
		}
		else {
			assert(formatCount >= 1);
			format = surfFormats[0].format;
		}
		color_space = surfFormats[0].colorSpace;

		quit = false;
		curFrame = 0;

		// Create semaphores to synchronize acquiring presentable buffers before
		// rendering and waiting for drawing to be complete before presenting
		auto const semaphoreCreateInfo = vk::SemaphoreCreateInfo();

		// Create fences that we can use to throttle if we get too far
		// ahead of the image presents
		auto const fence_ci = vk::FenceCreateInfo().setFlags(vk::FenceCreateFlagBits::eSignaled);
		for (uint32_t i = 0; i < FRAME_LAG; i++) {
			result = device.createFence(&fence_ci, nullptr, &fences[i]);
			VERIFY(result == vk::Result::eSuccess);

			result = device.createSemaphore(&semaphoreCreateInfo, nullptr, &image_acquired_semaphores[i]);
			VERIFY(result == vk::Result::eSuccess);

			result = device.createSemaphore(&semaphoreCreateInfo, nullptr, &draw_complete_semaphores[i]);
			VERIFY(result == vk::Result::eSuccess);

		}
		frame_index = 0;

		// Get Memory information and properties
		gpu.getMemoryProperties(&memory_properties);
		GLog->Info("vulkan memory properties type count %d heap count %d", 
			memory_properties.memoryTypeCount, memory_properties.memoryHeapCount);

	}


	void prepare_buffers();
	void prepare_depth();
	void prepare_textures();
	void prepare_cube_data_buffers();

	void prepare_descriptor_layout();
	void prepare_render_pass();
	void prepare_pipeline();

	void prepare_descriptor_pool();
	void prepare_descriptor_set();

	void prepare_framebuffers();



	void prepare_textures()
	{
	}


	void draw_build_cmd(vk::CommandBuffer commandBuffer) {
		auto const commandInfo = vk::CommandBufferBeginInfo().setFlags(vk::CommandBufferUsageFlagBits::eSimultaneousUse);

		vk::ClearValue const clearValues[2] = { vk::ClearColorValue(std::array<float, 4>({{0.2f, 0.2f, 0.2f, 0.2f}})),
											   vk::ClearDepthStencilValue(1.0f, 0u) };

		auto const passInfo = vk::RenderPassBeginInfo()
			.setRenderPass(render_pass)
			.setFramebuffer(swapchain_image_resources[current_buffer].framebuffer)
			.setRenderArea(vk::Rect2D(vk::Offset2D(0, 0), vk::Extent2D((uint32_t)width, (uint32_t)height)))
			.setClearValueCount(2)
			.setPClearValues(clearValues);

		auto result = commandBuffer.begin(&commandInfo);
		VERIFY(result == vk::Result::eSuccess);

		commandBuffer.beginRenderPass(&passInfo, vk::SubpassContents::eInline);
		commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
		commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipeline_layout, 0, 1,
			&swapchain_image_resources[current_buffer].descriptor_set, 0, nullptr);
		//float viewport_dimension;
		float viewport_x = 0.0f;
		float viewport_y = 0.0f;
		//if (width < height) {
		//	viewport_dimension = (float)width;
		//	viewport_y = (height - width) / 2.0f;
		//}
		//else {
		//	viewport_dimension = (float)height;
		//	viewport_x = (width - height) / 2.0f;
		//}
		auto const viewport = vk::Viewport()
			.setX(viewport_x)
			.setY(viewport_y)
			.setWidth((float)width)
			.setHeight((float)height)
			.setMinDepth((float)0.0f)
			.setMaxDepth((float)1.0f);
		commandBuffer.setViewport(0, 1, &viewport);

		vk::Rect2D const scissor(vk::Offset2D(0, 0), vk::Extent2D(width, height));
		commandBuffer.setScissor(0, 1, &scissor);
		//commandBuffer.draw(12 * 3, 1, 0, 0);
		commandBuffer.draw(3, 1, 0, 0);
		// Note that ending the renderpass changes the image's layout from
		// COLOR_ATTACHMENT_OPTIMAL to PRESENT_SRC_KHR
		commandBuffer.endRenderPass();


		result = commandBuffer.end();
		VERIFY(result == vk::Result::eSuccess);
	}

	void flush_init_cmd() {
		// TODO: hmm.
		// This function could get called twice if the texture uses a staging
		// buffer
		// In that case the second call should be ignored
		if (!cmd) {
			return;
		}

		auto result = cmd.end();
		VERIFY(result == vk::Result::eSuccess);

		auto const fenceInfo = vk::FenceCreateInfo();
		vk::Fence fence;
		result = device.createFence(&fenceInfo, nullptr, &fence);
		VERIFY(result == vk::Result::eSuccess);

		vk::CommandBuffer const commandBuffers[] = { cmd };
		auto const submitInfo = vk::SubmitInfo().setCommandBufferCount(1).setPCommandBuffers(commandBuffers);

		result = graphics_queue.submit(1, &submitInfo, fence);
		VERIFY(result == vk::Result::eSuccess);

		result = device.waitForFences(1, &fence, VK_TRUE, UINT64_MAX);
		VERIFY(result == vk::Result::eSuccess);

		device.freeCommandBuffers(cmd_pool, 1, commandBuffers);
		device.destroyFence(fence, nullptr);

		cmd = vk::CommandBuffer();
	}

	void prepare() {
		auto const cmd_pool_info = vk::CommandPoolCreateInfo().setQueueFamilyIndex(graphics_queue_family_index);
		auto result = device.createCommandPool(&cmd_pool_info, nullptr, &cmd_pool);
		VERIFY(result == vk::Result::eSuccess);

		auto const cmdInfo = vk::CommandBufferAllocateInfo()
			.setCommandPool(cmd_pool)
			.setLevel(vk::CommandBufferLevel::ePrimary)
			.setCommandBufferCount(1);

		result = device.allocateCommandBuffers(&cmdInfo, &cmd);
		VERIFY(result == vk::Result::eSuccess);

		auto const cmd_buf_info = vk::CommandBufferBeginInfo().setPInheritanceInfo(nullptr);

		result = cmd.begin(&cmd_buf_info);
		VERIFY(result == vk::Result::eSuccess);

		prepare_buffers();
		prepare_depth();
		prepare_textures();
		prepare_cube_data_buffers();

		prepare_descriptor_layout();
		prepare_render_pass();
		prepare_pipeline();

		for (uint32_t i = 0; i < swapchainImageCount; ++i) {
			result = device.allocateCommandBuffers(&cmdInfo, &swapchain_image_resources[i].cmd);
			VERIFY(result == vk::Result::eSuccess);
		}
		
		prepare_descriptor_pool();
		prepare_descriptor_set();

		prepare_framebuffers();

		for (uint32_t i = 0; i < swapchainImageCount; ++i) {
			current_buffer = i;
			draw_build_cmd(swapchain_image_resources[i].cmd);
		}

		/*
		 * Prepare functions above may generate pipeline commands
		 * that need to be flushed before beginning the render loop.
		 */
		flush_init_cmd();
		//if (staging_texture.buffer) {
		//	destroy_texture(&staging_texture);
		//}

		current_buffer = 0;
		prepared = true;
	}

	void prepare_buffers() {
		vk::SwapchainKHR oldSwapchain = swapchain;

		// Check the surface capabilities and formats
		vk::SurfaceCapabilitiesKHR surfCapabilities;
		auto result = gpu.getSurfaceCapabilitiesKHR(surface, &surfCapabilities);
		VERIFY(result == vk::Result::eSuccess);

		uint32_t presentModeCount;
		result = gpu.getSurfacePresentModesKHR(surface, &presentModeCount, static_cast<vk::PresentModeKHR*>(nullptr));
		VERIFY(result == vk::Result::eSuccess);

		std::unique_ptr<vk::PresentModeKHR[]> presentModes(new vk::PresentModeKHR[presentModeCount]);
		result = gpu.getSurfacePresentModesKHR(surface, &presentModeCount, presentModes.get());
		VERIFY(result == vk::Result::eSuccess);

		vk::Extent2D swapchainExtent;
		// width and height are either both -1, or both not -1.
		if (surfCapabilities.currentExtent.width == (uint32_t)-1) {
			// If the surface size is undefined, the size is set to
			// the size of the images requested.
			swapchainExtent.width = width;
			swapchainExtent.height = height;
		}
		else {
			// If the surface size is defined, the swap chain size must match
			swapchainExtent = surfCapabilities.currentExtent;
			width = surfCapabilities.currentExtent.width;
			height = surfCapabilities.currentExtent.height;
		}

		// The FIFO present mode is guaranteed by the spec to be supported
		// and to have no tearing.  It's a great default present mode to use.
		vk::PresentModeKHR swapchainPresentMode = vk::PresentModeKHR::eFifo;

		//  There are times when you may wish to use another present mode.  The
		//  following code shows how to select them, and the comments provide some
		//  reasons you may wish to use them.
		//
		// It should be noted that Vulkan 1.0 doesn't provide a method for
		// synchronizing rendering with the presentation engine's display.  There
		// is a method provided for throttling rendering with the display, but
		// there are some presentation engines for which this method will not work.
		// If an application doesn't throttle its rendering, and if it renders much
		// faster than the refresh rate of the display, this can waste power on
		// mobile devices.  That is because power is being spent rendering images
		// that may never be seen.

		// VK_PRESENT_MODE_IMMEDIATE_KHR is for applications that don't care
		// about
		// tearing, or have some way of synchronizing their rendering with the
		// display.
		// VK_PRESENT_MODE_MAILBOX_KHR may be useful for applications that
		// generally render a new presentable image every refresh cycle, but are
		// occasionally early.  In this case, the application wants the new
		// image
		// to be displayed instead of the previously-queued-for-presentation
		// image
		// that has not yet been displayed.
		// VK_PRESENT_MODE_FIFO_RELAXED_KHR is for applications that generally
		// render a new presentable image every refresh cycle, but are
		// occasionally
		// late.  In this case (perhaps because of stuttering/latency concerns),
		// the application wants the late image to be immediately displayed,
		// even
		// though that may mean some tearing.

		if (presentMode != swapchainPresentMode) {
			for (size_t i = 0; i < presentModeCount; ++i) {
				if (presentModes[i] == presentMode) {
					swapchainPresentMode = presentMode;
					break;
				}
			}
		}

		if (swapchainPresentMode != presentMode) {
			ERR_EXIT("Present mode specified is not supported\n", "Present mode unsupported");
		}

		// Determine the number of VkImages to use in the swap chain.
		// Application desires to acquire 3 images at a time for triple
		// buffering
		uint32_t desiredNumOfSwapchainImages = 3;
		if (desiredNumOfSwapchainImages < surfCapabilities.minImageCount) {
			desiredNumOfSwapchainImages = surfCapabilities.minImageCount;
		}

		// If maxImageCount is 0, we can ask for as many images as we want,
		// otherwise
		// we're limited to maxImageCount
		if ((surfCapabilities.maxImageCount > 0) && (desiredNumOfSwapchainImages > surfCapabilities.maxImageCount)) {
			// Application must settle for fewer images than desired:
			desiredNumOfSwapchainImages = surfCapabilities.maxImageCount;
		}

		vk::SurfaceTransformFlagBitsKHR preTransform;
		if (surfCapabilities.supportedTransforms & vk::SurfaceTransformFlagBitsKHR::eIdentity) {
			preTransform = vk::SurfaceTransformFlagBitsKHR::eIdentity;
		}
		else {
			preTransform = surfCapabilities.currentTransform;
		}

		// Find a supported composite alpha mode - one of these is guaranteed to be set
		vk::CompositeAlphaFlagBitsKHR compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
		vk::CompositeAlphaFlagBitsKHR compositeAlphaFlags[4] = {
			vk::CompositeAlphaFlagBitsKHR::eOpaque,
			vk::CompositeAlphaFlagBitsKHR::ePreMultiplied,
			vk::CompositeAlphaFlagBitsKHR::ePostMultiplied,
			vk::CompositeAlphaFlagBitsKHR::eInherit,
		};
		for (uint32_t i = 0; i < ARRAY_SIZE(compositeAlphaFlags); i++) {
			if (surfCapabilities.supportedCompositeAlpha & compositeAlphaFlags[i]) {
				compositeAlpha = compositeAlphaFlags[i];
				break;
			}
		}

		auto const swapchain_ci = vk::SwapchainCreateInfoKHR()
			.setSurface(surface)
			.setMinImageCount(desiredNumOfSwapchainImages)
			.setImageFormat(format)
			.setImageColorSpace(color_space)
			.setImageExtent({ swapchainExtent.width, swapchainExtent.height })
			.setImageArrayLayers(1)
			.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
			.setImageSharingMode(vk::SharingMode::eExclusive)
			.setQueueFamilyIndexCount(0)
			.setPQueueFamilyIndices(nullptr)
			.setPreTransform(preTransform)
			.setCompositeAlpha(compositeAlpha)
			.setPresentMode(swapchainPresentMode)
			.setClipped(true)
			.setOldSwapchain(oldSwapchain);

		result = device.createSwapchainKHR(&swapchain_ci, nullptr, &swapchain);
		VERIFY(result == vk::Result::eSuccess);

		// If we just re-created an existing swapchain, we should destroy the
		// old
		// swapchain at this point.
		// Note: destroying the swapchain also cleans up all its associated
		// presentable images once the platform is done with them.
		if (oldSwapchain) {
			device.destroySwapchainKHR(oldSwapchain, nullptr);
		}

		result = device.getSwapchainImagesKHR(swapchain, &swapchainImageCount, static_cast<vk::Image*>(nullptr));
		VERIFY(result == vk::Result::eSuccess);

		std::unique_ptr<vk::Image[]> swapchainImages(new vk::Image[swapchainImageCount]);
		result = device.getSwapchainImagesKHR(swapchain, &swapchainImageCount, swapchainImages.get());
		VERIFY(result == vk::Result::eSuccess);

		swapchain_image_resources.reset(new SwapchainImageResources[swapchainImageCount]);

		for (uint32_t i = 0; i < swapchainImageCount; ++i) {
			auto color_image_view = vk::ImageViewCreateInfo()
				.setViewType(vk::ImageViewType::e2D)
				.setFormat(format)
				.setSubresourceRange(vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));

			swapchain_image_resources[i].image = swapchainImages[i];

			color_image_view.image = swapchain_image_resources[i].image;

			result = device.createImageView(&color_image_view, nullptr, &swapchain_image_resources[i].view);
			VERIFY(result == vk::Result::eSuccess);
		}
	}






	static const float g_vertex_buffer_data[] = {
		-1.0f,-1.0f,-1.0f,  // -X side
		-1.0f,-1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,

		-1.0f,-1.0f,-1.0f,  // -Z side
		 1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,
		 1.0f, 1.0f,-1.0f,

		-1.0f,-1.0f,-1.0f,  // -Y side
		 1.0f,-1.0f,-1.0f,
		 1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		 1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,

		-1.0f, 1.0f,-1.0f,  // +Y side
		-1.0f, 1.0f, 1.0f,
		 1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		 1.0f, 1.0f, 1.0f,
		 1.0f, 1.0f,-1.0f,

		 1.0f, 1.0f,-1.0f,  // +X side
		 1.0f, 1.0f, 1.0f,
		 1.0f,-1.0f, 1.0f,
		 1.0f,-1.0f, 1.0f,
		 1.0f,-1.0f,-1.0f,
		 1.0f, 1.0f,-1.0f,

		-1.0f, 1.0f, 1.0f,  // +Z side
		-1.0f,-1.0f, 1.0f,
		 1.0f, 1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		 1.0f,-1.0f, 1.0f,
		 1.0f, 1.0f, 1.0f,
	};

	static const float g_uv_buffer_data[] = {
		0.0f, 1.0f,  // -X side
		1.0f, 1.0f,
		1.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 1.0f,

		1.0f, 1.0f,  // -Z side
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,

		1.0f, 0.0f,  // -Y side
		1.0f, 1.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,

		1.0f, 0.0f,  // +Y side
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,

		1.0f, 0.0f,  // +X side
		0.0f, 0.0f,
		0.0f, 1.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,

		0.0f, 0.0f,  // +Z side
		0.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
	};
	// clang-format on


	Matrix4f matMVP;

	void VulkanSetMatrixMVP(const Matrix4f& m)
	{
		matMVP = m;
	}


	void prepare_cube_data_buffers() {
		//mat4x4 VP;
		//mat4x4_mul(VP, projection_matrix, view_matrix);

		//mat4x4 MVP;
		//mat4x4_mul(MVP, VP, model_matrix);

		vkcube_vs_uniform data;
		memcpy(data.mvp, matMVP.Ptr(), sizeof(matMVP));
		//    dumpMatrix("MVP", MVP)

		for (int32_t i = 0; i < 12 * 3; i++) {
			data.position[i][0] = g_vertex_buffer_data[i * 3];
			data.position[i][1] = g_vertex_buffer_data[i * 3 + 1];
			data.position[i][2] = g_vertex_buffer_data[i * 3 + 2];
			data.position[i][3] = 1.0f;
			data.color[i][0] = g_uv_buffer_data[2 * i];
			data.color[i][1] = g_uv_buffer_data[2 * i + 1];
			data.color[i][2] = 0;
			data.color[i][3] = 0;
		}

		auto const buf_info = vk::BufferCreateInfo().setSize(sizeof(data)).setUsage(vk::BufferUsageFlagBits::eUniformBuffer);

		for (unsigned int i = 0; i < swapchainImageCount; i++) {
			auto result = device.createBuffer(&buf_info, nullptr, &swapchain_image_resources[i].uniform_buffer);
			VERIFY(result == vk::Result::eSuccess);

			vk::MemoryRequirements mem_reqs;
			device.getBufferMemoryRequirements(swapchain_image_resources[i].uniform_buffer, &mem_reqs);

			auto mem_alloc = vk::MemoryAllocateInfo().setAllocationSize(mem_reqs.size).setMemoryTypeIndex(0);

			bool const pass = memory_type_from_properties(
				mem_reqs.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
				&mem_alloc.memoryTypeIndex);
			VERIFY(pass);

			result = device.allocateMemory(&mem_alloc, nullptr, &swapchain_image_resources[i].uniform_memory);
			VERIFY(result == vk::Result::eSuccess);

			result = device.mapMemory(swapchain_image_resources[i].uniform_memory, 0, VK_WHOLE_SIZE, vk::MemoryMapFlags(),
				&swapchain_image_resources[i].uniform_memory_ptr);
			VERIFY(result == vk::Result::eSuccess);

			memcpy(swapchain_image_resources[i].uniform_memory_ptr, &data, sizeof(data));

			result =
				device.bindBufferMemory(swapchain_image_resources[i].uniform_buffer, swapchain_image_resources[i].uniform_memory, 0);
			VERIFY(result == vk::Result::eSuccess);
		}
	}

	bool memory_type_from_properties(uint32_t typeBits, vk::MemoryPropertyFlags requirements_mask, uint32_t* typeIndex) {
		// Search memtypes to find first index with those properties
		for (uint32_t i = 0; i < VK_MAX_MEMORY_TYPES; i++) {
			if ((typeBits & 1) == 1) {
				// Type is available, does it match user properties?
				if ((memory_properties.memoryTypes[i].propertyFlags & requirements_mask) == requirements_mask) {
					*typeIndex = i;
					return true;
				}
			}
			typeBits >>= 1;
		}

		// No memory types matched, return failure
		return false;
	}

	void prepare_depth() {
		depth.format = vk::Format::eD24UnormS8Uint;

		auto const image = vk::ImageCreateInfo()
			.setImageType(vk::ImageType::e2D)
			.setFormat(depth.format)
			.setExtent({ (uint32_t)width, (uint32_t)height, 1 })
			.setMipLevels(1)
			.setArrayLayers(1)
			.setSamples(vk::SampleCountFlagBits::e1)
			.setTiling(vk::ImageTiling::eOptimal)
			.setUsage(vk::ImageUsageFlagBits::eDepthStencilAttachment)
			.setSharingMode(vk::SharingMode::eExclusive)
			.setQueueFamilyIndexCount(0)
			.setPQueueFamilyIndices(nullptr)
			.setInitialLayout(vk::ImageLayout::eUndefined);

		auto result = device.createImage(&image, nullptr, &depth.image);
		VERIFY(result == vk::Result::eSuccess);

		vk::MemoryRequirements mem_reqs;
		device.getImageMemoryRequirements(depth.image, &mem_reqs);

		depth.mem_alloc.setAllocationSize(mem_reqs.size);
		depth.mem_alloc.setMemoryTypeIndex(0);

		auto const pass = memory_type_from_properties(mem_reqs.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal,
			&depth.mem_alloc.memoryTypeIndex);
		VERIFY(pass);

		result = device.allocateMemory(&depth.mem_alloc, nullptr, &depth.mem);
		VERIFY(result == vk::Result::eSuccess);

		result = device.bindImageMemory(depth.image, depth.mem, 0);
		VERIFY(result == vk::Result::eSuccess);

		auto const view = vk::ImageViewCreateInfo()
			.setImage(depth.image)
			.setViewType(vk::ImageViewType::e2D)
			.setFormat(depth.format)
			.setSubresourceRange(vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eDepth, 0, 1, 0, 1));
		result = device.createImageView(&view, nullptr, &depth.view);
		VERIFY(result == vk::Result::eSuccess);
	}

	void prepare_descriptor_layout() {
		//vk::DescriptorSetLayoutBinding const layout_bindings[2] = { vk::DescriptorSetLayoutBinding()
		//															   .setBinding(0)
		//															   .setDescriptorType(vk::DescriptorType::eUniformBuffer)
		//															   .setDescriptorCount(1)
		//															   .setStageFlags(vk::ShaderStageFlagBits::eVertex)
		//															   .setPImmutableSamplers(nullptr),
		//														   vk::DescriptorSetLayoutBinding()
		//															   .setBinding(1)
		//															   .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
		//															   .setDescriptorCount(texture_count)
		//															   .setStageFlags(vk::ShaderStageFlagBits::eFragment)
		//															   .setPImmutableSamplers(nullptr) };

		vk::DescriptorSetLayoutBinding const layout_bindings[] = { vk::DescriptorSetLayoutBinding()
															   .setBinding(0)
															   .setDescriptorType(vk::DescriptorType::eUniformBuffer)
															   .setDescriptorCount(1)
															   .setStageFlags(vk::ShaderStageFlagBits::eVertex)
															   .setPImmutableSamplers(nullptr) };

		auto const descriptor_layout = vk::DescriptorSetLayoutCreateInfo().setBindingCount(1).setPBindings(layout_bindings);

		auto result = device.createDescriptorSetLayout(&descriptor_layout, nullptr, &desc_layout);
		VERIFY(result == vk::Result::eSuccess);

		auto const pPipelineLayoutCreateInfo = vk::PipelineLayoutCreateInfo().setSetLayoutCount(1).setPSetLayouts(&desc_layout);
		result = device.createPipelineLayout(&pPipelineLayoutCreateInfo, nullptr, &pipeline_layout);
		VERIFY(result == vk::Result::eSuccess);
	}

	void prepare_descriptor_pool() {
		//vk::DescriptorPoolSize const poolSizes[2] = {
		//	vk::DescriptorPoolSize().setType(vk::DescriptorType::eUniformBuffer).setDescriptorCount(swapchainImageCount),
		//	vk::DescriptorPoolSize()
		//		.setType(vk::DescriptorType::eCombinedImageSampler)
		//		.setDescriptorCount(swapchainImageCount * texture_count) };

		vk::DescriptorPoolSize const poolSizes[] = {
	vk::DescriptorPoolSize().setType(vk::DescriptorType::eUniformBuffer).setDescriptorCount(swapchainImageCount)};

		auto const descriptor_pool =
			vk::DescriptorPoolCreateInfo().setMaxSets(swapchainImageCount).setPoolSizeCount(1).setPPoolSizes(poolSizes);

		auto result = device.createDescriptorPool(&descriptor_pool, nullptr, &desc_pool);
		VERIFY(result == vk::Result::eSuccess);
	}



	void prepare_descriptor_set() {
		auto const alloc_info =
			vk::DescriptorSetAllocateInfo().setDescriptorPool(desc_pool).setDescriptorSetCount(1).setPSetLayouts(&desc_layout);

		auto buffer_info = vk::DescriptorBufferInfo().setOffset(0).setRange(sizeof(vkcube_vs_uniform));

		vk::DescriptorImageInfo tex_descs[texture_count];
		for (uint32_t i = 0; i < texture_count; i++) {
			tex_descs[i].setSampler(textures[i].sampler);
			tex_descs[i].setImageView(textures[i].view);
			tex_descs[i].setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal);
		}

		vk::WriteDescriptorSet writes[2];

		writes[0].setDescriptorCount(1);
		writes[0].setDescriptorType(vk::DescriptorType::eUniformBuffer);
		writes[0].setPBufferInfo(&buffer_info);

		//writes[1].setDstBinding(1);
		//writes[1].setDescriptorCount(texture_count);
		//writes[1].setDescriptorType(vk::DescriptorType::eCombinedImageSampler);
		//writes[1].setPImageInfo(tex_descs);

		for (unsigned int i = 0; i < swapchainImageCount; i++) {
			auto result = device.allocateDescriptorSets(&alloc_info, &swapchain_image_resources[i].descriptor_set);
			VERIFY(result == vk::Result::eSuccess);

			buffer_info.setBuffer(swapchain_image_resources[i].uniform_buffer);
			writes[0].setDstSet(swapchain_image_resources[i].descriptor_set);
			//writes[1].setDstSet(swapchain_image_resources[i].descriptor_set);
			device.updateDescriptorSets(1, writes, 0, nullptr);
		}
	}

	void prepare_framebuffers() {
		vk::ImageView attachments[2];
		attachments[1] = depth.view;

		auto const fb_info = vk::FramebufferCreateInfo()
			.setRenderPass(render_pass)
			.setAttachmentCount(2)
			.setPAttachments(attachments)
			.setWidth((uint32_t)width)
			.setHeight((uint32_t)height)
			.setLayers(1);

		for (uint32_t i = 0; i < swapchainImageCount; i++) {
			attachments[0] = swapchain_image_resources[i].view;
			auto const result = device.createFramebuffer(&fb_info, nullptr, &swapchain_image_resources[i].framebuffer);
			VERIFY(result == vk::Result::eSuccess);
		}
	}

	//vk::ShaderModule prepare_fs() {
	//	const uint32_t fragShaderCode[] = {
	//#include "cube.frag.inc"
	//	};

	//	frag_shader_module = prepare_shader_module(fragShaderCode, sizeof(fragShaderCode));

	//	return frag_shader_module;
	//}

	vk::ShaderModule prepare_shader_module(const uint32_t* code, size_t size);

	vk::ShaderModule CreateShaderModule(const string& pathName)
	{
		FilePtr file(GFileSys->OpenFile(pathName));
		if (file) {
			auto size = file->Size();
			unique_ptr<uint8[]> buffer(new uint8[size]);
			file->ReadArray(buffer.get(), size);

			vk::ShaderModule module = prepare_shader_module((uint32*)buffer.get(), size);
			return module;
		}

		return nullptr;		
	}



	void prepare_pipeline() {
		vk::PipelineCacheCreateInfo const pipelineCacheInfo;
		auto result = device.createPipelineCache(&pipelineCacheInfo, nullptr, &pipelineCache);
		VERIFY(result == vk::Result::eSuccess);

		auto vsModule = CreateShaderModule("../dev/CompiledShader/VulkanSample_vert.spv");
		auto psModule = CreateShaderModule("../dev/CompiledShader/VulkanSample_frag.spv");

		vk::PipelineShaderStageCreateInfo const shaderStageInfo[2] = {
			vk::PipelineShaderStageCreateInfo().setStage(vk::ShaderStageFlagBits::eVertex).setModule(vsModule).setPName("main"),
			vk::PipelineShaderStageCreateInfo().setStage(vk::ShaderStageFlagBits::eFragment).setModule(psModule).setPName("main") };

		vk::PipelineVertexInputStateCreateInfo const vertexInputInfo;

		auto const inputAssemblyInfo = vk::PipelineInputAssemblyStateCreateInfo().setTopology(vk::PrimitiveTopology::eTriangleList);

		// TODO: Where are pViewports and pScissors set?
		auto const viewportInfo = vk::PipelineViewportStateCreateInfo().setViewportCount(1).setScissorCount(1);

		auto const rasterizationInfo = vk::PipelineRasterizationStateCreateInfo()
			.setDepthClampEnable(VK_FALSE)
			.setRasterizerDiscardEnable(VK_FALSE)
			.setPolygonMode(vk::PolygonMode::eFill)
			.setCullMode(vk::CullModeFlagBits::eBack)
			.setFrontFace(vk::FrontFace::eCounterClockwise)
			.setDepthBiasEnable(VK_FALSE)
			.setLineWidth(1.0f);

		auto const multisampleInfo = vk::PipelineMultisampleStateCreateInfo();

		auto const stencilOp =
			vk::StencilOpState().setFailOp(vk::StencilOp::eKeep).setPassOp(vk::StencilOp::eKeep).setCompareOp(vk::CompareOp::eAlways);

		auto const depthStencilInfo = vk::PipelineDepthStencilStateCreateInfo()
			.setDepthTestEnable(VK_TRUE)
			.setDepthWriteEnable(VK_TRUE)
			.setDepthCompareOp(vk::CompareOp::eLessOrEqual)
			.setDepthBoundsTestEnable(VK_FALSE)
			.setStencilTestEnable(VK_FALSE)
			.setFront(stencilOp)
			.setBack(stencilOp);

		vk::PipelineColorBlendAttachmentState const colorBlendAttachments[1] = {
			vk::PipelineColorBlendAttachmentState().setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
																	  vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA) };

		auto const colorBlendInfo =
			vk::PipelineColorBlendStateCreateInfo().setAttachmentCount(1).setPAttachments(colorBlendAttachments);

		vk::DynamicState const dynamicStates[2] = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };

		auto const dynamicStateInfo = vk::PipelineDynamicStateCreateInfo().setPDynamicStates(dynamicStates).setDynamicStateCount(2);

		auto const pipelineInfo = vk::GraphicsPipelineCreateInfo()
			.setStageCount(2)
			.setPStages(shaderStageInfo)
			.setPVertexInputState(&vertexInputInfo)
			.setPInputAssemblyState(&inputAssemblyInfo)
			.setPViewportState(&viewportInfo)
			.setPRasterizationState(&rasterizationInfo)
			.setPMultisampleState(&multisampleInfo)
			.setPDepthStencilState(&depthStencilInfo)
			.setPColorBlendState(&colorBlendInfo)
			.setPDynamicState(&dynamicStateInfo)
			.setLayout(pipeline_layout)
			.setRenderPass(render_pass);

		result = device.createGraphicsPipelines(pipelineCache, 1, &pipelineInfo, nullptr, &pipeline);
		VERIFY(result == vk::Result::eSuccess);

		device.destroyShaderModule(vsModule, nullptr);
		device.destroyShaderModule(psModule, nullptr);
	}

	void prepare_render_pass() {
		// The initial layout for the color and depth attachments will be LAYOUT_UNDEFINED
		// because at the start of the renderpass, we don't care about their contents.
		// At the start of the subpass, the color attachment's layout will be transitioned
		// to LAYOUT_COLOR_ATTACHMENT_OPTIMAL and the depth stencil attachment's layout
		// will be transitioned to LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL.  At the end of
		// the renderpass, the color attachment's layout will be transitioned to
		// LAYOUT_PRESENT_SRC_KHR to be ready to present.  This is all done as part of
		// the renderpass, no barriers are necessary.
		const vk::AttachmentDescription attachments[2] = { vk::AttachmentDescription()
															  .setFormat(format)
															  .setSamples(vk::SampleCountFlagBits::e1)
															  .setLoadOp(vk::AttachmentLoadOp::eClear)
															  .setStoreOp(vk::AttachmentStoreOp::eStore)
															  .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
															  .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
															  .setInitialLayout(vk::ImageLayout::eUndefined)
															  .setFinalLayout(vk::ImageLayout::ePresentSrcKHR),
														  vk::AttachmentDescription()
															  .setFormat(depth.format)
															  .setSamples(vk::SampleCountFlagBits::e1)
															  .setLoadOp(vk::AttachmentLoadOp::eClear)
															  .setStoreOp(vk::AttachmentStoreOp::eDontCare)
															  .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
															  .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
															  .setInitialLayout(vk::ImageLayout::eUndefined)
															  .setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal) };

		auto const color_reference = vk::AttachmentReference().setAttachment(0).setLayout(vk::ImageLayout::eColorAttachmentOptimal);

		auto const depth_reference =
			vk::AttachmentReference().setAttachment(1).setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

		auto const subpass = vk::SubpassDescription()
			.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
			.setInputAttachmentCount(0)
			.setPInputAttachments(nullptr)
			.setColorAttachmentCount(1)
			.setPColorAttachments(&color_reference)
			.setPResolveAttachments(nullptr)
			.setPDepthStencilAttachment(&depth_reference)
			.setPreserveAttachmentCount(0)
			.setPPreserveAttachments(nullptr);

		vk::PipelineStageFlags stages = vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eLateFragmentTests;
		vk::SubpassDependency const dependencies[2] = {
			vk::SubpassDependency()  // Depth buffer is shared between swapchain images
				.setSrcSubpass(VK_SUBPASS_EXTERNAL)
				.setDstSubpass(0)
				.setSrcStageMask(stages)
				.setDstStageMask(stages)
				.setSrcAccessMask(vk::AccessFlagBits::eDepthStencilAttachmentWrite)
				.setDstAccessMask(vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite)
				.setDependencyFlags(vk::DependencyFlags()),
			vk::SubpassDependency()  // Image layout transition
				.setSrcSubpass(VK_SUBPASS_EXTERNAL)
				.setDstSubpass(0)
				.setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
				.setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
				.setSrcAccessMask(vk::AccessFlagBits())
				.setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eColorAttachmentRead)
				.setDependencyFlags(vk::DependencyFlags()),
		};

		auto const rp_info = vk::RenderPassCreateInfo()
			.setAttachmentCount(2)
			.setPAttachments(attachments)
			.setSubpassCount(1)
			.setPSubpasses(&subpass)
			.setDependencyCount(2)
			.setPDependencies(dependencies);

		auto result = device.createRenderPass(&rp_info, nullptr, &render_pass);
		VERIFY(result == vk::Result::eSuccess);
	}

	vk::ShaderModule prepare_shader_module(const uint32_t* code, size_t size) {
		const auto moduleCreateInfo = vk::ShaderModuleCreateInfo().setCodeSize(size).setPCode(code);

		vk::ShaderModule module;
		auto result = device.createShaderModule(&moduleCreateInfo, nullptr, &module);
		VERIFY(result == vk::Result::eSuccess);

		return module;
	}
	


	void draw() {
		// Ensure no more than FRAME_LAG renderings are outstanding
		device.waitForFences(1, &fences[frame_index], VK_TRUE, UINT64_MAX);
		device.resetFences(1, &fences[frame_index]);

		vk::Result result;
		do {
			result =
				device.acquireNextImageKHR(swapchain, UINT64_MAX, image_acquired_semaphores[frame_index], vk::Fence(), &current_buffer);
			if (result == vk::Result::eErrorOutOfDateKHR) {
				// demo->swapchain is out of date (e.g. the window was resized) and
				// must be recreated:
				//resize();

				GLog->Warning("acquireNextImageKHR eErrorOutOfDateKHR");
			}
			else if (result == vk::Result::eSuboptimalKHR) {
				// swapchain is not as optimal as it could be, but the platform's
				// presentation engine will still present the image correctly.
				break;
			}
			else if (result == vk::Result::eErrorSurfaceLostKHR) {
				//inst.destroySurfaceKHR(surface, nullptr);
				//create_surface();
				//resize();
				GLog->Warning("acquireNextImageKHR eErrorSurfaceLostKHR");
			}
			else {
				VERIFY(result == vk::Result::eSuccess);
			}
		} while (result != vk::Result::eSuccess);

		//update_data_buffer();

		// Wait for the image acquired semaphore to be signaled to ensure
		// that the image won't be rendered to until the presentation
		// engine has fully released ownership to the application, and it is
		// okay to render to the image.
		vk::PipelineStageFlags const pipe_stage_flags = vk::PipelineStageFlagBits::eColorAttachmentOutput;
		auto const submit_info = vk::SubmitInfo()
			.setPWaitDstStageMask(&pipe_stage_flags)
			.setWaitSemaphoreCount(1)
			.setPWaitSemaphores(&image_acquired_semaphores[frame_index])
			.setCommandBufferCount(1)
			.setPCommandBuffers(&swapchain_image_resources[current_buffer].cmd)
			.setSignalSemaphoreCount(1)
			.setPSignalSemaphores(&draw_complete_semaphores[frame_index]);

		result = graphics_queue.submit(1, &submit_info, fences[frame_index]);
		VERIFY(result == vk::Result::eSuccess);


		// If we are using separate queues we have to wait for image ownership,
		// otherwise wait for draw complete
		auto const presentInfo = vk::PresentInfoKHR()
			.setWaitSemaphoreCount(1)
			.setPWaitSemaphores(&draw_complete_semaphores[frame_index])
			.setSwapchainCount(1)
			.setPSwapchains(&swapchain)
			.setPImageIndices(&current_buffer);

		result = present_queue.presentKHR(&presentInfo);
		frame_index += 1;
		frame_index %= FRAME_LAG;
		if (result == vk::Result::eErrorOutOfDateKHR) {
			// swapchain is out of date (e.g. the window was resized) and
			// must be recreated:
			//resize();
			GLog->Warning("acquireNextImageKHR eErrorOutOfDateKHR");
		}
		else if (result == vk::Result::eSuboptimalKHR) {
			// swapchain is not as optimal as it could be, but the platform's
			// presentation engine will still present the image correctly.
		}
		else if (result == vk::Result::eErrorSurfaceLostKHR) {
			//inst.destroySurfaceKHR(surface, nullptr);
			//create_surface();
			//resize();
			GLog->Warning("acquireNextImageKHR eErrorSurfaceLostKHR");
		}
		else {
			VERIFY(result == vk::Result::eSuccess);
		}
	}



	IRenderDevice* IRenderDevice::CreateDevice(int nWidth, int nHeight)
	{
		width = nWidth;
		height = nHeight;
		init_vk();
		init_vk_swapchain();
		prepare();
		return &RendererVulkan;
	}



	Handle CRendererVulkan::CreateShader(const ShaderCode& code)
	{
		return 0;
	}



	void CreateGlobalParameterBuffer()
	{
	}



	Handle CreateShaderParameterBinding(Handle shaderHandle, const ShaderParamterBindings& bindings)
	{
		return 0;
	}



	Handle CreateShaderTextureBinding(Handle shaderHandle, const ShaderTextureBinding& bindings)
	{
		return 0;
	}


	void UpdateShaderParameter(Handle bindingHandle)
	{
	}

	void BindVertexShaderParameter(Handle bindingHandle)
	{
	}

	void BindPixelShaderParameter(Handle bindingHandle)
	{
	}


	void BindGlobalParameter(Handle handle)
	{
	}

	void BindVertexShader(Handle shaderHandle)
	{
	}

	void BindPixelShader(Handle shaderHandle)
	{
	}


	Texture* CRendererVulkan::CreateTexture(File* file)
	{
		return nullptr;
	}




	void BindTexture(Handle binding, Texture* texture)
	{
	}


	Texture* CRendererVulkan::CreateTexture(Texture::Type type, const Texture::Desc& desc)
	{
		return NULL;
	}

	RenderTarget* CRendererVulkan::CreateRenderTarget(const RenderTarget::Desc& desc)
	{		
		return nullptr;
	}



	struct VulkanBufferInfo
	{
		int Occupied = 0;
		vk::Buffer	buffer;
		vk::DeviceMemory	memory;
	};
	

	vector<VulkanBufferInfo>	DeviceBuffers_;


	Handle CreateVertexBufferHandle(const void* data, int32 size)
	{
		vk::BufferCreateInfo createInfo;
		createInfo.setSize(size).setSharingMode(vk::SharingMode::eExclusive)
			.setUsage(vk::BufferUsageFlagBits::eVertexBuffer);

		auto res = device.createBuffer(createInfo);
		if (res.result != vk::Result::eSuccess) {
			GLog->Error("vulkan create buffer failed result :%d", res.result);
			return -1;
		}

		vk::MemoryRequirements mem_reqs;
		device.getBufferMemoryRequirements(res.value, &mem_reqs);

		uint32 memoryTypeIndex = 0;
		bool const pass = memory_type_from_properties(mem_reqs.memoryTypeBits, 
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
			&memoryTypeIndex);
		VERIFY(pass);

		auto mem_alloc = vk::MemoryAllocateInfo().
			setAllocationSize(mem_reqs.size).
			setMemoryTypeIndex(memoryTypeIndex);

		vk::DeviceMemory memory;

		auto result = device.allocateMemory(&mem_alloc, nullptr, &memory);
		VERIFY(result == vk::Result::eSuccess);

		if (data != nullptr) {
			void* pData = nullptr;
			result = device.mapMemory(memory, 0, VK_WHOLE_SIZE, vk::MemoryMapFlags(), &pData);
			VERIFY(result == vk::Result::eSuccess);

			memcpy(pData, data, size);
			device.unmapMemory(memory);
		}
		
		result = device.bindBufferMemory(res.value, memory, 0);
		VERIFY(result == vk::Result::eSuccess);

		VulkanBufferInfo deviceBuffer;
		deviceBuffer.buffer = res.value;
		deviceBuffer.memory = memory;
		deviceBuffer.Occupied = 1;
		
		int slot = FindAvailableSlot(DeviceBuffers_);
		DeviceBuffers_[slot] = deviceBuffer;

		return slot;
	}


	Handle CreateIndexBufferHandle(const void* data, int32 size)
	{
		return 0;
	}


	void CRendererVulkan::Clear(int clearMask, const Color& clearColor, float fDepth , uint8 nStencil)
	{
	}
	
	void CRendererVulkan::BeginScene()
	{

	}

	void CRendererVulkan::EndScene()
	{

	}

	void CRendererVulkan::Present()
	{
		draw();
	}

	RenderTarget* CRendererVulkan::GetFrameBuffer()
	{
		return nullptr;
	}

	RenderTarget* CRendererVulkan::GetDepthStecil()
	{
		return nullptr;
	}

	void CRendererVulkan::SetVertexDesc(VertexLayout* pVertDesc)
	{

	}

	void CRendererVulkan::SetVertexShader(Shader* pShader)
	{
	}

	void CRendererVulkan::SetPixelShader(Shader* pShader)
	{
	}


	void CRendererVulkan::SetRenderTarget(uint idx, RenderTarget* pRenderTarget)
	{

	}

	void CRendererVulkan::SetDepthStencil(RenderTarget* pDepthStencil)
	{

	}

	void CRendererVulkan::SetRenderTarget(uint nRTs, RenderTarget** pRenderTargets, RenderTarget* pDepthStencil)
	{
	}

	void CRendererVulkan::OnReset()
	{

	}

	void CRendererVulkan::OnLost()
	{

	}

	void CRendererVulkan::GetFrameBufferSize(uint& nWidth, uint& nHeight)
	{

	}

	void CRendererVulkan::Reset(int nWidth, int nHeight)
	{

	}

	void CRendererVulkan::RestoreFrameBuffer()
	{

	}

	


	vk::Format MapVulkanVertexElemType(Vertex::ElemType type)
	{
		switch (type)
		{
		case Aurora::Vertex::TYPE_FLOAT:
			return vk::Format::eR32Sfloat;
		case Aurora::Vertex::TYPE_FLOAT2:
			return vk::Format::eR32G32Sfloat;
		case Aurora::Vertex::TYPE_FLOAT3:
			return vk::Format::eR32G32B32Sfloat;
		case Aurora::Vertex::TYPE_FLOAT4:
			return vk::Format::eR32G32B32A32Sfloat;
		case Aurora::Vertex::TYPE_UBYTE4_UINT:
			return vk::Format::eR8G8B8A8Uint;
		case Aurora::Vertex::TYPE_UBYTE4_UNORM:
			return vk::Format::eR8G8B8A8Unorm;
		case Aurora::Vertex::TYPE_USHORT_UINT:
			return vk::Format::eR16Uint;
		case Aurora::Vertex::TYPE_UINT:
			return vk::Format::eR32Uint;
		default:

			assert(0);
			return vk::Format::eR32Sfloat;
		}
	}

	vk::PrimitiveTopology MapVulkanPrimitiveType(RenderOperator::EPrimitiveType type) 
	{
		switch (type)
		{
		case Aurora::RenderOperator::PT_POINTLIST:
			return vk::PrimitiveTopology::ePointList;
		case Aurora::RenderOperator::PT_LINELIST:
			return vk::PrimitiveTopology::eLineList;
		case Aurora::RenderOperator::PT_LINESTRIP:
			return vk::PrimitiveTopology::eLineStrip;
		case Aurora::RenderOperator::PT_TRIANGLELIST:
			return vk::PrimitiveTopology::eTriangleList;
		case Aurora::RenderOperator::PT_TRIANGLESTRIP:
			return vk::PrimitiveTopology::eTriangleStrip;
		default:
			assert(0);
			return vk::PrimitiveTopology::eTriangleList;
		}
	}




	struct VertexLayoutInfo
	{
		int Occupied = 0;
		int stride = 0;
		int binding = 0;
		vector< vk::VertexInputBindingDescription>	bindingDescs;
		vector< vk::VertexInputAttributeDescription> inputAttriDescs;
	};


	vector<VertexLayoutInfo>	VertexLayoutInfos_;


	

	Handle CreateVertexLayoutHandle(const vector<VertexLayoutItem>& layoutItems)
	{

		vector< vk::VertexInputAttributeDescription> inputAttriDescs;
		int loc = 0;
		int offset = 0;
		for (auto it = layoutItems.begin(); it != layoutItems.end(); ++it) {
			vk::VertexInputAttributeDescription  desc;
			desc.setBinding(0)
				.setLocation(loc++)
				.setOffset(offset)
				.setFormat(MapVulkanVertexElemType((Vertex::ElemType)it->type));

			inputAttriDescs.push_back(desc);
			offset += Geometry::GetSizeOfType((Vertex::ElemType)it->type);
		}

		vk::VertexInputBindingDescription bindingDescription;
		bindingDescription.setBinding(0).setInputRate(vk::VertexInputRate::eVertex).setStride(offset);

		VertexLayoutInfo info;
		info.binding = 0;
		info.stride = offset;
		info.bindingDescs.clear();
		info.bindingDescs.push_back(bindingDescription);
		info.inputAttriDescs = inputAttriDescs;
		info.Occupied = 1;

		int slot = FindAvailableSlot(VertexLayoutInfos_);
		VertexLayoutInfos_[slot] = info;

		return slot;
	}


	void CRendererVulkan::ExecuteOperator(const RenderOperator& op)
	{
	}


}