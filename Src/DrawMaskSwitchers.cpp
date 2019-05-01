#include <common.h>
#pragma hdrstop

#include <DrawMaskSwitchers.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////
bool CColorMaskSwitcher::currentValue = true;

CColorMaskSwitcher::CColorMaskSwitcher( bool newValue ) :
	prevValue( currentValue )
{
	gl::ColorMask( newValue, newValue, newValue, newValue );
	currentValue = newValue;
}

CColorMaskSwitcher::~CColorMaskSwitcher()
{
	gl::ColorMask( prevValue, prevValue, prevValue, prevValue );
	currentValue = prevValue;
}

//////////////////////////////////////////////////////////////////////////
bool CDepthMaskSwitcher::currentValue = true;

CDepthMaskSwitcher::CDepthMaskSwitcher( bool newValue ) :
	prevValue( currentValue )
{
	gl::DepthMask( newValue );
	currentValue = newValue;
}

CDepthMaskSwitcher::~CDepthMaskSwitcher()
{
	gl::DepthMask( prevValue );
	currentValue = prevValue;
}

//////////////////////////////////////////////////////////////////////////
bool CStencilMaskSwitcher::currentValue = true;

CStencilMaskSwitcher::CStencilMaskSwitcher( bool newValue ) :
	prevValue( currentValue )
{
	gl::StencilMask( newValue );
	currentValue = newValue;
}

CStencilMaskSwitcher::~CStencilMaskSwitcher()
{
	gl::StencilMask( prevValue );
	currentValue = prevValue;
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.
