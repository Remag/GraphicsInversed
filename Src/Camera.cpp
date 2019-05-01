#include <common.h>
#pragma hdrstop

#include <Camera.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

CCamera::CCamera() :
	zNear( 0.f ),
	zFar( 0.f ),
	aspectRatio( 0.f )
{
	projectionMatrix( 2, 3 ) = -1.f;
}

bool CCamera::IsInitialized() const
{
	return GetZNear() != 0 && GetZFar() != 0 && GetFrustumScale() != 0 && GetAspectRatio() != 0 && !GetOrientation().IsNull();
}

CVector3<float> CCamera::GetDir() const
{
	CVector3<float> result;
	result[0] = -cameraMatrix( 0, 2 );
	result[1] = -cameraMatrix( 1, 2 );
	result[2] = -cameraMatrix( 2, 2 );
	return result;
}

void CCamera::SetViewPort( CAARect<int> viewRect )
{
	const int width = viewRect.Width();
	const int height = viewRect.Height();
	gl::Viewport( viewRect.Left(), viewRect.Top(), width, height );
	SetAspectRatio( ( 1.f * width ) / height );
}

void CCamera::SetPos( CVector3<float> newValue )
{
	pos = newValue;
	recalcCameraMatrix();
	recalcWorldToClipMatrix();
}

void CCamera::SetOrientation( CQuaternion<float> newOrientation )
{
	orientation = newOrientation.Normalize();

	recalcCameraMatrix();
	recalcWorldToClipMatrix();
}

void CCamera::setCameraData( CVector3<float> newPos, CQuaternion<float> newOrientation )
{
	pos = newPos;
	orientation = newOrientation.Normalize();
	recalcCameraMatrix();
}

void CCamera::recalcCameraMatrix()
{
	cameraMatrix = orientation.MatrixForm();
	const CVector3<float> oppositePos = -pos;

	cameraMatrix( 3, 0 ) = cameraMatrix( 0, 0 ) * oppositePos.X() + cameraMatrix( 1, 0 ) * oppositePos.Y() + cameraMatrix( 2, 0 ) * oppositePos.Z();
	cameraMatrix( 3, 1 ) = cameraMatrix( 0, 1 ) * oppositePos.X() + cameraMatrix( 1, 1 ) * oppositePos.Y() + cameraMatrix( 2, 1 ) * oppositePos.Z();
	cameraMatrix( 3, 2 ) = cameraMatrix( 0, 2 ) * oppositePos.X() + cameraMatrix( 1, 2 ) * oppositePos.Y() + cameraMatrix( 2, 2 ) * oppositePos.Z();
}

void CCamera::SetZNear( float newValue )
{
	zNear = newValue;
	recalcProjectionZBounds();
	recalcWorldToClipMatrix();
}

void CCamera::SetZFar( float newValue )
{
	zFar = newValue;
	recalcProjectionZBounds();
	recalcWorldToClipMatrix();
}

void CCamera::recalcProjectionZBounds()
{
	const float zDeltaInv = 1.f / ( zNear - zFar );
	const float zSum = zNear + zFar;
	const float zMult = 2 * zNear * zFar;

	projectionMatrix( 2, 2 ) = zSum * zDeltaInv;
	projectionMatrix( 3, 2 ) = zMult * zDeltaInv;
}

void CCamera::SetFrustumScale( float newValue )
{
	projectionMatrix( 0, 0 ) = newValue / GetAspectRatio();
	projectionMatrix( 1, 1 ) = newValue;
	recalcWorldToClipMatrix();
}

void CCamera::SetAspectRatio( float newValue )
{
	aspectRatio = newValue;
	projectionMatrix( 0, 0 ) = GetFrustumScale() / GetAspectRatio();
	recalcWorldToClipMatrix();
}

void CCamera::SetProjectionParams( float _zNear, float _zFar, float _frustumScale, float _aspectRatio )
{
	setProjectionData( _zNear, _zFar, _frustumScale, _aspectRatio );
	recalcWorldToClipMatrix();
}

void CCamera::setProjectionData( float _zNear, float _zFar, float _frustumScale, float _aspectRatio )
{
	zNear = _zNear;
	zFar = _zFar;
	recalcProjectionZBounds();
	aspectRatio = _aspectRatio;
	projectionMatrix( 1, 1 ) = _frustumScale;
	projectionMatrix( 0, 0 ) = GetFrustumScale() / GetAspectRatio();
}

void CCamera::SetCameraParams( const CVector3<float>& newPos, const CQuaternion<float>& newOrientation, float _zNear, float _zFar, float _frustumScale, float _aspectRatio )
{
	setProjectionData( _zNear, _zFar, _frustumScale, _aspectRatio );
	setCameraData( newPos, newOrientation );
	recalcWorldToClipMatrix();
}

void CCamera::recalcWorldToClipMatrix()
{
	worldToClipMatrix = projectionMatrix * cameraMatrix;
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

