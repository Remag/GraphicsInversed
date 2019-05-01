#pragma once
#include <Gindefs.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

// Camera in the world. Has position, direction and projection parameters.
class GINAPI CCamera {
public:
	// Default constructor doesn't initialize the camera. All the parameters must be set before the camera is usable.
	CCamera();
	CCamera( const CCamera& ) = default;

	bool IsInitialized() const;

	const CVector3<float>& GetPos() const
		{ return pos; }
	const CQuaternion<float>& GetOrientation() const
		{ return orientation; }
	CVector3<float> GetDir() const;

	float GetZNear() const
		{ return zNear; }
	float GetZFar() const
		{ return zFar; }
	float GetFrustumScale() const
		{ return projectionMatrix( 1, 1 ); }
	float GetAspectRatio() const
		{ return aspectRatio; }
	
	// Camera transformation matrices.
	const CMatrix<float, 4, 4>& GetCameraMatrix() const
		{ return cameraMatrix; }
	const CMatrix<float, 4, 4>& GetProjectionMatrix() const
		{ return projectionMatrix; }
	const CMatrix<float, 4, 4>& GetWorldToClipMatrix() const
		{ return worldToClipMatrix; }

	// Project the camera on the given rectangle and adjust camera's aspect ratio.
	// Equivalent of calling gl::Viewport and SetAspectRatio.
	void SetViewPort( CAARect<int> viewRect );

	// Methods that change camera params.
	// The uniform buffer object is set right after the parameter is changed.
	// Therefore it's recommended to set as many parameters at once as possible.

	void SetPos( CVector3<float> pos );
	void SetOrientation( CQuaternion<float> newValue );

	void SetZNear( float newValue );
	void SetZFar( float newValue );
	void SetFrustumScale( float newValue );
	void SetAspectRatio( float newValue );
	void SetProjectionParams( float zNear, float zFar, float frustumScale, float aspectRatio );

	void SetCameraParams( const CVector3<float>& newPos, const CQuaternion<float>& newOrientation, float zNear, float zFar, float frustumScale, float aspectRatio );

private:
	// Position in world coordinates.
	CVector3<float> pos;
	// Orientation.
	// Camera with a no-op orientation is facing the -Z axis and uses the +Y axis as its up vector.
	CQuaternion<float> orientation;

	// Z boundaries.
	float zNear;
	float zFar;

	// Frustum aspect ratio.
	float aspectRatio;

	// Camera transformation matrix.
	CMatrix<float, 4, 4> cameraMatrix;
	// Matrix of the perspective projection.
	CMatrix<float, 4, 4> projectionMatrix;
	// Combination of two previous matrices.
	CMatrix<float, 4, 4> worldToClipMatrix;

	void setCameraData( CVector3<float> newPos, CQuaternion<float> newOrientation );
	void setProjectionData( float zNear, float zFar, float frustumScale, float aspectRatio );

	void recalcProjectionZBounds();
	void recalcCameraMatrix();
	void recalcWorldToClipMatrix();
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

