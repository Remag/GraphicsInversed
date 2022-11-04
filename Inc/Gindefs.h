#pragma once

#ifdef _WIN64
#define PLATFORM_SUFFIX "64"
#else
#define PLATFORM_SUFFIX ""
#endif

#define GIN_SUFFIX PLATFORM_SUFFIX

#if defined( GINBUILD_DYNAMIC )
#define GINAPI __declspec( dllexport )
#elif defined( GINBUILD ) || defined( USE_STATIC_GIN )
#define GINAPI
#else
#define GINAPI __declspec( dllimport )
#endif

#ifndef GINBUILD
#pragma comment( lib, "GraphicsInversed" GIN_SUFFIX ".lib" )
#endif

#if defined( GINBUILD ) || defined( USE_STATIC_GIN )
#pragma comment( lib, "OpenGL32.lib" )
#pragma comment( lib, "glload.lib" )
#pragma comment( lib, "FreeType.lib" )

#ifndef GIN_NO_AUDIO
#pragma comment( lib, "libOpenAL" GIN_SUFFIX ".dll.a" )
#pragma comment( lib, "libVorbis" GIN_SUFFIX ".lib" )
#pragma comment( lib, "libVorbisfile" GIN_SUFFIX ".lib" )
#pragma comment( lib, "libOGG" GIN_SUFFIX ".lib" )
#include <vorbis\vorbisfile.h>
#include <OpenAl\al.h>
#endif

#include <GlLoad\gl_core.hpp>
#include <GlLoad\wgl_all.hpp>
#endif

#include <Redefs.h>

namespace Gin {
	enum TWindowRendererType;
	enum TTextureBindingTarget;
	enum TTextureGlFormat;
	enum TTexelFormat;
	enum TBufferType;

	class CApplication;
	class CMainFrame;
	class CGlWindow;
	class CGlContextManager;
	class CStateManager;

	class CAdditionalWindowContainer;
	class CInputHandler;
	class CInputTranslator;
	class CControlScheme;
	class CInputBinding;
	class CSystemInputBinding;
	class IInputController;
	class IRenderMechanism;
	class IState;

	class CVertexShader;
	class CFragmentShader;
	class CShader;
	class CShaderProgram;
	class CShaderProgramOwner;
	class CMesh;
	class CModel;
	class CCamera;
	class CImageData;
	class CUniformLocation;
	class CFramebuffer; 
	class CDefaultSamplerContainer;

	class CFontView;
	class CFontEdit;
	class CFontOwner;
	class CFontSizeView;
	class CFontSizeOwner;
	class CGlyph;
	class IGlyphProvider;

	template <class Dispatcher>
	class CWindowClass;
	template <TBufferType target>
	class CRawGlBuffer;
	template <TBufferType target, class... Types>
	class CGlBuffer;
	template <TBufferType target, class... Types>
	class CGlBufferOwner;

	template <TTextureBindingTarget target, TTextureGlFormat format>
	class CTexture;
	template <TTextureBindingTarget target, TTextureGlFormat format>
	class CEditableTexture;
	template <TTextureBindingTarget target, TTextureGlFormat format>
	class CTextureOwner;
	template <TTextureBindingTarget target>
	class CTypelessTexture;
	template <TTextureBindingTarget target>
	class CEditableTypelessTexture;

	template <class... ComponentClasses>
	class CUniformFilterOwner;
	template <class ComponentClass>
	class CUniformFilter;

	typedef IAction<void()> TUserAction;

	namespace Audio {
		class CAlContextManager;
	}

	namespace GinInternal {
		template <class... Types>
		class CUniformBlockBase;
		class CUniformBase;
		class CTextureData;

		template <TTextureGlFormat format, TTextureBindingTarget target>
		class CConstTextureBase;
		template <TTextureGlFormat format, TTextureBindingTarget target>
		class CEditTextureBase;
		template <TTextureBindingTarget target>
		class CTypelessTextureOperations;

		class CActionListInputController;
	}
}

using namespace Gin;