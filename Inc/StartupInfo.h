#pragma once

namespace Gin {

//////////////////////////////////////////////////////////////////////////

// Information parsed from the command line.
class IStartupInfo {
public:
	virtual ~IStartupInfo() = 0;
};

//////////////////////////////////////////////////////////////////////////

inline IStartupInfo::~IStartupInfo()
{
}

//////////////////////////////////////////////////////////////////////////

}	// namespace Gin.

