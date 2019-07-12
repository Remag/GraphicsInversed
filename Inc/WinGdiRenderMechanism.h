#pragma once
#include <RenderMechanism.h>

namespace Gin {

class CWinGdiRenderParameters : public IRenderParameters {
public:
	explicit CWinGdiRenderParameters( HDC dc ) : deviceContext( dc ) {}

	HDC GetDrawContext() const
		{ return deviceContext; }

private:
	HDC deviceContext;
};

//////////////////////////////////////////////////////////////////////////

// Renderer that uses WinAPI for drawing on screen.
class CWinGdiRenderMechanism : public IRenderMechanism {
public:
	explicit CWinGdiRenderMechanism();
	~CWinGdiRenderMechanism();

	virtual void AttachNewWindow( const CGlWindow& newWindow ) override final;
	virtual void ActivateWindowTarget() override final;
	virtual void SetBackgroundColor( CColor newValue ) override final;
	virtual void OnWindowResize( CVector2<int> newSize ) override final;
	virtual LRESULT OnEraseBackground( HWND window, WPARAM wParam, LPARAM lParam ) override final;
	virtual void OnDraw( const IState& currentState ) const override final;
	virtual void OnPostDraw() const override final;
	virtual LRESULT OnPaintMessage( HWND window, WPARAM wParam, LPARAM lParam, const IState& currentState ) const override final;
	virtual CArray<BYTE> ReadScreenBuffer( TTexelFormat format, CVector2<int>& screenSize ) const override final;

private:
	HDC windowDc = nullptr;
	HDC backBufferDc = nullptr;
	HBRUSH backgroundBrush = nullptr;
	HRGN clipRegion = nullptr;

	HBITMAP backBufferBitmap = nullptr;
	CVector2<int> windowSize;

	void swapRedBlue( CArrayBuffer<BYTE> result, int scanLineWidth, int lineCount, int pixelSize ) const;

	// Copying is prohibited.
	CWinGdiRenderMechanism( CWinGdiRenderMechanism& ) = delete;
	void operator=( CWinGdiRenderMechanism& ) = delete;
};

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

