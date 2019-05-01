#include <common.h>
#pragma hdrstop

#include <FaceCullSwitcher.h>
#include <GinGlobals.h>
#include <GlContextManager.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////
bool CFaceCullEnabler::currentIsEnabled = false;

CFaceCullEnabler::CFaceCullEnabler( bool isEnabled ) :
	prevIsEnabled( currentIsEnabled )
{
	enableFaceCulling( isEnabled );
	currentIsEnabled = isEnabled;
}

CFaceCullEnabler::~CFaceCullEnabler()
{
	enableFaceCulling( prevIsEnabled );
	currentIsEnabled = prevIsEnabled;
}

void CFaceCullEnabler::enableFaceCulling( bool isEnabled )
{
	if( isEnabled ) {
		gl::Enable( gl::CULL_FACE );
		gl::CullFace( gl::BACK );
	} else {
		gl::Disable( gl::CULL_FACE );
	}
}

//////////////////////////////////////////////////////////////////////////
TTriangleWindingOrder CFaceCullSwitcher::currentOrder = TWO_CounterClockwise;

CFaceCullSwitcher::CFaceCullSwitcher( TTriangleWindingOrder windingOrder ) :
	enabler( true ),
	prevOrder( currentOrder )
{
	GetGlContextManager().SetTriangleWindingOrder( windingOrder );
	currentOrder = windingOrder;
}

CFaceCullSwitcher::~CFaceCullSwitcher()
{
	GetGlContextManager().SetTriangleWindingOrder( prevOrder );
	currentOrder = prevOrder;
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.
