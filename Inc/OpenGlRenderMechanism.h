#pragma once
#include <RenderMechanism.h>

namespace Gin {

class CGlContextManager;
//////////////////////////////////////////////////////////////////////////

class COpenGlRenderParameters : public IRenderParameters {
public:
	COpenGlRenderParameters( const CGlWindow& _targetWindow ) :  targetWindow( _targetWindow ) {}

	const CGlWindow& GetTargetWindow() const
		{ return targetWindow; }

private:
	const CGlWindow& targetWindow;
};

//////////////////////////////////////////////////////////////////////////

// Render mechanism that uses OpenGL.
class COpenGlRenderMechanism : public IRenderMechanism {
public:
	explicit COpenGlRenderMechanism( CGlContextManager& _glContextManager );
	~COpenGlRenderMechanism();

	virtual void AttachNewWindow( const CGlWindow& newWindow ) override final;
	virtual void ActivateWindowTarget() override final;
	virtual void SetBackgroundColor( CColor newValue ) override final;
	virtual void OnWindowResize( CVector2<int> newSize ) override final;
	virtual LRESULT OnEraseBackground( HWND window, WPARAM wParam, LPARAM lParam ) override final;
	virtual void OnDraw( const IState& currentState ) const override final;
	virtual void OnPostDraw() const override final;
	virtual LRESULT OnPaintMessage( HWND window, WPARAM wParam, LPARAM lParam, const IState& currentState ) const override final;
	virtual CArray<BYTE> ReadScreenBuffer( TTexelFormat format, CVector2<int>& bufferSize ) const override final;

private:
	CGlContextManager& glContextManager;
	const CGlWindow* targetWindow = nullptr;
	CColor backgroundColor;
	HBRUSH backgroundBrush;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

