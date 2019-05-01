#include <common.h>
#pragma hdrstop

#include <AlGlobals.h>
#include <AlContextManager.h>
#include <GinGlobals.h>
#include <MainFrame.h>
#include <AudioSequence.h>

namespace Gin {

#ifndef GIN_NO_AUDIO

namespace Audio {

//////////////////////////////////////////////////////////////////////////

CAudioListener& GetAudioListener()
{
	return *CAudioListener::GetInstance();
}

CAlContextManager& GetAudioContextManager()
{
	return GinInternal::GetMainFrame().AlContextManager();
}

CAudioRecord PlaySound( CSoundView seq, CVector3<float> pos, CVector3<float> velocity, bool isLooping, TSourcePriority priority )
{
	return GetAudioContextManager().CreateRecord( seq, priority, pos, velocity, isLooping );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Audio.

#endif

}	// namespace Gin.
