#include <complex>
#include <windows.h>
#include "FileSystem.h"
#include "Shader.h"
#include "Log.h"
#include "Renderer.h"
#include "PlatformWin.h"
#include "glad.h"
#include "glad_wgl.h"

#include <strsafe.h>


#pragma comment(lib, "opengl32.lib")

namespace Aurora
{

    HDC m_deviceContext;
    HGLRC m_renderingContext;


    bool InitializeGLExtensions(HWND hwnd)
    {
        HDC deviceContext;
        PIXELFORMATDESCRIPTOR pixelFormat;
        int error;
        HGLRC renderContext;

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

        HDC HDC = GetDC(hwnd);

        // Initialize the OpenGL extensions needed for this application.  Note that a temporary rendering context was needed to do so.
        int result = gladLoadWGL(HDC);

        if (!result)
        {
            GLog->Error("Load Wgl failed!");
            return false;
        }        

        result = gladLoadGL();
        
        if (!result)
        {
            GLog->Error("Load GL error!");
            return false;
        }        
        
        const char*  GLVersion = (char*)glGetString(GL_VERSION);
        GLog->Info("OpenGL Version : %s", GLVersion);     

        // Release the temporary rendering context now that the extensions have been loaded.
        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(renderContext);
        renderContext = NULL;

        // Release the device context for this window.
        ReleaseDC(hwnd, deviceContext);
        deviceContext = 0;

        return true;
    }

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
            GLog->Error("open gl wglChoosePixelFormatARB failed!");
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
        attributeList[3] = 6;

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
            GLog->Error("Could not initialize the OpenGL extensions.");
            return false;
        }

        result = InitializeOpenGL(hwnd, InWidth, InHeight, false);
        if(!result)
        {
            GLog->Error("Could not initialize OpenGL, check if video card supports OpenGL 4.0.");
            return false;
        }

        return true;
    }

    char const* glErrorString(GLenum const err) noexcept
    {
        switch (err)
        {
            // opengl 2 errors (8)
            case GL_NO_ERROR:
                return "GL_NO_ERROR";

        case GL_INVALID_ENUM:
            return "GL_INVALID_ENUM";

        case GL_INVALID_VALUE:
            return "GL_INVALID_VALUE";

        case GL_INVALID_OPERATION:
            return "GL_INVALID_OPERATION";

        case GL_STACK_OVERFLOW:
            return "GL_STACK_OVERFLOW";

        case GL_STACK_UNDERFLOW:
            return "GL_STACK_UNDERFLOW";

        case GL_OUT_OF_MEMORY:
            return "GL_OUT_OF_MEMORY";

        default:
                assert(!"unknown error");
            return nullptr;
        }
    }

    bool TestNoGLError(const String& Message = "")
    {
        GLenum err;
        bool NoError = true;
        while((err = glGetError()) != GL_NO_ERROR)
        {
            GLog->Error("GL error : %s (%s)", glErrorString(err), Message.c_str());
            NoError = false;
        }

        return NoError;
    }

    
    class CShaderCodeDataBase
    {
    public:
        void Initialize()
        {
            const String ShaderPath = GPlatform->GetShaderFilePath();
            auto ShaderFiles = GPlatform->ListFile(ShaderPath, "*.shader");

            for(auto& fileName : ShaderFiles)
            {
                String fullPath = FileSystem::CombinePath(ShaderPath, fileName);
                FilePtr file(GFileSys->OpenFile(fullPath, false));

                String ShaderCode = file->ReadAsString();

                mShaderCode.insert(make_pair(Util::StringToLower(fileName), ShaderCode));
            }            
        }

        map<String, String> mShaderCode;
        
    };


    class GLRenderDevice : public IRenderDevice
    {
    public:
        ~GLRenderDevice() override;
        GPUShaderObject* CreateGPUShaderObject(const ShaderCode& code, BaseShader* InShader) override;
        Texture* CreateTexture(File* file) override;
        Texture* CreateTexture(Texture::Type type, const Texture::Desc& desc) override;
        RenderTarget* CreateRenderTarget(const RenderTarget::Desc& desc) override;
        void Clear(int clearMask, const Color& clearColor, float fDepth, uint8 nStencil) override;
        void Present() override;
        CGPUGeometryBuffer* CreateGeometryBuffer(const CGeometry* InGeometry, CVertexFactory* InVertexFactory) override;
        GPUShaderParameterBuffer* CreateShaderParameterBuffer(CShaderParameterBuffer* InBuffer) override;
        void UpdateGPUShaderParameterBuffer(GPUShaderParameterBuffer* InBuffer, const Array<int8>& InData) override;
        RenderTarget* GetFrameBuffer() override;
        RenderTarget* GetDepthStencil() override;
        void GetFrameBufferSize(uint& nWidth, uint& nHeight) override;
        void SetRenderTarget(uint idx, RenderTarget* pRenderTarget) override;
        void SetDepthStencil(RenderTarget* pDepthStencil) override;
        void SetRenderTarget(uint nRTs, RenderTarget** pRenderTargets, RenderTarget* pDepthStencil) override;
        void ExecuteOperator(const RenderOperator& op) override;
        void BindVertexShader(GPUShaderObject* shaderHandle) override;
        void BindPixelShader(GPUShaderObject* shaderHandle) override;
        Handle CreateShaderTextureBinding(GPUShaderObject* shaderHandle, const ShaderTextureBinding& bindings) override;
        void BindTexture(Handle binding, Texture* texture) override;

        RenderStateObject* CreateBlendState(bool BlendEnable, BlendFactor SrcBlend, BlendFactor DestBlend, BlendOp op, uint8 WriteMask, bool AlphaToCoverageEnable) override;
        void BindBlendState(RenderStateObject* handle, const Color& BlendFactor) override;
        RenderStateObject* CreateRasterizerState(bool FrontCounterClockwise, CullMode cullMode, FillMode fillMode, int DepthBias, int SlopeScaledDepthBias, bool AntialiasedLineEnable) override;
        void BindRasterizerState(RenderStateObject* handle) override;

    private:
    };


    CShaderCodeDataBase     ShaderCodeDataBase;

    
    GLRenderDevice::~GLRenderDevice()
    {
    }
    
    static GLRenderDevice GLRenderDevice;
    

    
    bool CreateOpenGLDevice(CScreen* InScreen)
    {
        HWND MainWnd = GetHWNDFromScreen(InScreen);

        RectSize ScreenSize = InScreen->GetSize();

        if(!InitializeOpenGLDevice(MainWnd, ScreenSize.Width, ScreenSize.Height))
        {
            GLog->Error("InitializeOpenGLDevice failed!");
            return false;
        }

        ShaderCodeDataBase.Initialize();
        
        GRenderDevice = &GLRenderDevice;
        
        return true;
    }



    struct ShaderMarcoDefine
    {
        String Name;
        String Vaue;        
    };


    class GLShaderCodeBuilder
    {
    public:
        GLShaderCodeBuilder(const String& InBaseCode, const String& InName)
            : mBaseCode(InBaseCode),
            mName(InName)
        {
        }

        const String IncludeIndentifier = "#include";

        static String GetIncludeNameFromString(const String& line)
        {
            const size_t start = line.find_first_of("\"<");
            const size_t end = line .find_last_of("\">");

            if(start == String::npos || end == String::npos)
            {
                return "";
            }

            return Util::StringToLower(line.substr(start + 1, end - start - 1));
        }

        void IncludeCode(StringStream& InStream, const String& InCode)
        {            
            std::istringstream codeStream(InCode);
            String line;

            int NumLine = 0;
            while (std::getline(codeStream, line))
            {
                // Look for the new shader include identifier
                if (line.find(IncludeIndentifier) != String::npos)
                {
                    const String target = GetIncludeNameFromString(line);
                    if(target.empty())
                    {
                        GLog->Warning("can not analysis include line \"%s\" in shader %s", line.c_str(), mName.c_str());
                        continue;                        
                    }

                    const auto found = mIncludeTargets.find(target);
                    if(found == mIncludeTargets.end())
                    {
                        GLog->Warning("can not find include \"%s\" in shader %s", target.c_str(), mName.c_str());
                        continue; 
                    }

                    if(found->second)
                    {
                        InStream << "// include " << target << "\r";
                        IncludeCode(InStream, *found->second);
                        InStream << "\r" << "// end include " << target << "\r";

                        // 一个文件只能include一次，否则可能会无限递归
                        found->second = nullptr;                        
                    }

                }
                else
                {
                    InStream << line << "\n";
                }                
            }
        }

        String Build(const CShaderCodeDataBase* IncludeManager, const BaseShader* Shader)
        {            
            for(auto& code : IncludeManager->mShaderCode)
            {
                mIncludeTargets.insert(make_pair(code.first, &code.second));
            }

            StringStream stream;

            stream << "#version 460\n";

            // for(auto& marco : mMarcos)
            // {
            //     stream << "#define " << marco.first << " " << marco.second << "\r";
            // }

            IncludeCode(stream, mBaseCode);

            return stream.str();
        }


        const String& mBaseCode;
        const String& mName;
        Array<pair<string, string>> mMarcos;
        std::map<const String, const String*> mIncludeTargets;
    };

    void GLSLUniformInfoTest(GLuint InProgram)
    {        
        glUseProgram(InProgram);

        int numUniform = 0;

        glGetProgramiv(InProgram, GL_ACTIVE_UNIFORMS, &numUniform);
        printf("active uniforms %d\n", numUniform);

        {
            for (int Index = 0; Index < numUniform; Index++)
            {
                char Name[256];
                int Size;
                GLenum type;
            
                glGetActiveUniform(InProgram, Index, sizeof(Name), NULL, &Size, &type, Name);
                printf("\t uniform %d %s size %d\n", Index, Name, Size);
            }        
        }
    
        glGetProgramiv(InProgram, GL_ACTIVE_ATTRIBUTES, &numUniform);
        printf("GL_ACTIVE_ATTRIBUTES %d\n", numUniform);
    
        glGetProgramiv(InProgram, GL_ACTIVE_UNIFORM_BLOCKS, &numUniform);
        printf("GL_ACTIVE_UNIFORM_BLOCKS %d\n", numUniform);

        {
            for (int Index = 0; Index < numUniform; Index++)
            {
                char Name[256];
                int Size;
                GLenum type;
            
                glGetActiveUniformBlockName(InProgram, Index, sizeof(Name), NULL, Name);
                printf("\t uniform block %d %s\n", Index, Name);

                int DataSize, SubUniforms;
                glGetActiveUniformBlockiv(InProgram, Index, GL_UNIFORM_BLOCK_DATA_SIZE, &DataSize);
                glGetActiveUniformBlockiv(InProgram, Index, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &SubUniforms);

                printf("\t\tdata size %d, sub-uniforms %d\n", DataSize, SubUniforms);
            }
        
        }
        
        glGetProgramInterfaceiv(InProgram, GL_UNIFORM, GL_MAX_NUM_ACTIVE_VARIABLES, &numUniform);
        printf("program active uniforms %d\n", numUniform);

        int numUniformBlocks = 0;
        glGetProgramInterfaceiv(InProgram, GL_UNIFORM_BLOCK, GL_MAX_NUM_ACTIVE_VARIABLES, &numUniformBlocks);
        printf("program active uniform blocks %d\n", numUniformBlocks);

        int numInput = 0;
        glGetProgramInterfaceiv(InProgram, GL_PROGRAM_INPUT, GL_MAX_NUM_ACTIVE_VARIABLES, &numInput);
        printf("program active program input %d\n", numInput);     

        
        
        
        GLint numBlocks = 0;
        glGetProgramInterfaceiv(InProgram, GL_UNIFORM_BLOCK, GL_ACTIVE_RESOURCES, &numBlocks);
        const GLenum blockProperties[1] = {GL_NUM_ACTIVE_VARIABLES};
        const GLenum activeUnifProp[1] = {GL_ACTIVE_VARIABLES};
        const GLenum unifProperties[3] ={GL_NAME_LENGTH, GL_TYPE, GL_LOCATION};

        for(int blockIx = 0; blockIx < numBlocks; ++blockIx)
        {
            GLint numActiveUnifs = 0;
            glGetProgramResourceiv(InProgram, GL_UNIFORM_BLOCK, blockIx, 1, blockProperties, 1, NULL, &numActiveUnifs);

            if(!numActiveUnifs)
                continue;

            std::vector<GLint> blockUnifs(numActiveUnifs);
            glGetProgramResourceiv(InProgram, GL_UNIFORM_BLOCK, blockIx, 1, activeUnifProp, numActiveUnifs, NULL, &blockUnifs[0]);

            for(int unifIx = 0; unifIx < numActiveUnifs; ++unifIx)
            {
                GLint values[3];
                glGetProgramResourceiv(InProgram, GL_UNIFORM, blockUnifs[unifIx], 3, unifProperties, 3, NULL, values);

                // Get the name. Must use a std::vector rather than a std::string for C++03 standards issues.
                // C++11 would let you use a std::string directly.
                std::vector<char> nameData(values[0]);
                glGetProgramResourceName(InProgram, GL_UNIFORM, blockUnifs[unifIx], nameData.size(), NULL, &nameData[0]);
                std::string name(nameData.begin(), nameData.end() - 1);

                GLog->Info("GLSLUniformInfoTest name %s", name.c_str());
            }
        }
    }

    void CollectGLProgramUniformInfomation(GLuint program, BaseShader* InShader)
    {
        // glUseProgram(program);

        {
            int numUniform = 0;

            glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &numUniform);
            GLog->Info("active uniforms %d\n", numUniform);

            {
                int Offset = 0;
                for (int Index = 0; Index < numUniform; Index++)
                {
                    char Name[256];
                    int Size;
                    GLenum type;

                    glGetActiveUniform(program, Index, sizeof(Name), NULL, &Size, &type, Name);
                    GLog->Info("\t uniform %d %s size %d\n", Index, Name, Size);

                    auto ParameterBinding = InShader->FindParameter(String(Name));
                    if(ParameterBinding)
                    {
                        ParameterBinding->mSizeInByte = (uint16)Size;
                        ParameterBinding->mOffset = Offset;
                        Offset += Size;
                    }
                    else
                    {
                        GLog->Warning("Can't find parameter %s in %s", Name, InShader->pathname_.c_str());                        
                    }
                }
            }

        }
        
        int numUniform = 0;

        
    }

    class GPUShaderObjectOGL : public GPUShaderObject
    {
    public:
        GLuint program = 0;
    };
    

    GPUShaderObject* GLRenderDevice::CreateGPUShaderObject(const ShaderCode& code, BaseShader* InShader)
    {
        
        GLShaderCodeBuilder CodeBuilder(code.text, InShader->mName);
        CodeBuilder.mMarcos = code.defines;

        String buildCode = CodeBuilder.Build(&ShaderCodeDataBase, InShader);
        GLog->Info("shader code:");
        GLog->Info("%s", buildCode.c_str());

        GLog->Info("compile shader %s", code.name.c_str());

        {            
            GLuint glShader = glCreateShader(InShader->mType == BaseShader::VERTEX_SHADER ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER);

            const char* data = buildCode.c_str();
            glShaderSource(glShader, 1, &data, NULL);
            glCompileShader(glShader);
            
            GLint state = 0;
            char errorMsg[512];
            glGetShaderiv(glShader, GL_COMPILE_STATUS, &state);
            if (state != GL_TRUE)
            {
                glGetShaderInfoLog(glShader, 512, NULL, errorMsg);
                GLog->Error("compile shader failed %s", InShader->mName.c_str());
                GLog->Error("%s", errorMsg);

                return nullptr;
            }

            GLuint program = glCreateProgram();
            // glProgramParameteri(program, GL_PROGRAM_SEPARABLE, GL_TRUE);
            glAttachShader(program, glShader);
            glLinkProgram(program);
            glGetProgramiv(program, GL_LINK_STATUS, &state);
            if (state != GL_TRUE)
            {
                glGetProgramInfoLog(program, 512, NULL, errorMsg);
                GLog->Error("gl link program  failed %s", InShader->mName.c_str());
                GLog->Error("%s", errorMsg);

                return nullptr;
            }
            // GLSLUniformInfoTest(program);

            CollectGLProgramUniformInfomation(program, InShader);
            
            auto ShaderObject = new GPUShaderObjectOGL();
            ShaderObject->program = program;

            glDetachShader(program, glShader);
            // glDeleteShader(glShader);
            
            return ShaderObject;
        }

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
        glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);

        glClearDepthf(fDepth);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    
    void GLRenderDevice::Present()
    {
        glFlush();
    }

    class CGPUGeometryBufferGL : public CGPUGeometryBuffer
    {
    public:    
        GLuint mVertexBuffer = -1;
        GLuint mIndexBuffer = -1;
        GLenum IndexType;
    };
    
#define BUFFER_OFFSET(offset) reinterpret_cast<GLvoid*>(offset)
    CGPUGeometryBuffer* GLRenderDevice::CreateGeometryBuffer(const CGeometry* InGeometry, CVertexFactory* InVertexFactory)
    {        
        GLuint vertexBuffer;
        GLuint indexBuffer;
        
        vector<int8> vertexData;
        InGeometry->PrepareVertexData(vertexData, InVertexFactory->mVertexLayouts);
        
        glGenBuffers(1, &vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, (ptrdiff_t)vertexData.size(), vertexData.data(), GL_STATIC_DRAW);

        {
            int Index = 0;
            int Offset = 0;
            uint Stride = InVertexFactory->GetStride();
            for(auto& VertexItem : InVertexFactory->mVertexLayouts)
            {
                uint NumElement = Vertex::NumElement((Vertex::ElemType)(VertexItem.type));
                glEnableVertexAttribArray(Index);
                glVertexAttribPointer(Index, (int)NumElement, GL_FLOAT, GL_FALSE, Stride, BUFFER_OFFSET(Offset));
                Offset += CGeometry::GetSizeOfType((Vertex::ElemType)VertexItem.type);
            }            
        }
        
        //
        // glEnableVertexAttribArray(1);
        // glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(MyVertex), BUFFER_OFFSET(12));
        //
        // glEnableVertexAttribArray(2);
        // glVertexAttribPointer(2, 2, GL_FLOAT,  GL_FALSE, sizeof(MyVertex), BUFFER_OFFSET(28));

        vector<int8> IndexData;
        InGeometry->PrepareIndexData(IndexData);
        
        glGenBuffers(1, &indexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, IndexData.size(), IndexData.data(), GL_STATIC_DRAW);

        glBindVertexArray(0);
        
        if(!TestNoGLError("GLRenderDevice::CreateGeometryBuffer"))
        {
            return nullptr;
        }

        CGPUGeometryBufferGL* GPUBuffer = new CGPUGeometryBufferGL();
        GPUBuffer->mVertexBuffer = vertexBuffer;
        GPUBuffer->mIndexBuffer = indexBuffer;

        assert(InGeometry->GetIndexStride() > 0);
        GPUBuffer->IndexType = InGeometry->GetIndexStride() == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;
        
        return GPUBuffer;
    }
    
    GPUShaderParameterBuffer* GLRenderDevice::CreateShaderParameterBuffer(CShaderParameterBuffer* InBuffer)
    {
        return new GPUShaderParameterBuffer();
    }
    
    void GLRenderDevice::UpdateGPUShaderParameterBuffer(GPUShaderParameterBuffer* InBuffer, const Array<int8>& InData)
    {
        IRenderDevice::UpdateGPUShaderParameterBuffer(InBuffer, InData);
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
        auto GeometryBuffer = static_cast<CGPUGeometryBufferGL*>(op.GeometryBuffer);
        
        glBindVertexArray(GeometryBuffer->mVertexBuffer);
	    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GeometryBuffer->mIndexBuffer);        
        
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        
        // glBindVertexArray(0);
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
    
    RenderStateObject* GLRenderDevice::CreateBlendState(bool BlendEnable, BlendFactor SrcBlend, BlendFactor DestBlend, BlendOp op, uint8 WriteMask, bool AlphaToCoverageEnable)
    {
        return new RenderStateObject();
    }

    
    void GLRenderDevice::BindBlendState(RenderStateObject* handle, const Color& BlendFactor)
    {
    }

    
    RenderStateObject* GLRenderDevice::CreateRasterizerState(bool FrontCounterClockwise, CullMode cullMode, FillMode fillMode, int DepthBias, int SlopeScaledDepthBias, bool AntialiasedLineEnable)
    {
        return new RenderStateObject;
    }
    
    void GLRenderDevice::BindRasterizerState(RenderStateObject* handle)
    {
    }

    struct VertexLayoutInfo
    {
        
    };
    static vector<VertexLayoutInfo*>  VertexLayouts_;
    
    
}
