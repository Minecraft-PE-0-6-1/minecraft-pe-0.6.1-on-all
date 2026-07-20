#include "SharedConstants.h"

namespace Common {

std::string getGameVersionString(const std::string& versionSuffix /* = "" */)
{
	std::string result = std::string("v0.1.0 (forked from v0.6.1 alpha)") + versionSuffix;
	// append 64-bit port marker only on Android 64‑bit targets
	#if defined(ANDROID) && (defined(__aarch64__) || defined(__x86_64__))
		result += " (64-bit)";
	#endif
	return result;
}

};
