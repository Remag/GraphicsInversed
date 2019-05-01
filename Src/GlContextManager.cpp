#include <common.h>
#pragma hdrstop

#include <GlContextManager.h>
#include <GinError.h>
#include <TextureData.h>
#include <GlLoad\gl_load.hpp>
#include <GlLoad\wgl_load.hpp>
#include <FaceCullSwitcher.h>
#include <DefaultSamplerContainer.h>
#include <FontRenderer.h>

namespace Gin {

const float CGlContextManager::depthZFar = 1.f;
//////////////////////////////////////////////////////////////////////////

CGlContextManager::CGlContextManager( COpenGlVersion _minVersion ) :
	minVersion( _minVersion )
{
}

CGlContextManager::~CGlContextManager()
{
	if( HasContext() ) {
		DestroyContext();
	}
}

void CGlContextManager::CreateContext( HDC dc )
{
	assert( !HasContext() );
	createContext( dc );
	loadFunctions( dc );
	samplerContainer = CreateOwner<CDefaultSamplerContainer>();
	enableFaceCulling();
	enableDepthTesting();
	setPixelAlignment();
	CheckGlError();
	CFontRenderer::InitializeShaderData();
}

void CGlContextManager::SetContextTarget( HDC newDc )
{
	assert( HasContext() );
	::wglMakeCurrent( newDc, renderContextHanle );
}

void CGlContextManager::createContext( HDC dc )
{
	assert( dc != 0 );

	// Set the pixel format.
	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory( &pfd, sizeof( pfd ) );
	pfd.nSize = sizeof( pfd );
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;
	pfd.cStencilBits = 8;
	pfd.iLayerType = PFD_MAIN_PLANE;
	const int formatIndex = ::ChoosePixelFormat( dc, &pfd );
	checkLastError( formatIndex != 0 );
	checkLastError( ::SetPixelFormat( dc, formatIndex, &pfd ) != 0 );

	// Create the rendering context and set is as current.
	renderContextHanle = ::wglCreateContext( dc );
	checkLastError( renderContextHanle != 0 );
	checkLastError( ::wglMakeCurrent( dc, renderContextHanle ) != 0 );

	updateToExtendedContext( dc );
}

typedef HGLRC ( WINAPI *TWglCreateContextProc )( HDC, HGLRC, const int * );
typedef const char * ( WINAPI *TWglGetExtensionStrProc )( HDC hdc );
static const int wglContextMajorVersionId = 0x2091;
static const int wglContextMinorVersionId = 0x2092;

static const int wglContextFlagsId = 0x2094;
static const int wglContextDebugBit = 1;

static const int wglContextProfileId = 0x9126;
static const int wglContextCoreProfileBit = 1;
void CGlContextManager::updateToExtendedContext( HDC dc )
{
	TWglCreateContextProc wglCreateContextAttribsARB;

	// Check that the extended context creation is supported.
	TWglGetExtensionStrProc wglGetExtensionsStringARB = reinterpret_cast<TWglGetExtensionStrProc>( wglGetProcAddress( "wglGetExtensionsStringARB" ) );
	assert( wglGetExtensionsStringARB != 0 );
	const CStringView extensionsStr = wglGetExtensionsStringARB( dc );
	assert( !extensionsStr.IsEmpty() );
	assert( extensionsStr.Find( "WGL_ARB_create_context" ) != NotFound );

	// Fill the attributes.
#ifdef _DEBUG
	int attributeList[] = {
		wglContextMajorVersionId, minVersion.Major,
		wglContextMinorVersionId, minVersion.Minor,
		wglContextFlagsId, wglContextDebugBit,
		0
	};
#else
	int attributeList[] = {
		wglContextMajorVersionId, minVersion.Major,
		wglContextMinorVersionId, minVersion.Minor,
		0
	};
#endif
	
	// Request the context creation function.
	wglCreateContextAttribsARB = reinterpret_cast<TWglCreateContextProc>( wglGetProcAddress( "wglCreateContextAttribsARB" ) );
	assert( wglCreateContextAttribsARB != 0 );

	// Create an extended context with the given attributes.
	HGLRC newContext = wglCreateContextAttribsARB( dc, 0, attributeList );
	checkGlGeneralError( newContext != nullptr );
	wglMakeCurrent( dc, 0 );
	wglDeleteContext( renderContextHanle );
	renderContextHanle = newContext;
	checkLastError( wglMakeCurrent( dc, renderContextHanle ) != 0 );
}

void CGlContextManager::loadFunctions( HDC dc )
{
	if( !glFunctionsLoaded ) {
		glload::LoadTest loadResult = glload::LoadFunctions();
		assert( loadResult );
		assert( glload::IsVersionGEQ( minVersion.Major, minVersion.Minor ) );
	
		glload::LoadTest wglLoadResult = glload::LoadFunctions( dc );
		assert( wglLoadResult );
		glFunctionsLoaded = true;
	}
}

void CGlContextManager::DestroyContext()
{
	assert( HasContext() );
	CFontRenderer::ClearShaderData();
	samplerContainer.Release();
	checkLastError( wglMakeCurrent( 0, 0 ) != 0 );
	checkLastError( wglDeleteContext( renderContextHanle ) != 0 );
	renderContextHanle = nullptr;
}

// Enable the option for the triangles to be culled if they are facing away from the camera.
// The default winding order is counter clockwise.
void CGlContextManager::enableFaceCulling()
{
	gl::Enable( gl::CULL_FACE );
	gl::CullFace( gl::BACK );
	SetTriangleWindingOrder( TWO_CounterClockwise );
}

void CGlContextManager::enableDepthTesting()
{
	gl::Enable( gl::DEPTH_TEST );
	gl::DepthMask( gl::TRUE_ );
	gl::DepthFunc( gl::LEQUAL );
	gl::DepthRange( 0.0f, GetDepthZFar() );
}

// Texture rows are assumed to be tightly packed.
void CGlContextManager::setPixelAlignment()
{
	gl::PixelStorei( gl::UNPACK_ALIGNMENT, 1 );
	gl::PixelStorei( gl::PACK_ALIGNMENT, 1 );
}

extern const CError Err_GeneralGlError;
void CGlContextManager::checkGlGeneralError( bool condition )
{
	if( !condition ) {
		const int errorCode = ::GetLastError();
		check( false, Err_GeneralGlError, errorCode );
	}
}

void CGlContextManager::SetTriangleWindingOrder( TTriangleWindingOrder newValue )
{
	gl::FrontFace( numeric_cast<GLenum>( newValue ) );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.


