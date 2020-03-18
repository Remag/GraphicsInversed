#pragma once

#ifndef GIN_NO_AUDIO

#include <GinDefs.h>
#include <AudioUtils.h>

namespace Gin {

namespace Audio {

class CAudioRecord;
class CSoundView;
class CAlContextManager;
class CAudioListener;
//////////////////////////////////////////////////////////////////////////

// Context manager.
GINAPI CAlContextManager& GetAudioContextManager();
// Audio listener.
GINAPI CAudioListener& GetAudioListener();
// Listener position.
GINAPI CVector3<float> GetListenerPos();

GINAPI void StopAllSounds();
GINAPI CAudioRecord PlaySound( CSoundView seq, CVector3<float> pos = CVector3<float>{}, 
	CVector3<float> velocity = CVector3<float>{}, bool isLooping = false, TSourcePriority priority = SP_LowPriority );

//////////////////////////////////////////////////////////////////////////

}	// namespace Audio.

}	// namespace Gin.

#endif

