#pragma once
#include <GinDefs.h>
#include <AudioUtils.h>

namespace Gin {

#ifndef GIN_NO_AUDIO

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

GINAPI CAudioRecord PlaySound( CSoundView seq, CVector3<float> pos, 
	CVector3<float> velocity = CVector3<float>{}, bool isLooping = false, TSourcePriority priority = SP_LowPriority );

//////////////////////////////////////////////////////////////////////////

}	// namespace Audio.

#endif

}	// namespace Gin.

