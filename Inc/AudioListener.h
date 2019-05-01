#pragma once
#include <Gindefs.h>

namespace Gin {

#ifndef GIN_NO_AUDIO

namespace Audio {

//////////////////////////////////////////////////////////////////////////

// Listener of the audio. All the audio is represented according to the listener.
class GINAPI CAudioListener : public CSingleton<CAudioListener> {
public:
	CAudioListener();

	const CVector3<float>& GetPos() const
		{ return position; }
	const CVector3<float>& GetDir() const
		{ return direction; }
	const CVector3<float>& GetUpVector() const
		{ return upVector; }
	const CVector3<float>& GetVelocity() const
		{ return velocity; }

	void SetPos( const CVector3<float>& newValue );
	void SetDir( const CVector3<float>& newDir, const CVector3<float>& newUpVecotr );
	void SetVelocity( const CVector3<float>& newValue );

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

#endif

}	// namespace Gin.

