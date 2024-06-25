#include <common.h>
#pragma hdrstop

#include <Engine.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

CEngine::CEngine()
{
	LARGE_INTEGER largeIntResolution;
	QueryPerformanceFrequency( &largeIntResolution );
	counterResolution = largeIntResolution.QuadPart;

	setLastUpdateTime( getCurrentTime() );
}

long long CEngine::getCurrentTime()
{
	LARGE_INTEGER count;
	QueryPerformanceCounter( &count );
	return count.QuadPart;
}

//////////////////////////////////////////////////////////////////////////

CFixedStepEngine::CFixedStepEngine( int maxFPS )
{
	SetMaxFPS( maxFPS );
}

void CFixedStepEngine::SetMaxFPS( int newValue )
{
	assert( newValue > 0 );

	stepSize = 1.0f / newValue;

	stepSizePerfUnits = static_cast<unsigned>( Round( stepSize * GetCounterResolution() ) );
}

CFrameInformation CFixedStepEngine::AdvanceFrame()
{
	const long long lastFrameTime = GetLastUpdateStartTime();
	const long long currentTicks = getCurrentTime();
	const unsigned timeSinceUpdate = static_cast<unsigned>( currentTicks - lastFrameTime );
	if( timeSinceUpdate > stepSizePerfUnits ) {
		const int frameDelay = timeSinceUpdate % stepSizePerfUnits;
		const auto newTime = currentTicks - frameDelay;
		setLastUpdateTime( newTime );
		setLastDrawTime( newTime );
		return CFrameInformation{ stepSize, true, true };
	}

	return CFrameInformation{ 0.0f, false, false };
}

//////////////////////////////////////////////////////////////////////////

CRealTimeStepEngine::CRealTimeStepEngine()
{
	const long long currentTicks = getCurrentTime();
	setLastUpdateTime( currentTicks );
	setLastDrawTime( currentTicks );
}

CFrameInformation CRealTimeStepEngine::AdvanceFrame()
{
	const long long lastFrameTime = GetLastUpdateStartTime();
	const long long currentTicks = getCurrentTime();
	setLastUpdateTime( currentTicks );
	setLastDrawTime( currentTicks );
	const unsigned timeSinceUpdate = static_cast<unsigned>( currentTicks - lastFrameTime );
	const auto step = timeSinceUpdate * 1.0f / GetCounterResolution();
	return CFrameInformation( step, true, true );
}

//////////////////////////////////////////////////////////////////////////

}	 // namespace Gin.
