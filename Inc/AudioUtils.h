#pragma once

namespace Gin {

namespace Audio {

//////////////////////////////////////////////////////////////////////////

enum TAudioEntityParameter {
	AEP_Position = 0x1004,	// AL_POSITION
	AEP_Orientation = 0x100F,	// AL_ORIENTATION
	AEP_Velocity = 0x1006	// AL_VELOCITY
};

// Source priority. High priority audio sources will not be replaced on source overflow.
// Most of the sounds are supposed to be low priority, the contexts throws on assertion if too many high priority sounds ( ~15 ) are playing at the same time.
enum TSourcePriority {
	SP_LowPriority,
	SP_HighPriority
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Audio.

}	// namespace Gin.

