#include <common.h>
#pragma hdrstop

#include <GifFile.h>

namespace Gin {

//////////////////////////////////////////////////////////////////////////

CGifFile::CGifFile( CUnicodePart _fileName ) :
	fileName( _fileName )
{
}

void CGifFile::Read( CArray<CColor>& result, CVector2<int>& resultSize ) const
{

}

void CGifFile::ReadRawData( CArrayView<BYTE> pngData, CArray<CColor>& result, CVector2<int>& resultSize )
{
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Twist.

