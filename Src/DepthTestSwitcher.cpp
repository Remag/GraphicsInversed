#include <common.h>
#pragma hdrstop

#include <DepthTestSwitcher.h>

namespace Gin {

bool CDepthTestSwitcher::isTestEnabled = true;
//////////////////////////////////////////////////////////////////////////

CDepthTestSwitcher::CDepthTestSwitcher( bool enableTesting )
	: prevTestEnabled( isTestEnabled )
{
	doEnableTesting( enableTesting );
}

CDepthTestSwitcher::~CDepthTestSwitcher()
{
	doEnableTesting( prevTestEnabled );
}

void CDepthTestSwitcher::doEnableTesting( bool enableTesting )
{
	isTestEnabled = enableTesting;

	if( enableTesting ) {
		gl::Enable( gl::DEPTH_TEST );
	} else {
		gl::Disable( gl::DEPTH_TEST );
	}
}

//////////////////////////////////////////////////////////////////////////

}	 // namespace Gin.