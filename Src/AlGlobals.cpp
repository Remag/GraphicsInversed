#include <common.h>
#pragma hdrstop

#ifndef GIN_NO_AUDIO

#include <AlGlobals.h>
#include <AlContextManager.h>
#include <GinGlobals.h>
#include <MainFrame.h>
#include <AudioSequence.h>

namespace Gin {

namespace Audio {

//////////////////////////////////////////////////////////////////////////

CAudioListener& GetAudioListener()
{
	return *CAudioListener::GetInstance();
}

GINAPI CVector3<float> GetListenerPos()
{
	return GetAudioListener().GetPos();
}

CAlContextManager& GetAudioContextManager()
{
	return GinInternal::GetMainFrame().AlContextManager();
}

void StopAllSounds()
{
	GetAudioContextManager().StopAllRecords();
}

CAudioRecord PlaySound( CSoundView seq, CVector3<float> pos, CVector3<float> velocity, bool isLooping, TSourcePriority priority )
{
	return GetAudioContextManager().CreateRecord( seq, priority, pos, velocity, isLooping );
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Audio.

}	// namespace Gin.

#endif