/* Rudimentary cross-platform (when completed) file modification polling */

namespace fwatch {

#if defined(_WIN32)
	#pragma error Polling not yet implemented on Windows
#else
	#include <sys/stat.h>
	#include <sys/time.h>
	typedef time_t Timestamp;
	const Timestamp ZERO_TIMESTAMP = 0;
#endif

/**
 * Checks if the file has been modified since the time stored at lastTime
 * If the file has been modified, the function returns true and writes the
 * modification time back to lastTime. If not, the function simply returns 
 * false.
 */
bool checkFileModified(const char* path, Timestamp* lastTime);

};