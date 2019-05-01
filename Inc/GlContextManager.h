#pragma once
#include <Gindefs.h>

namespace Gin {

enum TTriangleWindingOrder;
//////////////////////////////////////////////////////////////////////////

// Requested version of the context.
struct COpenGlVersion {
	int Major;
	int Minor;

	COpenGlVersion( int _major, int _minor ) : Major( _major ), Minor( _minor ) {}
};

// Class for managing OpenGL rendering context.
class GINAPI CGlContextManager : public CSingleton<CGlContextManager> {
public:
	explicit CGlContextManager( COpenGlVersion _minVersion );
	~CGlContextManager();

	const CDefaultSamplerContainer& GetSamplerContainer()
		{ return *samplerContainer; }

	// Is the context created.
	bool HasContext() const
		{ return renderContextHanle != nullptr; }
	// Create the context for the given window.
	void CreateContext( HDC dc );
	// Change a device context that is connected to the rendering context.
	void SetContextTarget( HDC newDc );
	// Delete the context.
	void DestroyContext();

	// Set the winding order for the triangles.
	// Triangles that are facing away from the camera will be culled.
	void SetTriangleWindingOrder( TTriangleWindingOrder newValue );

	float GetDepthZFar() const
		{ return depthZFar; }

private:
	// Native handle to the rendering context.
	HGLRC renderContextHanle = nullptr;
	COpenGlVersion minVersion;
	bool glFunctionsLoaded = false;

	// Container for default texture samplers.
	CPtrOwner<CDefaultSamplerContainer> samplerContainer;

	void createContext( HDC dc );
	void updateToExtendedContext( HDC dc );
	void loadFunctions( HDC dc );
	void enableFaceCulling();
	void enableDepthTesting();
	void setPixelAlignment();

	void checkGlGeneralError( bool condition );

	// Far point for the depth test range.
	static const float depthZFar;

	// Copying is prohibited.
	CGlContextManager( const CGlContextManager& ) = delete;
	void operator=( const CGlContextManager& ) = delete;
};

}	// namespace Gin.

