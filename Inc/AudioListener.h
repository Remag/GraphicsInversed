#pragma once

#ifndef GIN_NO_AUDIO

#include <Gindefs.h>

namespace Gin {

namespace Audio {

//////////////////////////////////////////////////////////////////////////

// Listener of the audio. All the audio is represented according to the listener.
class GINAPI CAudioListener : public CSingleton<CAudioListener> {
public:
	CAudioListener();

	CVector3<float> GetPos() const
		{ return position; }
	CVector3<float> GetDir() const
		{ return direction; }
	CVector3<float> GetUpVector() const
		{ return upVector; }
	CVector3<float> GetVelocity() const
		{ return velocity; }

	void SetPos( CVector3<float> newValue );
	void SetDir( CVector3<float> newDir, CVector3<float> newUpVecotr );
	void SetVelocity( CVector3<float> newValue );

private:
	CVector3<float> position;
	CVector3<float> direction;
	CVector3<float> upVector;
	CVector3<float> velocity;

	void checkLastAudioError() const;

	// Copying is prohibited.
	CAudioListener( CAudioListener& ) = delete;
	void operator=( CAudioListener& ) = delete;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Audio.

}	// namespace Gin.

#endif

