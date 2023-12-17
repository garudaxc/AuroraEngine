#include <windows.h>
#include <gl\gl.h>
#include "Shader.h"
#include "Log.h"

#include "Renderer.h"


#pragma comment(lib, "opengl32.lib")

namespace Aurora
{
#define WGL_DRAW_TO_WINDOW_ARB         0x2001
#define WGL_ACCELERATION_ARB           0x2003
#define WGL_SWAP_METHOD_ARB            0x2007
#define WGL_SUPPORT_OPENGL_ARB         0x2010
#define WGL_DOUBLE_BUFFER_ARB          0x2011
#define WGL_PIXEL_TYPE_ARB             0x2013
#define WGL_COLOR_BITS_ARB             0x2014
#define WGL_DEPTH_BITS_ARB             0x2022
#define WGL_STENCIL_BITS_ARB           0x2023
#define WGL_FULL_ACCELERATION_ARB      0x2027
#define WGL_SWAP_EXCHANGE_ARB          0x2028
#define WGL_TYPE_RGBA_ARB              0x202B
#define WGL_CONTEXT_MAJOR_VERSION_ARB  0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB  0x2092
#define GL_ARRAY_BUFFER                   0x8892
#define GL_STATIC_DRAW                    0x88E4
#define GL_FRAGMENT_SHADER                0x8B30
#define GL_VERTEX_SHADER                  0x8B31
#define GL_COMPILE_STATUS                 0x8B81
#define GL_LINK_STATUS                    0x8B82
#define GL_INFO_LOG_LENGTH                0x8B84
#define GL_TEXTURE0                       0x84C0
#define GL_BGRA                           0x80E1
#define GL_ELEMENT_ARRAY_BUFFER           0x8893


    typedef BOOL (WINAPI * PFNWGLCHOOSEPIXELFORMATARBPROC)(HDC hdc, const int* piAttribIList,
                                                           const FLOAT* pfAttribFList, UINT nMaxFormats,
                                                           int* piFormats, UINT* nNumFormats);
    typedef HGLRC (WINAPI * PFNWGLCREATECONTEXTATTRIBSARBPROC)(HDC hDC, HGLRC hShareContext, const int* attribList);
    typedef BOOL (WINAPI * PFNWGLSWAPINTERVALEXTPROC)(int interval);
    typedef void (APIENTRY * PFNGLATTACHSHADERPROC)(GLuint program, GLuint shader);
    typedef void (APIENTRY * PFNGLBINDBUFFERPROC)(GLenum target, GLuint buffer);
    typedef void (APIENTRY * PFNGLBINDVERTEXARRAYPROC)(GLuint array);
    typedef void (APIENTRY * PFNGLBUFFERDATAPROC)(GLenum target, ptrdiff_t size, const GLvoid* data, GLenum usage);
    typedef void (APIENTRY * PFNGLCOMPILESHADERPROC)(GLuint shader);
    typedef GLuint (APIENTRY * PFNGLCREATEPROGRAMPROC)(void);
    typedef GLuint (APIENTRY * PFNGLCREATESHADERPROC)(GLenum type);
    typedef void (APIENTRY * PFNGLDELETEBUFFERSPROC)(GLsizei n, const GLuint* buffers);
    typedef void (APIENTRY * PFNGLDELETEPROGRAMPROC)(GLuint program);
    typedef void (APIENTRY * PFNGLDELETESHADERPROC)(GLuint shader);
    typedef void (APIENTRY * PFNGLDELETEVERTEXARRAYSPROC)(GLsizei n, const GLuint* arrays);
    typedef void (APIENTRY * PFNGLDETACHSHADERPROC)(GLuint program, GLuint shader);
    typedef void (APIENTRY * PFNGLENABLEVERTEXATTRIBARRAYPROC)(GLuint index);
    typedef void (APIENTRY * PFNGLGENBUFFERSPROC)(GLsizei n, GLuint* buffers);
    typedef void (APIENTRY * PFNGLGENVERTEXARRAYSPROC)(GLsizei n, GLuint* arrays);
    typedef GLint (APIENTRY * PFNGLGETATTRIBLOCATIONPROC)(GLuint program, const char* name);
    typedef void (APIENTRY * PFNGLGETPROGRAMINFOLOGPROC)(GLuint program, GLsizei bufSize, GLsizei* length,
                                                         char* infoLog);
    typedef void (APIENTRY * PFNGLGETPROGRAMIVPROC)(GLuint program, GLenum pname, GLint* params);
    typedef void (APIENTRY * PFNGLGETSHADERINFOLOGPROC)(GLuint shader, GLsizei bufSize, GLsizei* length, char* infoLog);
    typedef void (APIENTRY * PFNGLGETSHADERIVPROC)(GLuint shader, GLenum pname, GLint* params);
    typedef void (APIENTRY * PFNGLLINKPROGRAMPROC)(GLuint program);
    typedef void (APIENTRY * PFNGLSHADERSOURCEPROC)(GLuint shader, GLsizei count, const char* * string,
                                                    const GLint* length);
    typedef void (APIENTRY * PFNGLUSEPROGRAMPROC)(GLuint program);
    typedef void (APIENTRY * PFNGLVERTEXATTRIBPOINTERPROC)(GLuint index, GLint size, GLenum type, GLboolean normalized,
                                                           GLsizei stride,
                                                           const GLvoid* pointer);
    typedef void (APIENTRY * PFNGLBINDATTRIBLOCATIONPROC)(GLuint program, GLuint index, const char* name);
    typedef GLint (APIENTRY * PFNGLGETUNIFORMLOCATIONPROC)(GLuint program, const char* name);
    typedef void (APIENTRY * PFNGLUNIFORMMATRIX4FVPROC)(GLint location, GLsizei count, GLboolean transpose,
                                                        const GLfloat* value);
    typedef void (APIENTRY * PFNGLACTIVETEXTUREPROC)(GLenum texture);
    typedef void (APIENTRY * PFNGLUNIFORM1IPROC)(GLint location, GLint v0);
    typedef void (APIENTRY * PFNGLGENERATEMIPMAPPROC)(GLenum target);
    typedef void (APIENTRY * PFNGLDISABLEVERTEXATTRIBARRAYPROC)(GLuint index);
    typedef void (APIENTRY * PFNGLUNIFORM3FVPROC)(GLint location, GLsizei count, const GLfloat* value);
    typedef void (APIENTRY * PFNGLUNIFORM4FVPROC)(GLint location, GLsizei count, const GLfloat* value);

    PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
    PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
    PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;


    PFNGLATTACHSHADERPROC glAttachShader;
    PFNGLBINDBUFFERPROC glBindBuffer;
    PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
    PFNGLBUFFERDATAPROC glBufferData;
    PFNGLCOMPILESHADERPROC glCompileShader;
    PFNGLCREATEPROGRAMPROC glCreateProgram;
    PFNGLCREATESHADERPROC glCreateShader;
    PFNGLDELETEBUFFERSPROC glDeleteBuffers;
    PFNGLDELETEPROGRAMPROC glDeleteProgram;
    PFNGLDELETESHADERPROC glDeleteShader;
    PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
    PFNGLDETACHSHADERPROC glDetachShader;
    PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
    PFNGLGENBUFFERSPROC glGenBuffers;
    PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
    PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation;
    PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
    PFNGLGETPROGRAMIVPROC glGetProgramiv;
    PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
    PFNGLGETSHADERIVPROC glGetShaderiv;
    PFNGLLINKPROGRAMPROC glLinkProgram;
    PFNGLSHADERSOURCEPROC glShaderSource;
    PFNGLUSEPROGRAMPROC glUseProgram;
    PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
    PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocation;
    PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
    PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
    PFNGLACTIVETEXTUREPROC glActiveTexture;
    PFNGLUNIFORM1IPROC glUniform1i;
    PFNGLGENERATEMIPMAPPROC glGenerateMipmap;
    PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
    PFNGLUNIFORM3FVPROC glUniform3fv;
    PFNGLUNIFORM4FVPROC glUniform4fv;


    HDC m_deviceContext;
    HGLRC m_renderingContext;


    bool LoadExtensionList()
    {
        // Load the OpenGL extensions that this application will be using.
        wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
        if (!wglChoosePixelFormatARB)
        {
            return false;
        }

        wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
        if (!wglCreateContextAttribsARB)
        {
            return false;
        }

        wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
        if (!wglSwapIntervalEXT)
        {
            return false;
        }

        glAttachShader = (PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader");
        if (!glAttachShader)
        {
            return false;
        }

        glBindBuffer = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
        if (!glBindBuffer)
        {
            return false;
        }

        glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)wglGetProcAddress("glBindVertexArray");
        if (!glBindVertexArray)
        {
            return false;
        }

        glBufferData = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
        if (!glBufferData)
        {
            return false;
        }

        glCompileShader = (PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader");
        if (!glCompileShader)
        {
            return false;
        }

        glCreateProgram = (PFNGLCREATEPROGRAMPROC)wglGetProcAddress("glCreateProgram");
        if (!glCreateProgram)
        {
            return false;
        }

        glCreateShader = (PFNGLCREATESHADERPROC)wglGetProcAddress("glCreateShader");
        if (!glCreateShader)
        {
            return false;
        }

        glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)wglGetProcAddress("glDeleteBuffers");
        if (!glDeleteBuffers)
        {
            return false;
        }

        glDeleteProgram = (PFNGLDELETEPROGRAMPROC)wglGetProcAddress("glDeleteProgram");
        if (!glDeleteProgram)
        {
            return false;
        }

        glDeleteShader = (PFNGLDELETESHADERPROC)wglGetProcAddress("glDeleteShader");
        if (!glDeleteShader)
        {
            return false;
        }

        glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)wglGetProcAddress("glDeleteVertexArrays");
        if (!glDeleteVertexArrays)
        {
            return false;
        }

        glDetachShader = (PFNGLDETACHSHADERPROC)wglGetProcAddress("glDetachShader");
        if (!glDetachShader)
        {
            return false;
        }

        glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glEnableVertexAttribArray");
        if (!glEnableVertexAttribArray)
        {
            return false;
        }

        glGenBuffers = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers");
        if (!glGenBuffers)
        {
            return false;
        }

        glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)wglGetProcAddress("glGenVertexArrays");
        if (!glGenVertexArrays)
        {
            return false;
        }

        glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC)wglGetProcAddress("glGetAttribLocation");
        if (!glGetAttribLocation)
        {
            return false;
        }

        glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)wglGetProcAddress("glGetProgramInfoLog");
        if (!glGetProgramInfoLog)
        {
            return false;
        }

        glGetProgramiv = (PFNGLGETPROGRAMIVPROC)wglGetProcAddress("glGetProgramiv");
        if (!glGetProgramiv)
        {
            return false;
        }

        glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)wglGetProcAddress("glGetShaderInfoLog");
        if (!glGetShaderInfoLog)
        {
            return false;
        }

        glGetShaderiv = (PFNGLGETSHADERIVPROC)wglGetProcAddress("glGetShaderiv");
        if (!glGetShaderiv)
        {
            return false;
        }

        glLinkProgram = (PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram");
        if (!glLinkProgram)
        {
            return false;
        }

        glShaderSource = (PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource");
        if (!glShaderSource)
        {
            return false;
        }

        glUseProgram = (PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram");
        if (!glUseProgram)
        {
            return false;
        }

        glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)wglGetProcAddress("glVertexAttribPointer");
        if (!glVertexAttribPointer)
        {
            return false;
        }

        glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC)wglGetProcAddress("glBindAttribLocation");
        if (!glBindAttribLocation)
        {
            return false;
        }

        glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation");
        if (!glGetUniformLocation)
        {
            return false;
        }

        glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)wglGetProcAddress("glUniformMatrix4fv");
        if (!glUniformMatrix4fv)
        {
            return false;
        }

        glActiveTexture = (PFNGLACTIVETEXTUREPROC)wglGetProcAddress("glActiveTexture");
        if (!glActiveTexture)
        {
            return false;
        }

        glUniform1i = (PFNGLUNIFORM1IPROC)wglGetProcAddress("glUniform1i");
        if (!glUniform1i)
        {
            return false;
        }

        glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)wglGetProcAddress("glGenerateMipmap");
        if (!glGenerateMipmap)
        {
            return false;
        }

        glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glDisableVertexAttribArray");
        if (!glDisableVertexAttribArray)
        {
            return false;
        }

        glUniform3fv = (PFNGLUNIFORM3FVPROC)wglGetProcAddress("glUniform3fv");
        if (!glUniform3fv)
        {
            return false;
        }

        glUniform4fv = (PFNGLUNIFORM4FVPROC)wglGetProcAddress("glUniform4fv");
        if (!glUniform4fv)
        {
            return false;
        }


        return true;
    }


    bool InitializeGLExtensions(HWND hwnd)
    {
        HDC deviceContext;
        PIXELFORMATDESCRIPTOR pixelFormat;
        int error;
        HGLRC renderContext;
        bool result;


        // Get the device context for this window.
        deviceContext = GetDC(hwnd);
        if (!deviceContext)
        {
            return false;
        }

        // Set a temporary default pixel format.
        error = SetPixelFormat(deviceContext, 1, &pixelFormat);
        if (error != 1)
        {
            return false;
        }

        // Create a temporary rendering context.
        renderContext = wglCreateContext(deviceContext);
        if (!renderContext)
        {
            return false;
        }

        // Set the temporary rendering context as the current rendering context for this window.
        error = wglMakeCurrent(deviceContext, renderContext);
        if (error != 1)
        {
            return false;
        }

        const char*  GLVersion = (char*)glGetString(GL_VERSION);
        GLog->Info("OpenGL Version : %s", GLVersion);        

        // Initialize the OpenGL extensions needed for this application.  Note that a temporary rendering context was needed to do so.
        result = LoadExtensionList();
        if (!result)
        {
            return false;
        }

        // Release the temporary rendering context now that the extensions have been loaded.
        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(renderContext);
        renderContext = NULL;

        // Release the device context for this window.
        ReleaseDC(hwnd, deviceContext);
        deviceContext = 0;

        return true;
    }

#include <windows.h>
#include <strsafe.h>

    void ErrorMessage(LPTSTR lpszFunction)
    {
        LPVOID lpMsgBuf;
        LPVOID lpDisplayBuf;
        DWORD dw = GetLastError(); 

        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | 
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            dw,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR) &lpMsgBuf,
            0, NULL );

        // Display the error message and exit the process

        lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
            (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR)); 
        StringCchPrintf((LPTSTR)lpDisplayBuf, 
            LocalSize(lpDisplayBuf) / sizeof(TCHAR),
            TEXT("%s failed with error %d: %s"), 
            lpszFunction, dw, lpMsgBuf); 
        MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK); 

    }


    bool InitializeOpenGL(HWND hwnd, int screenWidth, int screenHeight,  bool vsync)
    {
        int pixelFormat[1];
        unsigned int formatCount;
        int result;
        int attributeList[5];
        float fieldOfView, screenAspect;
        char *vendorString, *rendererString;


        // Get the device context for this window.
        m_deviceContext = GetDC(hwnd);
        if (!m_deviceContext)
        {
            return false;
        }
        
        const int attribList[] =
        {
            WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
            WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
            WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
            WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
            WGL_COLOR_BITS_ARB, 32,
            WGL_DEPTH_BITS_ARB, 24,
            WGL_STENCIL_BITS_ARB, 8,
            0, // End
        };


        // Query for a pixel format that fits the attributes we want.
        result = wglChoosePixelFormatARB(m_deviceContext, attribList, NULL, 1, pixelFormat, &formatCount);
        if (result != 1 || formatCount == 0)
        {
            return false;
        }

        
        PIXELFORMATDESCRIPTOR pfd = {0};


        // If the video card/display can handle our desired pixel format then we set it as the current one.
        result = SetPixelFormat(m_deviceContext, pixelFormat[0], &pfd);
        if (result != 1)
        {
            ErrorMessage(TEXT("SetPixelFormat"));
            return false;
        }

        // Set the 4.0 version of OpenGL in the attribute list.
        attributeList[0] = WGL_CONTEXT_MAJOR_VERSION_ARB;
        attributeList[1] = 4;
        attributeList[2] = WGL_CONTEXT_MINOR_VERSION_ARB;
        attributeList[3] = 0;

        // Null terminate the attribute list.
        attributeList[4] = 0;

        // Create a OpenGL 4.0 rendering context.
        m_renderingContext = wglCreateContextAttribsARB(m_deviceContext, 0, attributeList);
        if (m_renderingContext == NULL)
        {
            return false;
        }

        // Set the rendering context to active.
        result = wglMakeCurrent(m_deviceContext, m_renderingContext);
        if (result != 1)
        {
            return false;
        }

        // Set the depth buffer to be entirely cleared to 1.0 values.
        glClearDepth(1.0f);

        // Enable depth testing.
        glEnable(GL_DEPTH_TEST);

        // Set the polygon winding to front facing for the left handed system.
        glFrontFace(GL_CW);

        // Enable back face culling.
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        // Get the name of the video card.
        vendorString = (char*)glGetString(GL_VENDOR);
        rendererString = (char*)glGetString(GL_RENDERER);

        // Store the video card name in a class member variable so it can be retrieved later.
        /*strcpy_s(m_videoCardDescription, vendorString);
        strcat_s(m_videoCardDescription, " - ");
        strcat_s(m_videoCardDescription, rendererString);*/

        // Turn on or off the vertical sync depending on the input bool value.
        if (vsync)
        {
            result = wglSwapIntervalEXT(1);
        }
        else
        {
            result = wglSwapIntervalEXT(0);
        }

        // Check if vsync was set correctly.
        if (result != 1)
        {
            return false;
        }

        return true;
    }


    LRESULT CALLBACK DummyWinProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
    {
        if (umessage == WM_CLOSE)
        {
            PostQuitMessage(0);
            return 0;
        }

        return DefWindowProc(hwnd, umessage, wparam, lparam);
    };
        

    bool InitializeOpenGLDevice(HWND hwnd, int InWidth, int InHeight)
    {

        auto CreateDummyWindow = []()
        {
            int width = 800;
            int height = 600;
            
            HMODULE ModuleInstance = ::GetModuleHandle(nullptr);
            static wchar_t s_acWindowClass[] = L"Dummy GL Window";
            // Register the window class
            WNDCLASSEXW wc;
            wc.cbSize			= sizeof(wc);
            wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
            wc.lpfnWndProc		= DummyWinProc;
            wc.cbClsExtra		= 0;
            wc.cbWndExtra		= 0;
            wc.hInstance		= ModuleInstance;
            wc.hIcon			= nullptr;
            wc.hCursor			= ::LoadCursor(nullptr, IDC_ARROW);
            wc.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
            wc.lpszMenuName		= nullptr;
            wc.lpszClassName	= s_acWindowClass;
            wc.hIconSm			= nullptr;

            ::RegisterClassExW(&wc);

            // DWORD dwWindowStyle = WS_OVERLAPPEDWINDOW;
            DWORD dwWindowStyle = WS_POPUP;
            // require the window to have the specified client area

            RECT rcWindow;
            rcWindow.left	= 0;
            rcWindow.right	= width;
            rcWindow.top	= 0;
            rcWindow.bottom	= height;
            AdjustWindowRect(&rcWindow, dwWindowStyle, false);

            // create the application window

            const wchar_t* appName = L"Aurora";
    		
            HWND result = CreateWindowExW(WS_EX_APPWINDOW, s_acWindowClass, appName, dwWindowStyle,
            CW_USEDEFAULT, CW_USEDEFAULT, rcWindow.right - rcWindow.left, 
        rcWindow.bottom - rcWindow.top, NULL, NULL, ModuleInstance, NULL);

            return result;            
        };

        HWND TempWindow = CreateDummyWindow();
        if(!TempWindow)
        {
            return false;
        }
        
        // Initialize a temporary OpenGL window and load the OpenGL extensions.
        bool result = InitializeGLExtensions(TempWindow);
        if(!result)
        {
            // MessageBox(hwnd, L"Could not initialize the OpenGL extensions.", L"Error", MB_OK);
            return false;
        }

        result = InitializeOpenGL(hwnd, InWidth, InHeight, false);
        if(!result)
        {
            // MessageBox(m_hwnd, L"Could not initialize OpenGL, check if video card supports OpenGL 4.0.", L"Error", MB_OK);
            return false;
        }


        return true;
    }


    class GLRenderDevice : public IRenderDevice
    {
    public:
        GPUShaderObject* CreateShader(const ShaderCode& code) override;
        Texture* CreateTexture(File* file) override;
        Texture* CreateTexture(Texture::Type type, const Texture::Desc& desc) override;
        RenderTarget* CreateRenderTarget(const RenderTarget::Desc& desc) override;
        void Clear(int clearMask, const Color& clearColor, float fDepth, uint8 nStencil) override;
        void Present() override;
        RenderTarget* GetFrameBuffer() override;
        RenderTarget* GetDepthStencil() override;
        void GetFrameBufferSize(uint& nWidth, uint& nHeight) override;
        void SetVertexShader(Shader* pShader) override;
        void SetPixelShader(Shader* pShader) override;
        void SetRenderTarget(uint idx, RenderTarget* pRenderTarget) override;
        void SetDepthStencil(RenderTarget* pDepthStencil) override;
        void SetRenderTarget(uint nRTs, RenderTarget** pRenderTargets, RenderTarget* pDepthStencil) override;
        void ExecuteOperator(const RenderOperator& op) override;
        Handle CreateShaderParameterBinding(GPUShaderObject* shaderHandle, const ShaderParamterBindings& bindings) override;
        void UpdateShaderParameter(Handle bindingHandle) override;
        void BindVertexShader(GPUShaderObject* shaderHandle) override;
        void BindPixelShader(GPUShaderObject* shaderHandle) override;
        Handle CreateShaderTextureBinding(GPUShaderObject* shaderHandle, const ShaderTextureBinding& bindings) override;
        void BindTexture(Handle binding, Texture* texture) override;
        void BindGlobalParameter(Handle handle) override;
        Handle CreateVertexLayoutHandle(const vector<VertexLayoutItem>& layoutItems) override;
        Handle CreateVertexBufferHandle(const void* data, int32 size) override;
        Handle CreateIndexBufferHandle(const void* data, int32 size) override;
    };







    GPUShaderObject* GLRenderDevice::CreateShader(const ShaderCode& code)
    {
        return nullptr;
    }
    
    Texture* GLRenderDevice::CreateTexture(File* file)
    {
        return nullptr;
    }
    
    Texture* GLRenderDevice::CreateTexture(Texture::Type type, const Texture::Desc& desc)
    {
        return nullptr;
    }
    
    RenderTarget* GLRenderDevice::CreateRenderTarget(const RenderTarget::Desc& desc)
    {
        return nullptr;
    }
    
    void GLRenderDevice::Clear(int clearMask, const Color& clearColor, float fDepth, uint8 nStencil)
    {
        
    }
    
    void GLRenderDevice::Present()
    {
    }
    
    RenderTarget* GLRenderDevice::GetFrameBuffer()
    {
        return nullptr;
    }
    
    RenderTarget* GLRenderDevice::GetDepthStencil()
    {
        return nullptr;
    }
    
    void GLRenderDevice::GetFrameBufferSize(uint& nWidth, uint& nHeight)
    {
        
    }
    
    void GLRenderDevice::SetVertexShader(Shader* pShader)
    {
    }
    
    void GLRenderDevice::SetPixelShader(Shader* pShader)
    {
    }
    
    void GLRenderDevice::SetRenderTarget(uint idx, RenderTarget* pRenderTarget)
    {
    }
    
    void GLRenderDevice::SetDepthStencil(RenderTarget* pDepthStencil)
    {
    }
    
    void GLRenderDevice::SetRenderTarget(uint nRTs, RenderTarget** pRenderTargets, RenderTarget* pDepthStencil)
    {
    }
    
    void GLRenderDevice::ExecuteOperator(const RenderOperator& op)
    {
    }
    
    Handle GLRenderDevice::CreateShaderParameterBinding(GPUShaderObject* shaderHandle, const ShaderParamterBindings& bindings)
    {
        return 0;
    }
    
    void GLRenderDevice::UpdateShaderParameter(Handle bindingHandle)
    {
    }
    
    void GLRenderDevice::BindVertexShader(GPUShaderObject* shaderHandle)
    {
    }
    
    void GLRenderDevice::BindPixelShader(GPUShaderObject* shaderHandle)
    {
    }
    
    Handle GLRenderDevice::CreateShaderTextureBinding(GPUShaderObject* shaderHandle, const ShaderTextureBinding& bindings)
    {
        return 0;
    }
    
    void GLRenderDevice::BindTexture(Handle binding, Texture* texture)
    {
    }
    
    void GLRenderDevice::BindGlobalParameter(Handle handle)
    {
    }

    
    struct VertexLayoutInfo
    {
        
    };
    static vector<VertexLayoutInfo*>  VertexLayouts_;
    
    Handle GLRenderDevice::CreateVertexLayoutHandle(const vector<VertexLayoutItem>& layoutItems)
    {
        

        
        return 0;
    }
    
    Handle GLRenderDevice::CreateVertexBufferHandle(const void* data, int32 size)
    {
        return 0;
    }
    
    Handle GLRenderDevice::CreateIndexBufferHandle(const void* data, int32 size)
    {
        return 0;
    }
}
