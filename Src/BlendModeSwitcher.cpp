#include <common.h>
#pragma hdrstop

#include <BlendModeSwitcher.h>
#include <Gindefs.h>

namespace Gin {

bool CBlendModeEnabler::isEnabled = false;
//////////////////////////////////////////////////////////////////////////

CBlendModeEnabler::CBlendModeEnabler( bool newValue ) :
	prevMode( isEnabled )
{
	SetBlendModeEnabled( newValue );
}

CBlendModeEnabler::~CBlendModeEnabler()
{
	SetBlendModeEnabled( prevMode );
}

void CBlendModeEnabler::SetBlendModeEnabled( bool isSet )
{
	if( isEnabled && !isSet ) {
		isEnabled = isSet;
		gl::Disable( gl::BLEND );
	} else if( !isEnabled && isSet ) {
		isEnabled = isSet;
		gl::Enable( gl::BLEND );
	}
}

//////////////////////////////////////////////////////////////////////////

TBlendFunction CBlendModeSwitcher::currentSrcBlend = BF_One;
TBlendFunction CBlendModeSwitcher::currentDestBlend = BF_One;

CBlendModeSwitcher::CBlendModeSwitcher( TBlendFunction srcBlend, TBlendFunction destBlend ) :
	enabler( true ),
	prevSrcBlend( currentSrcBlend ),
	prevDestBlend( currentDestBlend )
{
	SetBlendMode( srcBlend, destBlend );
}

CBlendModeSwitcher::~CBlendModeSwitcher()
{
	SetBlendMode( prevSrcBlend, prevDestBlend );
}

void CBlendModeSwitcher::SetBlendMode( TBlendFunction srcBlend, TBlendFunction destBlend, bool isEnabled )
{
	CBlendModeEnabler::SetBlendModeEnabled( isEnabled );
	SetBlendMode( srcBlend, destBlend );
}

void CBlendModeSwitcher::SetBlendMode( TBlendFunction srcBlend, TBlendFunction destBlend )
{
	gl::BlendFunc( srcBlend, destBlend );
	currentSrcBlend = srcBlend;
	currentDestBlend = destBlend;
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.
