#include <windows.h>
#include <gl\gl.h>

#include "FileSystem.h"
#include "Shader.h"
#include "Log.h"
#include "Renderer.h"
#include "PlatformWin.h"
#include "GLFunctions.h"


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

    

    HDC m_deviceContext;
    HGLRC m_renderingContext;

    #define LOAD_GL_PROC_WIN(proc_def, proc) \
    proc = (proc_def) wglGetProcAddress(#proc); \
    if((proc) == nullptr) \
    { \
        GLog->Error("wglGetProcAddress %s failed!", #proc); \
        return false; \
    }
    

    static bool LoadExtensionList()
    {         
         LOAD_GL_PROC_WIN(PFNWGLCHOOSEPIXELFORMATARBPROC, wglChoosePixelFormatARB)
         LOAD_GL_PROC_WIN(PFNWGLCREATECONTEXTATTRIBSARBPROC, wglCreateContextAttribsARB)
         LOAD_GL_PROC_WIN(PFNWGLSWAPINTERVALEXTPROC, wglSwapIntervalEXT)

         
         LOAD_GL_PROC_WIN(PFNGLACTIVETEXTUREPROC, glActiveTexture)
         LOAD_GL_PROC_WIN(PFNGLATTACHSHADERPROC, glAttachShader)
         LOAD_GL_PROC_WIN(PFNGLBINDATTRIBLOCATIONPROC, glBindAttribLocation)
         LOAD_GL_PROC_WIN(PFNGLBINDBUFFERPROC, glBindBuffer)
         LOAD_GL_PROC_WIN(PFNGLBINDFRAMEBUFFERPROC, glBindFramebuffer)
         LOAD_GL_PROC_WIN(PFNGLBINDRENDERBUFFERPROC, glBindRenderbuffer)
         LOAD_GL_PROC_WIN(PFNGLBINDTEXTUREPROC, glBindTexture)
         LOAD_GL_PROC_WIN(PFNGLBLENDCOLORPROC, glBlendColor)
         LOAD_GL_PROC_WIN(PFNGLBLENDEQUATIONPROC, glBlendEquation)
         LOAD_GL_PROC_WIN(PFNGLBLENDEQUATIONSEPARATEPROC, glBlendEquationSeparate)
         LOAD_GL_PROC_WIN(PFNGLBLENDFUNCSEPARATEPROC, glBlendFuncSeparate)
         LOAD_GL_PROC_WIN(PFNGLBUFFERDATAPROC, glBufferData)
         LOAD_GL_PROC_WIN(PFNGLBUFFERSUBDATAPROC, glBufferSubData)
         LOAD_GL_PROC_WIN(PFNGLCHECKFRAMEBUFFERSTATUSPROC, glCheckFramebufferStatus)
         LOAD_GL_PROC_WIN(PFNGLCLEARDEPTHFPROC, glClearDepthf)
         LOAD_GL_PROC_WIN(PFNGLCOMPILESHADERPROC, glCompileShader)
         LOAD_GL_PROC_WIN(PFNGLCOMPRESSEDTEXIMAGE2DPROC, glCompressedTexImage2D)
         LOAD_GL_PROC_WIN(PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC, glCompressedTexSubImage2D)
         LOAD_GL_PROC_WIN(PFNGLCOPYTEXIMAGE2DPROC, glCopyTexImage2D)
         LOAD_GL_PROC_WIN(PFNGLCOPYTEXSUBIMAGE2DPROC, glCopyTexSubImage2D)
         LOAD_GL_PROC_WIN(PFNGLCREATEPROGRAMPROC, glCreateProgram)
         LOAD_GL_PROC_WIN(PFNGLCREATESHADERPROC, glCreateShader)
         LOAD_GL_PROC_WIN(PFNGLDELETEBUFFERSPROC, glDeleteBuffers)
         LOAD_GL_PROC_WIN(PFNGLDELETEFRAMEBUFFERSPROC, glDeleteFramebuffers)
         LOAD_GL_PROC_WIN(PFNGLDELETEPROGRAMPROC, glDeleteProgram)
         LOAD_GL_PROC_WIN(PFNGLDELETERENDERBUFFERSPROC, glDeleteRenderbuffers)
         LOAD_GL_PROC_WIN(PFNGLDELETESHADERPROC, glDeleteShader)
         LOAD_GL_PROC_WIN(PFNGLDELETETEXTURESPROC, glDeleteTextures)
         LOAD_GL_PROC_WIN(PFNGLDEPTHRANGEFPROC, glDepthRangef)
         LOAD_GL_PROC_WIN(PFNGLDETACHSHADERPROC, glDetachShader)
         LOAD_GL_PROC_WIN(PFNGLDISABLEVERTEXATTRIBARRAYPROC, glDisableVertexAttribArray)
         LOAD_GL_PROC_WIN(PFNGLDRAWARRAYSPROC, glDrawArrays)
         LOAD_GL_PROC_WIN(PFNGLDRAWELEMENTSPROC, glDrawElements)
         LOAD_GL_PROC_WIN(PFNGLENABLEVERTEXATTRIBARRAYPROC, glEnableVertexAttribArray)
         LOAD_GL_PROC_WIN(PFNGLFRAMEBUFFERRENDERBUFFERPROC, glFramebufferRenderbuffer)
         LOAD_GL_PROC_WIN(PFNGLFRAMEBUFFERTEXTURE2DPROC, glFramebufferTexture2D)
         LOAD_GL_PROC_WIN(PFNGLGENBUFFERSPROC, glGenBuffers)
         LOAD_GL_PROC_WIN(PFNGLGENERATEMIPMAPPROC, glGenerateMipmap)
         LOAD_GL_PROC_WIN(PFNGLGENFRAMEBUFFERSPROC, glGenFramebuffers)
         LOAD_GL_PROC_WIN(PFNGLGENRENDERBUFFERSPROC, glGenRenderbuffers)
         LOAD_GL_PROC_WIN(PFNGLGENTEXTURESPROC, glGenTextures)
         LOAD_GL_PROC_WIN(PFNGLGETACTIVEATTRIBPROC, glGetActiveAttrib)
         LOAD_GL_PROC_WIN(PFNGLGETACTIVEUNIFORMPROC, glGetActiveUniform)
         LOAD_GL_PROC_WIN(PFNGLGETATTACHEDSHADERSPROC, glGetAttachedShaders)
         LOAD_GL_PROC_WIN(PFNGLGETATTRIBLOCATIONPROC, glGetAttribLocation)
         LOAD_GL_PROC_WIN(PFNGLGETBUFFERPARAMETERIVPROC, glGetBufferParameteriv)
         LOAD_GL_PROC_WIN(PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC, glGetFramebufferAttachmentParameteriv)
         LOAD_GL_PROC_WIN(PFNGLGETPROGRAMIVPROC, glGetProgramiv)
         LOAD_GL_PROC_WIN(PFNGLGETPROGRAMINFOLOGPROC, glGetProgramInfoLog)
         LOAD_GL_PROC_WIN(PFNGLGETRENDERBUFFERPARAMETERIVPROC, glGetRenderbufferParameteriv)
         LOAD_GL_PROC_WIN(PFNGLGETSHADERIVPROC, glGetShaderiv)
         LOAD_GL_PROC_WIN(PFNGLGETSHADERINFOLOGPROC, glGetShaderInfoLog)
         LOAD_GL_PROC_WIN(PFNGLGETSHADERPRECISIONFORMATPROC, glGetShaderPrecisionFormat)
         LOAD_GL_PROC_WIN(PFNGLGETSHADERSOURCEPROC, glGetShaderSource)
         LOAD_GL_PROC_WIN(PFNGLGETUNIFORMFVPROC, glGetUniformfv)
         LOAD_GL_PROC_WIN(PFNGLGETUNIFORMIVPROC, glGetUniformiv)
         LOAD_GL_PROC_WIN(PFNGLGETUNIFORMLOCATIONPROC, glGetUniformLocation)
         LOAD_GL_PROC_WIN(PFNGLGETVERTEXATTRIBFVPROC, glGetVertexAttribfv)
         LOAD_GL_PROC_WIN(PFNGLGETVERTEXATTRIBIVPROC, glGetVertexAttribiv)
         LOAD_GL_PROC_WIN(PFNGLGETVERTEXATTRIBPOINTERVPROC, glGetVertexAttribPointerv)
         LOAD_GL_PROC_WIN(PFNGLISBUFFERPROC, glIsBuffer)
         LOAD_GL_PROC_WIN(PFNGLISFRAMEBUFFERPROC, glIsFramebuffer)
         LOAD_GL_PROC_WIN(PFNGLISPROGRAMPROC, glIsProgram)
         LOAD_GL_PROC_WIN(PFNGLISRENDERBUFFERPROC, glIsRenderbuffer)
         LOAD_GL_PROC_WIN(PFNGLISSHADERPROC, glIsShader)
         LOAD_GL_PROC_WIN(PFNGLISTEXTUREPROC, glIsTexture)
         LOAD_GL_PROC_WIN(PFNGLLINKPROGRAMPROC, glLinkProgram)
         LOAD_GL_PROC_WIN(PFNGLPOLYGONOFFSETPROC, glPolygonOffset)
         LOAD_GL_PROC_WIN(PFNGLRELEASESHADERCOMPILERPROC, glReleaseShaderCompiler)
         LOAD_GL_PROC_WIN(PFNGLRENDERBUFFERSTORAGEPROC, glRenderbufferStorage)
         LOAD_GL_PROC_WIN(PFNGLSAMPLECOVERAGEPROC, glSampleCoverage)
         LOAD_GL_PROC_WIN(PFNGLSHADERBINARYPROC, glShaderBinary)
         LOAD_GL_PROC_WIN(PFNGLSHADERSOURCEPROC, glShaderSource)
         LOAD_GL_PROC_WIN(PFNGLSTENCILFUNCSEPARATEPROC, glStencilFuncSeparate)
         LOAD_GL_PROC_WIN(PFNGLSTENCILMASKSEPARATEPROC, glStencilMaskSeparate)
         LOAD_GL_PROC_WIN(PFNGLSTENCILOPSEPARATEPROC, glStencilOpSeparate)
         LOAD_GL_PROC_WIN(PFNGLTEXSUBIMAGE2DPROC, glTexSubImage2D)
         LOAD_GL_PROC_WIN(PFNGLUNIFORM1FPROC, glUniform1f)
         LOAD_GL_PROC_WIN(PFNGLUNIFORM1FVPROC, glUniform1fv)
         LOAD_GL_PROC_WIN(PFNGLUNIFORM1IPROC, glUniform1i)
         LOAD_GL_PROC_WIN(PFNGLUNIFORM1IVPROC, glUniform1iv)
         LOAD_GL_PROC_WIN(PFNGLUNIFORM2FPROC, glUniform2f)
         LOAD_GL_PROC_WIN(PFNGLUNIFORM2FVPROC, glUniform2fv)
         LOAD_GL_PROC_WIN(PFNGLUNIFORM2IPROC, glUniform2i)
         LOAD_GL_PROC_WIN(PFNGLUNIFORM2IVPROC, glUniform2iv)
         LOAD_GL_PROC_WIN(PFNGLUNIFORM3FPROC, glUniform3f)
         LOAD_GL_PROC_WIN(PFNGLUNIFORM3FVPROC, glUniform3fv)
         LOAD_GL_PROC_WIN(PFNGLUNIFORM3IPROC, glUniform3i)
         LOAD_GL_PROC_WIN(PFNGLUNIFORM3IVPROC, glUniform3iv)
         LOAD_GL_PROC_WIN(PFNGLUNIFORM4FPROC, glUniform4f)
         LOAD_GL_PROC_WIN(PFNGLUNIFORM4FVPROC, glUniform4fv)
         LOAD_GL_PROC_WIN(PFNGLUNIFORM4IPROC, glUniform4i)
         LOAD_GL_PROC_WIN(PFNGLUNIFORM4IVPROC, glUniform4iv)
         LOAD_GL_PROC_WIN(PFNGLUNIFORMMATRIX2FVPROC, glUniformMatrix2fv)
         LOAD_GL_PROC_WIN(PFNGLUNIFORMMATRIX3FVPROC, glUniformMatrix3fv)
         LOAD_GL_PROC_WIN(PFNGLUNIFORMMATRIX4FVPROC, glUniformMatrix4fv)
         LOAD_GL_PROC_WIN(PFNGLUSEPROGRAMPROC, glUseProgram)
         LOAD_GL_PROC_WIN(PFNGLVALIDATEPROGRAMPROC, glValidateProgram)
         LOAD_GL_PROC_WIN(PFNGLVERTEXATTRIB1FPROC, glVertexAttrib1f)
         LOAD_GL_PROC_WIN(PFNGLVERTEXATTRIB1FVPROC, glVertexAttrib1fv)
         LOAD_GL_PROC_WIN(PFNGLVERTEXATTRIB2FPROC, glVertexAttrib2f)
         LOAD_GL_PROC_WIN(PFNGLVERTEXATTRIB2FVPROC, glVertexAttrib2fv)
         LOAD_GL_PROC_WIN(PFNGLVERTEXATTRIB3FPROC, glVertexAttrib3f)
         LOAD_GL_PROC_WIN(PFNGLVERTEXATTRIB3FVPROC, glVertexAttrib3fv)
         LOAD_GL_PROC_WIN(PFNGLVERTEXATTRIB4FPROC, glVertexAttrib4f)
         LOAD_GL_PROC_WIN(PFNGLVERTEXATTRIB4FVPROC, glVertexAttrib4fv)
         LOAD_GL_PROC_WIN(PFNGLVERTEXATTRIBPOINTERPROC, glVertexAttribPointer)

         LOAD_GL_PROC_WIN(PFNGLDRAWRANGEELEMENTSPROC, glDrawRangeElements)
         LOAD_GL_PROC_WIN(PFNGLTEXIMAGE3DPROC, glTexImage3D)
         LOAD_GL_PROC_WIN(PFNGLTEXSUBIMAGE3DPROC, glTexSubImage3D)
         LOAD_GL_PROC_WIN(PFNGLCOPYTEXSUBIMAGE3DPROC, glCopyTexSubImage3D)
         LOAD_GL_PROC_WIN(PFNGLCOMPRESSEDTEXIMAGE3DPROC, glCompressedTexImage3D)
         LOAD_GL_PROC_WIN(PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC, glCompressedTexSubImage3D)
         LOAD_GL_PROC_WIN(PFNGLGENQUERIESPROC, glGenQueries)
         LOAD_GL_PROC_WIN(PFNGLDELETEQUERIESPROC, glDeleteQueries)
         LOAD_GL_PROC_WIN(PFNGLISQUERYPROC, glIsQuery)
         LOAD_GL_PROC_WIN(PFNGLBEGINQUERYPROC, glBeginQuery)
         LOAD_GL_PROC_WIN(PFNGLENDQUERYPROC, glEndQuery)
         LOAD_GL_PROC_WIN(PFNGLGETQUERYIVPROC, glGetQueryiv)
         LOAD_GL_PROC_WIN(PFNGLGETQUERYOBJECTUIVPROC, glGetQueryObjectuiv)
         LOAD_GL_PROC_WIN(PFNGLUNMAPBUFFERPROC, glUnmapBuffer)
         LOAD_GL_PROC_WIN(PFNGLGETBUFFERPOINTERVPROC, glGetBufferPointerv)
         LOAD_GL_PROC_WIN(PFNGLDRAWBUFFERSPROC, glDrawBuffers)
         LOAD_GL_PROC_WIN(PFNGLUNIFORMMATRIX2X3FVPROC, glUniformMatrix2x3fv)
         LOAD_GL_PROC_WIN(PFNGLUNIFORMMATRIX3X2FVPROC, glUniformMatrix3x2fv)
         LOAD_GL_PROC_WIN(PFNGLUNIFORMMATRIX2X4FVPROC, glUniformMatrix2x4fv)
         LOAD_GL_PROC_WIN(PFNGLUNIFORMMATRIX4X2FVPROC, glUniformMatrix4x2fv)
         LOAD_GL_PROC_WIN(PFNGLUNIFORMMATRIX3X4FVPROC, glUniformMatrix3x4fv)
         LOAD_GL_PROC_WIN(PFNGLUNIFORMMATRIX4X3FVPROC, glUniformMatrix4x3fv)
         LOAD_GL_PROC_WIN(PFNGLBLITFRAMEBUFFERPROC, glBlitFramebuffer)
         LOAD_GL_PROC_WIN(PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC, glRenderbufferStorageMultisample)
         LOAD_GL_PROC_WIN(PFNGLFRAMEBUFFERTEXTURELAYERPROC, glFramebufferTextureLayer)
         LOAD_GL_PROC_WIN(PFNGLMAPBUFFERRANGEPROC, glMapBufferRange)
         LOAD_GL_PROC_WIN(PFNGLFLUSHMAPPEDBUFFERRANGEPROC, glFlushMappedBufferRange)
         LOAD_GL_PROC_WIN(PFNGLBINDVERTEXARRAYPROC, glBindVertexArray)
         LOAD_GL_PROC_WIN(PFNGLDELETEVERTEXARRAYSPROC, glDeleteVertexArrays)
         LOAD_GL_PROC_WIN(PFNGLGENVERTEXARRAYSPROC, glGenVertexArrays)
         LOAD_GL_PROC_WIN(PFNGLISVERTEXARRAYPROC, glIsVertexArray)
         LOAD_GL_PROC_WIN(PFNGLGETINTEGERI_VPROC, glGetIntegeri_v)
         LOAD_GL_PROC_WIN(PFNGLBEGINTRANSFORMFEEDBACKPROC, glBeginTransformFeedback)
         LOAD_GL_PROC_WIN(PFNGLENDTRANSFORMFEEDBACKPROC, glEndTransformFeedback)
         LOAD_GL_PROC_WIN(PFNGLBINDBUFFERRANGEPROC, glBindBufferRange)
         LOAD_GL_PROC_WIN(PFNGLBINDBUFFERBASEPROC, glBindBufferBase)
         LOAD_GL_PROC_WIN(PFNGLTRANSFORMFEEDBACKVARYINGSPROC, glTransformFeedbackVaryings)
         LOAD_GL_PROC_WIN(PFNGLGETTRANSFORMFEEDBACKVARYINGPROC, glGetTransformFeedbackVarying)
         LOAD_GL_PROC_WIN(PFNGLVERTEXATTRIBIPOINTERPROC, glVertexAttribIPointer)
         LOAD_GL_PROC_WIN(PFNGLGETVERTEXATTRIBIIVPROC, glGetVertexAttribIiv)
         LOAD_GL_PROC_WIN(PFNGLGETVERTEXATTRIBIUIVPROC, glGetVertexAttribIuiv)
         LOAD_GL_PROC_WIN(PFNGLVERTEXATTRIBI4IPROC, glVertexAttribI4i)
         LOAD_GL_PROC_WIN(PFNGLVERTEXATTRIBI4UIPROC, glVertexAttribI4ui)
         LOAD_GL_PROC_WIN(PFNGLVERTEXATTRIBI4IVPROC, glVertexAttribI4iv)
         LOAD_GL_PROC_WIN(PFNGLVERTEXATTRIBI4UIVPROC, glVertexAttribI4uiv)
         LOAD_GL_PROC_WIN(PFNGLGETUNIFORMUIVPROC, glGetUniformuiv)
         LOAD_GL_PROC_WIN(PFNGLGETFRAGDATALOCATIONPROC, glGetFragDataLocation)
         LOAD_GL_PROC_WIN(PFNGLUNIFORM1UIPROC, glUniform1ui)
         LOAD_GL_PROC_WIN(PFNGLUNIFORM2UIPROC, glUniform2ui)
         LOAD_GL_PROC_WIN(PFNGLUNIFORM3UIPROC, glUniform3ui)
         LOAD_GL_PROC_WIN(PFNGLUNIFORM4UIPROC, glUniform4ui)
         LOAD_GL_PROC_WIN(PFNGLUNIFORM1UIVPROC, glUniform1uiv)
         LOAD_GL_PROC_WIN(PFNGLUNIFORM2UIVPROC, glUniform2uiv)
         LOAD_GL_PROC_WIN(PFNGLUNIFORM3UIVPROC, glUniform3uiv)
         LOAD_GL_PROC_WIN(PFNGLUNIFORM4UIVPROC, glUniform4uiv)
         LOAD_GL_PROC_WIN(PFNGLCLEARBUFFERIVPROC, glClearBufferiv)
         LOAD_GL_PROC_WIN(PFNGLCLEARBUFFERUIVPROC, glClearBufferuiv)
         LOAD_GL_PROC_WIN(PFNGLCLEARBUFFERFVPROC, glClearBufferfv)
         LOAD_GL_PROC_WIN(PFNGLCLEARBUFFERFIPROC, glClearBufferfi)
         LOAD_GL_PROC_WIN(PFNGLGETSTRINGIPROC, glGetStringi)
         LOAD_GL_PROC_WIN(PFNGLCOPYBUFFERSUBDATAPROC, glCopyBufferSubData)
         LOAD_GL_PROC_WIN(PFNGLGETUNIFORMINDICESPROC, glGetUniformIndices)
         LOAD_GL_PROC_WIN(PFNGLGETACTIVEUNIFORMSIVPROC, glGetActiveUniformsiv)
         LOAD_GL_PROC_WIN(PFNGLGETUNIFORMBLOCKINDEXPROC, glGetUniformBlockIndex)
         LOAD_GL_PROC_WIN(PFNGLGETACTIVEUNIFORMBLOCKIVPROC, glGetActiveUniformBlockiv)
         LOAD_GL_PROC_WIN(PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC, glGetActiveUniformBlockName)
         LOAD_GL_PROC_WIN(PFNGLUNIFORMBLOCKBINDINGPROC, glUniformBlockBinding)
         LOAD_GL_PROC_WIN(PFNGLDRAWARRAYSINSTANCEDPROC, glDrawArraysInstanced)
         LOAD_GL_PROC_WIN(PFNGLDRAWELEMENTSINSTANCEDPROC, glDrawElementsInstanced)
         LOAD_GL_PROC_WIN(PFNGLFENCESYNCPROC, glFenceSync)
         LOAD_GL_PROC_WIN(PFNGLISSYNCPROC, glIsSync)
         LOAD_GL_PROC_WIN(PFNGLDELETESYNCPROC, glDeleteSync)
         LOAD_GL_PROC_WIN(PFNGLCLIENTWAITSYNCPROC, glClientWaitSync)
         LOAD_GL_PROC_WIN(PFNGLWAITSYNCPROC, glWaitSync)
         LOAD_GL_PROC_WIN(PFNGLGETINTEGER64VPROC, glGetInteger64v)
         LOAD_GL_PROC_WIN(PFNGLGETSYNCIVPROC, glGetSynciv)
         LOAD_GL_PROC_WIN(PFNGLGETINTEGER64I_VPROC, glGetInteger64i_v)
         LOAD_GL_PROC_WIN(PFNGLGETBUFFERPARAMETERI64VPROC, glGetBufferParameteri64v)
         LOAD_GL_PROC_WIN(PFNGLGENSAMPLERSPROC, glGenSamplers)
         LOAD_GL_PROC_WIN(PFNGLDELETESAMPLERSPROC, glDeleteSamplers)
         LOAD_GL_PROC_WIN(PFNGLISSAMPLERPROC, glIsSampler)
         LOAD_GL_PROC_WIN(PFNGLBINDSAMPLERPROC, glBindSampler)
         LOAD_GL_PROC_WIN(PFNGLSAMPLERPARAMETERIPROC, glSamplerParameteri)
         LOAD_GL_PROC_WIN(PFNGLSAMPLERPARAMETERIVPROC, glSamplerParameteriv)
         LOAD_GL_PROC_WIN(PFNGLSAMPLERPARAMETERFPROC, glSamplerParameterf)
         LOAD_GL_PROC_WIN(PFNGLSAMPLERPARAMETERFVPROC, glSamplerParameterfv)
         LOAD_GL_PROC_WIN(PFNGLGETSAMPLERPARAMETERIVPROC, glGetSamplerParameteriv)
         LOAD_GL_PROC_WIN(PFNGLGETSAMPLERPARAMETERFVPROC, glGetSamplerParameterfv)
         LOAD_GL_PROC_WIN(PFNGLVERTEXATTRIBDIVISORPROC, glVertexAttribDivisor)
         LOAD_GL_PROC_WIN(PFNGLBINDTRANSFORMFEEDBACKPROC, glBindTransformFeedback)
         LOAD_GL_PROC_WIN(PFNGLDELETETRANSFORMFEEDBACKSPROC, glDeleteTransformFeedbacks)
         LOAD_GL_PROC_WIN(PFNGLGENTRANSFORMFEEDBACKSPROC, glGenTransformFeedbacks)
         LOAD_GL_PROC_WIN(PFNGLISTRANSFORMFEEDBACKPROC, glIsTransformFeedback)
         LOAD_GL_PROC_WIN(PFNGLPAUSETRANSFORMFEEDBACKPROC, glPauseTransformFeedback)
         LOAD_GL_PROC_WIN(PFNGLRESUMETRANSFORMFEEDBACKPROC, glResumeTransformFeedback)
         LOAD_GL_PROC_WIN(PFNGLGETPROGRAMBINARYPROC, glGetProgramBinary)
         LOAD_GL_PROC_WIN(PFNGLPROGRAMBINARYPROC, glProgramBinary)
         LOAD_GL_PROC_WIN(PFNGLPROGRAMPARAMETERIPROC, glProgramParameteri)
         LOAD_GL_PROC_WIN(PFNGLINVALIDATEFRAMEBUFFERPROC, glInvalidateFramebuffer)
         LOAD_GL_PROC_WIN(PFNGLINVALIDATESUBFRAMEBUFFERPROC, glInvalidateSubFramebuffer)
         LOAD_GL_PROC_WIN(PFNGLTEXSTORAGE2DPROC, glTexStorage2D)
         LOAD_GL_PROC_WIN(PFNGLTEXSTORAGE3DPROC, glTexStorage3D)
         LOAD_GL_PROC_WIN(PFNGLGETINTERNALFORMATIVPROC, glGetInternalformativ)

         LOAD_GL_PROC_WIN(PFNGLDISPATCHCOMPUTEPROC, glDispatchCompute)
         LOAD_GL_PROC_WIN(PFNGLDISPATCHCOMPUTEINDIRECTPROC, glDispatchComputeIndirect)
         LOAD_GL_PROC_WIN(PFNGLDRAWARRAYSINDIRECTPROC, glDrawArraysIndirect)
         LOAD_GL_PROC_WIN(PFNGLDRAWELEMENTSINDIRECTPROC, glDrawElementsIndirect)
         LOAD_GL_PROC_WIN(PFNGLFRAMEBUFFERPARAMETERIPROC, glFramebufferParameteri)
         LOAD_GL_PROC_WIN(PFNGLGETFRAMEBUFFERPARAMETERIVPROC, glGetFramebufferParameteriv)
         LOAD_GL_PROC_WIN(PFNGLGETPROGRAMINTERFACEIVPROC, glGetProgramInterfaceiv)
         LOAD_GL_PROC_WIN(PFNGLGETPROGRAMRESOURCEINDEXPROC, glGetProgramResourceIndex)
         LOAD_GL_PROC_WIN(PFNGLGETPROGRAMRESOURCENAMEPROC, glGetProgramResourceName)
         LOAD_GL_PROC_WIN(PFNGLGETPROGRAMRESOURCEIVPROC, glGetProgramResourceiv)
         LOAD_GL_PROC_WIN(PFNGLGETPROGRAMRESOURCELOCATIONPROC, glGetProgramResourceLocation)
         LOAD_GL_PROC_WIN(PFNGLUSEPROGRAMSTAGESPROC, glUseProgramStages)
         LOAD_GL_PROC_WIN(PFNGLACTIVESHADERPROGRAMPROC, glActiveShaderProgram)
         LOAD_GL_PROC_WIN(PFNGLCREATESHADERPROGRAMVPROC, glCreateShaderProgramv)
         LOAD_GL_PROC_WIN(PFNGLBINDPROGRAMPIPELINEPROC, glBindProgramPipeline)
         LOAD_GL_PROC_WIN(PFNGLDELETEPROGRAMPIPELINESPROC, glDeleteProgramPipelines)
         LOAD_GL_PROC_WIN(PFNGLGENPROGRAMPIPELINESPROC, glGenProgramPipelines)
         LOAD_GL_PROC_WIN(PFNGLISPROGRAMPIPELINEPROC, glIsProgramPipeline)
         LOAD_GL_PROC_WIN(PFNGLGETPROGRAMPIPELINEIVPROC, glGetProgramPipelineiv)
         LOAD_GL_PROC_WIN(PFNGLPROGRAMUNIFORM1IPROC, glProgramUniform1i)
         LOAD_GL_PROC_WIN(PFNGLPROGRAMUNIFORM2IPROC, glProgramUniform2i)
         LOAD_GL_PROC_WIN(PFNGLPROGRAMUNIFORM3IPROC, glProgramUniform3i)
         LOAD_GL_PROC_WIN(PFNGLPROGRAMUNIFORM4IPROC, glProgramUniform4i)
         LOAD_GL_PROC_WIN(PFNGLPROGRAMUNIFORM1UIPROC, glProgramUniform1ui)
         LOAD_GL_PROC_WIN(PFNGLPROGRAMUNIFORM2UIPROC, glProgramUniform2ui)
         LOAD_GL_PROC_WIN(PFNGLPROGRAMUNIFORM3UIPROC, glProgramUniform3ui)
         LOAD_GL_PROC_WIN(PFNGLPROGRAMUNIFORM4UIPROC, glProgramUniform4ui)
         LOAD_GL_PROC_WIN(PFNGLPROGRAMUNIFORM1FPROC, glProgramUniform1f)
         LOAD_GL_PROC_WIN(PFNGLPROGRAMUNIFORM2FPROC, glProgramUniform2f)
         LOAD_GL_PROC_WIN(PFNGLPROGRAMUNIFORM3FPROC, glProgramUniform3f)
         LOAD_GL_PROC_WIN(PFNGLPROGRAMUNIFORM4FPROC, glProgramUniform4f)
         LOAD_GL_PROC_WIN(PFNGLPROGRAMUNIFORM1IVPROC, glProgramUniform1iv)
         LOAD_GL_PROC_WIN(PFNGLPROGRAMUNIFORM2IVPROC, glProgramUniform2iv)
         LOAD_GL_PROC_WIN(PFNGLPROGRAMUNIFORM3IVPROC, glProgramUniform3iv)
         LOAD_GL_PROC_WIN(PFNGLPROGRAMUNIFORM4IVPROC, glProgramUniform4iv)
         LOAD_GL_PROC_WIN(PFNGLPROGRAMUNIFORM1UIVPROC, glProgramUniform1uiv)
         LOAD_GL_PROC_WIN(PFNGLPROGRAMUNIFORM2UIVPROC, glProgramUniform2uiv)
         LOAD_GL_PROC_WIN(PFNGLPROGRAMUNIFORM3UIVPROC, glProgramUniform3uiv)
         LOAD_GL_PROC_WIN(PFNGLPROGRAMUNIFORM4UIVPROC, glProgramUniform4uiv)
         LOAD_GL_PROC_WIN(PFNGLPROGRAMUNIFORM1FVPROC, glProgramUniform1fv)
         LOAD_GL_PROC_WIN(PFNGLPROGRAMUNIFORM2FVPROC, glProgramUniform2fv)
         LOAD_GL_PROC_WIN(PFNGLPROGRAMUNIFORM3FVPROC, glProgramUniform3fv)
         LOAD_GL_PROC_WIN(PFNGLPROGRAMUNIFORM4FVPROC, glProgramUniform4fv)
         LOAD_GL_PROC_WIN(PFNGLPROGRAMUNIFORMMATRIX2FVPROC, glProgramUniformMatrix2fv)
         LOAD_GL_PROC_WIN(PFNGLPROGRAMUNIFORMMATRIX3FVPROC, glProgramUniformMatrix3fv)
         LOAD_GL_PROC_WIN(PFNGLPROGRAMUNIFORMMATRIX4FVPROC, glProgramUniformMatrix4fv)
         LOAD_GL_PROC_WIN(PFNGLPROGRAMUNIFORMMATRIX2X3FVPROC, glProgramUniformMatrix2x3fv)
         LOAD_GL_PROC_WIN(PFNGLPROGRAMUNIFORMMATRIX3X2FVPROC, glProgramUniformMatrix3x2fv)
         LOAD_GL_PROC_WIN(PFNGLPROGRAMUNIFORMMATRIX2X4FVPROC, glProgramUniformMatrix2x4fv)
         LOAD_GL_PROC_WIN(PFNGLPROGRAMUNIFORMMATRIX4X2FVPROC, glProgramUniformMatrix4x2fv)
         LOAD_GL_PROC_WIN(PFNGLPROGRAMUNIFORMMATRIX3X4FVPROC, glProgramUniformMatrix3x4fv)
         LOAD_GL_PROC_WIN(PFNGLPROGRAMUNIFORMMATRIX4X3FVPROC, glProgramUniformMatrix4x3fv)
         LOAD_GL_PROC_WIN(PFNGLVALIDATEPROGRAMPIPELINEPROC, glValidateProgramPipeline)
         LOAD_GL_PROC_WIN(PFNGLGETPROGRAMPIPELINEINFOLOGPROC, glGetProgramPipelineInfoLog)
         LOAD_GL_PROC_WIN(PFNGLBINDIMAGETEXTUREPROC, glBindImageTexture)
         LOAD_GL_PROC_WIN(PFNGLGETBOOLEANI_VPROC, glGetBooleani_v)
         LOAD_GL_PROC_WIN(PFNGLMEMORYBARRIERPROC, glMemoryBarrier)
         LOAD_GL_PROC_WIN(PFNGLMEMORYBARRIERBYREGIONPROC, glMemoryBarrierByRegion)
         LOAD_GL_PROC_WIN(PFNGLTEXSTORAGE2DMULTISAMPLEPROC, glTexStorage2DMultisample)
         LOAD_GL_PROC_WIN(PFNGLGETMULTISAMPLEFVPROC, glGetMultisamplefv)
         LOAD_GL_PROC_WIN(PFNGLSAMPLEMASKIPROC, glSampleMaski)
         LOAD_GL_PROC_WIN(PFNGLBINDVERTEXBUFFERPROC, glBindVertexBuffer)
         LOAD_GL_PROC_WIN(PFNGLVERTEXATTRIBFORMATPROC, glVertexAttribFormat)
         LOAD_GL_PROC_WIN(PFNGLVERTEXATTRIBIFORMATPROC, glVertexAttribIFormat)
         LOAD_GL_PROC_WIN(PFNGLVERTEXATTRIBBINDINGPROC, glVertexAttribBinding)
         LOAD_GL_PROC_WIN(PFNGLVERTEXBINDINGDIVISORPROC, glVertexBindingDivisor)

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

        // Initialize the OpenGL extensions needed for this application.  Note that a temporary rendering context was needed to do so.
        result = LoadExtensionList();
        if (!result)
        {
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
                FilePtr file(GFileSys->OpenFile(fullPath));

                String ShaderCode = file->ReadAsString();

                mShaderCode.insert(make_pair(Util::StringToLower(fileName), ShaderCode));
            }            
        }

        void CollectShaderCode(map<String, String*>& InOutShaderCode)
        {
            for(auto& code : mShaderCode)
            {
                InOutShaderCode.insert(make_pair(code.first, &code.second));
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


    //
    // GLuint LoadShader()
    // {
    //     GLint state = 0;
    //     char error[512];
    //     string code;
    //
    //     GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    //     LoadFile(code, "vs.glsl");
    //     CompileShader(vs, code.c_str());
    //
    //     GLuint ps = glCreateShader(GL_FRAGMENT_SHADER);
    //     LoadFile(code, "ps.glsl");
    //     CompileShader(ps, code.c_str());
    //
    //     GLuint program = glCreateProgram();
    //     glAttachShader(program, vs);
    //     glAttachShader(program, ps);
    //     glDeleteShader(vs);
    //     glDeleteShader(ps);
    //     glLinkProgram(program);
    //     glGetProgramiv(program, GL_LINK_STATUS, &state);
    //     if (state != GL_TRUE)
    //     {
    //         glGetProgramInfoLog(vs, 512, NULL, error);
    //         cout << error << endl;
    //         return GL_FALSE;
    //     }
    //
    //
    //     return program;
    // }
    //
    // void APIENTRY DebugCallbackFunction(GLenum source,
    //                                     GLenum type,
    //                                     GLuint id,
    //                                     GLenum severity,
    //                                     GLsizei length,
    //                                     const GLchar* message,
    //                                     void* userParam)
    // {
    //     printf("Debug Message: SOURCE(0x%04X),"
    //         "TYPE(0x%04X),"
    //         "ID(0x%08X),"
    //         "SEVERITY(0x%04X), \"%s\"\n",
    //         source, type, id, severity, message);
    // }



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
                    InStream << line;
                }                
            }
        }

        String Build()
        {
            StringStream stream;

            stream << "#version 460" << endl;

            for(auto& marco : mMarcos)
            {
                stream << "#define " << marco.first << " " << marco.second << "\r";
            }

            IncludeCode(stream, mBaseCode);

            return stream.str();
        }


        const String& mBaseCode;
        const String& mName;
        Array<pair<string, string>> mMarcos;
        std::map<String, String*> mIncludeTargets;        
    };

    void CollectGLProgramUniformInfomation(GLuint program, BaseShader* InShader)
    {
        int numUniform = 0;        
        
        glGetProgramInterfaceiv(program, GL_UNIFORM, GL_MAX_NUM_ACTIVE_VARIABLES, &numUniform);
        GLog->Info("program active uniforms %d", numUniform);

        int numUniformBlocks = 0;
        glGetProgramInterfaceiv(program, GL_UNIFORM_BLOCK, GL_MAX_NUM_ACTIVE_VARIABLES, &numUniformBlocks);
        GLog->Info("program active uniform blocks %d", numUniformBlocks);
        
    }


    GPUShaderObject* GLRenderDevice::CreateGPUShaderObject(const ShaderCode& code, BaseShader* InShader)
    {        
        GLShaderCodeBuilder CodeBuilder(code.text, InShader->mName);
        CodeBuilder.mMarcos = code.defines;

        ShaderCodeDataBase.CollectShaderCode(CodeBuilder.mIncludeTargets);

        String buildCode = CodeBuilder.Build();

        // GLog->Info(buildCode.c_str());

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
            glAttachShader(program, glShader);
            // glDeleteShader(glShader);
            glLinkProgram(program);
            glGetProgramiv(program, GL_LINK_STATUS, &state);
            if (state != GL_TRUE)
            {
                glGetProgramInfoLog(program, 512, NULL, errorMsg);
                GLog->Error("gl link program  failed %s", InShader->mName.c_str());
                GLog->Error("%s", errorMsg);

                return nullptr;
            }

            CollectGLProgramUniformInfomation(program, InShader);
        }
        
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
        glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);

        glClearDepthf(fDepth);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);        
    }
    
    void GLRenderDevice::Present()
    {
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
        return IRenderDevice::CreateShaderParameterBuffer(InBuffer);
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
    
    struct VertexLayoutInfo
    {
        
    };
    static vector<VertexLayoutInfo*>  VertexLayouts_;
    
    
}
