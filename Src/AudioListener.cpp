#include <common.h>
#pragma hdrstop

#include <AudioListener.h>
#include <AlContextManager.h>
#include <AlGlobals.h>

namespace Gin {

#ifndef GIN_NO_AUDIO

namespace Audio {


//////////////////////////////////////////////////////////////////////////

CAudioListener::CAudioListener() :
	direction( 0.f, 0.f, -1.f ),
	upVector( 0.f, 1.f, 0.f )
{

}

void CAudioListener::SetPos( const CVector3<float>& newValue )
{
	assert( GetAudioContextManager().HasContext() );
	alListenerfv( AEP_Position, newValue.Ptr() );
	checkLastAudioError();
	position = newValue;
}

void CAudioListener::checkLastAudioError() const
{
	assert( alGetError() == AL_NO_ERROR );
}

void CAudioListener::SetDir( const CVector3<float>& newDir, const CVector3<float>& newUpVecotr )
{
	assert( GetAudioContextManager().HasContext() );
	CVector<float, 6> orientation( newDir, newUpVecotr );
	alListenerfv( AEP_Orientation, orientation.Ptr() );
	checkLastAudioError();
	direction = newDir;
	upVector = newUpVecotr;
}

void CAudioListener::SetVelocity( const CVector3<float>& newValue )
{
	assert( GetAudioContextManager().HasContext() );
	alListenerfv( AEP_Velocity, newValue.Ptr() );
	checkLastAudioError();
	velocity = newValue;
}

//////////////////////////////////////////////////////////////////////////

} // namespace Audio.

#endif

}	// namespace Gin.

