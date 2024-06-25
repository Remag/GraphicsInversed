#pragma once
#include <GinDefs.h>
#include <State.h>

namespace Gin {

// Information about frame execution.
struct CFrameInformation {
	TTime Step = 0.0f;
	bool RunDraw = false;
	bool RunUpdate = false;

	CFrameInformation() = default;
	CFrameInformation( TTime step, bool runDraw, bool runUpdate ) : Step( step ), RunDraw( runDraw ), RunUpdate( runUpdate ) {}
};

//////////////////////////////////////////////////////////////////////////

// Manager of application updates. 
class GINAPI CEngine {
public:
	CEngine();
	virtual ~CEngine() {}

	// Get the game time passed in seconds since the last call to this function.
	// Application will update and draw the state with the returned step time.
	virtual CFrameInformation AdvanceFrame() = 0;

	// Get time at the beginning of the last update.
	// Time is returned in QueryPerformanceCounter units.
	long long GetLastUpdateStartTime() const
		{ return lastUpdateTime; }
	long long GetLastDrawStartTime() const
		{ return lastDrawTime; }
	// Resolution of the performance counter in performance counter units per second.
	long long GetCounterResolution() const
		{ return counterResolution; }

protected:
	// Set the last update time to a new value.
	void setLastUpdateTime( long long newValue )
		{ lastUpdateTime = newValue; }
	void setLastDrawTime( long long newValue )
		{ lastDrawTime = newValue; }

	// Find precise current time.
	static long long getCurrentTime();

private:
	long long counterResolution = 0;
	long long lastUpdateTime = 0;
	long long lastDrawTime = 0;
};

//////////////////////////////////////////////////////////////////////////

// Fixed step engine implementation. 
class GINAPI CFixedStepEngine : public CEngine {
public:
	explicit CFixedStepEngine( int maxFPS );

	// IEngine.
	virtual CFrameInformation AdvanceFrame() override final;

	// Set the FPS cap in milliseconds.
	void SetMaxFPS( int newValue );

private:
	// Size of the fixed step in seconds.
	TTime stepSize;
	// Step size in QueryPerformanceCounter units.
	unsigned stepSizePerfUnits;
};

//////////////////////////////////////////////////////////////////////////

// Unrestricted FPS engine implementation.
class GINAPI CRealTimeStepEngine : public CEngine {
public:
	CRealTimeStepEngine();

	// IEngine.
	virtual CFrameInformation AdvanceFrame() override final;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.